#include "ShopWidget.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"
#include "../Base/LoadHelper.h"
#include "../Core/CustomGameInstance.h"
#include "../Core/CustomController.h"
#include "../Character/PlayerCharacter.h"
#include "../Character/NpcCharacter.h"
#include "IndexedButton.h"
#include "ConfirmPopup.h"

static_assert(NUM_INVENTORY_ROW == 4);
static_assert(NUM_INVENTORY_COL == 4);
static_assert(MAX_ITEM_PER_TYPE == 16);
static_assert(static_cast<uint8>(EItemType::Count) == 3);

UShopWidget::UShopWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer), mSelectedIndex(-1), bBuying(false), bBound(false)
{
	mTabButtons_Character.SetNum(static_cast<uint8>(EItemType::Count));
	mTabButtons_Npc.SetNum(static_cast<uint8>(EItemType::Count));

	mItemSlotButtons_Character.SetNum(MAX_ITEM_PER_TYPE);
	mItemSlotButtons_Npc.SetNum(MAX_ITEM_PER_TYPE);

	mItemNumbers_Character.SetNum(MAX_ITEM_PER_TYPE);
	mItemNumbers_Npc.SetNum(MAX_ITEM_PER_TYPE);

	mNumpadPopupClass = LoadHelper::C_LoadObjectFromPath<UBlueprintGeneratedClass>(TEXT("/Game/UI/NumpadPopup_BP.NumpadPopup_BP_C"));
}

void UShopWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsDesignTime() == false)
	{
		mTabButtons_Character[static_cast<uint8>(EItemType::Cloth)] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemTab_Ch_Cloth")));
		mTabButtons_Character[static_cast<uint8>(EItemType::Weapon)] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemTab_Ch_Weapon")));
		mTabButtons_Character[static_cast<uint8>(EItemType::Item)] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemTab_Ch_Item")));

		mTabButtons_Npc[static_cast<uint8>(EItemType::Cloth)] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemTab_Npc_Cloth")));
		mTabButtons_Npc[static_cast<uint8>(EItemType::Weapon)] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemTab_Npc_Weapon")));
		mTabButtons_Npc[static_cast<uint8>(EItemType::Item)] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemTab_Npc_Item")));

		for (int i = 0; i < static_cast<uint8>(EItemType::Count); ++i)
		{
			mTabButtons_Character[i]->mIndex = i;
			mTabButtons_Npc[i]->mIndex = i;
		}

		mItemSlotButtons_Character[0] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Ch_00")));
		mItemSlotButtons_Character[1] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Ch_01")));
		mItemSlotButtons_Character[2] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Ch_02")));
		mItemSlotButtons_Character[3] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Ch_03")));
		mItemSlotButtons_Character[4] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Ch_10")));
		mItemSlotButtons_Character[5] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Ch_11")));
		mItemSlotButtons_Character[6] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Ch_12")));
		mItemSlotButtons_Character[7] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Ch_13")));
		mItemSlotButtons_Character[8] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Ch_20")));
		mItemSlotButtons_Character[9] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Ch_21")));
		mItemSlotButtons_Character[10] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Ch_22")));
		mItemSlotButtons_Character[11] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Ch_23")));
		mItemSlotButtons_Character[12] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Ch_30")));
		mItemSlotButtons_Character[13] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Ch_31")));
		mItemSlotButtons_Character[14] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Ch_32")));
		mItemSlotButtons_Character[15] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Ch_33")));

		mItemSlotButtons_Npc[0] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Npc_00")));
		mItemSlotButtons_Npc[1] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Npc_01")));
		mItemSlotButtons_Npc[2] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Npc_02")));
		mItemSlotButtons_Npc[3] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Npc_03")));
		mItemSlotButtons_Npc[4] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Npc_10")));
		mItemSlotButtons_Npc[5] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Npc_11")));
		mItemSlotButtons_Npc[6] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Npc_12")));
		mItemSlotButtons_Npc[7] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Npc_13")));
		mItemSlotButtons_Npc[8] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Npc_20")));
		mItemSlotButtons_Npc[9] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Npc_21")));
		mItemSlotButtons_Npc[10] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Npc_22")));
		mItemSlotButtons_Npc[11] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Npc_23")));
		mItemSlotButtons_Npc[12] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Npc_30")));
		mItemSlotButtons_Npc[13] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Npc_31")));
		mItemSlotButtons_Npc[14] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Npc_32")));
		mItemSlotButtons_Npc[15] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_Npc_33")));

		mItemNumbers_Character[0] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Ch_00")));
		mItemNumbers_Character[1] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Ch_01")));
		mItemNumbers_Character[2] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Ch_02")));
		mItemNumbers_Character[3] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Ch_03")));
		mItemNumbers_Character[4] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Ch_10")));
		mItemNumbers_Character[5] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Ch_11")));
		mItemNumbers_Character[6] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Ch_12")));
		mItemNumbers_Character[7] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Ch_13")));
		mItemNumbers_Character[8] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Ch_20")));
		mItemNumbers_Character[9] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Ch_21")));
		mItemNumbers_Character[10] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Ch_22")));
		mItemNumbers_Character[11] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Ch_23")));
		mItemNumbers_Character[12] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Ch_30")));
		mItemNumbers_Character[13] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Ch_31")));
		mItemNumbers_Character[14] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Ch_32")));
		mItemNumbers_Character[15] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Ch_33")));

		mItemNumbers_Npc[0] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Npc_00")));
		mItemNumbers_Npc[1] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Npc_01")));
		mItemNumbers_Npc[2] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Npc_02")));
		mItemNumbers_Npc[3] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Npc_03")));
		mItemNumbers_Npc[4] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Npc_10")));
		mItemNumbers_Npc[5] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Npc_11")));
		mItemNumbers_Npc[6] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Npc_12")));
		mItemNumbers_Npc[7] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Npc_13")));
		mItemNumbers_Npc[8] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Npc_20")));
		mItemNumbers_Npc[9] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Npc_21")));
		mItemNumbers_Npc[10] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Npc_22")));
		mItemNumbers_Npc[11] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Npc_23")));
		mItemNumbers_Npc[12] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Npc_30")));
		mItemNumbers_Npc[13] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Npc_31")));
		mItemNumbers_Npc[14] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Npc_32")));
		mItemNumbers_Npc[15] = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_Npc_33")));

		for (int i = 0; i < MAX_ITEM_PER_TYPE; ++i)
		{
			mItemSlotButtons_Character[i]->mIndex = i;
			mItemSlotButtons_Npc[i]->mIndex = i;
		}

		mExitButton = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Exit")));

		mPriceText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_Price")));
		mCurrentGoldText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_Gold")));

		mSlotBrush_Empty.SetResourceObject(LoadHelper::LoadObjectFromPath<UTexture2D>(TEXT("/Game/Texture/WidgetImage/Thumbnail/Empty_Normal.Empty_Normal")));
		//Tab Button Style
		mTabBrush_Bright.SetResourceObject(LoadHelper::LoadObjectFromPath<UTexture2D>(TEXT("/Game/Texture/WidgetImage/Button_Wood.Button_Wood")));
		mTabBrush_Bright.TintColor = FLinearColor::White;

		mTabBrush_Dark.SetResourceObject(LoadHelper::LoadObjectFromPath<UTexture2D>(TEXT("/Game/Texture/WidgetImage/Button_Wood.Button_Wood")));
		mTabBrush_Dark.TintColor = FLinearColor::Gray;
	}
}

void UShopWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bIsFocusable = true;

	for (int i = 0; i < static_cast<uint8>(EItemType::Count); ++i)
	{
		check(mTabButtons_Character[i]);
		check(mTabButtons_Npc[i]);
	}

	for (int i = 0; i < MAX_ITEM_PER_TYPE; ++i)
	{
		check(mItemSlotButtons_Character[i]);
		check(mItemSlotButtons_Npc[i]);

		check(mItemNumbers_Character[i]);
		check(mItemNumbers_Npc[i]);
	}

	check(mExitButton);

	check(mPriceText);
	check(mCurrentGoldText);

	if (bBound == false)
	{
		for (int i = 0; i < static_cast<uint8>(EItemType::Count); ++i)
		{
			mTabButtons_Character[i]->mClickedEvent.AddDynamic(this, &UShopWidget::OnClicked_ChangeTab_Ch);
			mTabButtons_Npc[i]->mClickedEvent.AddDynamic(this, &UShopWidget::OnClicked_ChangeTab_Npc);
		}

		for (int i = 0; i < MAX_ITEM_PER_TYPE; ++i)
		{
			mItemSlotButtons_Character[i]->mClickedEvent.AddDynamic(this, &UShopWidget::OnClicked_ConfirmSell);
			mItemSlotButtons_Npc[i]->mClickedEvent.AddDynamic(this, &UShopWidget::OnClicked_ConfirmBuy);

			mItemSlotButtons_Character[i]->mHoveredEvent.AddDynamic(this, &UShopWidget::OnHovered_ShowSellPrice);
			mItemSlotButtons_Npc[i]->mHoveredEvent.AddDynamic(this, &UShopWidget::OnHovered_ShowBuyPrice);

			mItemSlotButtons_Character[i]->OnUnhovered.AddDynamic(this, &UShopWidget::OnUnhovered_RemovePrice);
			mItemSlotButtons_Npc[i]->OnUnhovered.AddDynamic(this, &UShopWidget::OnUnhovered_RemovePrice);
		}

		mExitButton->OnClicked.AddDynamic(this, &UShopWidget::OnClicked_Exit);

		bBound = true;
	}
}

