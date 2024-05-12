#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionTypes.h"
#include "CustomCharacter.h"
#include "GenericTeamAgentInterface.h"
#include "../Common/Item.h"
#include "../Common/Quest.h"
#include "PlayerCharacter.generated.h"

class IInteraction;
class UCameraComponent;
class UAudioComponent;
class UWeapon;

/*
	EPlayerActionMode : �÷��̾� ĳ������ �ൿ Ÿ�� ����
		- Idle : ������ �ִ� ����
		- Attack : ���� ��
		- Interact : ��ȣ�ۿ� ��

	EPlayerMovementmode : �÷��̾� ĳ������ ������ Ÿ�� ����
		- Idle : ������ �ִ� ����
		- Walk, Run : �̵�(�ȱ�/�ٱ�)
		- Jump : ���� Ű�� ���� ���� ���� ����
		- Fall : ���� ���� ��Ȳ���� ���߿� �� ����

	APlayerCharacter: ���� ������ �÷��̾� ĳ����
		- �ȱ�, ���� ���� �̵� ���
		- �κ��丮
		- NPC, ���þ����� ����� ��ȣ�ۿ� ���
*/

UENUM(BlueprintType)
enum class EPlayerActionMode : uint8
{
	Idle,
	Attack,
	Interact,
	Count,
};

UENUM(BlueprintType)
enum class EPlayerMovementMode : uint8
{
	Idle,
	Walk,
	Run,
	Jump,
	Fall,
	Count,
};

UCLASS()
class PORTFOLIO_API APlayerCharacter : public ACustomCharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual FGenericTeamId GetGenericTeamId() { return FGenericTeamId(0); };
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnHurt() override;

// �Է� ���ε��� �Լ�
	// ��/�� �̵� (W/S �� ����)
	void MoveVertical(float val);
	// ��/�� �̵� (A/D �� ����)
	void MoveHorizontal(float val);
	// ��/�� ī�޶� �̵� (���콺 ���� ��)
	void TurnVertical(float val);
	// ��/�� ī�޶� �̵� (���콺 ���� ��)
	void TurnHorizontal(float val);
	// �̵� ��, �޸��� Ȱ��ȭ (Shift Ű ����)
	void StartRun();
	// �̵� ��, �޸��� ��Ȱ��ȭ (Shift Ű ��)
	void StopRun();
	// ���� (�����̽��� ����)
	void Jump();
	// ��ȣ �ۿ� �õ� (E Ű ����)
	void TryInteract();
	// ���� ��ȯ ������ (R Ű ����)
	void TurnQuickSlot();
	// ���� ����/���� (E Ű ����)
	void ChangeEquipment();
	// ���� (���콺 ��Ŭ��)
	void Attack();
	// ���� ���� (���콺 ��Ŭ�� ����)
	void Subattack();
	// ���� ���� ���� (���콺 ��Ŭ�� ��)
	void CancleSubattack();
	// �κ��丮 ui ���� (I Ű ����)
	void OpenInventory();
	// �޴� â ���� (Esc Ű ����)
	void OpenMenu();
	// ����Ʈ â ����
	void OpenQuestTable();
	// ä��â ���� (���� Ű ����)
	void ShowChattingBox();

	// mCurrentWeaponIndex�� ����� ����(���� ���·�)	
	void ChangeWeapon();
	// ���� ������ ���� ���¸� ����
	void SetEquipment(bool isEquip);
	// �������� �κ��丮�� �߰�
	void RootItem(FGameItem item);
	// �κ��丮 ���� Ư�� �������� ����
	void ThrowItem(EItemType type, int index, int num);
	// ���� ����� ��ȣ�ۿ� ������Ʈ�� ��ȣ �ۿ�
	void Interact();
	// ��ȣ �ۿ� ����
	void UnInteract();
	// ���� �ִϸ��̼��� ������ ȣ���
	void EndAction();

	// ����Ʈ ���
	void CommitQuest(int index);
	// ���� ����Ʈ ���
	void RegisterSubQuest(int questIndex, int subIndex);
	// ����Ʈ �Ϸ�
	void CompleteQuest(int index);
	// ���� ����Ʈ�� �Ϸ�
	void CompleteSubQuest(int questIndex, int subIndex);
	// ���� ����Ʈ�� �ٽ� �̿Ϸ� ���·� ��������
	void RevertSubQuest(int questIndex, int subIndex);
	// Arrival ����Ʈ�� �Ϸ��Ŵ
	void ReportArrival(int questIndex, int subIndex);
	// �ش� ���� ���� óġ�ϴ� ����Ʈ ���¸� ������Ʈ��
	void ReportKill(TArray<int> label);
	// �ش� �������� �����ϴ� ����Ʈ ���¸� ������Ʈ��
	void ReportItem(int infoIndex, int Num);

