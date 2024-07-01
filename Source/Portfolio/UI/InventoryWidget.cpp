#include "InventoryWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "../Core/CustomGameInstance.h"
#include "../Core/CustomController.h"
#include "../Core/CustomLevelScript.h"
#include "../Character/PlayerCharacter.h"
#include "../Character/PreviewCharacter.h"
#include "IndexedButton.h"
#include "../Base/LoadHelper.h"

static_assert(NUM_INVENTORY_ROW == 4);
static_assert(NUM_INVENTORY_COL == 4);
static_assert(MAX_ITEM_PER_TYPE == 16);
static_assert(static_cast<uint8>(EItemType::Count) == 3);

UInventoryWidget::UInventoryWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer), bBound(false), bDragging(false), mDraggedIndex(-1)
{
	mTabButtons.SetNum(static_cast<uint8>(EItemType::Count));
	mItemSlotButtons.SetNum(MAX_ITEM_PER_TYPE);
	mItemNumbers.SetNum(MAX_ITEM_PER_TYPE);

	mNumpadPopupClass = LoadHelper::C_LoadObjectFromPath<UBlueprintGeneratedClass>(TEXT("/Game/UI/NumpadPopup_BP.NumpadPopup_BP_C"));
}

void UInventoryWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsDesignTime() == false)
	{
		mTabButtons[static_cast<uint8>(EItemType::Cloth)] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemTab_Cloth")));
		mTabButtons[static_cast<uint8>(EItemType::Weapon)] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemTab_Weapon")));
		mTabButtons[static_cast<uint8>(EItemType::Item)] = Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemTab_Item")));

		for (int i = 0; i < static_cast<uint8>(EItemType::Count); ++i)
		{
			mTabButtons[i]->mIndex = i;
		}

		mItemSlotButtons[0]		= Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_00")));
		mItemSlotButtons[1]		= Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_01")));
		mItemSlotButtons[2]		= Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_02")));
		mItemSlotButtons[3]		= Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_03")));
		mItemSlotButtons[4]		= Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_10")));
		mItemSlotButtons[5]		= Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_11")));
		mItemSlotButtons[6]		= Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_12")));
		mItemSlotButtons[7]		= Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_13")));
		mItemSlotButtons[8]		= Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_20")));
		mItemSlotButtons[9]		= Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_21")));
		mItemSlotButtons[10]	= Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_22")));
		mItemSlotButtons[11]	= Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_23")));
		mItemSlotButtons[12]	= Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_30")));
		mItemSlotButtons[13]	= Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_31")));
		mItemSlotButtons[14]	= Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_32")));
		mItemSlotButtons[15]	= Cast<UIndexedButton>(GetWidgetFromName(TEXT("ItemSlot_33")));

		mItemNumbers[0]		= Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_00")));
		mItemNumbers[1]		= Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_01")));
		mItemNumbers[2]		= Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_02")));
		mItemNumbers[3]		= Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_03")));
		mItemNumbers[4]		= Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_10")));
		mItemNumbers[5]		= Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_11")));
		mItemNumbers[6]		= Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_12")));
		mItemNumbers[7]		= Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_13")));
		mItemNumbers[8]		= Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_20")));
		mItemNumbers[9]		= Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_21")));
		mItemNumbers[10]	= Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_22")));
		mItemNumbers[11]	= Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_23")));
		mItemNumbers[12]	= Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_30")));
		mItemNumbers[13]	= Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_31")));
		mItemNumbers[14]	= Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_32")));
		mItemNumbers[15]	= Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNumber_33")));

		for (int i = 0; i < MAX_ITEM_PER_TYPE; ++i)
		{
			mItemSlotButtons[i]->mIndex = i;
		}

		mThrowButton	= Cast<UButton>(GetWidgetFromName(TEXT("Btn_Throw")));
		mExitButton		= Cast<UButton>(GetWidgetFromName(TEXT("Btn_Exit")));
		mPreviewImage	= Cast<UImage>(GetWidgetFromName(TEXT("Img_Preview")));
		mExplanationText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_Explanation")));
		mCurrentGoldText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_CurrentGold")));

		mDraggedImage = Cast<UImage>(GetWidgetFromName(TEXT("Img_Dragged")));

		mSlotBrush_Empty.SetResourceObject(LoadHelper::LoadObjectFromPath<UTexture2D>(TEXT("/Game/Texture/WidgetImage/Thumbnail/Empty_Normal.Empty_Normal")));
		mTabBrush_Bright.SetResourceObject(LoadHelper::LoadObjectFromPath<UTexture2D>(TEXT("/Game/Texture/WidgetImage/Button_Wood.Button_Wood")));
		mTabBrush_Bright.TintColor = FLinearColor::White;

		mTabBrush_Dark.SetResourceObject(LoadHelper::LoadObjectFromPath<UTexture2D>(TEXT("/Game/Texture/WidgetImage/Button_Wood.Button_Wood")));
		mTabBrush_Dark.TintColor = FLinearColor::Gray;
	}
}

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bIsFocusable = true;
	
	for (int i = 0; i < static_cast<uint8>(EItemType::Count); ++i)
	{
		check(mTabButtons[i]);
	}

	for (int i = 0; i < MAX_ITEM_PER_TYPE; ++i)
	{
		check(mItemSlotButtons[i]);
	}

	check(mThrowButton);
	check(mExitButton);
	check(mPreviewImage);
	check(mExplanationText);
	check(mCurrentGoldText);
	
	check(mDraggedImage);

	if (bBound == false)
	{
		for (int i = 0; i < static_cast<uint8>(EItemType::Count); ++i)
		{
			mTabButtons[i]->mClickedEvent.AddDynamic(this, &UInventoryWidget::OnClicked_ChangeTab);
		}

		for (int i = 0; i < MAX_ITEM_PER_TYPE; ++i)
		{
			mItemSlotButtons[i]->mClickedEvent.AddDynamic(this, &UInventoryWidget::OnClicked_SelectItem);
			mItemSlotButtons[i]->mHoveredEvent.AddDynamic(this, &UInventoryWidget::OnHovered_ShowExplanation);
			mItemSlotButtons[i]->OnUnhovered.AddDynamic(this, &UInventoryWidget::OnUnhovered_RemoveExplanation);

			//TEMP : 드래그, 아이템 삭제 기능 비활성화 (Press method와 Click method를 둘다 바인딩 할 경우 Click method가 불리지 않는 현상)
			//mItemSlotButtons[i]->mPressedEvent.AddDynamic(this, &UInventoryWidget::OnPressed_DragSlot);
			//mItemSlotButtons[i]->OnReleased.AddDynamic(this, &UInventoryWidget::OnReleased_ThrowItem);
		}

		mExitButton->OnClicked.AddDynamic(this, &UInventoryWidget::OnClicked_Exit);

		mPreviewImage->SetBrushFromMaterial(LoadHelper::LoadObjectFromPath<UMaterial>(TEXT("/Game/Material/M_Preview.M_Preview")));

		bBound = true;
	}
}

