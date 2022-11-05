// Fill out your copyright notice in the Description page of Project Settings.


#include "Giant_Controller.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Actions/PawnActionsComponent.h"
#include "../Character_Root.h"
#include <Kismet/GameplayStatics.h>

AGiant_Controller::AGiant_Controller()
{
	EnemyKey = FName(TEXT("Enemy"));
	OriginLocKey = FName(TEXT("Origin_Loc"));
	OriginYawKey = FName(TEXT("Origin_Yaw"));
	HurtKey = FName(TEXT("Hurt"));

	BehaviorTree = CreateDefaultSubobject<UBehaviorTree>(TEXT("Behavior Tree"));
	
	bSetControlRotationFromPawnOrientation = false;
}

void AGiant_Controller::BeginPlay()
{
	Super::BeginPlay();
	RunBehaviorTree(BehaviorTree);

	GetBlackboardComponent()->SetValueAsObject(EnemyKey, UGameplayStatics::GetPlayerPawn(this, 0));
	GetBlackboardComponent()->SetValueAsVector(OriginLocKey, GetActionsComp()->GetOwner()->GetActorLocation());
}

void AGiant_Controller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto ControlledPawn = (ACharacter_Root*) GetPawn();
	GetBlackboardComponent()->SetValueAsBool(HurtKey, ControlledPawn->bHurt);
}

