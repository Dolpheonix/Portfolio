#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"

UENUM(BlueprintType)
enum class EInventorySpace : uint8
{
	noItem,
	Apple,
	End,
};

UENUM(BlueprintType)
enum class EArmorySpace : uint8
{
	Fist,
	Gun,
	End,
};


