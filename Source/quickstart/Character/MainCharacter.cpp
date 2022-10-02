// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletal(TEXT("/Game/ShootingGame/Character/Mesh/SK_Mannequin.SK_Mannequin"));
	if (skeletal.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(skeletal.Object);
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.f));
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	}

	WeaponComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));

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
	for (int i = 0; i < static_cast<int>(EinventorySpace::End); i++)
	{
		Inventory.Add(false);
		Usable.Add(false);
	}

	Inventory[0] = true;
	Armory[0] = true;
	// Status
	HP = 100.0f;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	FAttachmentTransformRules rule = {EAttachmentRule::KeepRelative, true};
	WeaponComponent->AttachToComponent(GetMesh(), rule, TEXT("hand_r_socket"));
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	elapsedTime += DeltaTime;

	// Ã¼·ÂÀÌ ±ð¿´À» ¶§ : Ä«¸Þ¶óÀÇ post processingÀ» Å´
	if (bHurt) 
	{
		hurtTimeStamp = elapsedTime;
		bHurt = false;
		Camera->PostProcessSettings.bOverride_SceneColorTint = true;
	}
	// 1.5ÃÊ°£ ºÓÀº ÇÊÅÍ¸¦ ¾º¿î´Ù.
	else if(hurtTimeStamp > 0.0f && elapsedTime - hurtTimeStamp < 1.5f)
	{
		float hurtIntensity = -abs(elapsedTime - hurtTimeStamp - 0.75f) + 1.75f; // 1.0f ~ 2.5f ~ 1.0f (0s ~ 3s)
		Camera->PostProcessSettings.SceneColorTint = FLinearColor(hurtIntensity, 1.0f, 1.0f);
	}
	// post processingÀ» ²ö´Ù
	else if (hurtTimeStamp > 0.0f && elapsedTime - hurtTimeStamp >= 1.5f)
	{
		Camera->PostProcessSettings.SceneColorTint = FLinearColor(1.0f, 1.0f, 1.0f);
		Camera->PostProcessSettings.bOverride_SceneColorTint = false;
		hurtTimeStamp = 0.0f;
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
	int cache = ItemIndex;

	ItemIndex = (ItemIndex + 1) % static_cast<int>(EinventorySpace::End);
	while (!Inventory[ItemIndex])
	{
		ItemIndex = (ItemIndex + 1) % static_cast<int>(EinventorySpace::End);
	}

	if (cache != ItemIndex)
	{
		InventoryDirty = true;
	}
}

void AMainCharacter::RollArmory()
{
	int cache = WeaponIndex;

	WeaponIndex = (WeaponIndex + 1) % static_cast<int>(EArmorySpace::End);
	while (!Armory[WeaponIndex])
	{
		WeaponIndex = (WeaponIndex + 1) % static_cast<int>(EArmorySpace::End);
	}

	if (cache != WeaponIndex)
	{
		ArmoryDirty = true;
	}
}

void AMainCharacter::Use()
{
	switch (ItemIndex)
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
	switch (WeaponIndex)
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