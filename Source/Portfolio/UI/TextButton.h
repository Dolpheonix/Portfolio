// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TextButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTextButtonEvent, int, index);

class UTextBlock;
class UButton;

UCLASS()
class PORTFOLIO_API UTextButton : public UUserWidget
{
	GENERATED_BODY()

public:
	UTextButton(const FObjectInitializer& objectInitializer);

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& e) override;

	UFUNCTION()
	void ClickTrigger();

	UFUNCTION()
	void OnHovered_ChangeTextColor();
	UFUNCTION()
	void OnUnHovered_ChangeTextColor();
	UFUNCTION()
	void OnPressed_ChangeTextColor();
	UFUNCTION()
	void OnReleased_ChangeTextColor();

public:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextBlock> mTextBlock;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mButton;

	FTextButtonEvent mClickEvent;

	FSlateColor mNormalColor;
	FSlateColor mHoveredColor;
	FSlateColor mPressedColor;

	bool bBounded;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	int mIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	float FontSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	FText Text;
};
