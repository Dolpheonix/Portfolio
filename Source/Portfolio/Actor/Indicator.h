#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Indicator.generated.h"

class UBoxComponent;
class UParticleSystemComponent;

/*
	도착 퀘스트를 받았을 때, 도착 위치를 띄워줄 파티클 액터
*/
UCLASS()
class PORTFOLIO_API AIndicator : public AActor
{
	GENERATED_BODY()
	
public:	
	AIndicator();

	// Overlap : 플레이어가 도달하면 퀘스트 완료 처리
	UFUNCTION()
	void OnOverlapped_CheckArrival(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Indicator와 퀘스트를 연결
	void RegisterQuest(int questIndex, int subIndex);

public:
	// 충돌 감지용 볼륨
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UBoxComponent> mVolumeComponent;
	// 파티클 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParticleSystemComponent> mParticleComponent;
	
	// 메인 퀘스트 인덱스
	int mQuestIndex;
	// 서브 퀘스트 인덱스
	int mSubIndex;
};
