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

	// ��Ʈ�� UI ǥ�� �߿��� �Ʒ��� ������ ������ �ʿ䰡 ����.
	if (gi->IsIntro() == true)
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
	const PlayerInfo tempInfo = gi->GetPlayerInfo();
	mDisplayName = tempInfo.PlayerName;
	mCurrentLevel = tempInfo.CurrentLevel;
	mCurrentGold = tempInfo.CurrentGold;
	mCurrentMap = tempInfo.CurrentMap;
	mQuestTable = tempInfo.QuestTable;
	mInventory = tempInfo.Inventory;
	mSlotIndex = tempInfo.SlotIndex;

	// location�� Z ���� 0���� ������ �⺻ ���� ��ġ�� ������Ų��.
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

	// ���� �ð����� ī�޶� ���� ����
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
	
	// Hp �� UI�� ����
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
	// ���߿� �� ���¿��� ����Ű�� ������ ������� ����
	if ((GetCharacterMovement()->IsFalling() == true))
	{
		return;
	}

	bPressedJump = true;
	bJumping = true;
}

void APlayerCharacter::TryInteract()
{
	// ��ȣ�ۿ� ������ ���Ͱ� �ִٸ� ��ȣ�ۿ�
	if (mNearestInteractableActor)
	{
		Interact();
	}
}

void APlayerCharacter::TurnQuickSlot()
{
	// ������ ���Ⱑ ���ٸ� �״�� ����
	if (mCurrentWeaponIndex < 0)
	{
		return;
	}

	// �κ��丮 ���� ���� ���⸦ ����
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

	// ���� ���¸� ����
	SetEquipment(!bEquipped);
}

void APlayerCharacter::Attack()
{
	// ���Ⱑ �����Ǿ����� ������ ������ �� ����.
	if (mWeaponType == EWeaponType::Fist)
	{
		return;
	}

	// �̵�, ����, ���� �߿��� ������ �� ����.
	if (mMovementMode != EPlayerMovementMode::Idle)
	{
		return;
	}

	// ���� ���� �ִϸ��̼��� ������ ����.
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
		// ���� ������ �ҷ��´�.
		const int itemInfoIndex = mInventory.GetTypeInventory(EItemType::Weapon).ItemList[mCurrentWeaponIndex].InfoIndex;
		mWeapon->LoadFromItemInfo(itemInfoIndex);
		mWeapon->SetVisibility(true);

		mWeaponType = mWeapon->GetType();
		
		// ���� ���·� ����
		SetEquipment(true);
	}

	TObjectPtr<ACustomController> controller = Cast<ACustomController>(GetController());
	check(controller);

	// UI �� ǥ�õǴ� �̹��� ����
	controller->UpdateWeaponImage();
}

void APlayerCharacter::SetEquipment(bool isEquip)
{
	// ����/���� ���¿� ���� ���Ͽ� ����ġ�Ѵ�.
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
		// �κ��丮�� �̹� �����ϴ� ������ : ������ ����
		foundPtr->Num += item.Num;

		ReportItem(foundPtr->InfoIndex, foundPtr->Num);
	}
	else
	{
		mInventory.GetTypeInventory(itemInfo.Type).ItemList.Add(item);

		ReportItem(item.InfoIndex, item.Num);

		if ((itemInfo.Type == EItemType::Weapon) && (mInventory.GetTypeInventory(EItemType::Weapon).ItemList.Num() == 1))
		{
			// ���� ���Ⱑ ���� �� ���⸦ ȹ���ϸ�, �ڵ����� �����Ѵ�.
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
		// ������ 0�� �Ǿ����Ƿ� �κ��丮���� �����Ѵ�.
		mInventory.TypeInventoryList[static_cast<uint8>(type)].ItemList.RemoveAt(index);

		ReportItem(index, 0);
		// TODO : �������� �ǻ� ���� ��
		// 
		// �������� ���Ⱑ ��������, �������� ���⸦ ������Ʈ�Ѵ�.
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
	// ����, ���� �߿��� ��ȣ�ۿ��� �� ����.
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
		// �Ϸ��Ų ��, ���� ����Ʈ�� ���
		mQuestTable[questIndex].SubStatus[subIndex].bCompleted = true;
		mQuestTable[questIndex].UpdateProgress();

		RegisterSubQuest(questIndex, subIndex + 1);
	}
	else
	{
		// �Ϸ� �� ���൵ ������Ʈ
		mQuestTable[questIndex].SubStatus[subIndex].bCompleted = true;
		mQuestTable[questIndex].UpdateProgress();
	}
}

void APlayerCharacter::RevertSubQuest(int questIndex, int subIndex)
{
	mQuestTable[questIndex].SubStatus[subIndex].bCompleted = false;

	if (mQuestTable[questIndex].Type == EQuestType::Serial)
	{
		//TEMP : ���� ����Ʈ�� ������ ������������ Revert�� �� �־�� ��.
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

		// ���� ����
		GetCharacterMovement()->StopMovementImmediately();

		TObjectPtr<APlayerController> pc = Cast<APlayerController>(GetController());
		check(pc); 
		pc->DisableInput(nullptr);
	}
	else if (hurtTimer < HURT_FREEZE_TIME)
	{
		// 1�ʰ� ���� ���� ���� (�������� ������)
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
		// �ȱ� or �޸��� ���¿��� ���߿� �� ��� �켱 Idle ���·� ��ȯ (�ִϸ��̼� BP ����ȭ)
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
		// ���� or ���� ���¿��� ����� ���� �켱 Idle ���·� ��ȯ (�ִϸ��̼� BP ����ȭ)
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

		// ���� ���� ���� ��ȣ�ۿ� ���� �� ���� ����� ���͸� ã��.
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

	// Nearest Actor���� Ż���� ���Ϳ��� UnNotify �̺�Ʈ ����
	if (oldNearest && (oldNearest != mNearestInteractableActor))
	{
		oldNearest->UnNotify(this);
	}

	// Nearest Actor�� Notify �̺�Ʈ ����
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
	if (stimulus.WasSuccessfullySensed() == true) // ���� ���� ���� ����
	{
		IGenericTeamAgentInterface* teamAgent = Cast<IGenericTeamAgentInterface>(source);
		if (teamAgent)	// team id�� ���� �ʿ��� ó���� �����Ѵ�.
		{
			switch (static_cast<uint8>(teamAgent->GetGenericTeamId()))
			{
			case 1:
				// ��ȣ�ۿ� ������Ʈ
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
		if (teamAgent)	// team id�� ���� �ʿ��� ó���� �����Ѵ�.
		{
			switch (static_cast<uint8>(teamAgent->GetGenericTeamId()))
			{
			case 1:
				// ��ȣ�ۿ� ������Ʈ
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