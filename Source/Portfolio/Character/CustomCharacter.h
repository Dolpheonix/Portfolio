#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CustomCharacter.generated.h"

// ACustomCharacter : 모든 캐릭터 클래스의 기반 클래스.
// 주요 기능 : 이름, hp, 경직 시간

UCLASS()
class PORTFOLIO_API ACustomCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACustomCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// 데미지를 입었을 때 호출되는 함수
	UFUNCTION()
	virtual void OnHurt() {};

	// HP가 0이 되었을 때 호출되는 함수
	UFUNCTION()
	virtual void OnDead() {};

	// 현재 체력 반환
	int GetCurrHp();
	// 최대 체력 반환
	int GetMaxHp();

public:
	// 게임에 표시되는 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Name")
	FString mDisplayName;

	// 최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status")
	int mMaxHp;

	// 현재 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status")
	float mHp;

	// 사망 여부
	UPROPERTY(BlueprintReadWrite, Category="Status")
	bool bDead;

	// 움직임 제한 여부
	UPROPERTY(BlueprintReadWrite)
	bool bForced;
};
