// Fill out your copyright notice in the Description page of Project Settings.


#include "EffectItem.h"

UEffectItem::UEffectItem()
{
	SetType(EItemType::Effect);
}

UEffectItem::~UEffectItem()
{
}

void UEffectItem::OnEffect(UAdventurer* Adventurer)
{
	EffectFunction(Adventurer);
}
