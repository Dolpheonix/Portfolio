#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestWidget.generated.h"

class UButton;
class UTextButton;
class UTextBlock;
class APlayerCharacter;

UCLASS()
class PORTFOLIO_API UQuestWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UQuestWidget(const FObjectInitializer& objinit);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	void Open(TObjectPtr<APlayerCharacter> player);
	void Close();

protected:
	UFUNCTION()
	void OnClicked_SelectQuest(int index);
	UFUNCTION()
	void OnClicked_Exit();

protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UTextButton>> mQuestSlotButtons;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mExitButton;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextBlock> mQuestNameText;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextBlock> mQuestExplanationText;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextBlock> mSubQuestExplanationText;

	TObjectPtr<APlayerCharacter> mOwnerPlayer;
	int mSelectedIndex;

private:
	bool bBound;
};
