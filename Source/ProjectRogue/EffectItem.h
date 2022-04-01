// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include <functional>
#include "EffectItem.generated.h"

class UAdventurer;

/**
 * This item applies an immediate effect when added to character
 *	inventory.
 */
 UCLASS()
class PROJECTROGUE_API UEffectItem : public UItem
{
	GENERATED_BODY()
private:
	std::function<void(UAdventurer*)> EffectFunction;

public:
	UEffectItem();
	~UEffectItem();

	void OnEffect(UAdventurer* Adventurer);

	void SetEffectFunction(std::function<void(UAdventurer*)> Function) { EffectFunction = Function; }
};
