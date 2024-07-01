#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

/*
	정해진 방향으로 날아가는 투사체 액터
	- Hit : 액터와 충돌 시 충돌한 액터가 밀려남
	- Overlap : 몹 캐릭터와 충돌 시 데미지 적용
*/

UCLASS()
class PORTFOLIO_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	ABullet();

protected:
	virtual void BeginPlay() override;

public:
	// Dynamic 액터와 충돌 : 밀려남
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
	// Enemy Character와 충돌 : 데미지 적용
	UFUNCTION()
	void OnOverlapped(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	// 정해진 방향으로 발사. 데미지 적용
	void Launch(const FVector& direction, float damage);

protected:
	// 충돌 감지용 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USphereComponent> mCollisionComponent;
	// 투사체 메시
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> mMeshComponent;
	// 투사체의 Movement Component
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UProjectileMovementComponent> mBulletMovementComponent;

	// 충돌시 적용할 데미지
	float mDamage;

	// 발사체 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	float Speed;
};
