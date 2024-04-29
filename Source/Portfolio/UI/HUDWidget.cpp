#include "HUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "../Core/CustomGameInstance.h"
#include "../Character/PlayerCharacter.h"
#include "../Base/LoadHelper.h"

UHUDWidget::UHUDWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer), bBound(false)
{
}

void UHUDWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsDesignTime() == false)
	{
		mHpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("Pgb_Hp")));
		mCurrentWeaponImage = Cast<UImage>(GetWidgetFromName(TEXT("Img_CurrWeapon")));
		mNextWeaponImage = Cast<UImage>(GetWidgetFromName(TEXT("Img_NextWeapon")));
	}
}

void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	check(mHpProgressBar);
	check(mCurrentWeaponImage);
	check(mNextWeaponImage);
}

void UHUDWidget::Open(TObjectPtr<APlayerCharacter> player)
{
	check(player);

	mOwnerPlayer = player;

	UpdateHpBar();
	UpdateWeaponImage();
}

void UHUDWidget::UpdateHpBar()
{
	float percent = static_cast<float>(mOwnerPlayer->GetMaxHp()) / mOwnerPlayer->GetCurrHp();
	mHpProgressBar->SetPercent(percent);
}

void UHUDWidget::UpdateWeaponImage()
{	
	const int currIndex = mOwnerPlayer->GetCurrentWeapon();
	if (currIndex < 0)
	{
		// 보유중인 무기가 없다
		mCurrentWeaponImage->Brush.SetResourceObject(LoadHelper::LoadObjectFromPath<UTexture2D>(TEXT("/Game/Texture/WidgetImage/Thumbnail/Empty_Normal.Empty_Normal")));
		mNextWeaponImage->Brush.SetResourceObject(LoadHelper::LoadObjectFromPath<UTexture2D>(TEXT("/Game/Texture/WidgetImage/Thumbnail/Empty_Normal.Empty_Normal")));

		return;
	}
	
	const int nextIndex = (currIndex + 1) % mOwnerPlayer->GetInventory().GetTypeInventory(EItemType::Weapon).ItemList.Num();

	UCustomGameInstance* gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(gi);

	const ItemInfo& currInfo = gi->GetItemInfo(mOwnerPlayer->GetInventory().GetTypeInventory(EItemType::Weapon).ItemList[currIndex].InfoIndex);
	const ItemInfo& nextInfo = (currIndex == nextIndex) ? currInfo : gi->GetItemInfo(mOwnerPlayer->GetInventory().GetTypeInventory(EItemType::Weapon).ItemList[nextIndex].InfoIndex);

	mCurrentWeaponImage->Brush.SetResourceObject(currInfo.Thumbnail[static_cast<uint8>(EThumbnailType::Normal)]);
	mNextWeaponImage->Brush.SetResourceObject(nextInfo.Thumbnail[static_cast<uint8>(EThumbnailType::Normal)]);
}
