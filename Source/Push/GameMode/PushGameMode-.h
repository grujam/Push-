#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Misc/Structures.h"
#include "PushGameMode.generated.h"

/** ���� ���ӿ� ���Ǵ� GameMode
 *  GameMode�� Server���� �����Ѵ�. Client ����X
 *
 *   ���ð� > ���ð� > ����ð�
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRoundEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRoundStart);

namespace MatchState
{
	extern PUSH_API const FName Round; // ���ð�
}

namespace MatchState
{
	extern PUSH_API const FName Result; // �����ǥ
}

UCLASS()
class PUSH_API APushGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	APushGameMode();
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly)
		float WarmupTime = 15.0f; // �����ð� 
	UPROPERTY(EditDefaultsOnly)
		float ResultTime = 5.0f; // ����ð�

	float CurrentTime = 0.0f; // ���ӷ����ʿ� �� �ð�
	float CountdownTime = 0.0f;
	float tempTime = 0.0f;

protected:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

private:
	FLinearColor Colors[4] = { FLinearColor::Red, FLinearColor::Blue, FLinearColor::Green, FLinearColor::Black };
	uint8 index = 0;
	TArray<class APushPlayerController*> Controllers;

public:
	// 24_01_16 ���μ�
	UFUNCTION(BlueprintCallable)
		void UpdatePlayerList();

	UPROPERTY(EditAnywhere, Category = "PlayerList")
		TArray<FPlayerList> PlayerListData;

	UPROPERTY(BlueprintReadWrite)
		TArray<class APlayerController*> AllPC;

	UPROPERTY(EditAnywhere, Category = "Ring")
		TSubclassOf<class ARing> RingClass; // ��� �� �پ��� ��

	UPROPERTY(EditAnywhere, Category = "Ring")
		float ShrinkRate;

	TWeakObjectPtr<class ARing> Ring;

	FPlayerList PlayerData;

public:
	//24-01-24 ������ ���� ����
	UPROPERTY(EditAnywhere)
		TArray<float> RoundTime;

	UPROPERTY(EditAnywhere)
		TArray<float> RingRadius;

	UPROPERTY(EditAnywhere)
		uint8 TotalNumOfGames = 0;

	uint8 Round = 0;
	uint8 Games = 1;

	//24-01-26 ������ �÷��̾� ���� Ȯ��
	UFUNCTION()
		void PlayerDead(APushPlayerController* InController);

	uint8 NumofDeadPlayers = 0;

	//24-01-24 ������ ��� ����
	UPROPERTY(EditAnywhere)
		TArray<int32> MoneyPerRank;

	UPROPERTY(EditAnywhere)
		int32 BaseMoney = 10;

	class APushGameState* PushGameState;


public:	
	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable)
		FOnRoundEnd OnRoundEnd;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable)
		FOnRoundStart OnRoundStart;

	UFUNCTION()
		void RoundEnd();

	UFUNCTION()
		void RoundStart();
};