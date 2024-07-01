#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CustomCharacter.generated.h"

/*
	게임 내에서 사용되면 Character 클래스
	- 체력
	- 데미지 처리, 사망 처리
*/

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
	// 데미지 적용
	UFUNCTION()
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	// 데미지를 입었을 때 호출
	UFUNCTION()
	virtual void OnHurt() {};
	// 사망 시 호출
	UFUNCTION()
	virtual void OnDead() {};

	int GetCurrHp();
	int GetMaxHp();

public:
	// 캐릭터 표시명
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Name")
	FString mDisplayName;

	// 최대 HP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status")
	int mMaxHp;
	// 현재 HP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status")
	float mHp;

	// 사망했는가?
	UPROPERTY(BlueprintReadWrite, Category="Status")
	bool bDead;
	// 움직임에 영향을 받은 상태인가?
	UPROPERTY(BlueprintReadWrite)
	bool bForced;
};
