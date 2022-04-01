// Fill out your copyright notice in the Description page of Project Settings.


#include "Armor.h"
#include "Adventurer.h"

UArmor::UArmor()
	: FixedProtection(0)
{
	SetType(EItemType::Armor);
}

UArmor::~UArmor()
{
}

void UArmor::OnUse(UAdventurer* Adventurer, int32 InventoryIndex)
{
	Adventurer->EquipItem(InventoryIndex);
}

int32 UArmor::GetProtection()
{
	if(FixedProtection != 0)
		return FixedProtection;
	return GetDiceBag()->Roll();
}
