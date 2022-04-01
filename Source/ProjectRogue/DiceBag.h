// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DiceBag.generated.h"

/**
 * A generic dice-rolling structure that contains previous
    rolls, total of previous rolls, and a list of dice to roll
 */
 UCLASS(Blueprintable)
class PROJECTROGUE_API UDiceBag : public UObject
{
	GENERATED_BODY()

private:
	TArray<int32> Faces;
	TArray<int32> Rolled;
	int32 Total;

public:
    UDiceBag();
    ~UDiceBag();

    int32 Roll();
    int32 GetTotal() { return Total; }
    const TArray<int32>& GetFaces() const { return Faces; }
    const TArray<int32>& GetRolls() const { return Rolled; }

    void AddDie(int32 NumberOfFaces) { Faces.Add(NumberOfFaces); }
    void SetDice(const TArray<int32>& ListOfFaces);
    void AddDice(const TArray<int32>& ListOfFaces);
    void Clear();
};
