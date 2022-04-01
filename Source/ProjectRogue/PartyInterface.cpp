// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyInterface.h"
#include "CharacterStatus.h"
#include "Components/VerticalBox.h"
#include "Components/ComboBox.h"
#include "Adventurer.h"

void UPartyInterface::SetPlayer(AProjectRogueCharacter* InPlayer)
{
    Player = InPlayer;
}

void UPartyInterface::ClearPartyInterface()
{
    StatusList->ClearChildren();
}

void UPartyInterface::DeletePartyMember(const int32 index)
{
    StatusList->RemoveChildAt(index);
}

void UPartyInterface::AddPartyMember(UAdventurer* Adventurer)
{
    check(Adventurer);
    PartyMemberAdded(Adventurer);
}

void UPartyInterface::UpdatePartySlot(UAdventurer* Adventurer, int32 Index)
{
    check(Adventurer);
    const auto& Child = StatusList->GetChildAt(Index);
    UCharacterStatus* Status = Cast<UCharacterStatus>(Child);
    if (Status)
    {
        Status->UpdateStatus(Adventurer, Player->GetContext());
    }
}

void UPartyInterface::HideButtons()
{
    const auto& StatusArray = StatusList->GetAllChildren();
    for (auto& Widget : StatusArray)
    {
        UCharacterStatus* Status = Cast<UCharacterStatus>(Widget);
        Status->CollapseButtons();
    }
}

void UPartyInterface::ShowAttackButton(UAdventurer* Adventurer)
{
    check(Adventurer);
    const auto& Child = StatusList->GetChildAt(Adventurer->GetPartyIndex());
    UCharacterStatus* Status = Cast<UCharacterStatus>(Child);
    if (Status)
    {
        Status->ShowAttackButton();
    }
}

void UPartyInterface::HideAttackButton(UAdventurer* Adventurer)
{
    check(Adventurer);
    const auto& Child = StatusList->GetChildAt(Adventurer->GetPartyIndex());
    UCharacterStatus* Status = Cast<UCharacterStatus>(Child);
    if (Status)
    {
        Status->HideAttackButton();
    }
}