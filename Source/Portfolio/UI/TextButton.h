// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TextButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTextButtonEvent, int, index);

class UTextBlock;
class UButton;

/*
	텍스트 + 보이지 않는 버튼 --> 텍스트가 버튼 자체인거처럼 보이는 UI
*/
UCLASS()
class PORTFOLIO_API UTextButton : public UUserWidget
{
	GENERATED_BODY()

public:
	UTextButton(const FObjectInitializer& objectInitializer);

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& e) override;

	UFUNCTION()
	void ClickTrigger();

	// 마우스 호버시 텍스트 색상 변경
	UFUNCTION()
	void OnHovered_ChangeTextColor();
	// 언호버시 텍스트 색상 변경
	UFUNCTION()
	void OnUnHovered_ChangeTextColor();
	UFUNCTION()
	void OnPressed_ChangeTextColor();
	UFUNCTION()
	void OnReleased_ChangeTextColor();

public:
	// 텍스트
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextBlock> mTextBlock;
	// 버튼
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mButton;

	// 클릭시 호출되는 이벤트
	FTextButtonEvent mClickEvent;

	// 평상시 텍스트 색상
	FSlateColor mNormalColor;
	// 호버시 텍스트 색상
	FSlateColor mHoveredColor;
	// 눌렀을 시 텍스트 색상
	FSlateColor mPressedColor;

	// 중복 바인딩 방지용 플래그
	bool bBounded;

// 블루프린트 표시용 파라미터
	// 버튼의 인덱스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	int mIndex;
	// 텍스트 폰트 크기
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	float FontSize;
	// 표시할 텍스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	FText Text;
};
