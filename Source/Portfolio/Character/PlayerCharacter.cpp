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

constexpr float HURT_FREEZE_TIME = 1.0f;
constexpr float CAMERA_LOC_X = -150.f;
constexpr float CAMERA_LOC_Y = 0.f;
constexpr float CAMERA_LOC_Z = 88.f;
constexpr float CAMERA_ROT_PITCH = -20.f;
constexpr float CAMERA_ROT_YAW = 0.f;
constexpr float CAMERA_ROT_ROLL = 0.f;
constexpr float SHOT_ANIM_TIME = 0.7f;

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

	// 인트로 UI 표시 중에는 아래의 동작을 수행할 필요가 없다.
	if (gi->IsIntro() == true)
	{
		return;
	}

	TObjectPtr<ACustomController> controller = Cast<ACustomController>(GetController());
	check(controller);

	TObjectPtr<UAIPerceptionComponent> perception = controller->GetPerceptionComponent();
	check(perception);

	perception->OnTargetPerceptionUpdated.AddDynamic(this, &APlayerCharacter::ChangeNotification);

	// Intro UI가 열려있다면 닫고, HUD UI를 표시함
	controller->CloseIntro();
	controller->OpenHUD(this);

	// 맵 이동 시에는 게임 인스턴스가 플레이어 정보를 임시로 보관하고 있다가, 플레이어 생성 시 해당 값을 받아 적용한다.
	const PlayerInfo tempInfo = gi->GetPlayerInfo();
	mDisplayName = tempInfo.PlayerName;
	mCurrentLevel = tempInfo.CurrentLevel;
	mCurrentGold = tempInfo.CurrentGold;
	mCurrentMap = tempInfo.CurrentMap;
	mQuestTable = tempInfo.QuestTable;
	mInventory = tempInfo.Inventory;
	mSlotIndex = tempInfo.SlotIndex;

	// location의 Z 값이 0보다 작으면 기본 스폰 위치에 스폰시킨다.
	if (tempInfo.CurrentLocation.Z >= 0.f)
	{
		SetActorLocation(tempInfo.CurrentLocation);
	}
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type type)
{
	if ((type == EEndPlayReason::LevelTransition) || (type == EEndPlayReason::Quit))
	{
		TObjectPtr<UCustomGameInstance> gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
		check(gi);

		PlayerInfo saveInfo;
		saveInfo.PlayerName = mDisplayName;
		saveInfo.CurrentLevel = mCurrentLevel;
		saveInfo.CurrentGold = mCurrentGold;
		saveInfo.CurrentMap = mCurrentMap;
		saveInfo.QuestTable = mQuestTable;
		saveInfo.Inventory = mInventory;
		saveInfo.CurrentLocation = GetActorLocation();
		
		gi->SetPlayerInfo(saveInfo);
	}

	Super::EndPlay(type);
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	static float elapsed = 0.f;
	elapsed += DeltaTime;
	if (elapsed > 1.0f)
	{
		elapsed = 0.f;
		testVal += 0.5f;
	}

	// 경직 시간동안 카메라 필터 적용
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
	
	// Hp 바 UI를 변경
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
}

void APlayerCharacter::StopRun()
{
	bRunning = false;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void APlayerCharacter::Jump()
{
	// 공중에 뜬 상태에서 점프키를 눌러도 적용되지 않음
	if ((GetCharacterMovement()->IsFalling() == true))
	{
		return;
	}

	bPressedJump = true;
	bJumping = true;
}

void APlayerCharacter::TryInteract()
{
	// 상호작용 가능한 액터가 있다면 상호작용
	if (mNearestInteractableActor)
	{
		Interact();
	}
}

void APlayerCharacter::TurnQuickSlot()
{
	// 선택할 무기가 없다면 그대로 리턴
	if (mCurrentWeaponIndex < 0)
	{
		return;
	}

	// 인벤토리 상의 다음 무기를 선택
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

	// 장착 상태를 변경
	SetEquipment(!bEquipped);
}

void APlayerCharacter::Attack()
{
	// 무기가 장착되어있지 않으면 공격할 수 없음.
	if (mWeaponType == EWeaponType::Fist)
	{
		return;
	}

	// 이동, 점프, 낙하 중에는 공격할 수 없음.
	if (mMovementMode != EPlayerMovementMode::Idle)
	{
		return;
	}

	// 아직 공격 애니메이션이 끝나지 않음.
	if (mActionMode == EPlayerActionMode::Attack)
	{
		return;
	}

	mActionMode = EPlayerActionMode::Attack;

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
		mCameraComponent->SetRelativeLocation(FVector(CAMERA_LOC_X, CAMERA_LOC_Y, CAMERA_LOC_Z));
		break;
	}
	default:
		break;
	}
}

