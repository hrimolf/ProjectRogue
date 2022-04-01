// Fill out your copyright notice in the Description page of Project Settings.


#include "HealTarget.h"
#include "Components/TextBlock.h"
#include "ProjectRogueCharacter.h"
#include "Adventurer.h"

void UHealTarget::Update(UAdventurer* InAdventurer)
{
    Adventurer = InAdventurer;
    NameText->SetText(InAdventurer->GetCharacterName());
    int CurrentHealth = InAdventurer->GetCurrentHealth();
    int MaxHealth = InAdventurer->GetMaxHealth();
    FString Health(FString::FromInt(CurrentHealth) + "/" + FString::FromInt(MaxHealth));
    HealthText->SetText(FText::FromString(Health));
}

void UHealTarget::Selected()
{
    Player->SetTarget(Adventurer);
    //TODO: this is seems really dirty
    if (Spell)
    {
        Player->OnSpellCasted(Spell);
    }
    else
    {
        Player->GiveItem();
    }
}