#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "../Common/Item.h"
#include "../Actor/Bullet.h"
#include "Weapon.generated.h"

class UAudioComponent;

/*
	UWeapon : 무기 컴포넌트. 타입별 공격 메커니즘 보유
*/
UCLASS()
class PORTFOLIO_API UWeapon : public UStaticMeshComponent
{
	GENERATED_BODY()
	
	UWeapon(const FObjectInitializer& objInitializer);
public:
	void Attack();
	void Punch();
	void Fire();

	FString GetWeaponName() const;
	EWeaponType GetType() const;
	float GetDamage() const;

	void LoadFromItemInfo(int itemIndex);
protected:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAudioComponent> mAudioComponent;

	TSubclassOf<ABullet> mBulletClass;
	
	FString mWeaponName;
	EWeaponType mType;
	float mDamage;
};
