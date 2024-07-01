#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Common/Interface/Interaction.h"
#include "../Common/Item.h"
#include "RootItem.generated.h"

class UCapsuleComponent;
class UNiagaraComponent;
class UBillboardComponent;
class UAIPerceptionStimuliSourceComponent;

/*
	플레이어와 상호작용 시 아이템을 확득하는 액터
*/

UCLASS()
class PORTFOLIO_API ARootItem : public AActor, public IInteraction
{
	GENERATED_BODY()
	
public:	
	ARootItem();

protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

	virtual void Notify(TObjectPtr<AActor> player) override;
	virtual void UnNotify(TObjectPtr<AActor> player) override;
	virtual void Interact(TObjectPtr<AActor> player) override;

	void SetItem(const FGameItem& gi);
	void SetResourceIndex(const int idx);

protected:
	// 충돌 처리용 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCapsuleComponent> mCapsuleComponent;
	// 파티클 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UNiagaraComponent> mNiagaraComponent;
	// 알림용 이미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UBillboardComponent> mNotifyBillboard;
	// 플레이어의 퍼셉션에 감지되게끔 하는 컴포넌트
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAIPerceptionStimuliSourceComponent> mPerceptionStimuliSourceComponent;
	
	// 상호작용 시 플레이어가 획득할 아이템
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	FGameItem Item;

	// 맵 상의 리소스 인덱스
	uint64 mResIdx;
};
