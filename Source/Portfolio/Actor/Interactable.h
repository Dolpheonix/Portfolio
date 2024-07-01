#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Common/Interface/Interaction.h"
#include "Interactable.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInteractionEvent);

class UStaticMeshComponent;
class UBillboardComponent;
class UAIPerceptionStimuliSourceComponent;

/*
	플레이어와 상호작용 시 커스텀 이벤트를 실행하는 액터
*/

UCLASS()
class PORTFOLIO_API AInteractable : public AActor, public IInteraction
{
	GENERATED_BODY()
	
public:
	AInteractable();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// 상호작용 범위에 들어왔음을 알림
	virtual void Notify(TObjectPtr<AActor> player) override;
	// 상호작용 범위에서 벗어났음을 알림
	virtual void UnNotify(TObjectPtr<AActor> player) override;
	// 플레이어와 상호작용 시 호출
	virtual void Interact(TObjectPtr<AActor> player) override;

public:
	// 상호작용 시에 실행될 이벤트
	UPROPERTY(BlueprintAssignable, Category="Event")
	FInteractionEvent mInteractionEvent;

protected:
	// 액터 메시
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> mStaticMeshComponent;
	// 알림용 이미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UBillboardComponent> mNotifyBillboard;
	// 플레이어의 퍼셉션에 감지되게끔 하는 컴포넌트
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAIPerceptionStimuliSourceComponent> mPerceptionStimuliSourceComponent;

};
