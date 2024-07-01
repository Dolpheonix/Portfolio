#include "NpcCharacter.h"
#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Controller/NpcController.h"
#include "Components/BillboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Core/CustomGameInstance.h"
#include "../Core/CustomController.h"
#include "../Base/LoadHelper.h"

static_assert(static_cast<uint8>(EItemType::Count) == 3);

ANpcCharacter::ANpcCharacter() :	Super(),
									mNpcIndex(-1), bInteracting(false),
									mController(nullptr),
									mShopItems(), mRewardItems(), mDialogue()
{
	PrimaryActorTick.bCanEverTick = true;

	mSelfCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("SelfCam"));
	mSelfCamera->SetupAttachment(RootComponent);

	mNotifyBillboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("NotifyBillboard"));
	mNotifyBillboard->SetupAttachment(RootComponent);
	const TObjectPtr<UTexture2D> notifyImage = LoadHelper::C_LoadObjectFromPath<UTexture2D>(TEXT("/Game/Texture/WidgetImage/NotifyInteraction.NotifyInteraction"));
	mNotifyBillboard->SetSprite(notifyImage);

	APawn::AIControllerClass = ANpcController::StaticClass();
}

void ANpcCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(mNpcIndex >= 0);

	mController = Cast<ANpcController>(APawn::GetController());
	check(mController);

	mNotifyBillboard->SetHiddenInGame(false);
	mNotifyBillboard->SetVisibility(false);
}

void ANpcCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANpcCharacter::Notify(TObjectPtr<AActor> player)
{
	mNotifyBillboard->SetVisibility(true);
}

void ANpcCharacter::UnNotify(TObjectPtr<AActor> player)
{
	mNotifyBillboard->SetVisibility(false);
}

void ANpcCharacter::Interact(TObjectPtr<AActor> player)
{
	mController->Interact(player);

	TObjectPtr<APlayerCharacter> pc = Cast<APlayerCharacter>(player);
	check(pc);
	TObjectPtr<ACustomController> controller = Cast<ACustomController>(UGameplayStatics::GetPlayerController(this, 0));
	check(controller);
	// 다이얼로그 UI를 엶
	controller->OpenDialogue(pc, this);

	bInteracting = true;
}

void ANpcCharacter::UnInteract(TObjectPtr<AActor> player)
{
	bInteracting = false;
	// 다이얼로그 UI를 닫음
	mController->UnInteract();

	TObjectPtr<APlayerCharacter> pc = Cast<APlayerCharacter>(player);
	check(pc);
	// 플레이어도 상호작용 종료
	pc->UnInteract();
}

TObjectPtr<UCameraComponent> ANpcCharacter::GetSelfCamera()
{
	return mSelfCamera;
}

TObjectPtr<ANpcController> ANpcCharacter::GetNpcController()
{
	return mController;
}

FInventory& ANpcCharacter::GetShopItems()
{
	return mShopItems;
}

const FInventory& ANpcCharacter::GetShopItems() const
{
	return mShopItems;
}

TArray<FGameItem>& ANpcCharacter::GetRewardItems()
{
	return mRewardItems;
}

FDialogueList& ANpcCharacter::GetDialogue()
{
	return mDialogue;
}

int ANpcCharacter::GetIndex() const
{
	return mNpcIndex;
}

bool ANpcCharacter::IsInteracting() const
{
	return bInteracting;
}

bool ANpcCharacter::LoadFromJson(const TSharedPtr<FJsonObject>& obj)
{
	check(obj);
	
	// 다이얼로그
	const TArray<TSharedPtr<FJsonValue>>& dialogueValues = obj->GetArrayField("Dialogues");
	mDialogue.Dialogues.SetNum(dialogueValues.Num());
	for (int i = 0; i < dialogueValues.Num(); ++i)
	{
		mDialogue.Dialogues[i].LoadFromJson(dialogueValues[i]->AsObject());
	}

	// 상점 아이템
	const TSharedPtr<FJsonObject>& shopItemObj = obj->GetObjectField("ShopItems");
	{
		// Cloth
		const TArray<TSharedPtr<FJsonValue>>& clothItemValues = shopItemObj->GetArrayField("Cloth");

		mShopItems.TypeInventoryList[static_cast<uint8>(EItemType::Cloth)].ItemList.SetNum(clothItemValues.Num());
		for (int i = 0; i < clothItemValues.Num(); ++i)
		{
			mShopItems.TypeInventoryList[static_cast<uint8>(EItemType::Cloth)].ItemList[i].LoadFromJson(clothItemValues[i]->AsObject());
		}

		// Weapon
		const TArray<TSharedPtr<FJsonValue>>& weaponItemValues = shopItemObj->GetArrayField("Weapon");

		mShopItems.TypeInventoryList[static_cast<uint8>(EItemType::Weapon)].ItemList.SetNum(weaponItemValues.Num());
		for (int i = 0; i < weaponItemValues.Num(); ++i)
		{
			mShopItems.TypeInventoryList[static_cast<uint8>(EItemType::Weapon)].ItemList[i].LoadFromJson(weaponItemValues[i]->AsObject());
		}

		// Item
		const TArray<TSharedPtr<FJsonValue>>& etcItemValues = shopItemObj->GetArrayField("Item");

		mShopItems.TypeInventoryList[static_cast<uint8>(EItemType::Item)].ItemList.SetNum(etcItemValues.Num());
		for (int i = 0; i < etcItemValues.Num(); ++i)
		{
			mShopItems.TypeInventoryList[static_cast<uint8>(EItemType::Item)].ItemList[i].LoadFromJson(etcItemValues[i]->AsObject());
		}
	}

	// 보상 아이템
	const TArray<TSharedPtr<FJsonValue>>& rewardItemValues = obj->GetArrayField("RewardItems");
	mRewardItems.SetNum(rewardItemValues.Num());
	for (int i = 0; i < rewardItemValues.Num(); ++i)
	{
		mRewardItems[i].LoadFromJson(rewardItemValues[i]->AsObject());
	}

	return true;
}

