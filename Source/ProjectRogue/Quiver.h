// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Quiver.generated.h"

/**
 * 
 */
 UCLASS()
class PROJECTROGUE_API UQuiver : public UItem
{
	GENERATED_BODY()
private:
	UPROPERTY()
	UItem* Item;
	int32 MaxItems;
	int32 Count;

public:
	UQuiver();
	~UQuiver();

	void Init(UItem* Item, int32 Max);

	// Return true if this item is contained within
	bool Contains(UItem* Item) const;

	// Return true if add successful
	bool AddOne();

	// Return true after successful removal (can only have > 0)
	bool RemoveOne();

	// If Count is zero, items cannot be added or removed from
	//	the container (consider it invalid, and must be 
	//	destroyed)
	UFUNCTION(BlueprintCallable)
	int32 GetCount() const { return Count; }

	UFUNCTION(BlueprintCallable)
	int32 GetMaxCount() const { return MaxItems; }

	//T: we need to check what kind of item this is for UI
	UItem* GetContainedItem() const { return Item; }
};
