// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "../Common/Quest.h"
#include "CustomPlayerState.generated.h"

UCLASS()
class PORTFOLIO_API ACustomPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
public:
	void CommitQuest(int index);
	void CompleteQuest(int index);

	TArray<FQuestStatus>& GetQuestStatusList();
	FQuestStatus& GetQuestStatus(int index);
private:
	TArray<FQuestStatus> mQuestStatus;
};
