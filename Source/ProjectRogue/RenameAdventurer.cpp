// Fill out your copyright notice in the Description page of Project Settings.


#include "RenameAdventurer.h"
#include "Components/TextBlock.h"
#include "Adventurer.h"

void URenameAdventurer::SetAdventurer(UAdventurer* InAdventurer)
{
    Adventurer = InAdventurer;
    FString Name = "Enter a new name for " + Adventurer->GetCharacterName().ToString();
    NameLabel->SetText(FText::FromString(Name));
}