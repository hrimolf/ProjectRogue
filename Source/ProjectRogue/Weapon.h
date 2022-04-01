// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment.h"
#include "Weapon.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTROGUE_API UWeapon : public UEquipment
{
	GENERATED_BODY()
private:
	int32 FixedDamage;
	int32 Range;

public:
	UWeapon();
	~UWeapon();

	virtual void OnUse(UAdventurer* Adventurer, int32 InventoryIndex) override;

	void SetFixedDamage(int32 Amount) { FixedDamage = Amount; }
	void SetRange(int32 Amount) { Range = Amount; }

	// M: Returns FixedDamage (if it's specified) or the dice
	//	bag roll. THIS CAN CHANGE EVERY CALL.
	int32 GetDamage();
	int32 GetRange() { return Range; }
};
