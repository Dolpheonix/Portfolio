#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Common/Item.h"
#include "PreviewCharacter.generated.h"

class USceneCaptureComponent2D;
class APlayerCharacter;

/*
	인벤토리 UI에서 무기/의상 장착 시 보여줄 프리뷰 캐릭터. 맵 아래쪽에 자동 스폰됨
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
	// 플레이어 캐릭터의 착장 상태와 동기화
	void SyncWithPlayer(TObjectPtr<APlayerCharacter> player);
	// 무기/의상을 장착
	void SetEquipment(EItemType type, int index);
	// 애니메이션 설정 (무기 타입에 따라 다름)
	void SetAnimation(EWeaponType type);

protected:
	// 외형을 보여줄 카메라
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	TObjectPtr<USceneCaptureComponent2D> mPreviewCamera;

	// 무기 메쉬 컴포넌트
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> mWeaponMeshComponent;

	// 현재 장착중인 무기의 타입
	EWeaponType mCurrentWeaponType;
};
