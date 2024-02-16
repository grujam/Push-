#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StoreUI.generated.h"

/** ���� UI: ������ ����, �Ǹ� ���� �۾��� �ϴ� UI
 * 
 */
UCLASS(Blueprintable)
class PUSH_API UStoreUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UStoreUI(const FObjectInitializer& ObjectInitializer);
		
};