void APlayerCharacter::OpenInventory()
{
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
	}
	else
	{
		// 무기 정보를 불러온다.
		const int itemInfoIndex = mInventory.GetTypeInventory(EItemType::Weapon).ItemList[mCurrentWeaponIndex].InfoIndex;
		mWeapon->LoadFromItemInfo(itemInfoIndex);
		mWeapon->SetVisibility(true);

		mWeaponType = mWeapon->GetType();
		
		// 착용 상태로 설정
		SetEquipment(true);
	}

	TObjectPtr<ACustomController> controller = Cast<ACustomController>(GetController());
	check(controller);

	// UI 상에 표시되는 이미지 변경
	controller->UpdateWeaponImage();
}

void APlayerCharacter::SetEquipment(bool isEquip)
{
	// 착용/해제 상태에 따라 소켓에 어태치한다.
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
		// 인벤토리에 이미 존재하는 아이템 : 개수만 증가
		foundPtr->Num += item.Num;

		ReportItem(foundPtr->InfoIndex, foundPtr->Num);
	}
	else
	{
		mInventory.GetTypeInventory(itemInfo.Type).ItemList.Add(item);

		ReportItem(item.InfoIndex, item.Num);

		if ((itemInfo.Type == EItemType::Weapon) && (mInventory.GetTypeInventory(EItemType::Weapon).ItemList.Num() == 1))
		{
			// 가진 무기가 없을 때 무기를 획득하면, 자동으로 장착한다.
			mCurrentWeaponIndex = 0;
			ChangeWeapon();
		}
	}
}

