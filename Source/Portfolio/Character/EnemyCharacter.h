#pragma once

#include "CoreMinimal.h"
#include "CustomCharacter.h"
#include "GenericTeamAgentInterface.h"
#include "Components/ProgressBar.h"
#include "Enemy/EnemyController.h"
#include "../UI/HpBar.h"
#include "EnemyCharacter.generated.h"

class UBillboardComponent;
class UWidgetComponent;

/*
	AEnemyCharacter : �÷��̾ �����ϴ� ĳ����(��)�� ��� Ŭ����
*/

UCLASS()
class PORTFOLIO_API AEnemyCharacter : public ACustomCharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// �÷��̾ ������ �ν��� (�÷��̾��� Team Id : 0)
	virtual FGenericTeamId GetGenericTeamId() { return FGenericTeamId(2); };

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void OnHurt() override;
	virtual void OnDead() override;

	// ��Ʈ�ѷ��� ���� �����Ǵ� Enemy State ���� �� ĳ���Ϳ� ������.
	void UpdateState(EEnemyState s);

protected:
	// ĳ���� ���� ǥ�õǴ� ü�� �� UI�� Ŭ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TSubclassOf<UHpBar> HpBarClass;
	// ����ǥ ��ũ (Caution ������ �� Ȱ��ȭ)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UBillboardComponent> mQuestionMarkComponent;
	// ����ǥ ��ũ (Detected ������ �� Ȱ��ȭ)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UBillboardComponent> mExclamationMarkComponent;
	// Hp �� (�÷��̾��� ���� ������ ������ Ȱ��ȭ)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UWidgetComponent> mHpBarWidget;
public:
	// ��Ʈ�ѷ��� ������ Perception Component�� ���ڵ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perception")
	float SightRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	float LoseSightRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	float SightAngle;
};
