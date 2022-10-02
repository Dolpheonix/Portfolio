// Common Level Design Logic

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "LevelActor.generated.h"

/**
 * 
 */
UCLASS()
class QUICKSTART_API ALevelActor : public ALevelScriptActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, Category = "Level_Common")
	float deadPoint = 0.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Level_Common")
	FVector startLocation;

	UPROPERTY(BlueprintReadWrite, Category = "Level_Common")
	FRotator startRotation;

	UPROPERTY(BlueprintReadWrite, Category = "Level_Common")
	bool bIsDead = false;
};
