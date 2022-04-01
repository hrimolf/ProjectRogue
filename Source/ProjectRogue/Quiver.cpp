// Fill out your copyright notice in the Description page of Project Settings.


#include "Quiver.h"

UQuiver::UQuiver()
{
	SetType(EItemType::Container);
}

UQuiver::~UQuiver()
{
}

void UQuiver::Init(UItem* InItem, int32 Max)
{
	const FText& ItemName = InItem->GetItemName();
	Super::Init("Quiver", 0, {}, "UPDATE ME");
	Count = 1;
	Item = InItem;
	MaxItems = Max;
}

bool UQuiver::Contains(UItem* InItem) const
{
	return (Item == InItem);
}

// If we're not at capacity, add one
bool UQuiver::AddOne()
{
	if (Count < MaxItems && Count > 0)
	{
		++Count;
		return true;
	}
	return false;
}

// If theres items, remove one
bool UQuiver::RemoveOne()
{
	if (Count > 0)
	{
		--Count;
		return true;
	}
	return false;
}
