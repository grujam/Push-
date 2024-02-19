#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "Structures.generated.h"

USTRUCT(BlueprintType)
struct FProduct
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        UTexture2D* Image;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float Price;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString SkillDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString CoolTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString Damage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString LaunchRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool isBought;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        UAnimMontage* ActionMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float PlayRate = 1;

};

USTRUCT()
struct FProjectileData
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Projectile")
        float Speed = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Projectile")
        float Gravity_Scale = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Projectile")
        bool CanRotate = false;

    UPROPERTY(EditAnywhere, Category = "Projectile")
        float InterpSpeed = 2.0f;
};

USTRUCT()
struct FHitData
{
    GENERATED_BODY()

public:
    //Character���� ������ ������ ��ġ
    UPROPERTY(EditAnywhere, Category = "Damage")
        float Damage = 0.0f;

    //LaunchPercent: �� ������ ���󰡴� Launch ����
    UPROPERTY(EditAnywhere, Category = "Launch")
        float xLaunchPower = 0.0;

    UPROPERTY(EditAnywhere, Category = "Launch")
        float zLaunchPower = 0.0;

    //Effect: �ǰ� �� ������ ����Ʈ
    UPROPERTY(EditAnywhere, Category = "Effect")
        class UFXSystemAsset* Effect = nullptr;

    //EffectScale: �ǰ� �� ������ ����Ʈ ũ�� ����
    UPROPERTY(EditAnywhere, Category = "Effect")
        FVector EffectScale = FVector(1.0f, 1.0f, 1.0f);

    //Location: �ǰ� �� ������ ����Ʈ�� ���� ��ġ
    UPROPERTY(EditAnywhere, Category = "Effect")
        FVector Location = FVector(0.0f, 0.0f, 0.0f);

    //Sound: �ǰ� �� ������ ����
    UPROPERTY(EditAnywhere, Category = "Sound")
        class USoundWave* Sound = nullptr;

    //Debuffs: �ǰ� �� �����ϴ� �������
    UPROPERTY(EditAnywhere, Category = "Debuff")
        TArray<EDebuff> Debuffs;

public:
    float xLaunch;
    float yLaunch;
    float zLaunch;
};

USTRUCT(BlueprintType)
struct FPlayerList
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
        FString PlayerName = TEXT("NNNNNN");

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
        int32 Kill = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
        int32 Death = 0;

    //UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    //	FTexture Skill_Icon;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
        int32 Gold = 0;
};

UCLASS()
class PUSH_API UStructures : public UObject
{
    GENERATED_BODY()

};