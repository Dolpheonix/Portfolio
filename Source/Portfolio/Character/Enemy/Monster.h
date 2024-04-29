#pragma once

#include "CoreMinimal.h"
#include "../EnemyCharacter.h"
#include "Monster.generated.h"

UCLASS()
class PORTFOLIO_API AMonster : public AEnemyCharacter
{
	GENERATED_BODY()
	
public:
	AMonster();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void Spit();
};
