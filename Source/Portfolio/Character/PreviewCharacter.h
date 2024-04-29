#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Common/Item.h"
#include "PreviewCharacter.generated.h"

class USceneCaptureComponent2D;
class APlayerCharacter;

/*
	APreviewCharacter : �� �ε� �� �ڵ����� �����Ǵ� ĳ���ͷ�, �κ��丮 â�� ������ �̹����� ���� ���� ĳ����
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
	// ���� �÷��̾��� ���� ���¿� ����ȭ�Ѵ�.
	void SyncWithPlayer(TObjectPtr<APlayerCharacter> player);
	// �ش� �������� ����
	void SetEquipment(EItemType type, int index);
	// ���⿡ �´� Idle �ִϸ��̼��� ���
	void SetAnimation(EWeaponType type);

protected:
	// ������� ī�޶�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	TObjectPtr<USceneCaptureComponent2D> mPreviewCamera;

	//TODO : Cloth mesh

	// ���� �޽�
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> mWeaponMeshComponent;

	// ���� �������� ������ Ÿ��
	EWeaponType mCurrentWeaponType;
};
