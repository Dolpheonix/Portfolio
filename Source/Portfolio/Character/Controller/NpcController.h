#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NpcController.generated.h"

class UBehaviorTree;
class UBlackboardData;

/*
	ANpcController : Npc 캐릭터에 사용되는 컨트롤러
*/

UCLASS()
class PORTFOLIO_API ANpcController : public AAIController
{
	GENERATED_BODY()
public:
	ANpcController(const FObjectInitializer& objectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

public:
	// 캐릭터와 플레이어가 상호작용 시, 블랙보드 값 업데이트
	void Interact(TObjectPtr<AActor> player);
	// 상호작용이 끝났을 때, 블랙보드 값 업데이트
	void UnInteract();
protected:
	// 사용할 비헤이비어 트리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BehaviorTree")
	TObjectPtr<UBehaviorTree> mBehaviorTree;
	// 사용할 블랙보드 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BehaviorTree")
	TObjectPtr<UBlackboardData> mBlackboardData;
};
