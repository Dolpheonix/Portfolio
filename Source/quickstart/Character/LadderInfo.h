#pragma once

#include "CoreMinimal.h"
#include "LadderInfo.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FLadderInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool onLadder;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Slope;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Width;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool dirty;
};