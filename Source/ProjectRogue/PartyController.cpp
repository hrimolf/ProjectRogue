// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyController.h"
#include "ProjectRogueCharacter.h"
#include "ReviewBoard.h"

UPartyController::UPartyController()
{
	MaxPartySize = 4;
}

UAdventurer* UPartyController::GetAdventurer(int32 Index)
{
	if (!Adventurers.IsValidIndex(Index))
		return nullptr;

	return Adventurers[Index];
}

int32 UPartyController::GetLivingAdventurerCount() const
{
	int32 Count = 0;
	for (const auto& Adventurer : Adventurers)
	{
		if (Adventurer->GetIsAlive())
		{
			++Count;
		}
	}
	return Count;
}

void UPartyController::AddInventorySlots(int32 Amount, int32 Index)
{
	if (!Adventurers.IsValidIndex(Index))
		return;

	UAdventurer* Adventurer = Adventurers[Index];
	Adventurer->AddInventorySlots(Index);
}

UAdventurer* UPartyController::AddAdventurer()
{
	if(Adventurers.Num() >= MaxPartySize)
		return nullptr;

	uint32 Index = Adventurers.Add(NewObject<UAdventurer>());
	Adventurers[Index]->SetPartyController(this);
	Adventurers[Index]->SetPartyIndex(Index);
	return Adventurers[Index];
}

void UPartyController::DeleteAdventurer(int32 Index)
{
	if(!Adventurers.IsValidIndex(Index))
		return;

	//modify gold before removing from away to avoid dividing by 0
	int32 GoldDividend = Gold / Adventurers.Num();
	
	Adventurers.RemoveAt(Index);
	AdjustGold(-GoldDividend);
}

bool UPartyController::RenameAdventurer(int32 Index, const FText& NewName)
{
	check(Index < Adventurers.Num());
	if (!IsNameAvailable(Adventurers[Index], NewName))
		return false;
	Adventurers[Index]->RenameCharacter(NewName);
	return true;
}

void UPartyController::SwapAdventurer(int32 FromIndex, int32 ToIndex)
{
	if (FromIndex != ToIndex && 
		Adventurers.IsValidIndex(FromIndex) && 
		Adventurers.IsValidIndex(ToIndex))
	{
		Adventurers[FromIndex]->SetPartyIndex(ToIndex);
		Adventurers[ToIndex]->SetPartyIndex(FromIndex);
		Adventurers.Swap(FromIndex, ToIndex);
	}
}

const TMap<ECombatDataType, FCombatDice>& UPartyController::GetRaceCombatData(ERace Race)
{
	return Player->GetRaceCombatData(Race);
}

const TMap<ECombatDataType, FCombatDice>& UPartyController::GetClassCombatData(EClass Class)
{
	return Player->GetClassCombatData(Class);
}

bool UPartyController::CanLevelUp(UAdventurer* Adventurer) const
{
	if (!ReviewBoard)
		return false;
	return ReviewBoard->CanLevelUp(Adventurer);
}

int UPartyController::GetExpToLevel(UAdventurer* Adventurer) const
{
	check(ReviewBoard);
	return ReviewBoard->GetExpToLevel(Adventurer);
}

bool UPartyController::LevelUp(UAdventurer* Adventurer) const
{
	check(ReviewBoard);
	return ReviewBoard->LevelUp(Adventurer);
}

void UPartyController::RestoreParty()
{
	for (auto& Adventurer : Adventurers)
	{
		Adventurer->RestoreHealth(Adventurer->GetMaxHealth());
		Adventurer->RestoreMana(Adventurer->GetMaxMana());
	}
}

void UPartyController::PartyMemberDied(int32 Index)
{
	//if party member dies, move it to last slot and move the rest of the party up
	size_t Count = Adventurers.Num();
	Adventurers[Index]->SetPartyIndex(Count - 1);
	for (int32 i = Index + 1; i < Count; ++i)
	{
		Adventurers[i]->SetPartyIndex(i - 1);
		Adventurers.Swap(i, i - 1);
	}
}

bool UPartyController::IsPartyDead() const
{
	for (const auto& Adventurer : Adventurers)
	{
		if (Adventurer->GetCurrentHealth() > 0)
			return false;
	}
	return true;
}

bool UPartyController::IsNameAvailable(UAdventurer* TempAdventurer, const FText& InName) const
{
	const auto& Party = Player->GetParty();
	for (const auto& PartyMember : Party)
	{
		const auto& CharName = PartyMember->GetCharacterName();
		const auto& CharString = CharName.ToString();
		//dont check for the adventurer that we are working on
		if (InName.ToString() == "Name Is Unavailable")
			return false;
		if (PartyMember == TempAdventurer)
			continue;
		if (CharString == InName.ToString())
			return false;
	}
	return true;
}