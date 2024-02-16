#include "PlayerController/LobbyPlayerController.h"
#include "Global.h"
#include "GameState/LobbyGameState.h"
#include "HUD/LobbyHUD.h"
#include "Widgets/LobbyCountDown.h"

// LobbyPlayerController ��ü������ ���� �۾� �κ� Check

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ALobbyHUD* LobbyHUD = Cast<ALobbyHUD>(GetHUD());

	if (IsValid(LobbyHUD))
	{
		LobbyHUD->AddWidget();
		if (LobbyHUD->CheckWidget("LobbyCountDown"))
		{
			LobbyHUD->GetWidget<ULobbyCountDown>("LobbyCountDown")->SetVisibility(ESlateVisibility::Visible);
			ALobbyGameState* state = Cast<ALobbyGameState>(UGameplayStatics::GetGameState(GetWorld()));
			if (state != nullptr)
				LobbyHUD->GetWidget<ULobbyCountDown>("LobbyCountDown")->SetPlayerNum(state->NumofPlayers);
		}
	}
}

void ALobbyPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//SetHUDCountdownTime(); // LobbyHUD�� �Լ� UpdateWidget()���� '������ �÷��̾� & �ð�' ������Ʈ 
}

void ALobbyPlayerController::UpdateTimer_Client_Implementation(int InTime)
{
	ALobbyHUD* LobbyHUD = Cast<ALobbyHUD>(GetHUD());


	if (LobbyHUD->CheckWidget("LobbyCountDown"))
	{
		LobbyHUD->GetWidget<ULobbyCountDown>("LobbyCountDown")->SetCountdown(InTime);
	}
}

void ALobbyPlayerController::UpdatePlayerNum_Client_Implementation(int InNum)
{
	ALobbyHUD* LobbyHUD = Cast<ALobbyHUD>(GetHUD());

	if(LobbyHUD == nullptr)
	{
		CLog::Log("HUD invalid");
		return;
	}

	if (LobbyHUD->CheckWidget("LobbyCountDown"))
	{
		LobbyHUD->GetWidget<ULobbyCountDown>("LobbyCountDown")->SetPlayerNum(InNum);
	}
	else
	{
		CLog::Log("No Widget");
	}
}

