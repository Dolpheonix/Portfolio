#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

class UProgressBar;
class UImage;
class APlayerCharacter;
class UScrollBox;
class UTextBlock;
class UEditableTextBox;
class UButton;
class UImage;

UCLASS()
class PORTFOLIO_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UHUDWidget(const FObjectInitializer& objectInitializer);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

public:
	void Open(TObjectPtr<APlayerCharacter> player);

	void SetChattingBoxVisibility(bool newVisibility);
	bool GetChattingBoxVisibility() const;

	void UpdateChatting();
	void UpdateHpBar();
	void UpdateWeaponImage();

	UFUNCTION()
	void SendChatting();

protected:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UProgressBar> mHpProgressBar;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UImage> mCurrentWeaponImage;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UImage> mNextWeaponImage;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UScrollBox> mChattingScroll;
	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UTextBlock>> mChattingTexts;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UEditableTextBox> mChattingInputText;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mSendChattingButton;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UImage> mChattingBoxBackground;

private:
	TObjectPtr<APlayerCharacter> mOwnerPlayer;
	bool bChattingBoxVisibility;
	bool bBound;
};
