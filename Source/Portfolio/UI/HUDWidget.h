#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

class UProgressBar;
class UImage;
class APlayerCharacter;

UCLASS()
class PORTFOLIO_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UHUDWidget(const FObjectInitializer& objectInitializer);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	void Open(TObjectPtr<APlayerCharacter> player);

	void UpdateHpBar();
	void UpdateWeaponImage();

protected:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UProgressBar> mHpProgressBar;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UImage> mCurrentWeaponImage;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UImage> mNextWeaponImage;

private:
	TObjectPtr<APlayerCharacter> mOwnerPlayer;
	bool bBound;
};