// Tick Method
	// �������� �Ծ��� ��, ���� ���¿� ȭ�� ���� ���� ���͸� �����.
	void UpdateHurtTimer(float deltaTime);
	// ���� ĳ������ �̵� ��Ȳ�� ���� Movment ��带 ����
	void UpdateMovement();
	// ���� ����� ��ȣ�ۿ� ������Ʈ�� ��ȣ�ۿ� �������� �˸�
	void UpdateNotifyInteraction();
	// Animation Bp �󿡼� � ������Ʈ �ӽ��� ����� �� ����
	void UpdateAnimationMode();

	// ��ȣ�ۿ� ���� ������Ʈ�� �����Ǹ�, �˸� �̹����� ������Ʈ
	UFUNCTION()
	void ChangeNotification(AActor* source, FAIStimulus stimulus);
	// �þ߿��� ����� ����Ʈ���� ����
	void RemoveFromInteractableList(TObjectPtr<AActor> actor);

	// Setter
	void SetActionMode(const EPlayerActionMode& e);
	void SetInteracting(const bool b);
	void SetCurrentCloth(int index);
	void SetCurrentWeapon(int index);
	void SetCurrentGold(int gold);

	// Getter
	EPlayerActionMode GetActionMode() const;
	bool GetInteracting() const;
	int GetCurrentCloth() const;
	int GetCurrentWeapon() const;
	EWeaponType GetCurrentWeaponType() const;
	int GetCurrentGold() const;
	int GetCurrentLevel() const;
	int GetCurrentMapIndex() const;
	int GetSlotIndex() const;
	FInventory& GetInventory();
	const FInventory& GetInventory() const;
	FInventory GetInventory_cpy() const;
	TArray<FQuestStatus>& GetQuestTable();
	const TArray<FQuestStatus>& GetQuestTable() const;
	TArray<FQuestStatus> GetQuestTable_cpy() const;

protected:
	// 3��Ī ī�޶�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	TObjectPtr<UCameraComponent> mCameraComponent;
	// �߻��� ����� �����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<UAudioComponent> mAudioComponent;
	// ���� �������� ���� ������Ʈ
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UWeapon> mWeapon;
	// ���� Action ���
	UPROPERTY(BlueprintReadWrite)
	EPlayerActionMode mActionMode;
	// ���� Movement ���
	UPROPERTY(BlueprintReadWrite)
	EPlayerMovementMode mMovementMode;
	// ���� �������� ������ Ÿ��
	UPROPERTY(BlueprintReadWrite)
	EWeaponType mWeaponType;

	// ��ȣ �ۿ� ������ ���� ����Ʈ
	TArray<TObjectPtr<AActor>> mInteractableList;
	// ���� ����� ��ȣ�ۿ� ����
	IInteraction* mNearestInteractableActor;

// ���� �οﰪ
	// ��ȣ�ۿ� ���ΰ�?
	bool bInteracting;
	// ���⸦ ���� ���ΰ�?
	bool bEquipped;
	// ���� ���ΰ�?
	bool bJumping;
	// �޸��� ���ΰ�?
	bool bRunning;
	// �������� ���� �����ΰ�?
	UPROPERTY(BlueprintReadWrite)
	bool bHurt;
	// Locomotive/Action/Blend (Animation BP�� � ������Ʈ �ӽ��� ����� ��)
	UPROPERTY(BlueprintReadWrite)
	uint8 mLoco_Action_Blend;

	int mCurrentClothIndex;
	// ���� ���õ� ���� �ε��� (�κ��丮 ��)
	int mCurrentWeaponIndex;

// ĳ���� State
	// ���� ����
	int mCurrentLevel;
	// ���� ���
	int mCurrentGold;
	// ���� ��ġ�� �� �ε���
	int mCurrentMap;
	// ���� ������� ���� ����
	int mSlotIndex;
	// ĳ���� �κ��丮
	FInventory mInventory;
	// ����Ʈ ���� ����
	TArray<FQuestStatus> mQuestTable;

// ETC
	FTimerHandle mAttackAnimationTimer;

	UPROPERTY(ReplicatedUsing=OnRep_Notify)
	float testVal;

	void OnTestValUpdated();
	UFUNCTION()
	void OnRep_Notify();
};