void UInventoryWidget::NativeTick(const FGeometry& Mygeometry, float InDeltaTime)
{
	Super::NativeTick(Mygeometry, InDeltaTime);

	// 드래그 중인 경우, 드래그 이미지가 커서를 따라가게 함
	if (bDragging)
	{
		FVector2D imagePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
		imagePos.X -= 50.f;
		imagePos.Y -= 50.f;
		mDraggedImage->SetRenderTranslation(imagePos);
	}
}

FReply UInventoryWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	int ascii = InKeyEvent.GetCharacter();
	if (ascii == 73 || ascii == 105)
	{
		TObjectPtr<ACustomController> pc = Cast<ACustomController>(UGameplayStatics::GetPlayerController(this, 0));
		check(pc);
		pc->CloseInventory(mOwnerPlayer);
	}

	return FReply::Handled();
}

void UInventoryWidget::Open(TObjectPtr<APlayerCharacter> player)
{
	check(player);

	mOwnerPlayer = player;

	mCurrentTab = EItemType::Cloth;
	UpdateTab();
	UpdateItemSlot();

	//인벤토리 안에서 현재 골드가 바뀔 일은 없기 때문에, UI를 열때만 골드를 업데이트.
	mExplanationText->SetText(FText::GetEmpty());
	mCurrentGoldText->SetText(FText::FromString("Gold : " + FString::FromInt(player->GetCurrentGold())));

	mDraggedImage->SetVisibility(ESlateVisibility::Hidden);

	//프리뷰 캐릭터를 찾아, 플레이어 캐릭터와 동기화
	TObjectPtr<ACustomLevelScript> levelScript = Cast<ACustomLevelScript>(GetWorld()->GetLevelScriptActor());
	check(levelScript);

	mPreviewClone = levelScript->GetPreviewCharacter();
	check(mPreviewClone);

	mPreviewClone->SyncWithPlayer(player);
}

void UInventoryWidget::Close()
{
	mOwnerPlayer = nullptr;
	mCurrentTab = EItemType::Count;
}

