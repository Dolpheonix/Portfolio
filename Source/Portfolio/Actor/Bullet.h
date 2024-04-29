#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

/*
	ABullet : �Ѿ� �޽ø� ������ �߻�ü ����
	- ������ �̺�Ʈ : �÷��̾�, �� ĳ���Ϳ� �浹 ��
	- ��(���) �̺�Ʈ : �� ���� Static, Dynamic ������Ʈ�� �浹 ��
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

	// ������ �������� ���͸� �߻��ϰ� ������ �̺�Ʈ �� �������� ����
	void Launch(const FVector& direction, float damage);

protected:
	// �浹 ������ ������Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USphereComponent> mCollisionComponent;
	// �Ѿ� �޽�
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> mMeshComponent;
	// �߻�ü�� �����Ʈ
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UProjectileMovementComponent> mBulletMovementComponent;

	// ������ ������ (����ϴ� ������ �������� ���� �����ǹǷ�, �������Ʈ���� ���� �Ұ�)
	float mDamage;

	// �߻� �ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	float Speed;
};
