// Fill out your copyright notice in the Description page of Project Settings.


#include "Consumable.h"

UConsumable::UConsumable()
{
	SetType(EItemType::Consumable);
}

UConsumable::~UConsumable()
{
}

void UConsumable::OnUse(UAdventurer* Adventurer, int32 InventoryIndex)
{
	UseFunction(Adventurer);
}
