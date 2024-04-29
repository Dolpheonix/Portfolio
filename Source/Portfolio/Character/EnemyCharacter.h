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
	AEnemyCharacter : 플레이어를 공격하는 캐릭터(몹)의 기반 클래스
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

	// 플레이어를 적으로 인식함 (플레이어의 Team Id : 0)
	virtual FGenericTeamId GetGenericTeamId() { return FGenericTeamId(2); };

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void OnHurt() override;
	virtual void OnDead() override;

	// 컨트롤러에 의해 관리되는 Enemy State 변경 시 캐릭터에 적용함.
	void UpdateState(EEnemyState s);

protected:
	// 캐릭터 위에 표시되는 체력 바 UI의 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TSubclassOf<UHpBar> HpBarClass;
	// 물음표 마크 (Caution 상태일 때 활성화)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UBillboardComponent> mQuestionMarkComponent;
	// 느낌표 마크 (Detected 상태일 때 활성화)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UBillboardComponent> mExclamationMarkComponent;
	// Hp 바 (플레이어의 감지 범위에 들어오면 활성화)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UWidgetComponent> mHpBarWidget;
public:
	// 컨트롤러에 적용할 Perception Component의 인자들
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perception")
	float SightRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	float LoseSightRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	float SightAngle;
};
