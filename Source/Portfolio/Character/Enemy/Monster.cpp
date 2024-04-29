#include "Monster.h"
#include "Components/BillboardComponent.h"
#include "Components/ProgressBar.h"
#include "Components/WidgetComponent.h"
#include "../../Base/LoadHelper.h"

AMonster::AMonster()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetSkeletalMesh(LoadHelper::C_LoadObjectFromPath<USkeletalMesh>(TEXT("/Game/Mesh/Monster/Monster.Monster")));
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));

	mQuestionMarkComponent->SetRelativeLocation(FVector(-20.f, 150.f, 60.f));
	mQuestionMarkComponent->SetRelativeScale3D(FVector(0.07f, 0.07f, 0.07f));

	mExclamationMarkComponent->SetRelativeLocation(FVector(-20.f, 150.f, 60.f));
	mExclamationMarkComponent->SetRelativeScale3D(FVector(0.07f, 0.07f, 0.07f));

	HpBarClass = LoadHelper::C_LoadObjectFromPath<UBlueprintGeneratedClass>(TEXT("/Game/UI/EnemyHpBar_BP.EnemyHpBar_BP_C"));
}

void AMonster::BeginPlay()
{
	Super::BeginPlay();

	mHpBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 60.f));
}

void AMonster::Spit()
{
}
