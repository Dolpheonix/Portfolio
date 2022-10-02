// Copyright Epic Games, Inc. All Rights Reserved.


#include "quickstartGameModeBase.h"
#include <Blueprint/UserWidget.h>

void AquickstartGameModeBase::BeginPlay()
{
    Super::BeginPlay();
    ChangeMenuWidget(StartingWidgetClass);
}

void AquickstartGameModeBase::StartPlay()
{
    Super::StartPlay();
}

void AquickstartGameModeBase::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
    if(CurrentWidget != nullptr)
    {
        CurrentWidget->RemoveFromViewport();
        CurrentWidget = nullptr;
    }

    if(NewWidgetClass != nullptr)
    {
        CurrentWidget = CreateWidget(GetWorld(), NewWidgetClass);
        if(CurrentWidget != nullptr) CurrentWidget->AddToViewport();
    }
}