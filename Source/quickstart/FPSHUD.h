// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FPSHUD.generated.h"

/**
 * 
 */
UCLASS()
class QUICKSTART_API AFPSHUD : public AHUD
{
	GENERATED_BODY()
		
protected:
	AFPSHUD();

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* CrossHairTexture;

public:
	virtual void DrawHUD() override;
	
};
