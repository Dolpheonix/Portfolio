#include "CustomPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "CustomGameInstance.h"

void ACustomPlayerState::BeginPlay()
{
	Super::BeginPlay();

	const TObjectPtr<UCustomGameInstance> gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(gi->IsLoaded() == true);
	mQuestStatus.SetNum(gi->GetQuestInfoList().Num());

	// TODO : 서버 데이터 적용
	for (int i = 0; i < mQuestStatus.Num(); ++i)
	{
		mQuestStatus[i].Index = i;
		mQuestStatus[i].CurrProgress = EQuestProgressType::Available;
	}
}

void ACustomPlayerState::CommitQuest(int index)
{
	mQuestStatus[index].CurrProgress = EQuestProgressType::InProgess;
}

void ACustomPlayerState::CompleteQuest(int index)
{
	mQuestStatus[index].CurrProgress = EQuestProgressType::Completed;
}

TArray<FQuestStatus>& ACustomPlayerState::GetQuestStatusList()
{
	return mQuestStatus;
}

FQuestStatus& ACustomPlayerState::GetQuestStatus(int index)
{
	return mQuestStatus[index];
}