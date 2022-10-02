// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Object/Item/FPSProjectile.h"
#include "../Object/Item/Throwable.h"
#include "../Object/Item/Itemspace.h"
#include "../Utils/MathUtil.h"
#include "LadderInfo.h"
#include <Engine/Classes/Camera/CameraComponent.h>
#include <Engine/Classes/Components/AudioComponent.h>
#include "MainCharacter.generated.h"

UCLASS()
class QUICKSTART_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Binding Functions
	UFUNCTION()
	void MoveForward(float val);

	UFUNCTION()
	void StopClimb();

	UFUNCTION()
	void StartRun();
	UFUNCTION()
	void StopRun();

	UFUNCTION()
	void StartWalk();
	UFUNCTION()
	void StopWalk();

	UFUNCTION()
	void MoveRight(float val);

	UFUNCTION()
	void Turn(float val);

	UFUNCTION()
	void LookUp(float val);

	UFUNCTION()
	void StartJump();

	UFUNCTION()
	void StopJump();

	UFUNCTION()
	void Interact();

	UFUNCTION()
	void unInteract();

	UFUNCTION()
	void RollInventory();

	UFUNCTION()
	void RollArmory();

	UFUNCTION()
	void Use();

	UFUNCTION()
	void Attack();
	UFUNCTION()
	void EndAttack();

	// Item Functions
	UFUNCTION()
	void NoItem();

	UFUNCTION()
	void ThrowApple();

	// Weapon Functions
	UFUNCTION()
	void Fist();

	UFUNCTION()
	void Fire();

	UFUNCTION()
	void RefreshInventory();

	UFUNCTION()
	void RefreshArmory();

	UFUNCTION()
	void GetWeapon(int weaponIndex);

	UFUNCTION()
	void Equip();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UCameraComponent* Camera;

	// 총구 위치 (in camera space)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GamePlay)
	FVector Muzzle;

	// Projectile actor 스폰을 위한 클래스 타입 변수
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<AFPSProjectile> ProjectileClass;

	// Audio Component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	UAudioComponent* FireAudio;

	// Player가 interact 상태인지 판단
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bInteracted = false;

	UPROPERTY(BlueprintReadWrite)
	bool bInteractable = false;

	// Player HP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float HP;

	// Player가 다친 상태인지 판단
	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	bool bHurt = false;

	UPROPERTY(BlueprintReadOnly)
	float hurtTimeStamp = 0.0f; // hurt effect 시작
	float elapsedTime = 0.0f; // 경과 시간

	// Player's Equipment
	UPROPERTY(BlueprintReadOnly)
	TArray<bool> Armory;
	// Equipped Weapon
	UPROPERTY(BlueprintReadOnly)
	int Weapon_Now=0;
	UPROPERTY(BlueprintReadOnly)
	int Weapon_Before=0;
	UPROPERTY(BlueprintReadOnly)
	int Weapon_After=0;
	UPROPERTY(BlueprintReadWrite)
	bool ArmoryDirty;

	// Player's Inventory
	UPROPERTY(BlueprintReadOnly)
	TArray<bool> Inventory;
	// Equipped Item
	UPROPERTY(BlueprintReadOnly)
	int Item_Now=0;
	UPROPERTY(BlueprintReadOnly)
	int Item_Before=0;
	UPROPERTY(BlueprintReadOnly)
	int Item_After=0;
	// Can Use Item (특정 지역에서만 사용이 가능한 item)
	UPROPERTY(BlueprintReadWrite)
	TArray<bool> Usable;
	UPROPERTY(BlueprintReadWrite)
	bool InventoryDirty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* RifleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLadderInfo LadderInfo;

	UPROPERTY(BlueprintReadOnly)
	bool bRunning;

	UPROPERTY(BlueprintReadOnly)
	bool bWalking;

	UPROPERTY(BlueprintReadOnly)
	bool bAttacking;
};
