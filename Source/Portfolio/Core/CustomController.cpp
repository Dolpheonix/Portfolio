#include "CustomController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Kismet/GameplayStatics.h"
#include "../Base/LoadHelper.h"
#include "CustomGameInstance.h"

ACustomController::ACustomController()
{
	IntroWidgetClass		= LoadHelper::C_LoadObjectFromPath<UBlueprintGeneratedClass>(TEXT("/Game/UI/IntroWidget_BP.IntroWidget_BP_C"));
	HUDWidgetClass			= LoadHelper::C_LoadObjectFromPath<UBlueprintGeneratedClass>(TEXT("/Game/UI/HUDWidget_BP.HUDWidget_BP_C"));
	MenuWidgetClass			= LoadHelper::C_LoadObjectFromPath<UBlueprintGeneratedClass>(TEXT("/Game/UI/MenuWidget_BP.MenuWidget_BP_C"));
	InventoryWidgetClass	= LoadHelper::C_LoadObjectFromPath<UBlueprintGeneratedClass>(TEXT("/Game/UI/InventoryWidget_BP.InventoryWidget_BP_C"));
	QuestWidgetClass		= LoadHelper::C_LoadObjectFromPath<UBlueprintGeneratedClass>(TEXT("/Game/UI/QuestWidget_BP.QuestWidget_BP_C"));
	DialogueWidgetClass		= LoadHelper::C_LoadObjectFromPath<UBlueprintGeneratedClass>(TEXT("/Game/UI/DialogueWidget_BP.DialogueWidget_BP_C"));
	ShopWidgetClass			= LoadHelper::C_LoadObjectFromPath<UBlueprintGeneratedClass>(TEXT("/Game/UI/ShopWidget_BP.ShopWidget_BP_C"));
	NotifyWidgetClass		= LoadHelper::C_LoadObjectFromPath<UBlueprintGeneratedClass>(TEXT("/Game/UI/NotifyWidget_BP.NotifyWidget_BP_C"));

	mPerceptionComponent	= CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
	mSenseConfig_Sight		= CreateOptionalDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sense_Sight"));
	mPerceptionComponent->ConfigureSense(*mSenseConfig_Sight);
	mPerceptionComponent->SetDominantSense(mSenseConfig_Sight->GetSenseImplementation());
	mPerceptionComponent->Activate();
}

void ACustomController::BeginPlay()
{
	Super::BeginPlay();

	mIntroWidget		= Cast<UIntroWidget>(CreateWidget(this, IntroWidgetClass, TEXT("IntroWidget")));
	mHUDWidget			= Cast<UHUDWidget>(CreateWidget(this, HUDWidgetClass, TEXT("NormalWidget")));
	mMenuWidget			= Cast<UMenuWidget>(CreateWidget(this, MenuWidgetClass, TEXT("MenuWidget")));
	mInventoryWidget	= Cast<UInventoryWidget>(CreateWidget(this, InventoryWidgetClass, TEXT("InventoryWidget")));
	mQuestWidget		= Cast<UQuestWidget>(CreateWidget(this, QuestWidgetClass, TEXT("QuestWidget")));
	mDialogueWidget		= Cast<UDialogueWidget>(CreateWidget(this, DialogueWidgetClass, TEXT("DialogueWidget")));
	mShopWidget			= Cast<UShopWidget>(CreateWidget(this, ShopWidgetClass, TEXT("ShopWidget")));
	mNotifyWidget		= Cast<UNotifyWidget>(CreateWidget(this, NotifyWidgetClass, TEXT("NotifyWidget")));

	TObjectPtr<UCustomGameInstance> gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(gi);
	if (gi->IsIntro() == true)
	{
		OpenIntro();
	}
	else
	{
		// 기본 감지 범위 설정
		mSenseConfig_Sight->SightRadius = 200.f;
		mSenseConfig_Sight->LoseSightRadius = 200.f;
		mSenseConfig_Sight->PeripheralVisionAngleDegrees = 360.f;

		FAISenseAffiliationFilter senseFilter;
		senseFilter.bDetectEnemies = true;
		senseFilter.bDetectFriendlies = false;
		senseFilter.bDetectNeutrals = false;
		mSenseConfig_Sight->DetectionByAffiliation = senseFilter;

		mPerceptionComponent->RequestStimuliListenerUpdate();
	}
}

void ACustomController::OpenIntro()
{
	SetInputMode(FInputModeUIOnly());
	SetShowMouseCursor(true);
	mIntroWidget->AddToViewport();
}

