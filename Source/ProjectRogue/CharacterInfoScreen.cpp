// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterInfoScreen.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Components/Image.h"
#include "Adventurer.h"
#include "DiceBag.h"

void UCharacterInfoScreen::Update(UAdventurer* InAdventurer, AProjectRogueCharacter* InPlayer)
{
    check(InAdventurer);
    Player = InPlayer;
    Adventurer = InAdventurer;

    //stat modifiers
    SetStatTextAndVisiblity(InAdventurer, EStat::Strength,      StrengthText,       StrengthModifier);
    SetStatTextAndVisiblity(InAdventurer, EStat::Intelligence,  IntelligenceText,   IntelligenceModifier);
    SetStatTextAndVisiblity(InAdventurer, EStat::Wisdom,        WisdomText,         WisdomModifier);
    SetStatTextAndVisiblity(InAdventurer, EStat::Dexterity,     DexterityText,      DexterityModifier);
    SetStatTextAndVisiblity(InAdventurer, EStat::Constitution,  ConstitutionText,   ConstitutionModifier);

    auto AdventurerIcon = InAdventurer->GetIcon();
    Icon->SetBrush(AdventurerIcon);
    Icon->SetVisibility(ESlateVisibility::Visible);

    FString Health = FString::FromInt(InAdventurer->GetCurrentHealth());
    FString Mana = FString::FromInt(InAdventurer->GetCurrentMana());
    Health.Append(" / ");
    Health.Append(FString::FromInt(InAdventurer->GetMaxHealth()));
    Mana.Append(" / ");
    Mana.Append(FString::FromInt(InAdventurer->GetMaxMana()));
    HealthText->SetText(FText::FromString(Health));
    ManaText->SetText(FText::FromString(Mana));

    auto& Data = Player->GetRaceCombatData(InAdventurer->GetRace());
    auto Itr = Data.Find(ECombatDataType::HitPoints);
    if (!Itr)
        return;

    auto& CombatData = *Itr;
    HealthDice->            SetText(FText::FromString(FString::FromInt(CombatData.DiceThrown)));
    HealthDieFaces->        SetText(FText::FromString(FString::FromInt(CombatData.DieFaces)));
    int DiceCount = 0;
    int FaceCount = 0;

    //T: these will be true unless the class is warrior, so set it once here
    ManaTypeText->SetIsEnabled(true);
    ManaText->SetIsEnabled(true);
    ManaDice->SetIsEnabled(true);
    ManaDieFaces->SetIsEnabled(true);
    ManaBox->SetVisibility(ESlateVisibility::Visible);
    ManaDiceBox->SetVisibility(ESlateVisibility::Visible);
    NameText->SetText(Adventurer->GetCharacterName());
    FString RaceAndClass;
    switch (Adventurer->GetRace())
    {
        case ERace::Human:  RaceAndClass += "Human "; break;
        case ERace::Dwarf: RaceAndClass += "Dwarf "; break;
        case ERace::Elf:    RaceAndClass += "Elf "; break;
        case ERace::Orc:    RaceAndClass += "Orc "; break;
    }

    switch (Adventurer->GetClass())
    {
        case EClass::Warrior:
        {
            RaceAndClass += "Warrior";
            ManaTypeText->SetText(FText::FromString("Spell Points"));
            ManaBox->SetVisibility(ESlateVisibility::Hidden);
            ManaDiceBox->SetVisibility(ESlateVisibility::Hidden);
        }
        break;
        case EClass::Magician:
        {
            ManaPerLevelText->SetText(FText::FromString("SP Per Level:"));
            ManaModifierText->SetText(FText::FromString("+ Int Modifier"));
            RaceAndClass += "Magician";
            ManaTypeText->SetText(FText::FromString("Spell Points"));
            ManaBox->SetToolTipText(FText::FromString("The amount of Spell Points you gain when you level up"));
            Itr = Data.Find(ECombatDataType::SpellPoints);
            if (!Itr)
                return;
            auto& ManaData = *Itr;
            DiceCount = ManaData.DiceThrown;
            FaceCount = ManaData.DieFaces;
        }
        break;
        case EClass::Cleric:
        {
            ManaPerLevelText->SetText(FText::FromString("PP Per Level:"));
            ManaModifierText->SetText(FText::FromString("+ Wis Modifier"));
            RaceAndClass += "Cleric";
            ManaTypeText->SetText(FText::FromString("Prayer Points"));
            ManaBox->SetToolTipText(FText::FromString("The amount of Prayer Points you gain when you level up"));
            Itr = Data.Find(ECombatDataType::PrayerPoints);
            if (!Itr)
                return;
            auto& ManaData = *Itr;
            DiceCount = ManaData.DiceThrown;
            FaceCount = ManaData.DieFaces;
        }
        break;
    }

    RaceAndClass += " Level " + FString::FromInt(InAdventurer->GetLevel());
    RaceAndClassText->SetText(FText::FromString(RaceAndClass));

    ManaDice->    SetText(FText::FromString(FString::FromInt(DiceCount)));
    ManaDieFaces->SetText(FText::FromString(FString::FromInt(FaceCount)));
    HealthDiceBox->SetToolTipText(FText::FromString("The amount of Hit Points you gain when you level up"));

    //the EXP box is hidden when in regular info, but visible in review board
    ExpBox->SetVisibility(ESlateVisibility::Hidden);
}

