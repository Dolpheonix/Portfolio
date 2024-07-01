#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ReplicatedController.generated.h"

class UBehaviorTree;
class UBlackboardData;

UCLASS()
class PORTFOLIO_API AReplicatedController : public AAIController
{
	GENERATED_BODY()
public:
	AReplicatedController(const FObjectInitializer& objectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BehaviorTree")
	TObjectPtr<UBehaviorTree> mBehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BehaviorTree")
	TObjectPtr<UBlackboardData> mBlackboardData;
};
