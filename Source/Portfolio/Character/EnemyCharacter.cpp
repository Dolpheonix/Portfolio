#include "EnemyCharacter.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/EnemyController.h"
#include "../Base/LoadHelper.h"

static_assert(static_cast<uint8>(EEnemyState::Count) == 4);

AEnemyCharacter::AEnemyCharacter() : SightRadius(1500.f), LoseSightRadius(2000.f), SightAngle(60.f)
{
	PrimaryActorTick.bCanEverTick = true;

	TObjectPtr<UCapsuleComponent> capsule = Cast<UCapsuleComponent>(RootComponent);
	check(capsule);
	capsule->SetCollisionProfileName("Enemy");

	mHpBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HpBar"));
	mHpBarWidget->SetupAttachment(RootComponent);

	mQuestionMarkComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("QuestionMark"));
	mQuestionMarkComponent->SetSprite(LoadHelper::C_LoadObjectFromPath<UTexture2D>(TEXT("/Game/Texture/InGameImage/QuestionMark.QuestionMark")));
	mQuestionMarkComponent->SetupAttachment(RootComponent);

	mExclamationMarkComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("ExclamationMark"));
	mExclamationMarkComponent->SetSprite(LoadHelper::C_LoadObjectFromPath<UTexture2D>(TEXT("/Game/Texture/InGameImage/ExclamationMark.ExclamationMark")));
	mExclamationMarkComponent->SetupAttachment(RootComponent);

	AIControllerClass = AEnemyController::StaticClass();
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	mHpBarWidget->SetWidgetClass(HpBarClass.Get());
	mHpBarWidget->SetDrawSize(FVector2D(100.f, 10.f));

	mQuestionMarkComponent->SetHiddenInGame(false);
	mExclamationMarkComponent->SetHiddenInGame(false);

	mQuestionMarkComponent->SetVisibility(false);
	mExclamationMarkComponent->SetVisibility(false);
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// HP바가 플레이어를 바라보게 한다.
	FVector dir = (UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
	FRotator rot = FRotationMatrix::MakeFromX(dir).Rotator();

	mHpBarWidget->SetWorldRotation(rot);
}

float AEnemyCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float dmg = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	// HP 바 업데이트
	TObjectPtr<UProgressBar> pgBar = Cast<UProgressBar>(mHpBarWidget->GetWidget());
	check(pgBar);
	pgBar->SetPercent(static_cast<float>(mHp) / mMaxHp);
	
	return dmg;
}

void AEnemyCharacter::OnHurt()
{
	Cast<AEnemyController>(GetController())->OnHurt();
}

void AEnemyCharacter::OnDead()
{
	// TODO : Labeling, controller ondead(), destroy handle
}

void AEnemyCharacter::UpdateState(EEnemyState s)
{
	switch (s)
	{
	case EEnemyState::Patrol:
		mQuestionMarkComponent->SetVisibility(false);
		mExclamationMarkComponent->SetVisibility(false);
		break;
	case EEnemyState::Caution:
		mQuestionMarkComponent->SetVisibility(true);
		mExclamationMarkComponent->SetVisibility(false);
		break;
	case EEnemyState::Detected:
		mQuestionMarkComponent->SetVisibility(false);
		mExclamationMarkComponent->SetVisibility(true);
		break;
	case EEnemyState::Hurt:
		break;
	default:
		break;
	}
}
