// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "IndexedButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIndexedButtonEvent, int, index);

UCLASS()
class PORTFOLIO_API UIndexedButton : public UButton
{
	GENERATED_BODY()

public:
	UIndexedButton();

	UFUNCTION()
	void HoverTrigger();
	UFUNCTION()
	void ReleaseTrigger();
	UFUNCTION()
	void PressTrigger();
	UFUNCTION()
	void ClickTrigger();

	FIndexedButtonEvent mHoveredEvent;
	FIndexedButtonEvent mReleasedEvent;
	FIndexedButtonEvent mPressedEvent;
	FIndexedButtonEvent mClickedEvent;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int mIndex;
};
