#include "ReplicatedCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimNode_StateMachine.h"
#include "../Base/LoadHelper.h"
#include "../Common/Item.h"
#include "../Component/Weapon.h"
#include "../Core/CustomGameInstance.h"

static_assert(static_cast<uint8>(EReplicatedActionMode::Count) == 2);
static_assert(static_cast<uint8>(EReplicatedMovementMode::Count) == 5);

AReplicatedCharacter::AReplicatedCharacter() : mActionMode(EReplicatedActionMode::Idle), mMovementMode(EReplicatedMovementMode::Idle)
											  ,bRunning(false), bJumping(false), bEquipping(false), mLoco_Action_Blend(0)
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Replicated"));
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetSkeletalMesh(LoadHelper::C_LoadObjectFromPath<USkeletalMesh>(TEXT("/Game/Character/Mannequins/Meshes/SKM_Manny.SKM_Manny")));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetAnimInstanceClass(LoadHelper::C_LoadObjectFromPath<UBlueprintGeneratedClass>(TEXT("/Game/Character/Mannequins/Animations/ABP_Mannequin.ABP_Mannequin_C")));
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetupAttachment(RootComponent);

	mWeapon = CreateDefaultSubobject<UWeapon>(TEXT("Weapon"));
}

void AReplicatedCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AReplicatedCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AReplicatedCharacter::UpdateLocationAndRotation(const FVector& loc, const FRotator& rot)
{
	SetActorLocation(loc, true);
	SetActorRotation(rot);
}

void AReplicatedCharacter::ChangeWeapon(int infoIndex)
{
	check(mWeapon);

	if (infoIndex < 0)
	{
		mWeapon->SetVisibility(false);
	}
	else
	{
		mWeapon->LoadFromItemInfo(infoIndex);
		mWeapon->SetVisibility(true);

		mWeaponType = mWeapon->GetType();

		SetEquipping(true);
	}
}

void AReplicatedCharacter::SetRunning(bool newval)
{
	bRunning = newval;
}

void AReplicatedCharacter::SetJumping(bool newval)
{
	bJumping = newval;
}

void AReplicatedCharacter::SetEquipping(bool newval)
{
	bEquipping = newval;

	FString socketStr;
	if (bEquipping == true)
	{
		socketStr = "Equipped_" + mWeapon->GetWeaponName();
		mWeaponType = mWeapon->GetType();
	}
	else
	{
		socketStr += "UnEquipped_" + mWeapon->GetWeaponName();
		mWeaponType = EWeaponType::Fist;
	}

	mWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName(socketStr));
}

bool AReplicatedCharacter::GetRunning() const
{
	return bRunning;
}

bool AReplicatedCharacter::GetJumping() const
{
	return bJumping;
}

bool AReplicatedCharacter::GetEquipping() const
{
	return bEquipping;
}

void AReplicatedCharacter::SetResourceIndex(const int idx)
{
	mResIdx = idx;
}
