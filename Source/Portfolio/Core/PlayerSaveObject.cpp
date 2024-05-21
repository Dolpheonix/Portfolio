#include "PlayerSaveObject.h"

static_assert(static_cast<uint8>(EItemType::Count) == 3);

UPlayerSaveObject::UPlayerSaveObject()
{
}

USaveSlot::USaveSlot()
{
}

void PlayerInfo::ConvertFromProto(const SaveObject::PlayerInfo& info)
{
	PlayerName = info.name().c_str();
	CurrentLevel = info.level();
	CurrentMap = info.map();
	CurrentLocation = FVector(info.loc_x(), info.loc_y(), info.loc_z());
	CurrentGold = info.gold();

	// Inventory
	const SaveObject::Inventory& invenInfo = info.inventory();

	assert(invenInfo.typeinventory_size() == static_cast<uint8>(EItemType::Count));

	for (int i = 0; i < static_cast<uint8>(EItemType::Count); ++i)
	{
		FTypeInventory& typeInventory = Inventory.TypeInventoryList[i];
		const SaveObject::TypeInventory& typeInvenInfo = invenInfo.typeinventory(i);

		for (int j = 0; j < typeInvenInfo.items_size(); ++j)
		{
			const SaveObject::GameItem& gameItemInfo = typeInvenInfo.items(j);
			typeInventory.ItemList.Add(FGameItem(gameItemInfo.index(), gameItemInfo.num()));
		}
	}

	// QuestStatus
	for (int i = 0; i < info.queststatus_size(); ++i)
	{
		const SaveObject::QuestStatus& qsInfo = info.queststatus(i);

		FQuestStatus qs;

		qs.Index = qsInfo.index();
		qs.Type = static_cast<EQuestType>(qsInfo.type());
		qs.CurrProgress = static_cast<EQuestProgressType>(qsInfo.progresstype());
		
		if (qs.Type == EQuestType::Serial)
		{
			qs.CurrPhase = qsInfo.currphase();
		}
		else
		{
			qs.Completed = qsInfo.completed();
		}

		for (int j = 0; j < qsInfo.substatus_size(); ++j)
		{
			const SaveObject::QuestStatus_SubQuestStatus& sqsInfo = qsInfo.substatus(j);
			
			FSubQuestStatus sqs;

			sqs.Type = static_cast<ESubQuestType>(sqsInfo.type());
			sqs.bStarted = sqsInfo.bstarted();
			sqs.bCompleted = sqsInfo.bcompleted();
			sqs.CurrAmount = sqsInfo.curramount();

			qs.SubStatus.Add(sqs);
		}

		QuestTable.Add(qs);
	}
}

void PlayerInfo::ConvertToProto(SaveObject::PlayerInfo& info)
{
	info.set_name(TCHAR_TO_ANSI(*PlayerName));
	info.set_level(CurrentLevel);
	info.set_map(CurrentMap);
	info.set_loc_x(CurrentLocation.X);
	info.set_loc_y(CurrentLocation.Y);
	info.set_loc_z(CurrentLocation.Z);
	info.set_gold(CurrentGold);
	
	// Inventory
	auto inventory = info.mutable_inventory();
	for (int i = 0; i < static_cast<uint8>(EItemType::Count); ++i)
	{
		auto typeInventory = inventory->add_typeinventory();
		
		for (int j = 0; j < Inventory.TypeInventoryList[i].ItemList.Num(); ++j)
		{
			auto item = typeInventory->add_items();
			item->set_index(Inventory.TypeInventoryList[i].ItemList[j].InfoIndex);
			item->set_num(Inventory.TypeInventoryList[i].ItemList[j].Num);
		}
	}

	// QuestStatus
	for (int i = 0; i < QuestTable.Num(); ++i)
	{
		auto qs = info.add_queststatus();

		qs->set_index(QuestTable[i].Index);
		qs->set_type(static_cast<SaveObject::QuestStatus_QuestType>(QuestTable[i].Type));
		qs->set_progresstype(static_cast<SaveObject::QuestStatus_QuestProgressType>(QuestTable[i].CurrProgress));
		
		if (QuestTable[i].Type == EQuestType::Serial)
		{
			qs->set_currphase(QuestTable[i].CurrPhase);
		}
		else
		{
			qs->set_completed(QuestTable[i].Completed);
		}

		for (int j = 0; j < QuestTable[i].SubStatus.Num(); ++j)
		{
			auto sqs = info.mutable_queststatus(i)->add_substatus();
			const auto& sqsInfo = QuestTable[i].SubStatus[j];

			sqs->set_type(static_cast<SaveObject::QuestStatus_SubQuestStatus_SubQuestType>(sqsInfo.Type));
			sqs->set_bstarted(sqsInfo.bStarted);
			sqs->set_bcompleted(sqsInfo.bCompleted);
			sqs->set_curramount(sqsInfo.CurrAmount);
		}
	}
}
