#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueWidget.generated.h"

class UCanvasPanel;
class UTextBlock;
class UTextButton;
class ANpcCharacter;
class APlayerCharacter;

/*
	NPC와 상호작용 시 대화 내용을 표시하는 UI
*/
UCLASS(Blueprintable)
class PORTFOLIO_API UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

	UDialogueWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	// UI가 화면에 표시될 때 호출되는 함수
	void Open(TObjectPtr<APlayerCharacter> player, TObjectPtr<ANpcCharacter> npc);
	// UI가 화면에서 사라질 때 호출되는 함수
	void Close();
	// 현재 인덱스의 다이얼로그를 UI에 표시하는 함수
	void RenderDialogue();
	// i번째 응답을 선택했을 때 호출되는 함수. 이벤트를 실행하고 다음 다이얼로그를 표시
	UFUNCTION()
	void OnPressed_Response(int i);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UCanvasPanel> mRootCanvas;
	// 다이얼로그의 대사를 표시하는 텍스트
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextBlock> mDialogueTextBlock;
	// 응답을 표시하는 텍스트 + 버튼
	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UTextButton>> mResponseTextButton;
	// 대화중인 NPC
	TObjectPtr<ANpcCharacter> mOwnerNpc;
	// 플레이어 캐릭터
	TObjectPtr<APlayerCharacter> mOwnerPlayer;
	// 현재 데화 인덱스
	int mCurrIndex;

	// 바인딩 중복 방지용 플래그
	bool bBounded;
};