void UInventoryWidget::UpdateTab()
{
	for (int i = 0; i < static_cast<uint8>(EItemType::Count); ++i)
	{
		if (mCurrentTab == static_cast<EItemType>(i))
		{
			mTabButtons[i]->WidgetStyle.SetNormal(mTabBrush_Bright);
			mTabButtons[i]->WidgetStyle.SetHovered(mTabBrush_Bright);
			mTabButtons[i]->WidgetStyle.SetPressed(mTabBrush_Bright);
		}
		else
		{
			mTabButtons[i]->WidgetStyle.SetNormal(mTabBrush_Dark);
			mTabButtons[i]->WidgetStyle.SetHovered(mTabBrush_Bright);
			mTabButtons[i]->WidgetStyle.SetPressed(mTabBrush_Bright);
		}
	}
}

void UInventoryWidget::UpdateItemSlot()
{
	const UCustomGameInstance* gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(gi);

	// 현재 탭에 맞는 인벤토리로 전환
	const FTypeInventory& typeInventory = mOwnerPlayer->GetInventory().GetTypeInventory(mCurrentTab);

	for (int i = 0; i < typeInventory.ItemList.Num(); ++i)
	{
		const FGameItem item = typeInventory.ItemList[i];
		
		const ItemInfo& info = gi->GetItemInfo(item.InfoIndex);
		// 선택된 아이템은 선택 썸네일로 설정
		FSlateBrush brush_N, brush_H;
		if ((mCurrentTab == EItemType::Cloth) && (i == mOwnerPlayer->GetCurrentCloth()))
		{
			brush_N.SetResourceObject(info.Thumbnail[static_cast<uint8>(EThumbnailType::Selected)]);
		}
		else if ((mCurrentTab == EItemType::Weapon) && (i == mOwnerPlayer->GetCurrentWeapon()))
		{
			brush_N.SetResourceObject(info.Thumbnail[static_cast<uint8>(EThumbnailType::Selected)]);
		}
		else
		{
			brush_N.SetResourceObject(info.Thumbnail[static_cast<uint8>(EThumbnailType::Normal)]);
		}

		brush_H.SetResourceObject(info.Thumbnail[static_cast<uint8>(EThumbnailType::Hovered)]);

		mItemSlotButtons[i]->WidgetStyle.SetNormal(brush_N);
		mItemSlotButtons[i]->WidgetStyle.SetHovered(brush_H);
		mItemSlotButtons[i]->WidgetStyle.SetPressed(brush_N);
		mItemSlotButtons[i]->SetIsEnabled(true);

		// 아이템 개수도 표시
		mItemNumbers[i]->SetText(FText::FromString(FString::FromInt(item.Num)));
	}

	for (int i = typeInventory.ItemList.Num(); i < MAX_ITEM_PER_TYPE; ++i)
	{
		mItemSlotButtons[i]->WidgetStyle.SetNormal(mSlotBrush_Empty);
		mItemSlotButtons[i]->SetIsEnabled(false);

		mItemNumbers[i]->SetText(FText::GetEmpty());
	}
}

void UInventoryWidget::SelectItem(int index)
{
	if (index < 0)
	{
		return;
	}

	const UCustomGameInstance* gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(gi);

	const FGameItem& item = mOwnerPlayer->GetInventory().GetTypeInventory(mCurrentTab).ItemList[index];
	const ItemInfo& info = gi->GetItemInfo(item.InfoIndex);

	// 선택 썸네일로 변경
	FSlateBrush brush_N;
	brush_N.SetResourceObject(info.Thumbnail[static_cast<uint8>(EThumbnailType::Selected)]);
	mItemSlotButtons[index]->WidgetStyle.SetNormal(brush_N);
}

void UInventoryWidget::UnselectItem(int index)
{
	if (index < 0)
	{
		return;
	}

	const UCustomGameInstance* gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(gi);

	const FGameItem item = mOwnerPlayer->GetInventory().GetTypeInventory(mCurrentTab).ItemList[index];
	const ItemInfo& info = gi->GetItemInfo(item.InfoIndex);

	// 일반 썸네일로 변경
	FSlateBrush brush_N;
	brush_N.SetResourceObject(info.Thumbnail[static_cast<uint8>(EThumbnailType::Normal)]);
	mItemSlotButtons[index]->WidgetStyle.SetNormal(brush_N);
}

