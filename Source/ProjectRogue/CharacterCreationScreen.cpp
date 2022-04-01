// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterCreationScreen.h"
#include "Components/TextBlock.h"
#include "Components/ComboBoxString.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Adventurer.h"
#include "CharacterInfoScreen.h"
#include "PartyController.h"

void UCharacterCreationScreen::Open(UAdventurer* InAdventurer, int32 InPartyIndex)
{
    PartyIndex = InPartyIndex;
    Adventurer = InAdventurer;
    SetVisibility(ESlateVisibility::Visible);
    Update();
    CharacterInfo->OpenCreateCharacter();
    ConfirmButton->SetIsEnabled(false);
    RaceSelector->SetSelectedOption(FString("Human"));
    ClassSelector->SetSelectedOption(FString("Warrior"));
    ModelSelector->SetSelectedOption(FString("ModelA"));
    NameTextField->SetText(FText());
}

void UCharacterCreationScreen::Update()
{
    CharacterInfo->Update(Adventurer, Player);
    FString StartingGold = "Starting Gold: " + FString::FromInt(Adventurer->GetGold());
    GoldText->SetText(FText::FromString(StartingGold));
    CharacterInfo->OpenCreateCharacter();
}

void UCharacterCreationScreen::ConfirmAdventurer()
{
    UE_LOG(LogTemp, Warning, TEXT("Created Adventurer with name %s"), *Name.ToString());
    Player->SetContext(EContext::Guild);
    Adventurer->TransferGoldToParty();
    SetVisibility(ESlateVisibility::Hidden);
}

bool UCharacterCreationScreen::IsNameAvailable() const
{
    check(Player);
    return Player->IsNameAvailable(Adventurer, Name);
}