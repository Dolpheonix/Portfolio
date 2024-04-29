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
	ARootItem : 플레이어가 상호작용 시 아이템을 획득할 수 있는 액터
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

protected:
	// 파티클 외형을 감싸는 블록용 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCapsuleComponent> mCapsuleComponent;
	// 파티클 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UNiagaraComponent> mNiagaraComponent;
	// 플레이어의 상호작용 감지 범위에 들어왔을 때 표시할 이미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UBillboardComponent> mNotifyBillboard;
	// 플레이어의 퍼셉션에 감지되기 위해 필요한 컴포넌트
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAIPerceptionStimuliSourceComponent> mPerceptionStimuliSourceComponent;
	
	// 상호 작용시 플레이어가 획득할 아이템
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	FGameItem Item;
};
