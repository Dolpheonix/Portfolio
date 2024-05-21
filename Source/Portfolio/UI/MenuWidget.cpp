#include "MenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "../Core/CustomController.h"
#include "../Core/CustomGameInstance.h"
#include "../Character/PlayerCharacter.h"

UMenuWidget::UMenuWidget(const FObjectInitializer& objinit) : Super(objinit), mOwnerPlayer(nullptr), bBound(false)
{
}

void UMenuWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsDesignTime() == false)
	{
		mBackToGameButton = Cast<UButton>(GetWidgetFromName(TEXT("Btn_BackToGame")));
		mExitGameButton = Cast<UButton>(GetWidgetFromName(TEXT("Btn_ExitGame")));
	}
}

void UMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	check(mBackToGameButton);
	check(mExitGameButton);

	if (bBound == false)
	{
		mBackToGameButton->OnClicked.AddDynamic(this, &UMenuWidget::OnClicked_BackToGame);
		mExitGameButton->OnClicked.AddDynamic(this, &UMenuWidget::OnClicked_ExitGame);

		bBound = true;
	}
}

void UMenuWidget::Open(TObjectPtr<APlayerCharacter> player)
{
	check(player);

	mOwnerPlayer = player;
}

void UMenuWidget::Close()
{
	mOwnerPlayer = nullptr;
}

void UMenuWidget::OnClicked_BackToGame()
{
	TObjectPtr<ACustomController> pc = Cast<ACustomController>(UGameplayStatics::GetPlayerController(this, 0));
	check(pc);

	pc->CloseMenu(mOwnerPlayer, false);
}

void UMenuWidget::OnClicked_ExitGame()
{
	TObjectPtr<ACustomController> pc = Cast<ACustomController>(UGameplayStatics::GetPlayerController(this, 0));
	check(pc);

	pc->CloseMenu(mOwnerPlayer, true);
}
