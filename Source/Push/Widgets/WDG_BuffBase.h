#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WDG_BuffBase.generated.h"

/*TODO : 2024.01.05 ������
  �׸��� ���� �ȿ� �߰�
*/

UCLASS()
class PUSH_API UWDG_BuffBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
		void AddBuff();
};
