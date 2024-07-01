#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionTypes.h"
#include "CustomCharacter.h"
#include "GenericTeamAgentInterface.h"
#include "../Common/Item.h"
#include "../Common/Quest.h"
#include "PlayerCharacter.generated.h"

class UCustomGameInstance;
class IInteraction;
class UCameraComponent;
class UAudioComponent;
class UWeapon;

/*
	플레이어의 Action 상태
*/
UENUM(BlueprintType)
enum class EPlayerActionMode : uint8
{
	Idle,		// 아무 Action도 취하지 않음
	Attack,		// 공격 중
	Interact,	// 상호작용 중
	Count,
};

/*
	플레이어의 Movement 상태
*/
UENUM(BlueprintType)
enum class EPlayerMovementMode : uint8
{
	Idle,	// 가만히 있는 중
	Walk,	// 걷는 중
	Run,	// 뛰는 중
	Jump,	// 점프 중
	Fall,	// 낙하 중
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
	virtual void EndPlay(const EEndPlayReason::Type type) override;

public:
	virtual void Tick(float DeltaTime) override;
	// 데미지 처리
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	// 플레이어 입력 처리
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// 데미지를 입었을 때 호출
	virtual void OnHurt() override;

// 입력 함수 모음
	// 앞/뒤 이동 (W/S)
	void MoveVertical(float val);
	// 좌/우 이동 (A/D)
	void MoveHorizontal(float val);
	// 상/하 회전 (마우스 상/하)
	void TurnVertical(float val);
	// 좌/우 회전 (마우스 좌/우)
	void TurnHorizontal(float val);
	// 달리기 전환 (Shift 키 누름)
	void StartRun();
	// 걷기 전환 (Shift 키 뗌)
	void StopRun();
	// 점프 (SpaceBar)
	void Jump();
	// 상호작용 시도 (E)
	void TryInteract();
	// 무기 퀵슬롯 전환 (T)
	void TurnQuickSlot();
	// 무기 장착/해제 (R)
	void ChangeEquipment();
	// 공격 (마우스 좌클릭)
	void Attack();
	// 공격 보조 (마우스 우클릭 누름)
	void Subattack();
	// 공격 보조 해제 (마우스 우클릭 뗌)
	void CancleSubattack();
	// 인벤토리 열기 (I)
	void OpenInventory();
	// 메인 메뉴 열기 (Esc)
	void OpenMenu();
	// 퀘스트 창 열기 (Q)
	void OpenQuestTable();
	// 채팅창 열기 (Enter)
	void ShowChattingBox();

	// mCurrentWeaponIndex로 무기 변환
	void ChangeWeapon();
	// 무기의 장착/해제 토글
	void SetEquipment(bool isEquip);
	// 아이템을 획득
	void RootItem(FGameItem item);
	// 아이템을 버림
	void ThrowItem(EItemType type, int index, int num);
	// 상호작용
	void Interact();
	// 상호작용 해제
	void UnInteract();
	// Action이 끝나면 Idle 상태로 돌려놓음
	void EndAction();

	// 퀘스트를 등록
	void CommitQuest(int index);
	// 서브 퀘스트를 등록
	void RegisterSubQuest(int questIndex, int subIndex);
	// 퀘스트 완료처리
	void CompleteQuest(int index);
	// 서브퀘스트 완료처리
	void CompleteSubQuest(int questIndex, int subIndex);
	// 서브퀘스트를 미완료 상태로 돌려놓음
	void RevertSubQuest(int questIndex, int subIndex);
	// 도착 서브퀘 완료 처리
	void ReportArrival(int questIndex, int subIndex);
	// 사냥 서브퀘 업데이트
	void ReportKill(TArray<int> label);
	// 수집 서브퀘 업데이트
	void ReportItem(int infoIndex, int Num);

// Tick Method
	// Hurt 경직 시에 붉은 필터를 업데이트
	void UpdateHurtTimer(float deltaTime);
	// Movement를 업데이트
	void UpdateMovement();
	// 가장 가까운 거리에 있는 상호작용 액터를 Notify
	void UpdateNotifyInteraction();
	// 애니메이션 블루프린트에 사용될 플래그 업데이트
	void UpdateAnimationMode();
	// 이동/회전 유무를 판단해 서버에 전송함
	void CheckMovementChanged();


	// 퍼셉션에 감지되는 액터 처리
	UFUNCTION()
	void ChangeNotification(AActor* source, FAIStimulus stimulus);
	// 퍼셉션 범위에서 벗어나면, 상호작용 가능 액터 리스트에서 삭제
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
	FInventory& GetInventory();
	const FInventory& GetInventory() const;
	FInventory GetInventory_cpy() const;
	TArray<FQuestStatus>& GetQuestTable();
	const TArray<FQuestStatus>& GetQuestTable() const;
	TArray<FQuestStatus> GetQuestTable_cpy() const;

protected:
	// 3인칭 카메라 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	TObjectPtr<UCameraComponent> mCameraComponent;
	// 각종 효과음 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<UAudioComponent> mAudioComponent;
	// 무기 컴포넌트
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UWeapon> mWeapon;
	// 현재 Action
	UPROPERTY(BlueprintReadWrite)
	EPlayerActionMode mActionMode;
	// 현재 Movement
	UPROPERTY(BlueprintReadWrite)
	EPlayerMovementMode mMovementMode;
	// 현재 장착중인 Weapon Type (공격 시에 사용)
	UPROPERTY(BlueprintReadWrite)
	EWeaponType mWeaponType;

	// 상호작용 범위 내의 액터 리스트
	TArray<TObjectPtr<AActor>> mInteractableList;
	// 가장 가까운 상호작용 가능 액터
	IInteraction* mNearestInteractableActor;

// Boolean 값
	// 상호작용 중인가?
	bool bInteracting;
	// 무기 장착 중인가?
	bool bEquipped;
	// 점프 중인가?
	bool bJumping;
	// 달리는 중인가?
	bool bRunning;
	// 경직 상태인가?
	UPROPERTY(BlueprintReadWrite)
	bool bHurt;
	// 애니메이션 BP에서 어떤 State Machine을 사용할 것인가?
	UPROPERTY(BlueprintReadWrite)
	uint8 mLoco_Action_Blend;

	// 현재 의상 인덱스
	int mCurrentClothIndex;
	// 현재 무기 인덱스
	int mCurrentWeaponIndex;

// 플레이어 정보
	// 현재 레벨
	int mCurrentLevel;
	// 현재 골드 보유량
	int mCurrentGold;
	// 현재 위치한 맵 인덱스
	int mCurrentMap;
	// 인벤토리
	FInventory mInventory;
	// 퀘스트 진행 상황
	TArray<FQuestStatus> mQuestTable;

	// 게임 인스턴스
	TObjectPtr<UCustomGameInstance> mGameInstance;
	// 공격 애니메이션 동안 지속되는 타이머
	FTimerHandle mAttackAnimationTimer;
	
	UPROPERTY(ReplicatedUsing=OnRep_Notify)
	float testVal;
	void OnTestValUpdated();
	UFUNCTION()
	void OnRep_Notify();
};
