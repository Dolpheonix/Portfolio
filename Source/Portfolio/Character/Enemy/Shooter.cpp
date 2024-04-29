#include "Shooter.h"
#include "Components/AudioComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/ProgressBar.h"
#include "Components/WidgetComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "../../Base/LoadHelper.h"
#include "../../Core/CustomGameInstance.h"
#include "Shooter/ShooterController.h"

AShooter::AShooter() : WeaponIndex(0)
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetSkeletalMesh(LoadHelper::C_LoadObjectFromPath<USkeletalMesh>(TEXT("/Game/Character/Mannequins/Meshes/SKM_Manny.SKM_Manny")));
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	mQuestionMarkComponent->SetRelativeLocation(FVector(0.f, 50.f, 120.f));
	mQuestionMarkComponent->SetRelativeScale3D(FVector(0.07f, 0.07f, 0.07f));

	mExclamationMarkComponent->SetRelativeLocation(FVector(0.f, 50.f, 120.f));
	mExclamationMarkComponent->SetRelativeScale3D(FVector(0.07f, 0.07f, 0.07f));

	mAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
	mAudioComponent->SetSound(LoadHelper::C_LoadObjectFromPath<USoundCue>(TEXT("/Game/Sound/Cue/Rifle_Fire.Rifle_Fire")));

	mWeapon = CreateDefaultSubobject<UWeapon>(TEXT("Weapon"));

	HpBarClass = LoadHelper::C_LoadObjectFromPath<UBlueprintGeneratedClass>(TEXT("/Game/UI/EnemyHpBar_BP.EnemyHpBar_BP_C"));

	AIControllerClass = AShooterController::StaticClass();
}

void AShooter::BeginPlay()
{
	Super::BeginPlay();

	// 무기 타입이 Rifle이어야만 함
	mWeapon->LoadFromItemInfo(WeaponIndex);
	check(mWeapon->GetType() == EWeaponType::Rifle);

	// 장착 상태로 무기 위치 조정
	const FString socketStr = "Equipped_" + mWeapon->GetWeaponName();
	mWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName(socketStr));

	mHpBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
}

void AShooter::Shoot()
{
	mWeapon->Attack();
}
