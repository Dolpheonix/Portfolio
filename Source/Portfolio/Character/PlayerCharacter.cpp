#include "PlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Animation/AnimNode_StateMachine.h"
#include "Kismet/GameplayStatics.h"
#include "../Actor/Indicator.h"
#include "../Base/LoadHelper.h"
#include "../Component/Weapon.h"
#include "../Common/Interface/Interaction.h"
#include "../Common/Enemy.h"
#include "../Core/CustomGameInstance.h"
#include "../Core/CustomController.h"
#include "../Core/PlayerSaveObject.h"

#define EPSILON_ZERO 0.000003

constexpr float HURT_FREEZE_TIME = 1.0f;	// Hurt 시 경직 시간
constexpr float CAMERA_LOC_X = -150.f;		// 카메라 위치
constexpr float CAMERA_LOC_Y = 0.f;
constexpr float CAMERA_LOC_Z = 88.f;
constexpr float CAMERA_ROT_PITCH = -20.f;	// 카메라 회전
constexpr float CAMERA_ROT_YAW = 0.f;
constexpr float CAMERA_ROT_ROLL = 0.f;
constexpr float SHOT_ANIM_TIME = 0.7f;		// 공격 애니메이션 지속 시간
constexpr float SEND_LOCATION_THRESHOLD = 10.0f;	// 이동으로 판단하고, 서버에 이동 정보를 전송할 최소 거리
constexpr float SEND_ROTATION_THRESHOLD = 5.0f;		// 회전으로 판단하고, 서버에 이동 정보를 전송할 최소 각도

static_assert(static_cast<uint8>(EPlayerActionMode::Count) == 3);
static_assert(static_cast<uint8>(EPlayerMovementMode::Count) == 5);

APlayerCharacter::APlayerCharacter() : testVal(0.0f), 
									   mActionMode(EPlayerActionMode::Idle), mMovementMode(EPlayerMovementMode::Idle), mWeaponType(EWeaponType::Fist),
									   bInteracting(false), bEquipped(false), bJumping(false), bRunning(false), bHurt(false),
									   mCurrentClothIndex(-1), mCurrentWeaponIndex(-1),
									   mLoco_Action_Blend(0), 
									   mCurrentGold(10000)
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Character"));
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);

	GetMesh()->SetSkeletalMesh(LoadHelper::C_LoadObjectFromPath<USkeletalMesh>(TEXT("/Game/Character/Mannequins/Meshes/SKM_Manny.SKM_Manny")));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetAnimInstanceClass(LoadHelper::C_LoadObjectFromPath<UBlueprintGeneratedClass>(TEXT("/Game/Character/Mannequins/Animations/ABP_Mannequin.ABP_Mannequin_C")));
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetupAttachment(RootComponent);

	mCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdCamera"));
	mCameraComponent->SetRelativeLocation(FVector(CAMERA_LOC_X, CAMERA_LOC_Y, CAMERA_LOC_Z));
	mCameraComponent->SetRelativeRotation(FRotator(CAMERA_ROT_PITCH, CAMERA_ROT_YAW, CAMERA_ROT_ROLL));
	mCameraComponent->bUsePawnControlRotation = true;
	mCameraComponent->SetupAttachment(RootComponent);

	mWeapon = CreateDefaultSubobject<UWeapon>(TEXT("Weapon"));
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayerCharacter, testVal);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	TObjectPtr<UCustomGameInstance> gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(gi);
	mGameInstance = gi;

	// ��Ʈ�� UI ǥ�� �߿��� �Ʒ��� ������ ������ �ʿ䰡 ����.
	if (mGameInstance->IsIntro() == true)
	{
		return;
	}

	TObjectPtr<ACustomController> controller = Cast<ACustomController>(GetController());
	check(controller);

	TObjectPtr<UAIPerceptionComponent> perception = controller->GetPerceptionComponent();
	check(perception);

	perception->OnTargetPerceptionUpdated.AddDynamic(this, &APlayerCharacter::ChangeNotification);

	// Intro UI�� �����ִٸ� �ݰ�, HUD UI�� ǥ����
	controller->CloseIntro();
	controller->OpenHUD(this);

	// �� �̵� �ÿ��� ���� �ν��Ͻ��� �÷��̾� ������ �ӽ÷� �����ϰ� �ִٰ�, �÷��̾� ���� �� �ش� ���� �޾� �����Ѵ�.
	const PlayerInfo tempInfo = mGameInstance->GetPlayerInfo();
	mDisplayName = tempInfo.PlayerName;
	mCurrentLevel = tempInfo.CurrentLevel;
	mCurrentGold = tempInfo.CurrentGold;
	mCurrentMap = tempInfo.CurrentMap;
	mQuestTable = tempInfo.QuestTable;
	mInventory = tempInfo.Inventory;

	// location�� Z ���� 0���� ������ �⺻ ���� ��ġ�� ������Ų��.
	if (tempInfo.CurrentLocation.Z >= 0.f)
	{
		SetActorLocation(tempInfo.CurrentLocation);
	}
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type type)
{
	switch (type)
	{
	// 게임 인스턴스에 플레이어 정보를 옮기고 액터 삭제
	case EEndPlayReason::LevelTransition:	// 레벨 전환
	case EEndPlayReason::EndPlayInEditor:	// 에디터에서 종료 시
	case EEndPlayReason::Quit:				// 게임 종료 시
	{
		check(mGameInstance);

		PlayerInfo saveInfo;
		saveInfo.PlayerName = mDisplayName;
		saveInfo.CurrentLevel = mCurrentLevel;
		saveInfo.CurrentGold = mCurrentGold;
		saveInfo.CurrentMap = mCurrentMap;
		saveInfo.QuestTable = mQuestTable;
		saveInfo.Inventory = mInventory;
		saveInfo.CurrentLocation = GetActorLocation();

		mGameInstance->SetPlayerInfo(saveInfo);
	}
	default:
		break;
	}

	Super::EndPlay(type);
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 경직 상태일 경우, 붉은 필터 적용 및 업데이트
	if (bHurt == true)
	{
		UpdateHurtTimer(DeltaTime);
	}
	
	UpdateMovement();
	UpdateNotifyInteraction();
	UpdateAnimationMode();
}

float APlayerCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float dmg = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	
	TObjectPtr<ACustomController> controller = Cast<ACustomController>(GetController());
	check(controller);	
	// HP 바 위젯 업데이트
	controller->UpdateHealthBar();

	return dmg;
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveVertical", this, &APlayerCharacter::MoveVertical);
	PlayerInputComponent->BindAxis("MoveHorizontal", this, &APlayerCharacter::MoveHorizontal);
	PlayerInputComponent->BindAxis("TurnVertical", this, &APlayerCharacter::TurnVertical);
	PlayerInputComponent->BindAxis("TurnHorizontal", this, &APlayerCharacter::TurnHorizontal);

	PlayerInputComponent->BindAction("StartRun", IE_Pressed, this, &APlayerCharacter::StartRun);
	PlayerInputComponent->BindAction("StopRun", IE_Released, this, &APlayerCharacter::StopRun);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::Jump);

	PlayerInputComponent->BindAction("TryInteract", IE_Pressed, this, &APlayerCharacter::TryInteract);

	PlayerInputComponent->BindAction("QuickSlot", IE_Pressed, this, &APlayerCharacter::TurnQuickSlot);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &APlayerCharacter::ChangeEquipment);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &APlayerCharacter::Attack);
	PlayerInputComponent->BindAction("Subattack", IE_Pressed, this, &APlayerCharacter::Subattack);
	PlayerInputComponent->BindAction("CancleSubattack", IE_Released, this, &APlayerCharacter::CancleSubattack);

	PlayerInputComponent->BindAction("OpenInventory", IE_Pressed, this, &APlayerCharacter::OpenInventory);
	PlayerInputComponent->BindAction("OpenMenu", IE_Pressed, this, &APlayerCharacter::OpenMenu);
	PlayerInputComponent->BindAction("OpenQuest", IE_Pressed, this, &APlayerCharacter::OpenQuestTable);

	PlayerInputComponent->BindAction("ShowChattingBox", IE_Pressed, this, &APlayerCharacter::ShowChattingBox);
}

void APlayerCharacter::OnHurt()
{
	bHurt = true;
}

void APlayerCharacter::MoveVertical(float val)
{
	const FVector forwardDir = GetActorForwardVector();
	AddMovementInput(forwardDir, val);
}

void APlayerCharacter::MoveHorizontal(float val)
{
	const FVector rightDir = GetActorRightVector();
	AddMovementInput(rightDir, val);
}

void APlayerCharacter::TurnVertical(float val)
{
	AddControllerPitchInput(val);
}

void APlayerCharacter::TurnHorizontal(float val)
{
	AddControllerYawInput(val);
}

