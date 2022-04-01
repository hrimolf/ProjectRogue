// Fill out your copyright notice in the Description page of Project Settings.


#include "AdventurersGuildScreen.h"
#include "Components/TextBlock.h"

void UAdventurersGuildScreen::EnterAdventurersGuild()
{
    HealedText->SetVisibility(ESlateVisibility::Visible);
    PartyDiedText->SetVisibility(ESlateVisibility::Collapsed);
}

void UAdventurersGuildScreen::PartyDied()
{
    PartyDiedText->SetVisibility(ESlateVisibility::Visible);
}