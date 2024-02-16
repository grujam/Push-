#pragma once

#include "CoreMinimal.h"
#include "WDG_Buff.h"
#include "Blueprint/UserWidget.h"
#include "WDG_BuffBoard.generated.h"

/*TODO : 2024.01.05 ������
  �׸��� ���� �ȿ� �߰�
*/

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnRemoveChild, UWDG_Buff*, widget);

UCLASS()
class PUSH_API UWDG_BuffBoard : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
public:
	UFUNCTION(BlueprintImplementableEvent)
		void AddBuff(UWDG_Buff* widget);

	UFUNCTION(BlueprintImplementableEvent)
		void OnRemoveChild(UWDG_Buff* widget);

	UFUNCTION()
		void SubWidget(UWDG_Buff* widget);


public:
	//2024.01.09 ������
	//������ ���ŵ����� ������ ��������Ʈ
	FOnRemoveChild RemoveChildDelegate;
};