void APlayerCharacter::StartRun()
{
	bRunning = true;
	GetCharacterMovement()->MaxWalkSpeed = 1200.f;

	mGameInstance->SendRunning(true);	// 서버에 Running 상태로 바뀜을 알림 (리플리케이션 캐릭터에 애니메이션 적용)
}

void APlayerCharacter::StopRun()
{
	bRunning = false;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;

	mGameInstance->SendRunning(false);	// 서버에 Walking 상태로 바뀜을 알림 (리플리케이션 캐릭터에 애니메이션 적용)
}


void APlayerCharacter::Jump()
{
	// 낙하중에는 점프키가 먹히지 않음
	if ((GetCharacterMovement()->IsFalling() == true))
	{
		return;
	}

	bPressedJump = true;
	bJumping = true;

	mGameInstance->SendJumping(true);	// 서버에 Jumping 상태로 바뀜을 알림 (리플리케이션 캐릭터에 애니메이션 적용)
}

void APlayerCharacter::TryInteract()
{
	// 가장 가까운 상호작용 액터와 상호작용
	if (mNearestInteractableActor)
	{
		Interact();
	}
}

void APlayerCharacter::TurnQuickSlot()
{
	// 가진 무기가 있어야 퀵슬롯 전환 가능
	if (mCurrentWeaponIndex < 0)
	{
		return;
	}

	// 인벤토리상 다음 무기로 전환
	const int weaponNum = mInventory.GetTypeInventory(EItemType::Weapon).ItemList.Num();
	if (weaponNum > 1)
	{
		mCurrentWeaponIndex = (mCurrentWeaponIndex + 1) % weaponNum;
		ChangeWeapon();
	}
}

void APlayerCharacter::ChangeEquipment()
{
	if (mCurrentWeaponIndex < 0)
	{
		return;
	}

	// 무기 장착/해제 토글
	SetEquipment(!bEquipped);
}

void APlayerCharacter::Attack()
{
	// 무기 장착 해제 상태 OR 무기가 없는 상태
	if (mWeaponType == EWeaponType::Fist)
	{
		return;
	}

	// 이동 중에는 무기 사용 불가
	if (mMovementMode != EPlayerMovementMode::Idle)
	{
		return;
	}

	// 이중 공격 불가
	if (mActionMode == EPlayerActionMode::Attack)
	{
		return;
	}

	mActionMode = EPlayerActionMode::Attack;

	// Rifle 무기 사용 시 공격 애니메이션 동안 공격 상태를 유지
	if(mWeaponType == EWeaponType::Rifle)
	{
		GetWorldTimerManager().SetTimer(mAttackAnimationTimer, this, &APlayerCharacter::EndAction, 0.5f, false, SHOT_ANIM_TIME);
	}

	mWeapon->Attack();
}

void APlayerCharacter::Subattack()
{
	switch (mWeaponType)
	{
	case EWeaponType::Fist:
		break;
	case EWeaponType::Rifle:
	{
		// 카메라를 총구 위치로 변경 --> 조준경 효과
		const FVector& muzzleLocation = mWeapon->GetSocketLocation(TEXT("Muzzle"));
		mCameraComponent->SetWorldLocation(muzzleLocation);
		break;
	}
	default:
		break;
	}
}

void APlayerCharacter::CancleSubattack()
{
	switch (mWeaponType)
	{
	case EWeaponType::Fist:
		break;
	case EWeaponType::Rifle:
	{
		// 카메라를 원래 위치로 돌려놓음
		mCameraComponent->SetRelativeLocation(FVector(CAMERA_LOC_X, CAMERA_LOC_Y, CAMERA_LOC_Z));
		break;
	}
	default:
		break;
	}
}

void APlayerCharacter::OpenInventory()
{
	// 인벤토리 UI를 엶
	TObjectPtr<ACustomController> controller = Cast<ACustomController>(UGameplayStatics::GetPlayerController(this, 0));
	check(controller);
	controller->OpenInventory(this);
}

void APlayerCharacter::OpenMenu()
{
	TObjectPtr<ACustomController> controller = Cast<ACustomController>(UGameplayStatics::GetPlayerController(this, 0));
	check(controller);
	controller->OpenMenu(this);
}

void APlayerCharacter::OpenQuestTable()
{
	TObjectPtr<ACustomController> controller = Cast<ACustomController>(UGameplayStatics::GetPlayerController(this, 0));
	check(controller);
	controller->OpenQuestTable(this);
}

