// Fill out your copyright notice in the Description page of Project Settings.


#include "ReviewBoard.h"
#include "PartyController.h"

bool AReviewBoard::CanLevelUp(UAdventurer* Adventurer)
{
	int ExpToLevel = GetExpToLevel(Adventurer);

	if ((ExpToLevel > 0) && (Adventurer->GetXP() >= ExpToLevel))
		return true;
	return false;
}

bool AReviewBoard::LevelUp(UAdventurer* Adventurer)
{
	return CanLevelUp(Adventurer);
}

int AReviewBoard::GetExpToLevel(UAdventurer* Adventurer)
{
	unsigned int Level = Adventurer->GetLevel();
	EClass Class = Adventurer->GetClass();
	
	if (!LevelRequirements[Class].ExpPerLevel.IsValidIndex(Level))
		return -1;

	return LevelRequirements[Class].ExpPerLevel[Level];
}
