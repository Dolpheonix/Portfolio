#include "Dialogue.h"
#include "../Base/Func.h"
#include "../Base/LoadHelper.h"

static_assert(static_cast<uint8>(EDialogueEventType::Count) == 7);

template<>
EDialogueEventType StringToEnum<EDialogueEventType>(const FString& str)
{
	if (str == "End")
	{
		return EDialogueEventType::End;
	}
	else if (str == "Jump")
	{
		return EDialogueEventType::Jump;
	}
	else if (str == "CommitQuest")
	{
		return EDialogueEventType::CommitQuest;
	}
	else if (str == "CompleteQuest")
	{
		return EDialogueEventType::CompleteQuest;
	}
	else if (str == "OpenShop")
	{
		return EDialogueEventType::OpenShop;
	}
	else if (str == "GiveItem")
	{
		return EDialogueEventType::GiveItem;
	}
	else if (str == "SetBookmark")
	{
		return EDialogueEventType::SetBookmark;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Invalid DialogueEventType Value [%s]"), *str);
	}

	return EDialogueEventType::Count;
}

template<>
FString EnumToString<EDialogueEventType>(const EDialogueEventType& e)
{
	switch (e)
	{
	case EDialogueEventType::End:
		return "End";
	case EDialogueEventType::Jump:
		return "Jump";
	case EDialogueEventType::CommitQuest:
		return "CommitQuest";
	case EDialogueEventType::CompleteQuest:
		return "CompleteQuest";
	case EDialogueEventType::OpenShop:
		return "OpenShop";
	case EDialogueEventType::GiveItem:
		return "GiveItem";
	case EDialogueEventType::SetBookmark:
		return "SetBookmark";
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid DialogueEventType Value"));
		break;
	}

	return "";
}

FDialogueEvent::FDialogueEvent() : EventType(EDialogueEventType::Count), JumpIndex(-1)
{
}

bool FDialogueEvent::LoadFromJson(const TSharedPtr<FJsonObject>& eventValue)
{
	EventType = StringToEnum<EDialogueEventType>(eventValue->GetStringField("Type"));
	
	switch (EventType)
	{
	case EDialogueEventType::End:
	case EDialogueEventType::OpenShop:
		break;
	case EDialogueEventType::Jump:
		JumpIndex = eventValue->GetIntegerField("JumpIndex");
		break;
	case EDialogueEventType::CommitQuest:
	case EDialogueEventType::CompleteQuest:
		QuestIndex = eventValue->GetIntegerField("QuestIndex");
		break;
	case EDialogueEventType::GiveItem:
		ItemIndex = eventValue->GetIntegerField("ItemIndex");
		break;
	case EDialogueEventType::SetBookmark:
		BookmarkIndex = eventValue->GetIntegerField("BookmarkIndex");
		break;
	default:
		return false;
	}

	return true;
}

FDialogueResponse::FDialogueResponse() : Text(""), Events()
{
}

bool FDialogueResponse::LoadFromJson(const TSharedPtr<FJsonObject>& responseValue)
{
	Text = responseValue->GetStringField("Text");
	const TArray<TSharedPtr<FJsonValue>>& eventValues = responseValue->GetArrayField("Events");
	for (int i = 0; i < eventValues.Num(); ++i)
	{
		FDialogueEvent eve;
		eve.LoadFromJson(eventValues[i]->AsObject());

		Events.Add(eve);
	}

	return true;
}

FDialogueLine::FDialogueLine() : SpeakerName(""), Text("")
{
	Responses.SetNum(MAX_DIALOGUE_RESPONSE);
}


bool FDialogueLine::LoadFromJson(const TSharedPtr<FJsonObject>& lineValue)
{
	SpeakerName = lineValue->GetStringField("Speaker");
	Text = lineValue->GetStringField("Text");
	
	const TArray<TSharedPtr<FJsonValue>>& responseValues = lineValue->GetArrayField("Responses");
	check(responseValues.Num() <= MAX_DIALOGUE_RESPONSE);

	for (int i = 0; i < responseValues.Num(); ++i)
	{
		FDialogueResponse response;
		response.LoadFromJson(responseValues[i]->AsObject());

		Responses[i] = response;
	}

	return true;
}

FDialogueList::FDialogueList() : Dialogues(), Bookmark(0)
{

}

bool FDialogueList::LoadFromJson(const TSharedPtr<FJsonObject>& listValue)
{
	const TArray<TSharedPtr<FJsonValue>>& lineValues = listValue->GetArrayField("Dialogues");
	for (int i = 0; i < lineValues.Num(); ++i)
	{
		FDialogueLine line;
		line.LoadFromJson(lineValues[i]->AsObject());

		Dialogues.Add(line);
	}

	return true;
}

