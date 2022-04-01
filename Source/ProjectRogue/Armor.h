// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment.h"
#include "Armor.generated.h"

/**
 *
 */
UCLASS()
class PROJECTROGUE_API UArmor : public UEquipment
{
	GENERATED_BODY()
private:
	int32 FixedProtection;

public:
	UArmor();
	~UArmor();

	virtual void OnUse(UAdventurer* Adventurer, int32 InventoryIndex) override;

	void SetFixedProtection(int32 Amount) { FixedProtection = Amount; }
	
	// M: Returns a random protection roll, or FixedProtection.
	//	VALUE CAN CHANGE EVERY CALL
	int32 GetProtection();

};