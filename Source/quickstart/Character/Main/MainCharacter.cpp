// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletal(TEXT("/Game/ShootingGame/Character/Main/Mesh/SK_Mannequin.SK_Mannequin"));
	if (skeletal.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(skeletal.Object);
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.f));
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	}

	RifleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	Camera->SetupAttachment(RootComponent);
	Camera->SetRelativeLocation(FVector(88.0f, 0.0f, -10.0f));
	Camera->SetRelativeRotation(FRotator(-20.0f, 0.0f, 0.0f));
	Camera->bUsePawnControlRotation = true;

	static ConstructorHelpers::FObjectFinder<UMaterial> EdgeLight(TEXT("/Game/ShootingGame/Material/PostProcess/M_Highlight.M_Highlight"));

	if (EdgeLight.Succeeded())
	{
		Camera->PostProcessSettings.AddBlendable(EdgeLight.Object, 1);
	}

	Muzzle = FVector(100.0f, 0.0f, 0.0f);

	// Sound Component
	FireAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
	FireAudio->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<USoundWave> SoundFinder(TEXT("/Game/StarterContent/Audio/Explosion01.Explosion01"));
	if (SoundFinder.Succeeded())
	{
		FireAudio->SetSound(SoundFinder.Object);
		FireAudio->bAutoActivate = false;
	}

	// Weapon
	for (int i = 0; i < static_cast<int>(EArmorySpace::End); i++)
	{
		Armory.Add(false);
	}

	// Item
	for (int i = 0; i < static_cast<int>(EInventorySpace::End); i++)
	{
		Inventory.Add(false);
		Usable.Add(false);
	}

	Inventory[0] = true;
	Armory[0] = true;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	InventoryDirty = true;
	ArmoryDirty = true;
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	elapsedTime += DeltaTime;

	// Ã¼·ÂÀÌ ±ð¿´À» ¶§ : Ä«¸Þ¶óÀÇ post processingÀ» Å´
	if (bHurt && hurtFrameStep < 0)
	{
		hurtFrameStep = 0;
		Camera->PostProcessSettings.bOverride_SceneColorTint = true;
		DisableInput(UGameplayStatics::GetPlayerController(this, 0));
	}
	// 1.5ÃÊ°£ ºÓÀº ÇÊÅÍ¸¦ ¾º¿î´Ù.
	if (hurtFrameStep >= 0)
	{
		if (hurtFrameStep < 35)
		{
			float hurtIntensity = 1.5f * FMath::Sin(hurtFrameStep * (PI /30.0f)) + 1.0f; // 1.0f ~ 2.5f ~ 1.0f (0s ~ 3s)
			Camera->PostProcessSettings.SceneColorTint = FLinearColor(hurtIntensity, 1.0f, 1.0f);
			hurtFrameStep++;
		}
		// post processingÀ» ²ö´Ù
		else if (hurtFrameStep >= 35)
		{
			Camera->PostProcessSettings.SceneColorTint = FLinearColor(1.0f, 1.0f, 1.0f);
			Camera->PostProcessSettings.bOverride_SceneColorTint = false;
			hurtFrameStep = -1;
			bHurt = false;
			EnableInput(UGameplayStatics::GetPlayerController(this, 0));
		}
	}
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Move_Forward", this, &AMainCharacter::MoveForward);                       // W,S
	PlayerInputComponent->BindAction("Climb_Stop", IE_Released, this, &AMainCharacter::StopClimb);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AMainCharacter::StartRun);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AMainCharacter::StopRun);
	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &AMainCharacter::StartWalk);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &AMainCharacter::StopWalk);

	PlayerInputComponent->BindAxis("Move_Right", this, &AMainCharacter::MoveRight);                           // D,A
	PlayerInputComponent->BindAxis("Turn", this, &AMainCharacter::Turn);                                      // Mouse LR
	PlayerInputComponent->BindAxis("LookUp", this, &AMainCharacter::LookUp);                                  // Mouse FB
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::StartJump);                   // Spacebar press
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMainCharacter::StopJump);                   // Spacebar release
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMainCharacter::Interact);                // E press
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AMainCharacter::unInteract);             // E release

	PlayerInputComponent->BindAction("RollInventory", IE_Pressed, this, &AMainCharacter::RollInventory);      // R
	PlayerInputComponent->BindAction("RollArmory", IE_Pressed, this, &AMainCharacter::RollArmory);            // T
	PlayerInputComponent->BindAction("Use", IE_Pressed, this, &AMainCharacter::Use);                          // Mouse Right
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AMainCharacter::Attack);                    // Mouse Left
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &AMainCharacter::EndAttack);
}

void AMainCharacter::MoveForward(float val)
{
	// get Character's X axis (forward) & add input to forward axis
	if (LadderInfo.onLadder)
	{
		FVector direction = GetActorForwardVector().RotateAngleAxis(-LadderInfo.Slope, GetActorRightVector());
		AddMovementInput(direction, val);
	}
	else
	{
		FVector direction = GetActorForwardVector();
		if (bRunning)
		{
			AddMovementInput(direction, val);
		}
		else if (bWalking)
		{
			AddMovementInput(direction, val * 0.5f);
		}
		else
		{
			AddMovementInput(direction, val);
		}
	}
}

void AMainCharacter::StopClimb()
{
	if (LadderInfo.onLadder)
	{
		GetCharacterMovement()->StopMovementImmediately();
	}
}

void AMainCharacter::StartRun()
{
	bRunning = true;
	GetCharacterMovement()->MaxWalkSpeed = 1200.0f;
}

void AMainCharacter::StopRun()
{
	bRunning = false;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
}

void AMainCharacter::StartWalk()
{
	bWalking = true;
}

