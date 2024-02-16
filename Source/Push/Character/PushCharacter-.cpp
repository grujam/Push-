// Copyright Epic Games, Inc. All Rights Reserved.

#include "PushCharacter.h"

#include <GameFramework/PlayerStart.h>

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/ResourceComponent.h"
#include "Components/MoveComponent.h"
#include "Components/BuffComponent.h"
#include "Components/ItemComponent.h"
#include "Components/ShopComponent.h"
#include "Components/ChatComponent.h"
#include "Engine/DecalActor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Skill/Area/Area.h"
#include "Particles/ParticleSystem.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Global.h"
#include "Components/SkillComponent.h"
#include "Components/StateComponent.h"
#include "Components/WidgetComponent.h"
#include "GameInstance/PushGameInstance.h"
#include "GameMode/PushGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/WDG_EffectBase.h"
#include "Widgets/SkillSlots.h"
#include "Skill/SkillData.h"
#include "PlayerController/PushPlayerController.h"
#include "Widgets/PlayerNameTag.h"
#include "Widgets/SkillSlots.h"

//////////////////////////////////////////////////////////////////////////
// APushCharacter

APushCharacter::APushCharacter()
{
    // ĸ�� ������ ����
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // Don't rotate when the controller rotates. Let that just affect the camera.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    //ĳ���� Movement����
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
    GetCharacterMovement()->JumpZVelocity = 600.f;
    GetCharacterMovement()->AirControl = 0.2f;

    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.0f;
    CameraBoom->bUsePawnControlRotation = true;

    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    //Component
    Helpers::CreateActorComponent<UMoveComponent>(this, &MoveComponent, "MoveComponent");
    Helpers::CreateActorComponent<UResourceComponent>(this, &ResourceComponent, "ResourceComponent");
    Helpers::CreateActorComponent<USkillComponent>(this, &SkillComponent, "SkillComponent");
    Helpers::CreateActorComponent<UBuffComponent>(this, &BuffComponent, "BuffComponent");
    Helpers::CreateActorComponent<UItemComponent>(this, &ItemComponent, "ItemComponent");
    Helpers::CreateActorComponent<UShopComponent>(this, &ShopComponent, "ShopComponent");
    Helpers::CreateActorComponent<UStateComponent>(this, &StateComponent, "StateComponent");

    if(SkillComponent != nullptr)
    {
        SkillComponent->SetNetAddressable();
        SkillComponent->SetIsReplicated(true);
    }
}

// Input
void APushCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    //// Set up gameplay key bindings
    check(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", MoveComponent, &UMoveComponent::OnMoveForward);
    PlayerInputComponent->BindAxis("MoveRight", MoveComponent, &UMoveComponent::OnMoveRight);

    PlayerInputComponent->BindAxis("Turn", MoveComponent, &UMoveComponent::OnTurnAt);
    PlayerInputComponent->BindAxis("LookUp", MoveComponent, &UMoveComponent::OnLookUp);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

    PlayerInputComponent->BindAction("KD", EInputEvent::IE_Pressed, ResourceComponent, &UResourceComponent::OnKillDeathUI);
    PlayerInputComponent->BindAction("KD", EInputEvent::IE_Released, ResourceComponent, &UResourceComponent::OffKillDeathUI);
}

// ���� �۾� �κ� Check
void APushCharacter::Hit(AActor* InAttacker, const FHitData& InHitData)
{
    FVector direction = GetActorLocation() - InAttacker->GetActorLocation();
    direction = direction.GetSafeNormal();

    FVector launch = FVector(InHitData.xLaunchPower * direction.X, InHitData.xLaunchPower * direction.Y, InHitData.zLaunchPower);

    if (InAttacker->GetOwner() != nullptr)
    {
        APushCharacter* attacker = Cast<APushCharacter>(InAttacker->GetOwner());
        if (attacker != nullptr)
        {
            SetAttacker_Server(attacker);
        }
    }

    if(ResourceComponent != nullptr)
    {
        if (StateComponent->IsDeadMode() == true)
        {
            return;
        }

        if (ResourceComponent->GetHP() - InHitData.Damage <= 0)
        {
            // ų �α� ���
            if(Attacker != nullptr)
            {
                CLog::Log("Attacker Exists");
                UResourceComponent* resource = Helpers::GetComponent<UResourceComponent>(Attacker);

                if (resource != nullptr)
                    resource->AdjustKill_Server(1);

                ResourceComponent->ShowKillLog(Attacker, this);
            }
            else
            {
				ResourceComponent->ShowKillLog(InAttacker, this);
            }

            Ragdoll();
            StateComponent->SetDeadMode();

            if(IsLocallyControlled())
				Dead_Server();
        }
        else
        {
            if(IsLocallyControlled())
				ResourceComponent->AdjustHP_Server(-InHitData.Damage);
        }
    }

    DoCameraShake(InHitData.Damage);

    if(launch.X + launch.Y + launch.Z > 0.0f)
    {
        if(IsLocallyControlled())
    		LaunchServer(launch);
    }

    if(InHitData.Effect != nullptr)
    {
        UParticleSystem* particle = Cast<UParticleSystem>(InHitData.Effect);
        UNiagaraSystem* niagara = Cast<UNiagaraSystem>(InHitData.Effect);

        if(particle != nullptr)
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), particle, InHitData.Location, FRotator::ZeroRotator, InHitData.EffectScale);
        }

        if(niagara != nullptr)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), niagara, InHitData.Location, FRotator::ZeroRotator, InHitData.EffectScale);
        }
    }

    if(InHitData.Sound != nullptr)
    {
        UGameplayStatics::SpawnSoundAtLocation(GetWorld(), InHitData.Sound, InHitData.Location);
    }
}

