// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueWidget.generated.h"

class UCanvasPanel;
class UTextBlock;
class UTextButton;
class ANpcCharacter;
class APlayerCharacter;

UCLASS(Blueprintable)
class PORTFOLIO_API UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

	UDialogueWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	void Open(TObjectPtr<APlayerCharacter> player, TObjectPtr<ANpcCharacter> npc);
	void Close();
	void RenderDialogue();
	UFUNCTION()
	void OnPressed_Response(int i);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UCanvasPanel> mRootCanvas;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextBlock> mDialogueTextBlock;
	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UTextButton>> mResponseTextButton;
	
	TObjectPtr<ANpcCharacter> mOwnerNpc;
	TObjectPtr<APlayerCharacter> mOwnerPlayer;
	int mCurrIndex;

	bool bBounded;
};