void APlayerCharacter::ShowChattingBox()
{
	TObjectPtr<ACustomController> controller = Cast<ACustomController>(UGameplayStatics::GetPlayerController(this, 0));
	check(controller);
	controller->OpenChattingBox();
}

void APlayerCharacter::ChangeWeapon()
{
	check(mWeapon);

	if (mCurrentWeaponIndex < 0)
	{
		mWeapon->SetVisibility(false);

		mGameInstance->SendEquipmentChange(EItemType::Weapon, -1);
	}
	else
	{
		// mCurrentWepaonIndex에 맞는 무기로 변경
		const int itemInfoIndex = mInventory.GetTypeInventory(EItemType::Weapon).ItemList[mCurrentWeaponIndex].InfoIndex;
		mWeapon->LoadFromItemInfo(itemInfoIndex);
		mWeapon->SetVisibility(true);

		mWeaponType = mWeapon->GetType();
		
		// 변경 초기에는 장착 상태로 설정
		SetEquipment(true);

		mGameInstance->SendEquipmentChange(EItemType::Weapon, itemInfoIndex);
	}

	TObjectPtr<ACustomController> controller = Cast<ACustomController>(GetController());
	check(controller);
	// 퀵슬롯 위젯 이미지 변경
	controller->UpdateWeaponImage();
}

void APlayerCharacter::SetEquipment(bool isEquip)
{
	// 장착/해제 상태에 맞는 소켓에 Attach
	FString socketStr;
	if (isEquip == true)
	{
		socketStr = "Equipped_" + mWeapon->GetWeaponName();
		mWeaponType = mWeapon->GetType();
	}
	else
	{
		socketStr = "Unequipped_" + mWeapon->GetWeaponName();
		mWeaponType = EWeaponType::Fist;
	}

	bEquipped = isEquip;
	mWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName(socketStr));

	mGameInstance->SendEquipping(isEquip);	// 서버에 장착/해제 상태 변화를 알림
}

void APlayerCharacter::RootItem(FGameItem item)
{
	auto& itemInfoList = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this))->GetItemInfoList();
	
	check(item.InfoIndex < itemInfoList.Num());

	auto& itemInfo = itemInfoList[item.InfoIndex];
	const int idx = item.InfoIndex;
	auto foundPtr = mInventory.GetTypeInventory(itemInfo.Type).ItemList.FindByPredicate([idx](const FGameItem& i) 
		{
			return (i.InfoIndex == idx);
		});

	if (foundPtr)
	{
		// 인벤토리에 있는 아이템 --> 개수 업데이트 및 수집 서브퀘 업데이트
		foundPtr->Num += item.Num;

		ReportItem(foundPtr->InfoIndex, foundPtr->Num);
	}
	else
	{
		// 인벤토리에 업는 아이템 --> 인벤토리에 추가
		mInventory.GetTypeInventory(itemInfo.Type).ItemList.Add(item);

		ReportItem(item.InfoIndex, item.Num);

		if ((itemInfo.Type == EItemType::Weapon) && (mInventory.GetTypeInventory(EItemType::Weapon).ItemList.Num() == 1))
		{
			// 무기 아이템 획득 AND 기존 무기가 없음 ==> 바로 해당 무기 장착
			mCurrentWeaponIndex = 0;
			ChangeWeapon();
		}
	}
}

void APlayerCharacter::ThrowItem(EItemType type, int index, int num)
{
	FGameItem& item = mInventory.TypeInventoryList[static_cast<uint8>(type)].ItemList[index];

	check(num <= item.Num);

	// 보유량과 버리는 양이 같음 --> 인벤토리에서 아이템 삭제
	if (item.Num == num)
	{
		mInventory.TypeInventoryList[static_cast<uint8>(type)].ItemList.RemoveAt(index);

		ReportItem(index, 0);	// 수집 서브퀘 업데이트
		// 현재 장착중인 무기가 삭제됨 --> 무기 변환
		if ((type == EItemType::Weapon) && (mCurrentWeaponIndex == index))
		{
			if (mInventory.TypeInventoryList[static_cast<uint8>(type)].ItemList.Num() == 0)
			{
				mCurrentWeaponIndex = -1;
				ChangeWeapon();
			}
			else
			{
				ChangeWeapon();
			}
		}
	}
	// 버려도 아이템 개수가 0이 아님 --> 개수만 업데이트
	else
	{
		item.Num -= num;

		ReportItem(index, item.Num);
	}
}

