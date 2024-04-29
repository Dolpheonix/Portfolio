#include "CustomCharacter.h"

ACustomCharacter::ACustomCharacter() : mDisplayName(""), mMaxHp(100.f), mHp(100.0f), bDead(false), bForced(false)
{
}

void ACustomCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ACustomCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ACustomCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float damage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	mHp -= damage;

	if (mHp <= 0)
	{
		// 체력이 0이 되면 사망 처리
		bDead = true;
		OnDead();
	}
	else
	{
		OnHurt();
	}	
	return damage;
}

int ACustomCharacter::GetCurrHp()
{
	return mHp;
}

int ACustomCharacter::GetMaxHp()
{
	return mMaxHp;
}


