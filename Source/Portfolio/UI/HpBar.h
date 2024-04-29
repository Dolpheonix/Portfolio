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

	void SetHp(float progress);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UProgressBar> mProgressBar;
};