void AMainCharacter::StopWalk()
{
	bWalking = false;
}

void AMainCharacter::MoveRight(float val)
{
	if (!LadderInfo.onLadder)
	{
		FVector direction = GetActorRightVector();
		if (bRunning)
		{
			AddMovementInput(direction, val);
		}
		else if (bWalking)
		{
			AddMovementInput(direction, val * 0.5f);
		}
		else
		{
			AddMovementInput(direction, val);
		}
	}
}

void AMainCharacter::Turn(float val)
{
	if (!LadderInfo.onLadder)
	{
		AddControllerYawInput(val);
	}
}

void AMainCharacter::LookUp(float val)
{
	AddControllerPitchInput(val);
}

void AMainCharacter::StartJump()
{
	if (LadderInfo.onLadder)
	{
		LadderInfo.onLadder = false;
		LadderInfo.dirty = true;
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
	bPressedJump = true;
}

void AMainCharacter::StopJump()
{
	bPressedJump = false;
}

void AMainCharacter::Interact()
{
	bInteracted = true;
}

void AMainCharacter::unInteract()
{
	bInteracted = false;
}

void AMainCharacter::RollInventory()
{
	int before = Item_Now;
	Item_Now = MathUtil::CircularPlus(before, static_cast<int>(EInventorySpace::End));
	while (!Inventory[Item_Now])
	{
		Item_Now = MathUtil::CircularPlus(Item_Now, static_cast<int>(EInventorySpace::End));
	}

	if (before != Item_Now)
	{
		InventoryDirty = true;
		Item_Before = before;

		int after = Item_Now;
		after = MathUtil::CircularPlus(Item_Now, static_cast<int>(EInventorySpace::End));
		while (!Inventory[after])
		{
			after = MathUtil::CircularPlus(after, static_cast<int>(EInventorySpace::End));
		}

		Item_After = after;
	}
}

void AMainCharacter::RollArmory()
{
	int before = Weapon_Now;

	Weapon_Now = MathUtil::CircularPlus(before, static_cast<int>(EArmorySpace::End));
	while (!Armory[Weapon_Now])
	{
		Weapon_Now = MathUtil::CircularPlus(Weapon_Now, static_cast<int>(EArmorySpace::End));
	}

	if (before != Weapon_Now)
	{
		ArmoryDirty = true;
		Weapon_Before = before;

		int after = Weapon_Now;
		after = MathUtil::CircularPlus(Weapon_Now, static_cast<int>(EArmorySpace::End));
		while (!Armory[after])
		{
			after = MathUtil::CircularPlus(after, static_cast<int>(EArmorySpace::End));
		}

		Weapon_After = after;

		Equip();
	}
}

void AMainCharacter::RefreshInventory()
{
	int	before = MathUtil::CircularMinus(Item_Now, static_cast<int>(EInventorySpace::End));
	while (!Inventory[before])
	{
		before = MathUtil::CircularMinus(before, static_cast<int>(EInventorySpace::End));
	}

	int	after = MathUtil::CircularPlus(Item_Now, static_cast<int>(EInventorySpace::End));
	while (!Inventory[after])
	{
		after = MathUtil::CircularPlus(after, static_cast<int>(EInventorySpace::End));
	}

	if (before != Item_Before || after != Item_After)
	{
		Item_Before = before;
		Item_After = after;

		InventoryDirty = true;
	}
}

void AMainCharacter::RefreshArmory()
{
	int	before = MathUtil::CircularMinus(Weapon_Now, static_cast<int>(EArmorySpace::End));
	while (!Armory[before])
	{
		before = MathUtil::CircularMinus(before, static_cast<int>(EArmorySpace::End));
	}

	int	after = MathUtil::CircularPlus(Weapon_Now, static_cast<int>(EArmorySpace::End));
	while (!Armory[after])
	{
		after = MathUtil::CircularPlus(after, static_cast<int>(EArmorySpace::End));
	}

	if (before != Weapon_Before || after != Weapon_After)
	{
		Weapon_Before = before;
		Weapon_After = after;

		ArmoryDirty = true;
	}
}

void AMainCharacter::GetWeapon(int weaponIndex)
{
	switch (weaponIndex)
	{
	case 1:
		UStaticMesh * rifle = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("/Game/ShootingGame/Asset/Rifle/Rifle.Rifle")));
		if (rifle)
		{
			RifleComponent->SetStaticMesh(rifle);
			FAttachmentTransformRules rule = { EAttachmentRule::SnapToTarget, true };
			RifleComponent->AttachToComponent(GetMesh(), rule, TEXT("spine_03_Rifle"));
		}
	}
}

void AMainCharacter::Equip()
{
	switch (Weapon_Now)
	{
	case 0:
		RifleComponent->AttachToComponent(GetMesh(), { EAttachmentRule::SnapToTarget, true }, TEXT("spine_03_Rifle"));
		break;
	case 1:
		RifleComponent->AttachToComponent(GetMesh(), { EAttachmentRule::SnapToTarget, true }, TEXT("hand_r_Rifle"));
		break;	
	}
}

void AMainCharacter::Use()
{
	switch (Item_Now)
	{
	case 0: // no item
		NoItem();
		break;
	case 1: // apple
		// TODO : ´øÁö±â
		ThrowApple();
		break;
	default:
		break;
	}
}

void AMainCharacter::Attack()
{
	bAttacking = true;
	switch (Weapon_Now)
	{
	case 0: // fist
		Fist();
		break;
	case 1: // rifle
		Fire();
		break;
	case 2: // sword
		// TODO : Ä®Áú
		break;
	default:
		break;
	}
}

void AMainCharacter::EndAttack()
{
	bAttacking = false;
}