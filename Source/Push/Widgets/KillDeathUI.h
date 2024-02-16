#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Misc/Structures.h"
#include "KillDeathUI.generated.h"

/** Kill/Death�� �����ִ� UI: ��� �� TabŰ�� ���� Ȯ���ϴ� UI
 * 
 */
UCLASS(Blueprintable)
class PUSH_API UKillDeathUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
		void UpdatePlayerList(const TArray<FPlayerList>& PlayerData);

	UFUNCTION(BlueprintNativeEvent)
		void OpenLeaderBoard();
	void OpenLeaderBoard_Implementation();

	UFUNCTION(BlueprintNativeEvent)
		void OffLeaderBoard();
	void OffLeaderBoard_Implementation();

};
