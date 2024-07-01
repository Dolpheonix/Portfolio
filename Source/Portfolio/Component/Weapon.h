#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "../Common/Item.h"
#include "../Actor/Bullet.h"
#include "Weapon.generated.h"

class UAudioComponent;

/*
	캐릭터에 부착 가능한 무기 컴포넌트
	- 공격 기능
*/
UCLASS()
class PORTFOLIO_API UWeapon : public UStaticMeshComponent
{
	GENERATED_BODY()
	
	UWeapon(const FObjectInitializer& objInitializer);
public:
	// 공격
	void Attack();
private:
	// Fist 타입 무기의 공격
	void Punch();
	// Rifle 타입 무기의 공격
	void Fire();

public:
	// Getter
	FString GetWeaponName() const;
	EWeaponType GetType() const;
	float GetDamage() const;

	// Json 오브젝트로부터 게임 데이터 로드
	void LoadFromItemInfo(int itemIndex);
protected:
	// 효과음 컴포넌트
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAudioComponent> mAudioComponent;
	// Rifle 타입 무기에 한해, 발사할 Bullet 클래스
	TSubclassOf<ABullet> mBulletClass;
	// 무기 이름
	FString mWeaponName;
	// 무기 타입
	EWeaponType mType;
	// 무기 데미지
	float mDamage;
};