void UShopWidget::Open(TObjectPtr<APlayerCharacter> player, TObjectPtr<ANpcCharacter> npc)
{
	check(npc);
	check(player);

	mOwnerPlayer = player;
	mOwnerNpc = npc;

	mCurrentTab_Ch = EItemType::Cloth;
	mCurrentTab_Npc = EItemType::Cloth;

	UpdateTab(true);
	UpdateTab(false);

	UpdateItemSlot(true);
	UpdateItemSlot(false);

	UpdateCurrentGold();
}

void UShopWidget::Close()
{
	mOwnerPlayer = nullptr;
	mOwnerNpc = nullptr;
}

void UShopWidget::ChangeTab(int index, bool isNpcTab)
{
	check(index < static_cast<uint8>(EItemType::Count));

	if (isNpcTab == true)
	{
		mCurrentTab_Npc = static_cast<EItemType>(index);
	}
	else
	{
		mCurrentTab_Ch = static_cast<EItemType>(index);
	}

	UpdateTab(isNpcTab);
	UpdateItemSlot(isNpcTab);
}

void UShopWidget::UpdateTab(bool isNpcTab)
{
	if (isNpcTab == true)
	{
		for (int i = 0; i < static_cast<uint8>(EItemType::Count); ++i)
		{
			if (mCurrentTab_Npc == static_cast<EItemType>(i))
			{
				mTabButtons_Npc[i]->WidgetStyle.SetNormal(mTabBrush_Bright);
				mTabButtons_Npc[i]->WidgetStyle.SetHovered(mTabBrush_Bright);
				mTabButtons_Npc[i]->WidgetStyle.SetPressed(mTabBrush_Bright);
			}
			else
			{
				mTabButtons_Npc[i]->WidgetStyle.SetNormal(mTabBrush_Dark);
				mTabButtons_Npc[i]->WidgetStyle.SetHovered(mTabBrush_Bright);
				mTabButtons_Npc[i]->WidgetStyle.SetPressed(mTabBrush_Bright);
			}
		}
	}
	else
	{
		for (int i = 0; i < static_cast<uint8>(EItemType::Count); ++i)
		{
			if (mCurrentTab_Ch == static_cast<EItemType>(i))
			{
				mTabButtons_Character[i]->WidgetStyle.SetNormal(mTabBrush_Bright);
				mTabButtons_Character[i]->WidgetStyle.SetHovered(mTabBrush_Bright);
				mTabButtons_Character[i]->WidgetStyle.SetPressed(mTabBrush_Bright);
			}
			else
			{
				mTabButtons_Character[i]->WidgetStyle.SetNormal(mTabBrush_Dark);
				mTabButtons_Character[i]->WidgetStyle.SetHovered(mTabBrush_Bright);
				mTabButtons_Character[i]->WidgetStyle.SetPressed(mTabBrush_Bright);
			}
		}
	}
}

