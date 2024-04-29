#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

/*
	ABullet : 총알 메시를 포함한 발사체 액터
	- 오버랩 이벤트 : 플레이어, 적 캐릭터와 충돌 시
	- 힛(블록) 이벤트 : 그 외의 Static, Dynamic 오브젝트와 충돌 시
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
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void OnOverlapped(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	// 지정된 방향으로 액터를 발사하고 오버랩 이벤트 시 데미지를 적용
	void Launch(const FVector& direction, float damage);

protected:
	// 충돌 관리용 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USphereComponent> mCollisionComponent;
	// 총알 메시
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> mMeshComponent;
	// 발사체의 무브먼트
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UProjectileMovementComponent> mBulletMovementComponent;

	// 적용할 데미지 (사용하는 무기의 데미지에 의해 결정되므로, 블루프린트에서 변경 불가)
	float mDamage;

	// 발사 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	float Speed;
};
