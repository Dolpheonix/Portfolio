#include "RootItem.h"
#include "Components/BillboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "../Base/LoadHelper.h"
#include "../Character/PlayerCharacter.h"
#include "../Core/CustomGameInstance.h"

ARootItem::ARootItem() : Item()
{
	PrimaryActorTick.bCanEverTick = true;

	mCapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Root"));
	RootComponent = mCapsuleComponent;

	mNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	mNiagaraComponent->SetupAttachment(RootComponent);
	mNiagaraComponent->SetAutoActivate(true);

	mNotifyBillboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("NotifyBillboard"));
	mNotifyBillboard->SetupAttachment(RootComponent);
	const TObjectPtr<UTexture2D> notifyImage = LoadHelper::C_LoadObjectFromPath<UTexture2D>(TEXT("/Game/Texture/WidgetImage/NotifyInteraction.NotifyInteraction"));
	mNotifyBillboard->SetSprite(notifyImage);

	mPerceptionStimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("PerceptionSource"));
	mPerceptionStimuliSourceComponent->bAutoRegister = true;
}

void ARootItem::BeginPlay()
{
	Super::BeginPlay();

	mCapsuleComponent->SetVisibility(false);

	mNotifyBillboard->SetHiddenInGame(false);
	mNotifyBillboard->SetVisibility(false);

	mPerceptionStimuliSourceComponent->RegisterForSense(UAISense_Sight::StaticClass());
	mPerceptionStimuliSourceComponent->RegisterWithPerceptionSystem();
}

void ARootItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARootItem::Notify(TObjectPtr<AActor> player)
{
	mNotifyBillboard->SetVisibility(true);
}

void ARootItem::UnNotify(TObjectPtr<AActor> player)
{
	mNotifyBillboard->SetVisibility(false);
}

void ARootItem::Interact(TObjectPtr<AActor> player)
{
	TObjectPtr<APlayerCharacter> pc = Cast<APlayerCharacter>(player);
	check(pc);

	pc->RootItem(Item);	// 캐릭터는 아이템 획득

	pc->UnInteract();	// 루팅 후 상호작용 종료

	TObjectPtr<UCustomGameInstance> gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(gi);

	gi->SendItemRemoval(mResIdx);	// 맵 리소스가 삭제됨을 서버에 알림

	Destroy();	// 액터 삭제
}

void ARootItem::SetItem(const FGameItem& gi)
{
	Item = gi;
}

void ARootItem::SetResourceIndex(const int idx)
{
	mResIdx = idx;
}

