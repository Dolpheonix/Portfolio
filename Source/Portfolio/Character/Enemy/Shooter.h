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
	AShooter : Enemy Character의 파생 캐릭터.
		- Rifle 무기 장착 및 발사 함수 구현
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
	UFUNCTION()
	void Shoot();

protected:
	// 발사음 재생용 오디오
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UAudioComponent> mAudioComponent;

	// 스폰할 총알 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TSubclassOf<ABullet> BulletClass;
	
	// Item Info로부터 불러올 무기의 인덱스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	int WeaponIndex;

	// 무기 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UWeapon> mWeapon;
};
