#pragma once

#include "CoreMinimal.h"
#include "../EnemyCharacter.h"
#include "../../Actor/Bullet.h"
#include "../../Component/Weapon.h"
#include "Shooter.generated.h"

class UAudioComponent;
class USoundCue;
class UWeapon;

/*
	플레이어에 총을 쏘는 몹 캐릭터
	- 정면으로 발사
	- 무기 컴포넌트 설정 가능
*/

UCLASS()
class PORTFOLIO_API AShooter : public AEnemyCharacter
{
	GENERATED_BODY()

public:
	AShooter();

protected:
	virtual void BeginPlay() override;

public:
	// 정면으로 Bullet을 발사
	UFUNCTION()
	void Shoot();

protected:
	// 발사음 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UAudioComponent> mAudioComponent;

	// Bullet 투사체 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TSubclassOf<ABullet> BulletClass;
	
	// 무기의 ItemInfo 상의 인덱스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	int WeaponIndex;

	// 무기 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UWeapon> mWeapon;
};
