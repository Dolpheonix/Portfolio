#include "Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshSocket.h"
#include "GameFramework/Pawn.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "../Base/LoadHelper.h"
#include "../Core/CustomGameInstance.h"

static_assert(static_cast<uint8>(EWeaponType::Count) == 2);

UWeapon::UWeapon(const FObjectInitializer& objInitializer) : Super(objInitializer),
															 mWeaponName(""), mType(EWeaponType::Count), mDamage(0.0f)
{
	mAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AttackAudio"));
	mAudioComponent->SetupAttachment(this);
	mAudioComponent->SetAutoActivate(false);
}

void UWeapon::Attack()
{
	switch (mType)
	{
	case EWeaponType::Fist:
		Punch();
		break;
	case EWeaponType::Rifle:
		Fire();
		break;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid EWeaponType"));
		break;
	}
}

void UWeapon::Punch()
{

}

void UWeapon::Fire()
{
	// 효과음 재생
	mAudioComponent->Play();

	// 총구 위치가 소켓으로 지정되어 있음
	const FVector muzzleLocation = GetSocketLocation(TEXT("Muzzle"));	
	
	FActorSpawnParameters spawnParam;
	spawnParam.Owner = GetOwner();
	spawnParam.Instigator = GetOwner()->GetInstigator();
	
	// 스폰된 총알 액터를 정면으로 발사
	TObjectPtr<ABullet> bullet = GetWorld()->SpawnActor<ABullet>(mBulletClass, muzzleLocation, FRotator(), spawnParam);
	if (bullet)
	{
		FVector launchDirection = Cast<APawn>(GetOwner())->GetControlRotation().Vector();
		bullet->Launch(launchDirection, mDamage);
	}
}

FString UWeapon::GetWeaponName() const
{
	return mWeaponName;
}

EWeaponType UWeapon::GetType() const
{
	return mType;
}

float UWeapon::GetDamage() const
{
	return mDamage;
}

void UWeapon::LoadFromItemInfo(int itemIndex)
{
	check(itemIndex >= 0);

	TObjectPtr<UCustomGameInstance> gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(gi);

	// ItemInfo 상의 아이템 정보를 통해 데미지, 효과음 등을 설정
	const ItemInfo& itemInfo = gi->GetItemInfo(itemIndex);
	check(itemInfo.Type == EItemType::Weapon);
	SetStaticMesh(LoadHelper::LoadObjectFromPath<UStaticMesh>(FName(*itemInfo.ItemMesh)));
	mWeaponName	= itemInfo.Name;
	mType		= itemInfo.SubInfo_Weapon.Type;
	mDamage		= itemInfo.SubInfo_Weapon.Damage;
	mAudioComponent->SetSound(LoadHelper::LoadObjectFromPath<USoundCue>(FName(*itemInfo.SubInfo_Weapon.Sound)));
	mBulletClass = LoadHelper::LoadObjectFromPath<UBlueprintGeneratedClass>(FName(*(itemInfo.SubInfo_Weapon.BulletBP)));
}