void APlayerCharacter::Interact()
{
	// 점프, 낙하 중에는 상호작용 불가능
	if ((mMovementMode == EPlayerMovementMode::Jump) || (mMovementMode == EPlayerMovementMode::Fall))
	{
		return;
	}
	// 가장 가까운 액터와 상호작용
	if (mNearestInteractableActor)
	{
		mActionMode = EPlayerActionMode::Interact;
		bInteracting = true;

		mNearestInteractableActor->Interact(this);
	}
}

void APlayerCharacter::UnInteract()
{
	mActionMode = EPlayerActionMode::Idle;
	bInteracting = false;
}

void APlayerCharacter::EndAction()
{
	GetWorldTimerManager().ClearTimer(mAttackAnimationTimer);
	mActionMode = EPlayerActionMode::Idle;
}

void APlayerCharacter::CommitQuest(int index)
{
	check(index < mQuestTable.Num());

	int committed = 0;
	for (int i = 0; i < mQuestTable.Num(); ++i)
	{
		committed += (mQuestTable[i].CurrProgress == EQuestProgressType::InProgess) || (mQuestTable[i].CurrProgress == EQuestProgressType::Completable);
	}

	// 등록 가능한 최대 퀘스트 수를 넘음
	if (committed >= MAX_QUEST_COMMIT)
	{
		// TODO : Noitfy UI
		return;
	}

	mQuestTable[index].CurrProgress = EQuestProgressType::InProgess;
	// Serial Quest : 첫 서브퀘만 등록
	if (mQuestTable[index].Type == EQuestType::Serial)
	{
		RegisterSubQuest(index, 0);
	}
	// Parallel Quest : 모든 서브퀘를 등록
	else
	{
		for (int i = 0; i < mQuestTable[index].SubStatus.Num(); ++i)
		{
			RegisterSubQuest(index, i);
		}
	}
}

void APlayerCharacter::RegisterSubQuest(int questIndex, int subIndex)
{
	if (subIndex == mQuestTable[questIndex].SubStatus.Num())
	{
		return;
	}

	mQuestTable[questIndex].SubStatus[subIndex].bStarted = true;
	// 도착 퀘스트의 경우, Indicator 액터를 스폰함
	if (mQuestTable[questIndex].SubStatus[subIndex].Type == ESubQuestType::Arrival)
	{
		TObjectPtr<UCustomGameInstance> gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
		check(gi);

		const FVector dest = gi->GetQuestInfo(questIndex).SubQuests[subIndex].ArrivalInfo.Destination;

		FActorSpawnParameters spawnParam;
		spawnParam.Owner = this;
		spawnParam.Instigator = this;
		
		TObjectPtr<AIndicator> indicator = GetWorld()->SpawnActor<AIndicator>(AIndicator::StaticClass(), dest, FRotator::ZeroRotator, spawnParam);
		indicator->SetActorEnableCollision(true);
		indicator->RegisterQuest(questIndex, subIndex);
	}
}

void APlayerCharacter::CompleteQuest(int index)
{
	check(index < mQuestTable.Num());

	mQuestTable[index].CurrProgress = EQuestProgressType::Completed;
}

void APlayerCharacter::CompleteSubQuest(int questIndex, int subIndex)
{
	// Serial 퀘스트의 경우, 다음 서브퀘를 등록함
	if (mQuestTable[questIndex].Type == EQuestType::Serial)
	{
		mQuestTable[questIndex].SubStatus[subIndex].bCompleted = true;
		mQuestTable[questIndex].UpdateProgress();

		RegisterSubQuest(questIndex, subIndex + 1);
	}
	else
	{
		mQuestTable[questIndex].SubStatus[subIndex].bCompleted = true;
		mQuestTable[questIndex].UpdateProgress();
	}
}

void APlayerCharacter::RevertSubQuest(int questIndex, int subIndex)
{
	// 완료되어있던 퀘스트를 미완료 상태로 돌려놓음
	mQuestTable[questIndex].SubStatus[subIndex].bCompleted = false;

	if (mQuestTable[questIndex].Type == EQuestType::Serial)
	{
		mQuestTable[questIndex].CurrPhase--;
	}
	else
	{
		mQuestTable[questIndex].Completed--;
	}
}

void APlayerCharacter::ReportArrival(int questIndex, int subIndex)
{
	check(questIndex < mQuestTable.Num());
	check(subIndex < mQuestTable[questIndex].SubStatus.Num());
	check(mQuestTable[questIndex].SubStatus[subIndex].Type == ESubQuestType::Arrival);

	CompleteSubQuest(questIndex, subIndex);
}

