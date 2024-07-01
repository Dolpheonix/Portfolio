#include "EnemyCharacter.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/EnemyController.h"
#include "../Base/LoadHelper.h"
#include "../Character/PlayerCharacter.h"
#include "../Core/CustomGameInstance.h"

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

	// Hp 바 위젯은 캐릭터를 바라보도록 설정
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

	Super::OnHurt();
}

void AEnemyCharacter::OnDead()
{
	TObjectPtr<UCustomGameInstance> gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(gi);
	// 서버에 몹이 제거됨을 알림
	gi->SendEnemyRemoval(mResIdx);

	TObjectPtr<APlayerCharacter> pc = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	check(pc);
	// 플레이어의 사냥 퀘스트를 업데이트
	pc->ReportKill(gi->GetEnemyInfo(mEnemyInfoIndex).Labels);
	
	Super::OnDead();

	Destroy();
}

void AEnemyCharacter::UpdateState(EEnemyState s)
{
	// State에 따라 느낌표/물음표 위젯 Visibility 업데이트
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

void AEnemyCharacter::SetEnemyInfoIndex(const int idx)
{
	mEnemyInfoIndex = idx;
}

void AEnemyCharacter::SetResourceIndex(const int idx)
{
	mResIdx = idx;
}
