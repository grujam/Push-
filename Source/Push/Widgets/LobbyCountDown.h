#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyCountDown.generated.h"

/** Lobby���� ���Ǵ� ����
 *
 */
UCLASS(Blueprintable)
class PUSH_API ULobbyCountDown : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* PlayerAmount; // ������ �÷��̾� ��
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* CountDown; // ī��Ʈ �ٿ�

	UFUNCTION()
		void UpdateWidget();

	UFUNCTION()
		void SetPlayerNum(int InNum);
	UFUNCTION()
		void SetCountdown(int InNum);

private:
	int NumofPlayers = 0;
	int Countdown = 0;
};