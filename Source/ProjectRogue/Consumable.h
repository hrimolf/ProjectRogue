// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include <functional>
#include "Consumable.generated.h"

class UAdventurer;

/**
 *
 */
 UCLASS()
class PROJECTROGUE_API UConsumable : public UItem
{
	GENERATED_BODY()
private:
	std::function<void(UAdventurer* Adventurer)> UseFunction;

public:
	UConsumable();
	~UConsumable();

	UFUNCTION(BlueprintCallable)
	virtual void OnUse(UAdventurer* Adventurer, int32 InventoryIndex) override;

	void SetUseFunction(std::function<void(UAdventurer* Adventurer)> Function) { UseFunction = Function; }
};
