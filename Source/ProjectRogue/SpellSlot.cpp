// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellSlot.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Spell.h"

void USpellSlot::Update(ASpell* InSpell, const int32 InIndex)
{
    check(InSpell);
    Spell = InSpell;
    LevelText->SetText(FText::FromString(FString::FromInt(InSpell->GetLevel())));
    ManaCostText->SetText(FText::FromString(FString::FromInt(InSpell->GetManaCost())));
    int32 Range = InSpell->GetRange();
    if (Range == 0)
    {
        RangeText->SetText(FText::FromString("Party"));
    }
    else
    {
        RangeText->SetText(FText::FromString(FString::FromInt(Range)));
    }
    NameText->SetText(FText::FromString("[" + FString::FromInt(InIndex) + "]" + InSpell->GetName().ToString()));
    DescriptionText->SetText(InSpell->GetDescription());

    SpellButton->SetIsEnabled(bCanUse);
}