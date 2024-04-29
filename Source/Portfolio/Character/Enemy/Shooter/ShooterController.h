#pragma once

#include "CoreMinimal.h"
#include "../EnemyController.h"
#include "ShooterController.generated.h"

/*
	AShooterController : AShooter Ŭ������ ����Ǵ� ��Ʈ�ѷ�
*/

UCLASS()
class PORTFOLIO_API AShooterController : public AEnemyController
{
	GENERATED_BODY()
	
public:
	AShooterController(const FObjectInitializer& objectInitializer);
};
