// Fill out your copyright notice in the Description page of Project Settings.


#include "IndexedButton.h"

UIndexedButton::UIndexedButton() : mIndex(-1)
{
	SetPressMethod(EButtonPressMethod::DownAndUp);

	OnHovered.AddDynamic(this, &UIndexedButton::HoverTrigger);
	OnReleased.AddDynamic(this, &UIndexedButton::ReleaseTrigger);
	OnPressed.AddDynamic(this, &UIndexedButton::PressTrigger);
	OnClicked.AddDynamic(this, &UIndexedButton::ClickTrigger);
}

void UIndexedButton::HoverTrigger()
{
	mHoveredEvent.Broadcast(mIndex);
}

void UIndexedButton::ReleaseTrigger()
{
	mReleasedEvent.Broadcast(mIndex);
}

void UIndexedButton::PressTrigger()
{
	mPressedEvent.Broadcast(mIndex);
}

void UIndexedButton::ClickTrigger()
{
	mClickedEvent.Broadcast(mIndex);
}