void APushCharacter::SetLocation_Implementation(FVector InLocation)
{
    SetActorLocation(InLocation);
}

void APushCharacter::DoCameraShake(float Damage)
{
    float Velocity = Damage / 10;

    if (CameraShakeBase != nullptr)
    {
        APushPlayerController* OwnController = Cast<APushPlayerController>(GetController());

        if(OwnController != nullptr)
        {
	        if(OwnController->IsLocalController())
	        {
                OwnController->ClientPlayCameraShake(CameraShakeBase, Velocity);
	        }
        }
    }
}

void APushCharacter::Create_DynamicMaterial()
{
    for (int32 i = 0; i < this->GetMesh()->GetMaterials().Num(); i++)
    {
        UMaterialInterface* material = this->GetMesh()->GetMaterials()[i];

        this->GetMesh()->SetMaterial(i, UMaterialInstanceDynamic::Create(material, this));
    }
}

void APushCharacter::Change_Color(FLinearColor InColor)
{
    for(UMaterialInterface* material : this->GetMesh()->GetMaterials())
    {
        UMaterialInstanceDynamic* MaterialDynamic = Cast<UMaterialInstanceDynamic>(material);

        if (MaterialDynamic)
        {
            MaterialDynamic->SetVectorParameterValue("BodyColor", InColor);
        }
    }

}

// ���� �۾� �κ� Check
void APushCharacter::LaunchServer_Implementation(FVector InLaunch)
{
    LaunchNMC_Implementation(InLaunch);
}

// ���� �۾� �κ� Check
void APushCharacter::LaunchNMC_Implementation(FVector InLaunch)
{
    LaunchCharacter(InLaunch, false, false);
}

void APushCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(APushCharacter, BodyColor);
    DOREPLIFETIME(APushCharacter, CustomPlayerName);
    DOREPLIFETIME(APushCharacter, Attacker);
}

void APushCharacter::SetUpLocalName()
{
    if (IsLocallyControlled())
    {
        UPushGameInstance* gameInstance = Cast<UPushGameInstance>(GetGameInstance());
        if (gameInstance)
			SetPlayerNameServer(gameInstance->GetPlayerName());
    }
}

void APushCharacter::Ragdoll()
{
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    GetMesh()->SetCollisionProfileName("Ragdoll");
    GetMesh()->SetSimulatePhysics(true);

    FVector ImpulseDirection = GetActorRotation().Vector() * -1.0f;
    ImpulseDirection.Normalize();

    //�浹 ����
    float ImpulseStrength = 5000.0f;

    FVector FinalImpulse = ImpulseDirection * ImpulseStrength;

    //GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);
    GetMesh()->AddImpulseToAllBodiesBelow(FinalImpulse, NAME_None);
    
}

// ���� �۾� �κ� Check
void APushCharacter::SetAttacker_Server_Implementation(APushCharacter* InAttacker)
{
    Attacker = InAttacker;
}

void APushCharacter::SetSpawnPoint_Implementation()
{
    //SetSpawnPointNMC();
}

// ���� �۾� �κ� Check
void APushCharacter::Dead_Server_Implementation()
{
    APushGameMode* GameMode = Cast<APushGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

    if (GameMode == nullptr)
        return;

    APushPlayerController* controller = Cast<APushPlayerController>(GetController());

    if (controller == nullptr)
        return;

    GameMode->PlayerDead(controller);
}


void APushCharacter::SetSpawnPointNMC_Implementation(FVector InLocation)
{
    CLog::Log("SetSpawnPoint");
    // Ragdoll�� �и��� ��� capsule �ٽ� ���̱�
    if (GetCapsuleComponent()->GetCollisionEnabled() == ECollisionEnabled::NoCollision)
    {
        GetMesh()->SetSimulatePhysics(false);
        GetMesh()->SetAllBodiesSimulatePhysics(false);

        GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
        GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.f, -90.f, 0.f));

        GetMesh()->SetCollisionProfileName("PhysicsActor");
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }

    if(IsLocallyControlled())
    {
		StateComponent->SetIdleMode(); // �⺻ ���·� �ǵ���
		ResourceComponent->SetHP_Server(ResourceComponent->GetMaxHP()); // HP 100���� ����
    }

    SetActorLocation(InLocation);

}

void APushCharacter::SetPlayerNameServer_Implementation(const FString& NewPlayerName)
{
    CustomPlayerName = NewPlayerName;
}

void APushCharacter::BeginPlay()
{
    Super::BeginPlay();

    Create_DynamicMaterial();
    Change_Color(BodyColor);

    SetUpLocalName();

    
}

void APushCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

}
