#include "TracePoint.h"
#include "Components/DecalComponent.h"
#include "Global.h"

ATracePoint::ATracePoint()
{
	PrimaryActorTick.bCanEverTick = true;

	Helpers::CreateComponent(this, &Root, "Root", RootComponent);
	Helpers::CreateComponent(this, &TracePoint, "TracePoint", Root);

	TracePoint->SetVisibility(true);
	TracePoint->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
}

void ATracePoint::BeginPlay()
{
	Super::BeginPlay();
	
	Owner = Cast<ACharacter>(GetOwner());

	if (Owner == nullptr)
		return;

}

void ATracePoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// TODO :: SkillData���� ���� �״� �� �� �ִ� bool�� �޾ƿ���
	TraceDecal();

}

void ATracePoint::TraceDecal()
{
	// 231229_���μ�
	// ī�޶� �ٶ󺸴� ���⿡ Decal_Cursor Spawn

	FVector start;
	FRotator PlayerViewRotation;
	APlayerController* controller = Cast<APlayerController>(Owner->GetController());

	if (controller)
		controller->GetPlayerViewPoint(start, PlayerViewRotation);

	FVector direction = PlayerViewRotation.Vector();
	FVector end = start + (direction * MaxDistance);

	FHitResult HitResult;
	TArray<AActor*> ignores;
	ignores.Add(Owner);

	UKismetSystemLibrary::LineTraceSingle(Owner->GetWorld(), start, end, TraceType, false, ignores, DrawDebug_Decal, HitResult, true);

	// �ִ� �Ÿ� ���� Hit ������ ���ٸ�, �Ʒ��� �ѹ� �� ����
	if (HitResult.bBlockingHit)
	{
		// Decal�� ���� �İ� ���� -direction �������� ��ġ ����
		FVector DownStart = HitResult.ImpactPoint - (direction * 20.0f);
		FVector DownEnd = DownStart - FVector(0, 0, MaxDistance + 1000.0f);

		FHitResult DownHitResult;
		UKismetSystemLibrary::LineTraceSingle(Owner->GetWorld(), DownStart, DownEnd, TraceType, false, ignores, DrawDebug_Decal, DownHitResult, true);

		if (DownHitResult.bBlockingHit)
			TraceLocation = DownHitResult.ImpactPoint;

		else
			TraceLocation = HitResult.ImpactPoint;
	}

	// Decal Spawn �����Ÿ� ����
	// �ִ� �Ÿ��� �Ѿ�� Hit�� �ȵ��� ���� �ִ�Ÿ� ������ Decal Spawn
	else
	{
		FVector DownStart = end;
		FVector DownEnd = DownStart - FVector(0, 0, MaxDistance + 1000.0f);

		FHitResult DownHitResult;
		UKismetSystemLibrary::LineTraceSingle(Owner->GetWorld(), DownStart, DownEnd, TraceType, false, ignores, DrawDebug_Decal, DownHitResult, true);

		// �Ʒ��� �������� �� Hit�� �Ǹ� �� ������ Decal Spawn
		if (DownHitResult.bBlockingHit)
			TraceLocation = DownHitResult.ImpactPoint;

		// �׷��� Hit�� �ȵǸ� ����ó�� �ִ�Ÿ� ������ Decal Spawn
		else
			TraceLocation = end;

	}

	TracePoint->SetWorldLocation(TraceLocation);
}