void UShopWidget::UpdateItemSlot(bool isNpcTab)
{
	const UCustomGameInstance* gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(gi);

	if (isNpcTab == true)
	{
		const FTypeInventory& typeInventory = mOwnerNpc->GetShopItems().GetTypeInventory(mCurrentTab_Npc);

		for (int i = 0; i < typeInventory.ItemList.Num(); ++i)
		{
			const FGameItem& item = typeInventory.ItemList[i];

			const ItemInfo& info = gi->GetItemInfo(item.InfoIndex);

			FSlateBrush brush_N, brush_H;

			brush_N.SetResourceObject(info.Thumbnail[static_cast<uint8>(EThumbnailType::Normal)]);
			brush_H.SetResourceObject(info.Thumbnail[static_cast<uint8>(EThumbnailType::Normal)]);
			brush_H.TintColor = FLinearColor::White;

			mItemSlotButtons_Npc[i]->WidgetStyle.SetNormal(brush_N);
			mItemSlotButtons_Npc[i]->WidgetStyle.SetHovered(brush_H);
			mItemSlotButtons_Npc[i]->WidgetStyle.SetPressed(brush_N);
			mItemSlotButtons_Npc[i]->SetIsEnabled(true);

			mItemNumbers_Npc[i]->SetText(FText::FromString(FString::FromInt(item.Num)));
		}

		for (int i = typeInventory.ItemList.Num(); i < MAX_ITEM_PER_TYPE; ++i)
		{
			mItemSlotButtons_Npc[i]->WidgetStyle.SetNormal(mSlotBrush_Empty);
			mItemSlotButtons_Npc[i]->SetIsEnabled(false);

			mItemNumbers_Npc[i]->SetText(FText::GetEmpty());
		}
	}
	else
	{
		const FTypeInventory& typeInventory = mOwnerPlayer->GetInventory().GetTypeInventory(mCurrentTab_Ch);

		for (int i = 0; i < typeInventory.ItemList.Num(); ++i)
		{
			const FGameItem& item = typeInventory.ItemList[i];

			const ItemInfo& info = gi->GetItemInfo(item.InfoIndex);

			FSlateBrush brush_N, brush_H;

			if ((mCurrentTab_Ch == EItemType::Cloth) && (i == mOwnerPlayer->GetCurrentCloth()))
			{
				brush_N.SetResourceObject(info.Thumbnail[static_cast<uint8>(EThumbnailType::Selected)]);
			}
			else if ((mCurrentTab_Ch == EItemType::Weapon) && (i == mOwnerPlayer->GetCurrentWeapon()))
			{
				brush_N.SetResourceObject(info.Thumbnail[static_cast<uint8>(EThumbnailType::Selected)]);
			}
			else
			{
				brush_N.SetResourceObject(info.Thumbnail[static_cast<uint8>(EThumbnailType::Normal)]);
			}

			brush_H.SetResourceObject(info.Thumbnail[static_cast<uint8>(EThumbnailType::Hovered)]);

			mItemSlotButtons_Character[i]->WidgetStyle.SetNormal(brush_N);
			mItemSlotButtons_Character[i]->WidgetStyle.SetHovered(brush_H);
			mItemSlotButtons_Character[i]->WidgetStyle.SetPressed(brush_N);
			mItemSlotButtons_Character[i]->SetIsEnabled(true);

			mItemNumbers_Character[i]->SetText(FText::FromString(FString::FromInt(item.Num)));
		}

		for (int i = typeInventory.ItemList.Num(); i < MAX_ITEM_PER_TYPE; ++i)
		{
			mItemSlotButtons_Character[i]->WidgetStyle.SetNormal(mSlotBrush_Empty);
			mItemSlotButtons_Character[i]->SetIsEnabled(false);

			mItemNumbers_Character[i]->SetText(FText::GetEmpty());
		}
	}
}

void UShopWidget::UpdateCurrentGold()
{
	mCurrentGoldText->SetText(FText::FromString("Gold : " + FString::FromInt(mOwnerPlayer->GetCurrentGold())));
}

void UShopWidget::OpenConfirmPopup(int index, bool isBuying)
{
	bBuying = isBuying;
	mSelectedIndex = index;

	if (!mConfirmPopup)
	{
		mConfirmPopup = WidgetTree->ConstructWidget<UNumpadPopup>(mNumpadPopupClass, TEXT("Popup"));

		TObjectPtr<UCanvasPanel> rootCanvas = Cast<UCanvasPanel>(WidgetTree->RootWidget);
		rootCanvas->AddChild(mConfirmPopup);
		Cast<UCanvasPanelSlot>(mConfirmPopup->Slot)->SetZOrder(2);

		mConfirmPopup->mNumpadConfirmEvent.AddDynamic(this, &UShopWidget::OnClicked_BuySell);
	}
	else
	{
		TObjectPtr<UCanvasPanel> rootCanvas = Cast<UCanvasPanel>(WidgetTree->RootWidget);
		rootCanvas->AddChild(mConfirmPopup);
		Cast<UCanvasPanelSlot>(mConfirmPopup->Slot)->SetZOrder(2);
	}

	Cast<UCanvasPanelSlot>(mConfirmPopup->Slot)->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));

	if (isBuying == true)
	{
		const FGameItem& item = mOwnerNpc->GetShopItems().GetTypeInventory(mCurrentTab_Npc).ItemList[index];
		
		UCustomGameInstance* gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
		const ItemInfo& info = gi->GetItemInfo(item.InfoIndex);

		// 살 수 있는 개수와 상점 보유 개수 중 작은 쪽
		const int maxNum = FMath::Min<int>((mOwnerPlayer->GetCurrentGold() / info.Price), item.Num);

		//mConfirmPopup->SetTitle(TEXT("얼마나 구매하시겠습니까?"));
		mConfirmPopup->SetNumpad(maxNum);
	}
	else
	{
		const FGameItem& item = mOwnerPlayer->GetInventory().GetTypeInventory(mCurrentTab_Ch).ItemList[index];

		//mConfirmPopup->SetTitle(TEXT("얼마나 판매하시겠습니까?"));
		mConfirmPopup->SetNumpad(item.Num);
	}
}

