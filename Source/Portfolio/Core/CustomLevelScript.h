#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "CustomLevelScript.generated.h"

class APreviewCharacter;

/*
	레벨 블루프린트가 상속받는 클래스
	- 프리뷰 캐릭터 스폰
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
