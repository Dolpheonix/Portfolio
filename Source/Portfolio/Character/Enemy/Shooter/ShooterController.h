#pragma once

#include "CoreMinimal.h"
#include "../EnemyController.h"
#include "ShooterController.generated.h"

/*
	AShooterController : AShooter 클래스에 적용되는 컨트롤러
*/

UCLASS()
class PORTFOLIO_API AShooterController : public AEnemyController
{
	GENERATED_BODY()
	
public:
	AShooterController(const FObjectInitializer& objectInitializer);
};
