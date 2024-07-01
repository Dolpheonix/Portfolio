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
	몹 캐릭터
	- 데미지 처리
	- 퍼셉션 감지 상태에 따라 느낌표/물음표 이미지 띄움
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

	virtual FGenericTeamId GetGenericTeamId() { return FGenericTeamId(2); };

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void OnHurt() override;
	virtual void OnDead() override;

	// State 변화에 따라 Billboard Component 업데이트
	void UpdateState(EEnemyState s);

	void SetEnemyInfoIndex(const int idx);
	void SetResourceIndex(const int idx);

protected:
	// HP 바 위젯 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TSubclassOf<UHpBar> HpBarClass;
	// Caution 상태에 보여줄 물음표 위젯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UBillboardComponent> mQuestionMarkComponent;
	// Detected 상태에 보여줄 느낌표 위젯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UBillboardComponent> mExclamationMarkComponent;
	// HP 바 위젯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UWidgetComponent> mHpBarWidget;
public:
	// 감지 반경
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perception")
	float SightRadius;
	// 감지 해제 반경
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	float LoseSightRadius;
	// 감지 시야각
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	float SightAngle;
	
	// 몹의 EnemyInfo 상 인덱스 (몹 종류)
	uint64 mEnemyInfoIndex;
	// 맵 상의 리소스 인덱스
	uint64 mResIdx;
};
