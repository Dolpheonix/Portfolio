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
	EPlayerActionMode : 플레이어 캐릭터의 행동 타입 정의
		- Idle : 가만히 있는 상태
		- Attack : 공격 중
		- Interact : 상호작용 중

	EPlayerMovementmode : 플레이어 캐릭터의 움직임 타입 정의
		- Idle : 가만히 있는 상태
		- Walk, Run : 이동(걷기/뛰기)
		- Jump : 점프 키를 통해 점프 중인 상태
		- Fall : 점프 외의 상황에서 공중에 뜬 상태

	APlayerCharacter: 조작 가능한 플레이어 캐릭터
		- 걷기, 점프 등의 이동 기능
		- 인벤토리
		- NPC, 루팅아이템 등과의 상호작용 기능
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

// 입력 바인딩용 함수
	// 앞/뒤 이동 (W/S 축 매핑)
	void MoveVertical(float val);
	// 오/왼 이동 (A/D 축 매핑)
	void MoveHorizontal(float val);
	// 상/하 카메라 이동 (마우스 수직 축)
	void TurnVertical(float val);
	// 오/왼 카메라 이동 (마우스 수평 축)
	void TurnHorizontal(float val);
	// 이동 중, 달리기 활성화 (Shift 키 누름)
	void StartRun();
	// 이동 중, 달리기 비활성화 (Shift 키 뗌)
	void StopRun();
	// 점프 (스페이스바 누름)
	void Jump();
	// 상호 작용 시도 (E 키 누름)
	void TryInteract();
	// 무기 변환 퀵슬롯 (R 키 누름)
	void TurnQuickSlot();
	// 무기 장착/해제 (E 키 누름)
	void ChangeEquipment();
	// 공격 (마우스 좌클릭)
	void Attack();
	// 공격 보조 (마우스 우클릭 누름)
	void Subattack();
	// 공격 보조 해제 (마우스 우클릭 뗌)
	void CancleSubattack();
	// 인벤토리 ui 열기 (I 키 누름)
	void OpenInventory();
	// 메뉴 창 열기 (Esc 키 누름)
	void OpenMenu();
	// 퀘스트 창 열기
	void OpenQuestTable();
	// 채팅창 열기 (엔터 키 누름)
	void ShowChattingBox();

	// mCurrentWeaponIndex의 무기로 변경(장착 상태로)	
	void ChangeWeapon();
	// 현재 무기의 장착 상태를 변경
	void SetEquipment(bool isEquip);
	// 아이템을 인벤토리에 추가
	void RootItem(FGameItem item);
	// 인벤토리 내의 특정 아이템을 버림
	void ThrowItem(EItemType type, int index, int num);
	// 가장 가까운 상호작용 오브젝트와 상호 작용
	void Interact();
	// 상호 작용 해제
	void UnInteract();
	// 공격 애니메이션이 끝나면 호출됨
	void EndAction();

	// 퀘스트 등록
	void CommitQuest(int index);
	// 서브 퀘스트 등록
	void RegisterSubQuest(int questIndex, int subIndex);
	// 퀘스트 완료
	void CompleteQuest(int index);
	// 서브 퀘스트를 완료
	void CompleteSubQuest(int questIndex, int subIndex);
	// 서브 퀘스트를 다시 미완료 상태로 돌려놓음
	void RevertSubQuest(int questIndex, int subIndex);
	// Arrival 퀘스트를 완료시킴
	void ReportArrival(int questIndex, int subIndex);
	// 해당 라벨의 몹을 처치하는 퀘스트 상태를 업데이트함
	void ReportKill(TArray<int> label);
	// 해당 아이템을 수집하는 퀘스트 상태를 업데이트함
	void ReportItem(int infoIndex, int Num);

// Tick Method
	// 데미지를 입었을 때, 경직 상태와 화면 상의 붉은 필터를 재생함.
	void UpdateHurtTimer(float deltaTime);
	// 현재 캐릭터의 이동 상황에 따라 Movment 모드를 결정
	void UpdateMovement();
	// 가장 가까운 상호작용 오브젝트와 상호작용 가능함을 알림
	void UpdateNotifyInteraction();
	// Animation Bp 상에서 어떤 스테이트 머신을 사용할 지 결정
	void UpdateAnimationMode();

	// 상호작용 가능 오브젝트가 감지되면, 알림 이미지를 업데이트
	UFUNCTION()
	void ChangeNotification(AActor* source, FAIStimulus stimulus);
	// 시야에서 벗어나면 리스트에서 삭제
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
	// 3인칭 카메라
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	TObjectPtr<UCameraComponent> mCameraComponent;
	// 발사음 재생용 오디오
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<UAudioComponent> mAudioComponent;
	// 현재 장착중인 무기 컴포넌트
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UWeapon> mWeapon;
	// 현재 Action 모드
	UPROPERTY(BlueprintReadWrite)
	EPlayerActionMode mActionMode;
	// 현재 Movement 모드
	UPROPERTY(BlueprintReadWrite)
	EPlayerMovementMode mMovementMode;
	// 현재 장착중인 무기의 타입
	UPROPERTY(BlueprintReadWrite)
	EWeaponType mWeaponType;

	// 상호 작용 가능한 액터 리스트
	TArray<TObjectPtr<AActor>> mInteractableList;
	// 가장 가까운 상호작용 액터
	IInteraction* mNearestInteractableActor;

// 상태 부울값
	// 상호작용 중인가?
	bool bInteracting;
	// 무기를 장착 중인가?
	bool bEquipped;
	// 점프 중인가?
	bool bJumping;
	// 달리는 중인가?
	bool bRunning;
	// 데미지를 입은 상태인가?
	UPROPERTY(BlueprintReadWrite)
	bool bHurt;
	// Locomotive/Action/Blend (Animation BP의 어떤 스테이트 머신을 사용할 지)
	UPROPERTY(BlueprintReadWrite)
	uint8 mLoco_Action_Blend;

	int mCurrentClothIndex;
	// 현재 선택된 무기 인덱스 (인벤토리 상)
	int mCurrentWeaponIndex;

// 캐릭터 State
	// 현재 레벨
	int mCurrentLevel;
	// 현재 골드
	int mCurrentGold;
	// 현재 위치한 맵 인덱스
	int mCurrentMap;
	// 현재 사용중인 저장 슬롯
	int mSlotIndex;
	// 캐릭터 인벤토리
	FInventory mInventory;
	// 퀘스트 진행 상태
	TArray<FQuestStatus> mQuestTable;

// ETC
	FTimerHandle mAttackAnimationTimer;

	UPROPERTY(ReplicatedUsing=OnRep_Notify)
	float testVal;

	void OnTestValUpdated();
	UFUNCTION()
	void OnRep_Notify();
};
