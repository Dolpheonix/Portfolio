#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CustomCharacter.generated.h"

// ACustomCharacter : ��� ĳ���� Ŭ������ ��� Ŭ����.
// �ֿ� ��� : �̸�, hp, ���� �ð�

UCLASS()
class PORTFOLIO_API ACustomCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACustomCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// �������� �Ծ��� �� ȣ��Ǵ� �Լ�
	UFUNCTION()
	virtual void OnHurt() {};

	// HP�� 0�� �Ǿ��� �� ȣ��Ǵ� �Լ�
	UFUNCTION()
	virtual void OnDead() {};

	// ���� ü�� ��ȯ
	int GetCurrHp();
	// �ִ� ü�� ��ȯ
	int GetMaxHp();

public:
	// ���ӿ� ǥ�õǴ� �̸�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Name")
	FString mDisplayName;

	// �ִ� ü��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status")
	int mMaxHp;

	// ���� ü��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status")
	float mHp;

	// ��� ����
	UPROPERTY(BlueprintReadWrite, Category="Status")
	bool bDead;

	// ������ ���� ����
	UPROPERTY(BlueprintReadWrite)
	bool bForced;
};