void APlayerCharacter::ReportKill(TArray<int> labelList)
{
	TObjectPtr<UCustomGameInstance> gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(gi);
	
	// label에 맞는 사냥 퀘스트를 업데이트
	for (int i = 0; i < mQuestTable.Num(); ++i)
	{
		if (mQuestTable[i].CurrProgress != EQuestProgressType::InProgess)
		{
			continue;
		}

		for (int j = 0; j < mQuestTable[i].SubStatus.Num(); ++j)
		{
			if ((mQuestTable[i].SubStatus[j].Type != ESubQuestType::Hunt) || (mQuestTable[i].SubStatus[j].bStarted == false))
			{
				continue;
			}
			
			for (auto labelIndex : labelList)
			{
				if (gi->GetQuestInfo(i).SubQuests[j].HuntInfo.Index == labelIndex)
				{
					mQuestTable[i].SubStatus[j].CurrAmount++;
					
					if (gi->GetQuestInfo(i).SubQuests[j].HuntInfo.Num == mQuestTable[i].SubStatus[j].CurrAmount)
					{
						CompleteSubQuest(i, j);
					}
				}
			}
		}
	}
}

void APlayerCharacter::ReportItem(int infoIndex, int Num)
{
	TObjectPtr<UCustomGameInstance> gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(gi);

	// 아이템 종류에 맞는 수집 퀘스트를 업데이트
	for (int i = 0; i < mQuestTable.Num(); ++i)
	{
		if ((mQuestTable[i].CurrProgress != EQuestProgressType::InProgess) || (mQuestTable[i].CurrProgress != EQuestProgressType::Completable))
		{
			continue;
		}

		for (int j = 0; j < mQuestTable[i].SubStatus.Num(); ++j)
		{
			if ((mQuestTable[i].SubStatus[j].Type != ESubQuestType::Collect) || (mQuestTable[i].SubStatus[j].bStarted == false))
			{
				continue;
			}

			if (gi->GetQuestInfo(i).SubQuests[j].CollectInfo.Index == infoIndex)
			{
				mQuestTable[i].SubStatus[j].CurrAmount = Num;

				if ((mQuestTable[i].SubStatus[j].bCompleted == false) && (gi->GetQuestInfo(i).SubQuests[j].HuntInfo.Num <= mQuestTable[i].SubStatus[j].CurrAmount))
				{
					CompleteSubQuest(i, j);
				}
				else if ((mQuestTable[i].SubStatus[j].bCompleted == true) && (gi->GetQuestInfo(i).SubQuests[j].CollectInfo.Num > mQuestTable[i].SubStatus[j].CurrAmount))
				{
					RevertSubQuest(i, j);
				}
			}
		}
	}
}

void APlayerCharacter::UpdateHurtTimer(float deltaTime)
{
	static float hurtTimer = -1.f;

	if (hurtTimer < 0)
	{
		hurtTimer = 0.f;
		mCameraComponent->PostProcessSettings.bOverride_SceneColorTint = true;

		// 현재 움직임을 멈춤
		GetCharacterMovement()->StopMovementImmediately();
		// 입력 불가 상태로 변경
		TObjectPtr<APlayerController> pc = Cast<APlayerController>(GetController());
		check(pc); 
		pc->DisableInput(nullptr);
	}
	else if (hurtTimer < HURT_FREEZE_TIME)
	{
		// 붉은 필터 적용 (1 ~ 2.5 ~ 1 의 세기 변화)
		float intensity = 1.5f * FMath::Sin(hurtTimer * PI) + 1.0f;
		mCameraComponent->PostProcessSettings.SceneColorTint = FLinearColor(intensity, 1.f, 1.f);
		hurtTimer += deltaTime;
	}
	else
	{
		// 필터 제거 후 Hurt 경직 해제
		mCameraComponent->PostProcessSettings.SceneColorTint = FLinearColor::White;
		mCameraComponent->PostProcessSettings.bOverride_SceneColorTint = false;
		hurtTimer = -1.f;

		bHurt = false;

		TObjectPtr<APlayerController> pc = Cast<APlayerController>(GetController());
		check(pc);
		pc->EnableInput(nullptr);
	}
}