void UShopWidget::OnClicked_ChangeTab_Ch(int index)
{
	ChangeTab(index, false);
}

void UShopWidget::OnClicked_ChangeTab_Npc(int index)
{
	ChangeTab(index, true);
}

void UShopWidget::OnClicked_ConfirmBuy(int index)
{
	OpenConfirmPopup(index, true);
}

void UShopWidget::OnClicked_ConfirmSell(int index)
{
	OpenConfirmPopup(index, false);
}

void UShopWidget::OnHovered_ShowBuyPrice(int index)
{
	UCustomGameInstance* gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(gi);

	const int price = gi->GetItemInfo(mOwnerNpc->GetShopItems().GetTypeInventory(mCurrentTab_Npc).ItemList[index].InfoIndex).Price;

	mPriceText->SetText(FText::FromString("Price : " + FString::FromInt(price)));
}

void UShopWidget::OnHovered_ShowSellPrice(int index)
{
	UCustomGameInstance* gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(gi);

	const int price = gi->GetItemInfo(mOwnerPlayer->GetInventory().GetTypeInventory(mCurrentTab_Ch).ItemList[index].InfoIndex).Price;

	mPriceText->SetText(FText::FromString("Price : " + FString::FromInt(price)));
}

void UShopWidget::OnUnhovered_RemovePrice()
{
	mPriceText->SetText(FText::GetEmpty());
}

void UShopWidget::OnClicked_BuySell(int num)
{
	UCustomGameInstance* gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(gi);

	if (bBuying == true)
	{	
		// 거래 가능 확인
		FGameItem& item_npc = mOwnerNpc->GetShopItems().GetTypeInventory(mCurrentTab_Npc).ItemList[mSelectedIndex];
		const ItemInfo& info = gi->GetItemInfo(item_npc.InfoIndex);

		const int cost = num * info.Price;

		check(mOwnerPlayer->GetCurrentGold() >= cost);
		check(item_npc.Num >= num);

		// NPC 적용
		item_npc.Num -= num;
		if (item_npc.Num == 0)
		{
			mOwnerNpc->GetShopItems().GetTypeInventory(mCurrentTab_Npc).ItemList.RemoveAt(mSelectedIndex);
		}

		// 플레이어 적용
		FGameItem item_player(info.Index, num);
		mOwnerPlayer->RootItem(item_player);
		mOwnerPlayer->SetCurrentGold(mOwnerPlayer->GetCurrentGold() - cost);

		UpdateItemSlot(true);

		if (mCurrentTab_Ch == mCurrentTab_Npc)
		{
			UpdateItemSlot(false);
		}
	}
	else
	{	
		// 플레이어 적용
		FGameItem& item_player = mOwnerPlayer->GetInventory().GetTypeInventory(mCurrentTab_Ch).ItemList[mSelectedIndex];
		const ItemInfo& info = gi->GetItemInfo(item_player.InfoIndex);

		const int cost = num * info.Price;

		mOwnerPlayer->ThrowItem(mCurrentTab_Ch, mSelectedIndex, num);
		mOwnerPlayer->SetCurrentGold(mOwnerPlayer->GetCurrentGold() + cost);

		UpdateItemSlot(false);
	}

	UpdateCurrentGold();

	mConfirmPopup->RemoveFromParent();
}

void UShopWidget::OnClicked_Exit()
{
	TObjectPtr<ACustomController> pc = Cast<ACustomController>(UGameplayStatics::GetPlayerController(this, 0));
	check(pc);
	pc->CloseShop(mOwnerPlayer);
}


