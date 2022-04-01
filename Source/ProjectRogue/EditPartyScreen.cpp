// Fill out your copyright notice in the Description page of Project Settings.


#include "EditPartyScreen.h"
#include "Templates/UnrealTemplate.h"
#include "PartyInterface.h"
#include "ProjectRogueCharacter.h"

void UEditPartyScreen::DeletePartyMember(int32 Index)
{
    Player->DeleteAdventurer(Index);
    PopulatePartyInterface(Player, EPartyEditType::Delete);
    Update();
}

void UEditPartyScreen::MoveUp(int32 Index)
{
    //if index is 0, cycle back to the end
    if (Index == 0)
    {
        int32 PartySize = Player->GetPartySize() - 1;
        Player->SwapAdventurer(Index, PartySize);
    }
    else
    {
        Player->SwapAdventurer(Index, Index - 1);
    }
    Update();
}

void UEditPartyScreen::MoveDown(int32 Index)
{
    //if index is at the end, cycle back to 0
    int32 PartySize = Player->GetPartySize() - 1;
    if (Index == PartySize)
    {
        Player->SwapAdventurer(Index, 0);
    }
    else
    {
        Player->SwapAdventurer(Index, Index + 1);
    }
    Update();
}

void UEditPartyScreen::PopulatePartyInterface(AProjectRogueCharacter* InPlayer, EPartyEditType InEditType)
{
    EditType = InEditType;
    Player = InPlayer;
    const TArray<UAdventurer*>& Party = InPlayer->GetParty();
    PartyInterface->ClearPartyInterface();
    PartyInterface->SetPlayer(InPlayer);
    for (const auto& Adventurer : Party)
    {
        PartyInterface->AddPartyMember(Adventurer);
    }
    PartyInterface->HideButtons();
    PopulateEditBox(Party.Num());
    Update();
}

void UEditPartyScreen::RenameAdventurer(int32 Index)
{
    Player->RenameClicked(Index);
}

void UEditPartyScreen::ConfirmParty()
{
    Player->HideEditParty();
}

void UEditPartyScreen::Update()
{
    const TArray<UAdventurer*> Party = Player->GetParty();
    for (auto& Adventurer : Party)
    {
        PartyInterface->UpdatePartySlot(Adventurer, Adventurer->GetPartyIndex());
    }
}