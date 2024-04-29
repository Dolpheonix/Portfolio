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
	AInteractable : 플레이어와 상호작용 가능한 액터
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

	virtual void Notify(TObjectPtr<AActor> player) override;
	virtual void UnNotify(TObjectPtr<AActor> player) override;
	virtual void Interact(TObjectPtr<AActor> player) override;

public:
	// 상호 작용 시 실행할 이벤트 (하위 클래스나 레벨 액터, 레벨 블루프린트에서 바인드 할 수 있음)
	UPROPERTY(BlueprintAssignable, Category="Event")
	FInteractionEvent mInteractionEvent;

protected:
	// 외형 메시
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> mStaticMeshComponent;
	// 플레이어의 감지 범위에 들어왔을 때 표시할 알림용 이미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UBillboardComponent> mNotifyBillboard;
	// 플레이어의 퍼셉션에 감지되기 위해 필요한 컴포넌트
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAIPerceptionStimuliSourceComponent> mPerceptionStimuliSourceComponent;

};
