#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ConfirmPopup.generated.h"

class UTextBlock;
class UButton;
class UEditableTextBox;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNumpadClickConfirmEvent, int, num);

UCLASS()
class PORTFOLIO_API UNumpadPopup : public UUserWidget
{
	GENERATED_BODY()

	UNumpadPopup(const FObjectInitializer& objectInitializer);
	
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	void SetTitle(FName str);
	void SetNumpad(int max);
	void UpdateNumpad();

	UFUNCTION()
	void OnClicked_Increase();
	UFUNCTION()
	void OnClicked_Decrease();
	UFUNCTION()
	void OnCommitted_UpdateNumber(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
	void OnClicked_Confirm();
	UFUNCTION()
	void OnClicked_Exit();

public:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mIncreaseButton;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mDecreaseButton;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UEditableTextBox> mEditableNumBox;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mConfirmButton;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mExitButton;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextBlock> mTitleText;

	FNumpadClickConfirmEvent mNumpadConfirmEvent; // confirm 버튼을 눌렀을 때 mCurrentNumber를 인자로 실행되는 이벤트

private:
	bool bBound;
	int mCurrentNumber;
	int mMaxNumber;
};

UCLASS()
class PORTFOLIO_API UNotifyPopup : public UUserWidget
{
	GENERATED_BODY()

public:
	UNotifyPopup(const FObjectInitializer& objinit);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	void SetNotifyText(FName str);

protected:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextBlock> mNotifyText;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mConfirmButton;

private:
	int bBound;
};

UCLASS()
class PORTFOLIO_API UConfirmPopup : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//virtual void NativePreConstruct() override;

};