void APlayerCharacter::UpdateMovement()
{
	// Movement 전환 시
	// Walk, Run <--> Idle <--> Jump, Fall
	// 와 같이 Idle 상태를 거치게끔 설계
	// Why? 애니메이션 BP에서 현재 Movement에 맞게 애니메이션을 재생하는데, Transition Flow가 너무 복잡해져서 간소화함

	// 공중에 뜬 상태 (Jump, Fall)
	if (GetMovementComponent()->IsFalling() == true)
	{
		// Walk, Run 상태면 먼저 Idle로 전환
		if ((mMovementMode == EPlayerMovementMode::Walk) || (mMovementMode == EPlayerMovementMode::Run))
		{
			mMovementMode = EPlayerMovementMode::Idle;

			return;
		}
		
		// 점프 상태면 Jump, 아니면 Fall
		if (bJumping == true)
		{
			mMovementMode = EPlayerMovementMode::Jump;
		}
		else
		{
			mMovementMode = EPlayerMovementMode::Fall;
		}
	}
	// 땅에 닿은 상태 (Idle, Walk, Run)
	else
	{
		// Jump, Fall 상태면 먼저 Idle로 전환
		if ((mMovementMode == EPlayerMovementMode::Jump) || (mMovementMode == EPlayerMovementMode::Fall))
		{
			bJumping = false;
			mMovementMode = EPlayerMovementMode::Idle;
			
			mGameInstance->SendJumping(false);	// 점프가 끝났음을 알림
			return;
		}

		// 움직임이 없으면 Idle, 아니면 Walk, Run
		const FVector& velocity = GetMovementComponent()->Velocity;
		const float velocity2D = velocity.Size2D();

		if (velocity2D > EPSILON_ZERO)
		{
			if (bRunning == true)
			{
				mMovementMode = EPlayerMovementMode::Run;
			}
			else
			{
				mMovementMode = EPlayerMovementMode::Walk;
			}
		}
		else
		{
			mMovementMode = EPlayerMovementMode::Idle;
		}
	}
}

void APlayerCharacter::UpdateNotifyInteraction()
{
	float nearestDist = MAX_FLT;
	IInteraction* oldNearest = mNearestInteractableActor;
	for (auto interactable : mInteractableList)
	{
		if (!interactable)
		{
			mInteractableList.Remove(interactable);
			continue;
		}

		// 가장 가까운 거리의 상호작용 가능 액터를 설정
		const float dist = FVector::Dist(interactable->GetActorLocation(), GetActorLocation());
		if (nearestDist > dist)
		{
			nearestDist = dist;
			mNearestInteractableActor = Cast<IInteraction>(interactable);
			if (!mNearestInteractableActor)
			{
				UE_LOG(LogTemp, Fatal, TEXT("Failed to cast Actor[%s] to IInteraction. Check Generic Team ID."), *interactable->GetName());
			}
		}
	}

	// 이전 상호작용 액터와 다르다면, 이전의 액터는 알림 해제
	if (oldNearest && (oldNearest != mNearestInteractableActor))
	{
		oldNearest->UnNotify(this);
	}

	// 새로운 상호작용 액터는 Notify
	if (mNearestInteractableActor && (oldNearest != mNearestInteractableActor))
	{
		mNearestInteractableActor->Notify(this);
	}
}

void APlayerCharacter::UpdateAnimationMode()
{
	switch (mActionMode)
	{
	case EPlayerActionMode::Idle:
		mLoco_Action_Blend = 0; // use Locomotive state machine only
		break;
	case EPlayerActionMode::Attack:
		mLoco_Action_Blend = 1; // use Action state machine only
		break;
	case EPlayerActionMode::Interact:
		mLoco_Action_Blend = 1; // use Action state machine only
		break;
	default:
		break;
	}
}

void APlayerCharacter::CheckMovementChanged()
{
	static FVector lastLocation = GetActorLocation();
	static FRotator lastRotation = GetActorRotation();

	FVector currLocation = GetActorLocation();
	FRotator currRotation = GetActorRotation();

	// 이동/회전으로 판단되면, 서버에 이동 정보를 전송
	if (FVector::Dist2D(lastLocation, currLocation) >= SEND_LOCATION_THRESHOLD ||
		abs(lastRotation.Pitch - currRotation.Pitch) >= SEND_ROTATION_THRESHOLD ||
		abs(lastRotation.Yaw - currRotation.Yaw) >= SEND_ROTATION_THRESHOLD)
	{
		mGameInstance->SendLocation(currLocation, currRotation);
		lastLocation = currLocation;
		lastRotation = currRotation;
	}
}

