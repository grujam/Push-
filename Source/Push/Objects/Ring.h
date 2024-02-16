#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ring.generated.h"

UCLASS()
class PUSH_API ARing : public AActor
{
	GENERATED_BODY()
	
public:	
	ARing();

public:
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
public:	
	virtual void Tick(float DeltaTime) override;

public:
	//Collision�� ����ϴ� Capsule
	UPROPERTY(EditAnywhere, Replicated)
		class UCapsuleComponent* RingCapsule;

	//�ڱ��� ������� ����ϴ� Mesh
	UPROPERTY(EditAnywhere, Replicated)
		class UStaticMeshComponent* RingMesh;

	//�ڱ��� ���ط�
	UPROPERTY(EditAnywhere)
		float RingDamage = 1.0f;

	//�ڱ��� ����Ʈ Widget
	UPROPERTY(EditAnywhere)
		TSubclassOf<class UWDG_EffectBase> DamageEffectWidget;

public:
	//2024_01_02 ������ �ڱ��� ũ�� ���̴� �Լ�
	//InRadius: �پ�� �ڱ��� ������ ũ��, InTime: �پ�� �ð� (�ʴ���)
	UFUNCTION(BlueprintCallable)
		void Shrink(float InRadius, float InTime);

	//Timer�� ���� �ڱ��� �پ��� ���� �����ϴ� �Լ�
	UFUNCTION(BlueprintCallable)
		void ChangeRadius();

	//OnConstruction �� Mesh�� Capsule�� ũ�⸦ �����ֱ� ���� �Լ�
	UFUNCTION()
		void Refresh();

	UFUNCTION()
		void Reset();

	UFUNCTION(NetMulticast, Reliable)
		void SetCapsuleRadius_NMC(float InRadius);

	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	// Base�� ������ ���� ����ϸ� Capsule�� Mesh�� ũ�Ⱑ �� �½��ϴ�
	float Base = 50.0f;

	TArray<class APushCharacter*> OverlappedCharacters;
	FTimerHandle TimerHandle;

	float DeltaRadius = 0.0f;
	float TargetRadius;
	float StartRadius;
};
