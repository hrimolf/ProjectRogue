// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStatus.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "Adventurer.h"
#include "ProjectRogueCharacter.h"

UCharacterStatus::UCharacterStatus(const FObjectInitializer& ObjectInitializer)
    : Super{ ObjectInitializer }
{
}

void UCharacterStatus::UpdateStatus(UAdventurer* InAdventurer, EContext Context)
{
    Adventurer = InAdventurer;
    FText CharName(InAdventurer->GetCharacterName());
    //if this character doesnt have a name, something went wrong
    checkf(!CharName.IsEmpty(), TEXT("Character doesn't have a name"));
    NameText->SetText(CharName);

    //health
    FString Health = FString(" | HP : ");
    Health.Append(FString::FromInt(InAdventurer->GetCurrentHealth()));
    Health.Append("/");
    Health.Append(FString::FromInt(InAdventurer->GetMaxHealth()));
    HealthText->SetText(FText::FromString(Health));

    //mana
    FString Mana;
    //if the character slots get reordered, mana needs to be shown
    ManaText->SetVisibility(ESlateVisibility::Visible);
    //determine what kind of spell points show
    switch (InAdventurer->GetClass())
    {
        case EClass::Cleric: Mana.Append(" | PP : "); break;
        case EClass::Magician: Mana.Append(" | SP : "); break;
        default: 
        {
            ManaText->SetVisibility(ESlateVisibility::Collapsed);
            CastButton->SetVisibility(ESlateVisibility::Collapsed);
        }
    }

    Mana.Append(FString::FromInt(InAdventurer->GetCurrentMana()));
    Mana.Append("/");
    Mana.Append(FString::FromInt(InAdventurer->GetMaxMana()));
    ManaText->SetText(FText::FromString(Mana));

    //level
    FString Level = FString(" | LVL : ");
    Level.Append(FString::FromInt(InAdventurer->GetLevel()));
    LevelText->SetText(FText::FromString(Level));

    //experience
    FString Experience = FString(" | XP : ");
    Experience.Append(FString::FromInt(InAdventurer->GetXP()));
    Experience.Append(" |");
    ExpText->SetText(FText::FromString(Experience));

    float AttackCooldown = InAdventurer->GetRemainingAttackDelay(GetWorld()->GetTimerManager());
    if (AttackCooldown > 0.f)
    {
        AttackButton->SetIsEnabled(false);
    }
    else
    {
        AttackButton->SetIsEnabled(true);
    }

    if (Context == EContext::Loot || 
        Context == EContext::Shop || 
        Context == EContext::Tavern || 
        Context == EContext::Review)
    {
        ContextButton->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        ContextButton->SetVisibility(ESlateVisibility::Hidden);
    }
    switch (Context)
    {
        case EContext::Tavern: ContextSensitiveText->SetText(FText::FromString("[ [O]rder ]")); break;
        case EContext::Shop: ContextSensitiveText->SetText(FText::FromString("[ [B]uy ]")); break;
        case EContext::Review: ContextSensitiveText->SetText(FText::FromString("[ [R]eview ]")); break;
        case EContext::Loot: ContextSensitiveText->SetText(FText::FromString("[ [L]oot ]")); break;
        case EContext::None: ContextButton->SetVisibility(ESlateVisibility::Collapsed);
    }

    //disable actions if the character is dead
    if (InAdventurer->GetCurrentHealth() <= 0)
    {
        CastButton->SetIsEnabled(false);
        AttackButton->SetIsEnabled(false);
        ContextButton->SetIsEnabled(false);
    }
    else
    {
        CastButton->SetIsEnabled(true);
        ContextButton->SetIsEnabled(true);
    }
}

//T: this is necessary for the edit party screen
void UCharacterStatus::CollapseButtons()
{
    AttackButton->SetVisibility(ESlateVisibility::Collapsed);
    CastButton->SetVisibility(ESlateVisibility::Collapsed);
    ContextButton->SetVisibility(ESlateVisibility::Collapsed);
}

float UCharacterStatus::GetAttackCooldown()
{
    const FTimerHandle& CooldownHandle = Adventurer->GetAttackHandle();
    float Time = 0;
    if (CooldownHandle.IsValid())
    {
        Time = GetWorld()->GetTimerManager().GetTimerRemaining(CooldownHandle);
    }
    return Time;
}

void UCharacterStatus::ShowAttackButton()
{
    AttackButton->SetVisibility(ESlateVisibility::Visible);
}

void UCharacterStatus::HideAttackButton()
{
    AttackButton->SetVisibility(ESlateVisibility::Collapsed);
}