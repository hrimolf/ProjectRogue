// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Adventurer.h"
#include "PartyController.generated.h"

class AReviewBoard;
class AProjectRogueCharacter;
struct FCombatDice;

/**
 * The player character. the one that moves in the world
 */
 UCLASS()
class PROJECTROGUE_API UPartyController : public UObject
{
	GENERATED_BODY()

	friend AProjectRogueCharacter;

private:
	//T: manage a pointer to the review board so we can access leveling
	// functionality from the party controller
	AReviewBoard* ReviewBoard;
	UPROPERTY()
	TArray<UAdventurer*> Adventurers;
	int32 Gold;

protected:
	UPROPERTY(EditAnywhere)
	int32 MaxPartySize;

	AProjectRogueCharacter* Player;

public:
	UPartyController();

	UFUNCTION(BlueprintCallable)
	UAdventurer* GetAdventurer(int32 Index);

	int32 GetPartySize() const { return Adventurers.Num(); }
	int32 GetLivingAdventurerCount() const;
	const TArray<UAdventurer*>& GetParty() const { return Adventurers; }
	void SetParty(TArray<UAdventurer*> Party) { Adventurers = Party; }
	int32 GetGold() { return Gold; }

	void AdjustGold(int32 Amount) { Gold += Amount; }
	void AddInventorySlots(int32 Amount, int32 Index);

	UAdventurer* AddAdventurer();
	void DeleteAdventurer(int32 Index);
	void SwapAdventurer(int32 FromIndex, int32 ToIndex);
	bool RenameAdventurer(int32 Index, const FText& NewName);
	AProjectRogueCharacter* GetPlayer() const { return Player; }

	void SetReviewBoard(AReviewBoard* InReviewBoard) { ReviewBoard = InReviewBoard; }
	bool CanLevelUp(UAdventurer* Adventurer) const;
	int GetExpToLevel(UAdventurer* Adventurer) const;
	bool LevelUp(UAdventurer* Adventurer) const;

	void RestoreParty();
	void PartyMemberDied(int32 Index);
	bool IsPartyDead() const;
	bool IsNameAvailable(UAdventurer* TempAdventurer, const FText& InName) const;

	const TMap<ECombatDataType, FCombatDice>& GetRaceCombatData(ERace Race);
	const TMap<ECombatDataType, FCombatDice>& GetClassCombatData(EClass Class);
};
