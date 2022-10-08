// Fill out your copyright notice in the Description page of Project Settings.


#include "Giant_Controller.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Actions/PawnActionsComponent.h"
#include <Kismet/GameplayStatics.h>

AGiant_Controller::AGiant_Controller()
{
	EnemyKey = FName(TEXT("Enemy"));
	OriginLocKey = FName(TEXT("Origin_Loc"));
	OriginYawKey = FName(TEXT("Origin_Yaw"));

	BehaviorTree = CreateDefaultSubobject<UBehaviorTree>(TEXT("Behavior Tree"));

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTFinder(TEXT("/Game/ShootingGame/Character/Giant/BehaviorTree/BP_Giant_BT.BP_Giant_BT"));
	if (BTFinder.Succeeded())
	{
		BehaviorTree = BTFinder.Object;
	}

	bSetControlRotationFromPawnOrientation = false;
}

void AGiant_Controller::BeginPlay()
{
	Super::BeginPlay();
	RunBehaviorTree(BehaviorTree);

	GetBlackboardComponent()->SetValueAsObject(EnemyKey, UGameplayStatics::GetPlayerPawn(this, 0));
	GetBlackboardComponent()->SetValueAsVector(OriginLocKey, GetActionsComp()->GetOwner()->GetActorLocation());
}

