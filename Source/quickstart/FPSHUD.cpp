// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSHUD.h"
#include "Engine/Canvas.h"
#include <Kismet/GameplayStatics.h>
#include "Character/Main/MainCharacter.h"

AFPSHUD::AFPSHUD()
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> textureFinder(TEXT("/Game/ShootingGame/Image/crosshair.crosshair"));

	if (textureFinder.Succeeded())
	{
		CrossHairTexture = textureFinder.Object;
	}
}

void AFPSHUD::DrawHUD()
{
	Super::DrawHUD();

	if(CrossHairTexture)
	{
		// Canvas Center
		FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

		// CrossHair Draw StartingPoint (leftmost, upmost)
		FVector2D CrossHairDrawPosition(Center.X - (CrossHairTexture->GetSurfaceWidth() * 0.5f), Center.Y - (CrossHairTexture->GetSurfaceHeight() * 0.5f));

		// Drawing CrossHair
		FCanvasTileItem TileItem(CrossHairDrawPosition, CrossHairTexture->Resource, FLinearColor::Blue); // position, texture, color
		TileItem.BlendMode = SE_BLEND_Translucent; // Translucent : Åõ°ú
		Canvas->DrawItem(TileItem);
	}

	AMainCharacter* currentPlayer = (AMainCharacter*)UGameplayStatics::GetPlayerPawn(this, 0);
	float currentHP = currentPlayer->HP;

	FVector2D Status(Canvas->ClipX * 0.3f, Canvas->ClipY * 0.8f);
	FString StatusStr = FString(TEXT("HP : "));
	StatusStr += FString::SanitizeFloat(currentHP);
	FText StatusText = FText::FromString(StatusStr);
	FCanvasTextItem StatusItem(Status, StatusText, GEngine->GetMediumFont(), FLinearColor::Blue);

	Canvas->DrawItem(StatusItem);
}