#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestWidget.generated.h"

class UButton;
class UTextButton;
class UTextBlock;
class APlayerCharacter;

/*
	진행중인 퀘스트의 진행 상태를 보여주는 UI
	- 퀘스트 목록
	- 퀘스트 진행 상황, 서브퀘 진행 상황
*/
UCLASS()
class PORTFOLIO_API UQuestWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UQuestWidget(const FObjectInitializer& objinit);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	// UI를 엶
	void Open(TObjectPtr<APlayerCharacter> player);
	// UI를 닫음
	void Close();

protected:
	// 퀘스트를 선택함. 퀘스트 내용, 진행 상황을 오른쪽 UI에 표시
	UFUNCTION()
	void OnClicked_SelectQuest(int index);
	// UI 종료
	UFUNCTION()
	void OnClicked_Exit();

protected:
	// 퀘스트 선택 버튼
	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UTextButton>> mQuestSlotButtons;
	// 종료 버튼
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mExitButton;
	
// 퀘스트 정보 창
	// 퀘스트 이름
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextBlock> mQuestNameText;
	// 퀘스트 설명
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextBlock> mQuestExplanationText;
	// 서브퀘 설명
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextBlock> mSubQuestExplanationText;

	// 플레이어 캐릭터
	TObjectPtr<APlayerCharacter> mOwnerPlayer;
	// 선택된 퀘스트
	int mSelectedIndex;

private:
	// 중복 바인딩 방지용 플래그
	bool bBound;
};
