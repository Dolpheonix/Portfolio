#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "CustomLevelScript.generated.h"

class APreviewCharacter;

/*
	ACustomLevelScript : 게임에 사용되는 레벨 액터 클래스입니다.
	- 인벤토리에 표시할 프리뷰용 캐릭터를 스폰합니다.
*/

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
