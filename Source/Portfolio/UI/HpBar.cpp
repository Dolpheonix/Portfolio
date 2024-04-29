#include "HpBar.h"
#include "Components/ProgressBar.h"

UHpBar::UHpBar(const FObjectInitializer& objinit) : Super(objinit)
{
	
}

void UHpBar::SetHp(float progress)
{
	mProgressBar->SetPercent(progress);
}

void UHpBar::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsDesignTime() == false)
	{
		mProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("ProgressBar")));
	}
}

void UHpBar::NativeConstruct()
{
	Super::NativeConstruct();

	check(mProgressBar);
}
