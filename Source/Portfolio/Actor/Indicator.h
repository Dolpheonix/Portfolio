#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Indicator.generated.h"

class UBoxComponent;
class UParticleSystemComponent;

UCLASS()
class PORTFOLIO_API AIndicator : public AActor
{
	GENERATED_BODY()
	
public:	
	AIndicator();

	UFUNCTION()
	void OnOverlapped_CheckArrival(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void RegisterQuest(int questIndex, int subIndex);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UBoxComponent> mVolumeComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParticleSystemComponent> mParticleComponent;
	
	int mQuestIndex;
	int mSubIndex;
};
