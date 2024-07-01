#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HpBar.generated.h"

class UProgressBar;

UCLASS()
class PORTFOLIO_API UHpBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UHpBar(const FObjectInitializer& objinit);

	// HP바를 업데이트
	// @param progress : hp 퍼센트
	void SetHp(float progress);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

protected:
	// HP 바
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UProgressBar> mProgressBar;
};
