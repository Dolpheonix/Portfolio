#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Common/Item.h"
#include "PreviewCharacter.generated.h"

class USceneCaptureComponent2D;
class APlayerCharacter;

/*
	APreviewCharacter : 맵 로드 시 자동으로 스폰되는 캐릭터로, 인벤토리 창의 프리뷰 이미지를 위한 더미 캐릭터
*/

UCLASS()
class PORTFOLIO_API APreviewCharacter : public ACharacter
{

	GENERATED_BODY()

public:
	APreviewCharacter();

protected:
	virtual void BeginPlay() override;

public:
	// 현재 플레이어의 장착 상태와 동기화한다.
	void SyncWithPlayer(TObjectPtr<APlayerCharacter> player);
	// 해당 아이템을 장착
	void SetEquipment(EItemType type, int index);
	// 무기에 맞는 Idle 애니메이션을 재생
	void SetAnimation(EWeaponType type);

protected:
	// 프리뷰용 카메라
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	TObjectPtr<USceneCaptureComponent2D> mPreviewCamera;

	//TODO : Cloth mesh

	// 무기 메시
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> mWeaponMeshComponent;

	// 현재 장착중인 무기의 타입
	EWeaponType mCurrentWeaponType;
};
