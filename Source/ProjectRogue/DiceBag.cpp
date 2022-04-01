// Fill out your copyright notice in the Description page of Project Settings.


#include "DiceBag.h"

UDiceBag::UDiceBag()
{
}

UDiceBag::~UDiceBag()
{
}

int32 UDiceBag::Roll()
{
    Rolled.Empty();
    Total = 0;

    for (int32 i = 0; i < Faces.Num(); ++i)
    {
        // M: Rolls of 0 or 1 faces results in 0
        if (Faces[i] < 2)
        {
            Rolled.Add(0);
            continue;
        }

        int32 Roll = FMath::RandRange(1, Faces[i]);
        Rolled.Add(Roll);
        Total += Roll;
    }
    return Total;
}

void UDiceBag::SetDice(const TArray<int32>& ListOfFaces)
{
    if(ListOfFaces.Num() != 0)
        Faces = ListOfFaces;
}

void UDiceBag::AddDice(const TArray<int32>& ListOfFaces)
{
    Faces.Append(ListOfFaces);
}

void UDiceBag::Clear()
{
    if (Faces.Num() > 0)
    {
        Faces.Empty();
        Rolled.Empty();
    }
}