void ACustomController::CloseIntro()
{
	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);

	if (mIntroWidget->GetParent())
	{
		mIntroWidget->RemoveFromParent();
	}
}

void ACustomController::OpenMenu(TObjectPtr<APlayerCharacter> player)
{
	SetInputMode(FInputModeUIOnly());
	SetShowMouseCursor(true);
	mMenuWidget->AddToViewport();

	mMenuWidget->Open(player);
}

void ACustomController::CloseMenu(TObjectPtr<APlayerCharacter> player, bool openIntro)
{
	mMenuWidget->Close();
	mMenuWidget->RemoveFromParent();

	if (openIntro == true)
	{
		TObjectPtr<UCustomGameInstance> gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
		check(gi);
		gi->OpenIntro();
	}
	else
	{
		SetInputMode(FInputModeGameOnly());
		SetShowMouseCursor(false);

		OpenHUD(player);
	}
}

void ACustomController::OpenHUD(TObjectPtr<APlayerCharacter> player)
{
	SetInputMode(FInputModeGameOnly());

	mHUDWidget->AddToViewport();
	mHUDWidget->Open(player);
}

void ACustomController::OpenDialogue(TObjectPtr<APlayerCharacter> player, TObjectPtr<ANpcCharacter> npc)
{
	mHUDWidget->RemoveFromParent();

	SetInputMode(FInputModeUIOnly());
	SetShowMouseCursor(true);
	mDialogueWidget->AddToViewport();
	mDialogueWidget->Open(player, npc);
}

void ACustomController::CloseDialogue(TObjectPtr<APlayerCharacter> player, TObjectPtr<ANpcCharacter> npc, bool openShop)
{
	mDialogueWidget->Close();
	mDialogueWidget->RemoveFromParent();

	if (openShop == true)
	{
		OpenShop(player, npc);
	}
	else
	{
		SetInputMode(FInputModeGameOnly());
		SetShowMouseCursor(false);

		OpenHUD(player);
	}
}

void ACustomController::OpenInventory(TObjectPtr<APlayerCharacter> player)
{
	mHUDWidget->RemoveFromParent();

	SetInputMode(FInputModeUIOnly());
	SetShowMouseCursor(true);
	mInventoryWidget->AddToViewport();
	mInventoryWidget->Open(player);
}

void ACustomController::CloseInventory(TObjectPtr<APlayerCharacter> player)
{
	mInventoryWidget->Close();
	mInventoryWidget->RemoveFromParent();

	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);

	OpenHUD(player);
}

void ACustomController::OpenShop(TObjectPtr<APlayerCharacter> player, TObjectPtr<ANpcCharacter> npc)
{
	SetInputMode(FInputModeUIOnly());
	SetShowMouseCursor(true);
	mShopWidget->AddToViewport();
	mShopWidget->Open(player, npc);
}

void ACustomController::CloseShop(TObjectPtr<APlayerCharacter> player)
{
	mShopWidget->Close();
	mShopWidget->RemoveFromParent();

	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);

	OpenHUD(player);
}

void ACustomController::OpenQuestTable(TObjectPtr<APlayerCharacter> player)
{
	mHUDWidget->RemoveFromParent();

	SetInputMode(FInputModeUIOnly());
	SetShowMouseCursor(true);
	mQuestWidget->AddToViewport();
	mQuestWidget->Open(player);
}

void ACustomController::CloseQuestTable(TObjectPtr<APlayerCharacter> player)
{
	mQuestWidget->Close();
	mQuestWidget->RemoveFromParent();

	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);

	OpenHUD(player);
}

void ACustomController::OpenChattingBox()
{
	SetInputMode(FInputModeUIOnly());
	SetShowMouseCursor(true);

	mHUDWidget->SetChattingBoxVisibility(true);
	mHUDWidget->UpdateChatting();
}

void ACustomController::CloseChattingBox()
{
	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);

	mHUDWidget->SetChattingBoxVisibility(false);
}

void ACustomController::UpdateChatting()
{
	if (mHUDWidget->IsInViewport() == true)
	{
		mHUDWidget->UpdateChatting();
	}
}

void ACustomController::UpdateHealthBar()
{
	mHUDWidget->UpdateHpBar();
}

void ACustomController::UpdateWeaponImage()
{
	mHUDWidget->UpdateWeaponImage();
}

TObjectPtr<UAIPerceptionComponent> ACustomController::GetPerceptionComponent()
{
	return mPerceptionComponent;
}