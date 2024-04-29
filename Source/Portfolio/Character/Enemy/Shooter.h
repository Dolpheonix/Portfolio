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
	AShooter : Enemy Character�� �Ļ� ĳ����.
		- Rifle ���� ���� �� �߻� �Լ� ����
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
	// �߻��� ����� �����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UAudioComponent> mAudioComponent;

	// ������ �Ѿ� Ŭ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TSubclassOf<ABullet> BulletClass;
	
	// Item Info�κ��� �ҷ��� ������ �ε���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	int WeaponIndex;

	// ���� ������Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UWeapon> mWeapon;
};
