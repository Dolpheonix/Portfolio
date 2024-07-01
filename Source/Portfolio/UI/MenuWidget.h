#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuWidget.generated.h"

class UButton;
class APlayerCharacter;

/*
	메인 메뉴 UI
	- 인게임으로 돌아감
	- 인트로로 돌아감
*/
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
	// UI를 엶
	void Open(TObjectPtr<APlayerCharacter> player);
	// UI를 닫음
	void Close();
protected:
	// UI를 닫고 인게임으로 전환
	UFUNCTION()
	void OnClicked_BackToGame();
	// 인트로 UI로 전환
	UFUNCTION()
	void OnClicked_ExitGame();

protected:
	// 인게임으로 돌아가는 버튼
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mBackToGameButton;
	// 인트로로 전환하는 버튼
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mExitGameButton;

private:
	// 플레이어 캐릭터
	TObjectPtr<APlayerCharacter> mOwnerPlayer;

	// 중복 바인딩 방지용 플래그
	bool bBound;
};
