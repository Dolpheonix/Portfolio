#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NpcController.generated.h"

class UBehaviorTree;
class UBlackboardData;

/*
	ANpcController : Npc ĳ���Ϳ� ���Ǵ� ��Ʈ�ѷ�
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
	// ĳ���Ϳ� �÷��̾ ��ȣ�ۿ� ��, ������ �� ������Ʈ
	void Interact(TObjectPtr<AActor> player);
	// ��ȣ�ۿ��� ������ ��, ������ �� ������Ʈ
	void UnInteract();
protected:
	// ����� �����̺�� Ʈ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BehaviorTree")
	TObjectPtr<UBehaviorTree> mBehaviorTree;
	// ����� ������ ������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BehaviorTree")
	TObjectPtr<UBlackboardData> mBlackboardData;
};