void APlayerCharacter::ChangeNotification(AActor* source, FAIStimulus stimulus)
{
	// 새로운 액터가 감지됨
	if (stimulus.WasSuccessfullySensed() == true)
	{
		IGenericTeamAgentInterface* teamAgent = Cast<IGenericTeamAgentInterface>(source);
		if (teamAgent)
		{
			switch (static_cast<uint8>(teamAgent->GetGenericTeamId()))
			{
			case 1:
				// 상호작용 액터 --> 리스트에 추가
				mInteractableList.Add(source);
				break;
			case 2:
				//TODO : 적을 발견했을 때는 뭘 할까?
				break;
			default:
				break;
			}
		}
	}
	// 기존 액터가 감지 범위에서 벗어남
	else
	{
		IGenericTeamAgentInterface* teamAgent = Cast<IGenericTeamAgentInterface>(source);
		if (teamAgent)
		{
			switch (static_cast<uint8>(teamAgent->GetGenericTeamId()))
			{
			case 1:
				// 상호작용 액터 --> 리스트에서 제거
				RemoveFromInteractableList(source);
				break;
			case 2:
				//TODO : 적이 범위에서 벗어났을 때
				break;
			default:
				break;
			}
		}
	}
}

void APlayerCharacter::RemoveFromInteractableList(TObjectPtr<AActor> actor)
{
	mInteractableList.Remove(actor);
	// 최단거리 액터였다면, 알림 해제
	if (mNearestInteractableActor == Cast<IInteraction>(actor))
	{
		mNearestInteractableActor->UnNotify(this);
		mNearestInteractableActor = nullptr;
	}
}

void APlayerCharacter::SetActionMode(const EPlayerActionMode& e)
{
	check(e != EPlayerActionMode::Count);
	mActionMode = e;
}

void APlayerCharacter::SetInteracting(bool b)
{
	bInteracting = b;
}

void APlayerCharacter::SetCurrentCloth(int index)
{
	check(index < mInventory.GetTypeInventory(EItemType::Cloth).ItemList.Num());

	mCurrentClothIndex = index;

	//TODO : Cloth wearing
}

void APlayerCharacter::SetCurrentWeapon(int index)
{
	check(index < mInventory.GetTypeInventory(EItemType::Weapon).ItemList.Num());

	mCurrentWeaponIndex = index;

	ChangeWeapon();
}

void APlayerCharacter::SetCurrentGold(int gold)
{
	check(gold >= 0);
	mCurrentGold = gold;
}

FInventory& APlayerCharacter::GetInventory()
{
	return mInventory;
}

const FInventory& APlayerCharacter::GetInventory() const
{
	return mInventory;
}

FInventory APlayerCharacter::GetInventory_cpy() const
{
	return mInventory;
}

TArray<FQuestStatus>& APlayerCharacter::GetQuestTable()
{
	return mQuestTable;
}

const TArray<FQuestStatus>& APlayerCharacter::GetQuestTable() const
{
	return mQuestTable;
}

TArray<FQuestStatus> APlayerCharacter::GetQuestTable_cpy() const
{
	return mQuestTable;
}

EPlayerActionMode APlayerCharacter::GetActionMode() const
{
	return mActionMode;
}

bool APlayerCharacter::GetInteracting() const
{
	return bInteracting;
}

int APlayerCharacter::GetCurrentCloth() const
{
	return mCurrentClothIndex;
}

int APlayerCharacter::GetCurrentWeapon() const
{
	return mCurrentWeaponIndex;
}

EWeaponType APlayerCharacter::GetCurrentWeaponType() const
{
	return mWeaponType;
}

int APlayerCharacter::GetCurrentGold() const
{
	return mCurrentGold;
}

int APlayerCharacter::GetCurrentLevel() const
{
	return mCurrentLevel;
}

int APlayerCharacter::GetCurrentMapIndex() const
{
	return mCurrentMap;
}

void APlayerCharacter::OnTestValUpdated()
{
	if (!GetNetDriver()->IsServer())
	{
		FString msg = FString::Printf(TEXT("ClientMessage : %s's test value : %f"), *GetFName().ToString(), testVal);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, msg);
	}
	else
	{
		FString msg = FString::Printf(TEXT("ServerMessage : %s's test value : %f"), *GetFName().ToString(), testVal);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, msg);
	}
}

void APlayerCharacter::OnRep_Notify()
{
	OnTestValUpdated();
}