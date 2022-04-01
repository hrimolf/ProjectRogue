// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellSelect.h"
#include "Components/VerticalBox.h"
#include "SpellSlot.h"
#include "Spell.h"

void USpellSelect::Update(UAdventurer* InAdventurer)
{
    Adventurer = InAdventurer;
    SpellBox->ClearChildren();
    const TArray<ASpell*>& Spells = InAdventurer->GetSpells();
    for (int32 Index = 0; Index < Spells.Num(); ++Index)
    {
        bool bCanUse = InAdventurer->GetLevel() >= Spells[Index]->GetLevel();
        SpellAdded(Spells[Index], Index + 1, bCanUse);
    }
}