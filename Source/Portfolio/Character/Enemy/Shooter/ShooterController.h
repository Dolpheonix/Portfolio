#pragma once

#include "CoreMinimal.h"
#include "../EnemyController.h"
#include "ShooterController.generated.h"

/*
	Shooter 몹 전용 컨트롤러
	- 비헤이비어 트리, 블랙보드 설정
*/

UCLASS()
class PORTFOLIO_API AShooterController : public AEnemyController
{
	GENERATED_BODY()
	
public:
	AShooterController(const FObjectInitializer& objectInitializer);
};
