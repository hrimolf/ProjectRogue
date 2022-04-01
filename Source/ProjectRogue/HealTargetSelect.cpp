// Fill out your copyright notice in the Description page of Project Settings.


#include "HealTargetSelect.h"
#include "Components/VerticalBox.h"
#include "ProjectRogueCharacter.h"

void UHealTargetSelect::Populate(AProjectRogueCharacter* Player, UAdventurer* Source, ASpell* Spell)
{
    TargetBox->ClearChildren();
    TArray<UAdventurer*> Party = Player->GetParty();

    //if we're casting a spell, allow Caster to be a target
    if (Spell)
    {
        for (const auto& Adventurer : Party)
        {
            AddPartyMember(Adventurer, Player, Spell);
        }
    }
    //otherwise, we're giving an item, so Caster cant be target
    else
    {
        for (const auto& Adventurer : Party)
        {
            if (Adventurer == Source)
                continue;

            AddPartyMember(Adventurer, Player, Spell);
        }
    }
}