// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Adventurer.h"
#include "ReviewBoard.generated.h"

class UPartyController;

USTRUCT(Blueprintable)
struct FExpToLevelRequirements
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> ExpPerLevel;
};

/**
 * 
 */
 UCLASS()
class PROJECTROGUE_API AReviewBoard : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<EClass, FExpToLevelRequirements> LevelRequirements;

public:
	//T: we can pass a 'this' pointer from adventurer
	bool CanLevelUp(UAdventurer* Adventurer);
	bool LevelUp(UAdventurer* Adventurer);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetExpToLevel(UAdventurer* Adventurer);
};
