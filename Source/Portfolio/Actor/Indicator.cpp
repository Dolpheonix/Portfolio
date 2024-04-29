#include "Indicator.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "../Base/LoadHelper.h"
#include "../Character/PlayerCharacter.h"

AIndicator::AIndicator()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	mVolumeComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Volume"));
	mVolumeComponent->SetupAttachment(RootComponent);
	mVolumeComponent->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	mVolumeComponent->SetGenerateOverlapEvents(true);
	mVolumeComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	mVolumeComponent->SetCollisionProfileName("Aura");

	mParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle"));
	mParticleComponent->SetTemplate(LoadHelper::C_LoadObjectFromPath<UParticleSystem>(TEXT("/Game/FX/PortalFX.PortalFX")));
	mParticleComponent->SetupAttachment(RootComponent);
	mParticleComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
}

void AIndicator::OnOverlapped_CheckArrival(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1,5,FColor::Red, "overlapped");
	TObjectPtr<APlayerCharacter> player = Cast<APlayerCharacter>(OtherActor);
	if (player)
	{
		player->ReportArrival(mQuestIndex, mSubIndex);

		Destroy();
	}
}

void AIndicator::RegisterQuest(int questIndex, int subIndex)
{
	mQuestIndex = questIndex;
	mSubIndex = subIndex;

	mVolumeComponent->OnComponentBeginOverlap.AddDynamic(this, &AIndicator::OnOverlapped_CheckArrival);
}

