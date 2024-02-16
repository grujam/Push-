#include "Skill/Area/AreaSkills/Skill_Meteor_A.h"
#include "Global.h"
#include "Character/PushCharacter.h"
#include "Components/DecalComponent.h"
#include "Components/SkillComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Skill/SkillData.h"
#include "Skill/Area/PointDecal.h"
#include "Skill/Area/PointDecal_Meteor.h"

ASkill_Meteor_A::ASkill_Meteor_A()
{
    Helpers::CreateComponent(this, &SphereComponent, "Collision", Root);
    SphereComponent->SetSphereRadius(128.0f);

    Helpers::CreateActorComponent(this, &ProjectileComponent, "ProjectileComponent");
    ProjectileComponent->InitialSpeed = 1000.0f;
    ProjectileComponent->ProjectileGravityScale = 0.0f;

    Helpers::CreateComponent(this, &Particle, "Particle", Root);

    Particle->bAutoActivate = false;
    Particle->SetIsReplicated(true);

}

void ASkill_Meteor_A::BeginPlay()
{
    Super::BeginPlay();

    SkillComponent = Helpers::GetComponent<USkillComponent>(Owner);
    if (SkillComponent == nullptr)
        return;

    Particle->SetVisibility(true);
    Particle->Activate(true);
    ProjectileComponent->Activate(true);

    SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::ASkill_Meteor_A::OnComponentBeginOverlap);

    // ��Ŭ���� ���ؼ� ������ ��Ű�� �ߴµ� �׽�Ʈ������ BeginPlay���� ȣ��
    OnSkillClicked();
}

void ASkill_Meteor_A::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

}

void ASkill_Meteor_A::OnSkillPressed()
{
    Super::OnSkillPressed();


}

void ASkill_Meteor_A::OnSkillClicked()
{
    Super::OnSkillClicked();

    DecalLocation = SkillComponent->SpawnLocation - SkillComponent->curSkillData->RelativeLocation;

    // Meteor Direction
    FVector direction = (SkillComponent->SpawnLocation - DecalLocation).GetSafeNormal();
    ProjectileComponent->Velocity = ProjectileComponent->InitialSpeed * (-direction);

    FHitResult HitResult;
    TArray<AActor*> ignores;
    ignores.Add(Owner);

    // Meteor Trace
    UKismetSystemLibrary::LineTraceSingle(Owner->GetWorld(), SkillComponent->SpawnLocation, DecalLocation,
        CollisionType, false, ignores, DrawDebug, HitResult, true);

    // Spawn Decal
    OnSpawnPointDecal(DecalLocation);

}



void ASkill_Meteor_A::OnSpawnPointDecal(FVector InLocation)
{
    Super::OnSpawnPointDecal(InLocation);

    // �������� �ð� ���ϱ�
    float falling_Distance = FVector::Dist(SkillComponent->SpawnLocation, InLocation);
    float falling_Speed = ProjectileComponent->Velocity.Size();

    float fallingTime = falling_Distance / falling_Speed;

    // Point Decal Spawn
    FTransform decalTransform;
    decalTransform.SetLocation(InLocation);

    APointDecal_Meteor* DeferredDecal = Cast<APointDecal_Meteor>(
        Owner->GetWorld()->SpawnActorDeferred<APointDecal_Meteor>(PointDecal_Class, decalTransform));

    if (DeferredDecal != nullptr)
    {
        DeferredDecal->SetDecalTime(fallingTime);
        UGameplayStatics::FinishSpawningActor(DeferredDecal, decalTransform);
    }
}

void ASkill_Meteor_A::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    FVector CollisionLocation = SkillComponent->SpawnLocation - SkillComponent->curSkillData->RelativeLocation;

    OnDestroy(CollisionLocation);

    FVector start = CollisionLocation;
    FVector end = CollisionLocation;
    float radius = 150.0f;

    TArray<FHitResult> HitResult;
    TArray<AActor*> ignores;
    ignores.Add(Owner);

    UKismetSystemLibrary::SphereTraceMultiForObjects(Owner->GetWorld(), start, end, radius,
        ObjectType, false, ignores, DrawDebug, HitResult, true);

    for (FHitResult hitResult : HitResult)
    {
        APushCharacter* OwnerCharacter = Cast<APushCharacter>(hitResult.GetActor());

        if (OwnerCharacter == nullptr)
            continue;

        if (Hitted.Contains(OwnerCharacter))
            continue;

        Hitted.AddUnique(OwnerCharacter);
    }

    SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    for (ACharacter* hitted : Hitted)
    {
        APushCharacter* character = Cast<APushCharacter>(hitted);
        character->Hit(this, HitData);
    }

}

void ASkill_Meteor_A::OnDestroy(FVector InLocation)
{
    Super::OnDestroy(InLocation);

    // ���� ��ƼŬ ��ȯ
    if (Explosion)
    {
        Hitted.Empty();

        FTransform explosionTramsform;
        explosionTramsform.SetLocation(InLocation);
        explosionTramsform.SetScale3D(ExplosionScale);
        UGameplayStatics::SpawnEmitterAtLocation(Owner->GetWorld(), Explosion, explosionTramsform);

        Particle->SetActive(false);
    }

}