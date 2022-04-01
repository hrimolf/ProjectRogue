// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Adventurer.h"
#include "SaveData.generated.h"

class AProjectRogueCharacter;

USTRUCT()
struct FAdventurerData
{
	GENERATED_BODY()

	UPROPERTY()
	FText Name;
	UPROPERTY()
	ERace Race;
	UPROPERTY()
	EClass Class;
	UPROPERTY()
	FSlateBrush Icon;
	UPROPERTY()
	TMap<EStat, int32> Stats;
	UPROPERTY()
	int32 PartyIndex;
	UPROPERTY()
	int32 MaxHealth;
	UPROPERTY()
	int32 MaxMana;
	UPROPERTY()
	int32 Level;
	UPROPERTY()
	int32 Experience;
	UPROPERTY()
	TArray<int32> EquippedItems;
	UPROPERTY()
	TArray<FText> InventoryItems;
};

/**
 * 
 */
UCLASS()
class PROJECTROGUE_API USaveData : public USaveGame
{
	GENERATED_BODY()

private:
	// M: TArray only takes in pointers *That's dumb*
	UPROPERTY()
	TArray<FAdventurerData> Party;

	UPROPERTY()
	int32 Gold;

public:
	UFUNCTION(BlueprintCallable)
	void SavePartyData(AProjectRogueCharacter* Player);

	UFUNCTION(BlueprintCallable)
	void LoadPartyData(AProjectRogueCharacter* Player);
};
