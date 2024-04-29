#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuWidget.generated.h"

class UButton;
class APlayerCharacter;

UCLASS()
class PORTFOLIO_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UMenuWidget(const FObjectInitializer& objinit);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;


public:
	void Open(TObjectPtr<APlayerCharacter> player);
	void Close();
protected:
	UFUNCTION()
	void OnClicked_BackToGame();

	UFUNCTION()
	void OnClicked_SaveGame();

	UFUNCTION()
	void OnClicked_ExitGame();

protected:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mBackToGameButton;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mSaveGameButton;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mExitGameButton;

private:
	TObjectPtr<APlayerCharacter> mOwnerPlayer;
	bool bBound;
};