void UCharacterInfoScreen::SetStatTextAndVisiblity(UAdventurer* InAdventurer, EStat Stat, UTextBlock* InStatTextBlock, UTextBlock* InStatModifierBlock)
{
    int StatValue = InAdventurer->GetStatValue(Stat);
    int StatModifier = InAdventurer->GetStatModifier(Stat);
    FString Value = FString::FromInt(StatValue);
    FString Modifier = FString::FromInt(StatModifier);
    FString Symbol;
    
    if (StatModifier == 0)
    {
        InStatModifierBlock->SetVisibility(ESlateVisibility::Hidden);
    }
    else
    {
        Symbol += "(";
        if (StatModifier > 0)
        {
            Symbol += "+";
        }
        InStatModifierBlock->SetVisibility(ESlateVisibility::Visible);
    }

    InStatTextBlock->SetText(FText::FromString(Value));
    InStatModifierBlock->SetText(FText::FromString(Symbol + Modifier + ")"));
}

void UCharacterInfoScreen::OpenReviewBoard(UAdventurer* InAdventurer)
{
    Adventurer = InAdventurer;
    Update(Adventurer, Player);
    int CurrentExp = Adventurer->GetXP();
    int RequiredExp = Adventurer->GetExpToLevel();
    if (RequiredExp == -1)
    {
        ExpBox->SetVisibility(ESlateVisibility::Hidden);
    }
    else
    {
        ExpBox->SetVisibility(ESlateVisibility::Visible);
        CurrentExpText->SetText(FText::FromString(FString::FromInt(CurrentExp)));
        RequiredExpText->SetText(FText::FromString(FString::FromInt(RequiredExp)));
    }

    if (Adventurer->CanLevelUp())
    {
        CurrentExpText->SetColorAndOpacity(FSlateColor(FLinearColor(255, 255, 255)));
        PlusStrength->SetVisibility(ESlateVisibility::Visible);
        PlusIntelligence->SetVisibility(ESlateVisibility::Visible);
        PlusWisdom->SetVisibility(ESlateVisibility::Visible);
        PlusDexterity->SetVisibility(ESlateVisibility::Visible);
        PlusConstitution->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        CurrentExpText->SetColorAndOpacity(FSlateColor(FLinearColor(255, 0, 0)));
        PlusStrength->SetVisibility(ESlateVisibility::Hidden);
        PlusIntelligence->SetVisibility(ESlateVisibility::Hidden);
        PlusWisdom->SetVisibility(ESlateVisibility::Hidden);
        PlusDexterity->SetVisibility(ESlateVisibility::Hidden);
        PlusConstitution->SetVisibility(ESlateVisibility::Hidden);
    }
    Player->UpdatePartyInterface();
}

void UCharacterInfoScreen::OpenCreateCharacter()
{
    NameText->          SetVisibility(ESlateVisibility::Hidden);
    RaceAndClassText->  SetVisibility(ESlateVisibility::Hidden);
    Spacer->            SetVisibility(ESlateVisibility::Hidden);
    ExpBox->            SetVisibility(ESlateVisibility::Hidden);
    Icon->              SetVisibility(ESlateVisibility::Collapsed);
}