void APlayerCharacter::ThrowItem(EItemType type, int index, int num)
{
	FGameItem& item = mInventory.TypeInventoryList[static_cast<uint8>(type)].ItemList[index];

	check(num <= item.Num);

	if (item.Num == num)
	{
		// 개수가 0이 되었으므로 인벤토리에서 삭제한다.
		mInventory.TypeInventoryList[static_cast<uint8>(type)].ItemList.RemoveAt(index);

		ReportItem(index, 0);
		// TODO : 장착중인 의상 삭제 시
		// 
		// 장착중인 무기가 없어지면, 장착중인 무기를 업데이트한다.
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
	else
	{
		item.Num -= num;

		ReportItem(index, item.Num);
	}
}

void APlayerCharacter::Interact()
{
	// 점프, 낙하 중에는 상호작용할 수 없음.
	if ((mMovementMode == EPlayerMovementMode::Jump) || (mMovementMode == EPlayerMovementMode::Fall))
	{
		return;
	}

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

	if (committed >= MAX_QUEST_COMMIT)
	{
		//TODO : Notify
		return;
	}

	mQuestTable[index].CurrProgress = EQuestProgressType::InProgess;

	if (mQuestTable[index].Type == EQuestType::Serial)
	{
		RegisterSubQuest(index, 0);
	}
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
	if (mQuestTable[questIndex].Type == EQuestType::Serial)
	{
		// 완료시킨 후, 다음 퀘스트를 등록
		mQuestTable[questIndex].SubStatus[subIndex].bCompleted = true;
		mQuestTable[questIndex].UpdateProgress();

		RegisterSubQuest(questIndex, subIndex + 1);
	}
	else
	{
		// 완료 후 진행도 업데이트
		mQuestTable[questIndex].SubStatus[subIndex].bCompleted = true;
		mQuestTable[questIndex].UpdateProgress();
	}
}

void APlayerCharacter::RevertSubQuest(int questIndex, int subIndex)
{
	mQuestTable[questIndex].SubStatus[subIndex].bCompleted = false;

	if (mQuestTable[questIndex].Type == EQuestType::Serial)
	{
		//TEMP : 순차 퀘스트는 마지막 서브퀘에서만 Revert할 수 있어야 함.
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

		// 경직 적용
		GetCharacterMovement()->StopMovementImmediately();

		TObjectPtr<APlayerController> pc = Cast<APlayerController>(GetController());
		check(pc); 
		pc->DisableInput(nullptr);
	}
	else if (hurtTimer < HURT_FREEZE_TIME)
	{
		// 1초간 붉은 필터 적용 (진해졌다 연해짐)
		float intensity = 1.5f * FMath::Sin(hurtTimer * PI) + 1.0f;
		mCameraComponent->PostProcessSettings.SceneColorTint = FLinearColor(intensity, 1.f, 1.f);
		hurtTimer += deltaTime;
	}
	else
	{
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
	if (GetMovementComponent()->IsFalling() == true)
	{
		// 걷기 or 달리기 상태에서 공중에 뜰 경우 우선 Idle 상태로 전환 (애니메이션 BP 간소화)
		if ((mMovementMode == EPlayerMovementMode::Walk) || (mMovementMode == EPlayerMovementMode::Run))
		{
			mMovementMode = EPlayerMovementMode::Idle;

			return;
		}
		// JUMP OR FALL
		if (bJumping == true)
		{
			mMovementMode = EPlayerMovementMode::Jump;
		}
		else
		{
			mMovementMode = EPlayerMovementMode::Fall;
		}
	}
	else
	{
		// 점프 or 낙하 상태에서 벗어났을 때는 우선 Idle 상태로 전환 (애니메이션 BP 간소화)
		if ((mMovementMode == EPlayerMovementMode::Jump) || (mMovementMode == EPlayerMovementMode::Fall))
		{
			bJumping = false;
			mMovementMode = EPlayerMovementMode::Idle;
			
			return;
		}

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

		// 감지 범위 내의 상호작용 액터 중 가장 가까운 액터를 찾음.
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

	// Nearest Actor에서 탈락한 액터에는 UnNotify 이벤트 실행
	if (oldNearest && (oldNearest != mNearestInteractableActor))
	{
		oldNearest->UnNotify(this);
	}

	// Nearest Actor는 Notify 이벤트 실행
	if (mNearestInteractableActor)
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

void APlayerCharacter::ChangeNotification(AActor* source, FAIStimulus stimulus)
{
	if (stimulus.WasSuccessfullySensed() == true) // 범위 내로 들어온 상태
	{
		IGenericTeamAgentInterface* teamAgent = Cast<IGenericTeamAgentInterface>(source);
		if (teamAgent)	// team id에 따라 필요한 처리를 수행한다.
		{
			switch (static_cast<uint8>(teamAgent->GetGenericTeamId()))
			{
			case 1:
				// 상호작용 오브젝트
				mInteractableList.Add(source);
				break;
			case 2:
				//TODO : enemy notification
				break;
			default:
				break;
			}
		}
	}
	else
	{
		IGenericTeamAgentInterface* teamAgent = Cast<IGenericTeamAgentInterface>(source);
		if (teamAgent)	// team id에 따라 필요한 처리를 수행한다.
		{
			switch (static_cast<uint8>(teamAgent->GetGenericTeamId()))
			{
			case 1:
				// 상호작용 오브젝트
				RemoveFromInteractableList(source);
				break;
			case 2:
				//TODO : enemy notification
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

	if (mNearestInteractableActor == Cast<IInteraction>(actor))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "removed!");
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

int APlayerCharacter::GetSlotIndex() const
{
	return mSlotIndex;
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