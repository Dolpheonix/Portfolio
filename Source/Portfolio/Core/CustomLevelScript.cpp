#include "CustomLevelScript.h"
#include "../Base/LoadHelper.h"
#include "../Character/PreviewCharacter.h"

//TEMP : ���� ������ ���ȭ
//constexpr FVector PREVIEWACTOR_LOCATION = FVector(0.f, 0.f, -500.f);

ACustomLevelScript::ACustomLevelScript()
{
}

void ACustomLevelScript::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters spawnParam;
	spawnParam.Owner = this;
	spawnParam.Instigator = GetInstigator();

	// 프리뷰 캐릭터 생성
	mPreviewCharacter = GetWorld()->SpawnActor<APreviewCharacter>(APreviewCharacter::StaticClass(), FVector(0.f, 0.f, -500.f), FRotator(), spawnParam);
	check(mPreviewCharacter);

	// 프리뷰 캐릭터를 감싸는 방 오브젝트 생성. (조명 포함)
	TSubclassOf<AActor> lightRoomClass = LoadHelper::LoadObjectFromPath<UBlueprintGeneratedClass>(TEXT("/Game/Blueprint/Actor/lightRoom.lightRoom_C"));
	check(lightRoomClass.Get());

	TObjectPtr<AActor> lightRoom = GetWorld()->SpawnActor<AActor>(lightRoomClass, FVector(0.f, 0.f, -588.f), FRotator(), spawnParam);
	check(lightRoom);

	lightRoom->SetActorScale3D(FVector(5.f, 5.f, 5.f));
}

TObjectPtr<APreviewCharacter> ACustomLevelScript::GetPreviewCharacter()
{
	return mPreviewCharacter;
}