void UInventoryWidget::ShowDeletePopup()
{
	// 팝업 UI는 한번만 생성하고, 비지빌리티로 관리
	if (!mThrowPopup)
	{
		mThrowPopup = WidgetTree->ConstructWidget<UNumpadPopup>(mNumpadPopupClass, TEXT("Popup"));

		TObjectPtr<UCanvasPanel> rootCanvas = Cast<UCanvasPanel>(WidgetTree->RootWidget);
		rootCanvas->AddChild(mThrowPopup);

		mThrowPopup->mNumpadConfirmEvent.AddDynamic(this, &UInventoryWidget::OnClicked_DeleteItem);
	}
	else
	{
		TObjectPtr<UCanvasPanel> rootCanvas = Cast<UCanvasPanel>(WidgetTree->RootWidget);
		rootCanvas->AddChild(mThrowPopup);
	}


	Cast<UCanvasPanelSlot>(mThrowPopup->Slot)->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));

	const FGameItem& item = mOwnerPlayer->GetInventory().GetTypeInventory(mCurrentTab).ItemList[mDraggedIndex];

	mThrowPopup->SetTitle(TEXT("얼마나 버리시겠습니까?"));
	mThrowPopup->SetNumpad(item.Num);
}

void UInventoryWidget::OnClicked_ChangeTab(int index)
{
	check(index < static_cast<uint8>(EItemType::Count));

	if (mCurrentTab == static_cast<EItemType>(index))
	{
		return;
	}

	mCurrentTab = static_cast<EItemType>(index);

	UpdateTab();
	UpdateItemSlot();
}

void UInventoryWidget::OnClicked_SelectItem(int index)
{
	int lastSelected = -1;

	switch (mCurrentTab)
	{
	// 의상, 무기의 경우 아이템을 선택하고, 프리뷰 캐릭터를 업데이트함
	case EItemType::Cloth:
		lastSelected = mOwnerPlayer->GetCurrentCloth();
		mOwnerPlayer->SetCurrentCloth(index);
		mPreviewClone->SyncWithPlayer(mOwnerPlayer);
		break;
	case EItemType::Weapon:
		lastSelected = mOwnerPlayer->GetCurrentWeapon();
		mOwnerPlayer->SetCurrentWeapon(index);
		mPreviewClone->SyncWithPlayer(mOwnerPlayer);
		break;
	case EItemType::Item:
		return;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid EItemType"));
	}

	UnselectItem(lastSelected);
	SelectItem(index);
}

void UInventoryWidget::OnHovered_ShowExplanation(int index)
{
	check(index < mOwnerPlayer->GetInventory().GetTypeInventory(mCurrentTab).ItemList.Num());

	const UCustomGameInstance* gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(gi);

	const FGameItem item = mOwnerPlayer->GetInventory().GetTypeInventory(mCurrentTab).ItemList[index];
	const ItemInfo& info = gi->GetItemInfo(item.InfoIndex);

	mExplanationText->SetText(FText::FromString(info.Name));
}

void UInventoryWidget::OnPressed_DragSlot(int index)
{
	bDragging = true;
	mDraggedIndex = index;

	const FVector2D& mousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
	FVector2D imgPos = mousePos;
	imgPos.X -= 50.f;
	imgPos.Y -= 50.f;

	mDraggedImage->SetVisibility(ESlateVisibility::Visible);
	mDraggedImage->SetRenderTranslation(imgPos);
	mDraggedImage->SetOpacity(0.7f);
	mDraggedImage->SetBrushFromTexture(Cast<UTexture2D>(mItemSlotButtons[index]->WidgetStyle.Hovered.GetResourceObject()));
}

void UInventoryWidget::OnUnhovered_RemoveExplanation()
{
	mExplanationText->SetText(FText::GetEmpty());
}

void UInventoryWidget::OnReleased_ThrowItem()
{
	UCanvasPanelSlot* throwSlot = Cast<UCanvasPanelSlot>(mThrowButton->Slot);
	check(throwSlot);

	// 쓰레기통 이미지 위에서 드래그를 풀었다면, 아이템 삭제
	const FVector2D& throwButtonPos = throwSlot->GetPosition();
	const FVector2D& throwButtonSize = throwSlot->GetSize();

	const FBox2D throwRect(throwButtonPos, FVector2D(throwButtonPos.X + throwButtonSize.X, throwButtonPos.Y + throwButtonSize.Y));

	const FVector2D& mousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
	if (throwRect.IsInside(mousePos))
	{
		ShowDeletePopup();
	}

	bDragging = false;
	mDraggedImage->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryWidget::OnClicked_Exit()
{
	TObjectPtr<ACustomController> pc = Cast<ACustomController>(UGameplayStatics::GetPlayerController(this, 0));
	check(pc);
	pc->CloseInventory(mOwnerPlayer);
}

void UInventoryWidget::OnClicked_DeleteItem(int num)
{
	mOwnerPlayer->ThrowItem(mCurrentTab, mDraggedIndex, num);

	UpdateTab();
	UpdateItemSlot();

	mThrowPopup->RemoveFromParent();
}
