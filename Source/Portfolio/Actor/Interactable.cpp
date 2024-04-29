#include "Interactable.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BillboardComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "../Base/LoadHelper.h"

AInteractable::AInteractable()
{
	PrimaryActorTick.bCanEverTick = true;

	mStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainMesh"));
	RootComponent = mStaticMeshComponent;

	mNotifyBillboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("NotifyBillboard"));
	mNotifyBillboard->SetupAttachment(RootComponent);
	const TObjectPtr<UTexture2D> notifyImage = LoadHelper::C_LoadObjectFromPath<UTexture2D>(TEXT("/Game/Texture/WidgetImage/NotifyInteraction.NotifyInteraction"));
	mNotifyBillboard->SetSprite(notifyImage);

	mPerceptionStimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("PerceptionSource"));
	mPerceptionStimuliSourceComponent->bAutoRegister = true;
}

void AInteractable::BeginPlay()
{
	Super::BeginPlay();
	
	mNotifyBillboard->SetHiddenInGame(false);
	mNotifyBillboard->SetVisibility(false);

	mPerceptionStimuliSourceComponent->RegisterForSense(UAISense_Sight::StaticClass());
	mPerceptionStimuliSourceComponent->RegisterWithPerceptionSystem();
}

void AInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInteractable::Notify(TObjectPtr<AActor> player)
{
	mNotifyBillboard->SetVisibility(true);
}

void AInteractable::UnNotify(TObjectPtr<AActor> player)
{
	mNotifyBillboard->SetVisibility(false);
}

void AInteractable::Interact(TObjectPtr<AActor> player)
{
	mInteractionEvent.Broadcast();
}

