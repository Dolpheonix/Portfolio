#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NpcController.generated.h"

class UBehaviorTree;
class UBlackboardData;

/*
	Npc 캐릭터용 컨트롤러
	- Behavior Tree, Blackboard 설정
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
	// 플레이어와 상호작용 시, 블랙보드 값 변경
	void Interact(TObjectPtr<AActor> player);
	// 상호작용 해제 시, 블랙보드 값 변경
	void UnInteract();
protected:
	// 비헤이비어 트리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BehaviorTree")
	TObjectPtr<UBehaviorTree> mBehaviorTree;
	// 블랙보드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BehaviorTree")
	TObjectPtr<UBlackboardData> mBlackboardData;
};
