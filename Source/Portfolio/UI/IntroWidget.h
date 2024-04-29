#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ConfirmPopup.h"
#include "IntroWidget.generated.h"

class UButton;
class UTextButton;
class UTextBlock;
class UEditableTextBox;
class UWidgetSwitcher;

UCLASS()
class PORTFOLIO_API UIntroWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UIntroWidget(const FObjectInitializer& objectInitializer);
	
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

protected:
	UFUNCTION()
	void OnClicked_Back();

	UFUNCTION()
	void OnClicked_OpenNewGameMenu();
	UFUNCTION()
	void OnClicked_OpenLoadGameMenu();

	UFUNCTION()
	void OnClicked_StartNewGame();

	UFUNCTION()
	void OnClicked_SelectSaveSlot(int index);
	UFUNCTION()
	void OnClicked_LoadGame();

	enum class EIntroWidgetType : uint8
	{
		Main,
		NewGame,
		LoadGame,
		Count,
	};

	void SwitchMenu(EIntroWidgetType type);
	void ShowPopup(FName notifyText);

protected:
	// Widget Switcher
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UWidgetSwitcher> mWidgetSwitcher;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mBackButton;

	// Main Menu
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mNewGameButton;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mLoadGameButton;

	// New Game Menu
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UEditableTextBox> mNewNameText;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mNewButton;

	// Load Game Menu
	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UTextButton>> mSlotButtons;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextBlock> mSlotNameText;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextBlock> mSlotInfoText;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mLoadButton;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UNotifyPopup> NotifyPopupClass;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UNotifyPopup> mNotifyPopup;

	int mSelectedSlot;

private:
	bool bBounded;
};
