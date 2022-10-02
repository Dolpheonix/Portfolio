// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "quickstartGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class QUICKSTART_API AquickstartGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "UMG_Game")
	void ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass);

protected:
	virtual void BeginPlay() override;
	virtual void StartPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
	TSubclassOf<UUserWidget> StartingWidgetClass;
		
	UPROPERTY()
	UUserWidget* CurrentWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG_Game")
	bool bStarted = false;

	UPROPERTY(BlueprintReadWrite, Category = "UMG_Game")
	bool bIntroEnd = false;

	UPROPERTY(BlueprintReadWrite, Category = "UMG_Game")
	FText playerName;
};
