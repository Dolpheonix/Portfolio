#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "CustomLevelScript.generated.h"

class APreviewCharacter;

UCLASS()
class PORTFOLIO_API ACustomLevelScript : public ALevelScriptActor
{
	GENERATED_BODY()
	
public:
	ACustomLevelScript();

protected:
	virtual void BeginPlay() override;

public:
	TObjectPtr<APreviewCharacter> GetPreviewCharacter();

protected:
	TObjectPtr<APreviewCharacter> mPreviewCharacter;
};
