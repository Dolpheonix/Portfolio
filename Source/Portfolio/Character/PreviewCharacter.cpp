#include "PreviewCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "../Core/CustomGameInstance.h"
#include "../Character/PlayerCharacter.h"
#include "../Base/LoadHelper.h"

APreviewCharacter::APreviewCharacter() : mCurrentWeaponType(EWeaponType::Fist)
{
	GetMesh()->SetSkeletalMesh(LoadHelper::C_LoadObjectFromPath<USkeletalMesh>(TEXT("/Game/Character/Mannequins/Meshes/SKM_Manny.SKM_Manny")));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetupAttachment(RootComponent);
	
	mPreviewCamera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Camera"));
	mPreviewCamera->SetRelativeLocation(FVector(200.f, 0.f, 70.f));
	mPreviewCamera->SetRelativeRotation(FRotator(-10.f, -180.f, -0.f));
	mPreviewCamera->TextureTarget = LoadHelper::C_LoadObjectFromPath<UTextureRenderTarget2D>(TEXT("/Game/Texture/WidgetImage/PreviewImage.PreviewImage"));
	mPreviewCamera->CaptureSource = ESceneCaptureSource::SCS_SceneColorSceneDepth;
	mPreviewCamera->SetupAttachment(RootComponent);

	mWeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
	mWeaponMeshComponent->SetupAttachment(RootComponent);
}

void APreviewCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APreviewCharacter::SyncWithPlayer(TObjectPtr<APlayerCharacter> player)
{
	check(player);

	//TODO : Cloth

	// 무기 장착 및 타입에 맞는 애니메이션 재생
	const int& weaponInventoryIndex = player->GetCurrentWeapon();
	if (weaponInventoryIndex >= 0)
	{
		const int& weaponInfoIndex = player->GetInventory().GetTypeInventory(EItemType::Weapon).ItemList[weaponInventoryIndex].InfoIndex;
		SetEquipment(EItemType::Weapon, weaponInfoIndex);
	}

	SetAnimation(player->GetCurrentWeaponType());
}

void APreviewCharacter::SetEquipment(EItemType type, int index)
{
	TObjectPtr<UCustomGameInstance> gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(gi);

	const ItemInfo& itemInfo = gi->GetItemInfo(index);
	check(itemInfo.Type == type);

	mWeaponMeshComponent->SetStaticMesh(LoadHelper::LoadObjectFromPath<UStaticMesh>(FName(*itemInfo.ItemMesh)));
	const FString socketStr = "Equipped_" + itemInfo.Name;
	mWeaponMeshComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName(socketStr));
}

void APreviewCharacter::SetAnimation(EWeaponType type)
{
	switch (type)
	{
	case EWeaponType::Fist:
		GetMesh()->SetAnimation(LoadHelper::LoadObjectFromPath<UAnimSequence>(TEXT("/Game/Character/Mannequins/Animations/Manny/MM_Idle.MM_Idle")));
		break;
	case EWeaponType::Rifle:
		GetMesh()->SetAnimation(LoadHelper::LoadObjectFromPath<UAnimSequence>(TEXT("/Game/Character/Mannequins/Animations/Rifle/AS_ShootingRack.AS_ShootingRack")));
		break;
	default:
		break;
	}
}