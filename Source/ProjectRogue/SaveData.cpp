// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveData.h"
#include "ProjectRogueCharacter.h"
#include "Adventurer.h"
#include "Quiver.h"

void USaveData::SavePartyData(AProjectRogueCharacter* Player)
{
    // M: This is a required step.
    Party.Empty();

    TArray<UAdventurer*> PlayerParty = Player->GetParty();
    FAdventurerData Data;
    UAdventurer* Adventurer;
    for (int32 I = 0; I < PlayerParty.Num(); ++I)
    {
        // M: This is also a required step
        Data.InventoryItems.Empty();

        Adventurer = PlayerParty[I];
        Data.Name = Adventurer->GetCharacterName();
        Data.Race = Adventurer->GetRace();
        Data.Class = Adventurer->GetClass();
        Data.Icon = Adventurer->GetIcon();
        Data.Stats = Adventurer->GetStats();
        Data.PartyIndex = I;
        Data.MaxHealth = Adventurer->GetMaxHealth();
        Data.MaxMana = Adventurer->GetMaxMana();
        Data.Level = Adventurer->GetLevel();
        Data.Experience = Adventurer->GetXP();
        Data.EquippedItems = Adventurer->GetEquippedItems();

        TArray<UItem*> Items = Adventurer->GetInventory();
        for (int32 J = 0; J < Items.Num(); ++J)
        {
            if (Items[J]->GetType() == EItemType::Container)
            {
                UQuiver* Quiver = Cast<UQuiver>(Items[J]);
                if (IsValid(Quiver))
                {
                    FText ItemName = Quiver->GetContainedItem()->GetItemName();
                    for (int32 K = 0; K < Quiver->GetCount(); ++K)
                    {
                        Data.InventoryItems.Add(ItemName);
                    }
                }
            }
            else
            {
                Data.InventoryItems.Add(Items[J]->GetItemName());
            }
        }
        Party.Add(Data);
    }
    Gold = Player->GetGold();
}

void USaveData::LoadPartyData(AProjectRogueCharacter* Player)
{
    FAdventurerData Data;
    for (int32 I = 0; I < Party.Num(); ++I)
    {
        Data = Party[I];
        UAdventurer* Adventurer = Player->AddAdventurer();
        Adventurer->Update(Data.Name, Data.Race, Data.Class, Data.Icon);
        Adventurer->SetStats(Data.Stats);
        Adventurer->SetPartyIndex(Data.PartyIndex);
        Adventurer->IncreaseMaxHealth(Data.MaxHealth);
        Adventurer->IncreaseMaxMana(Data.MaxMana);
        Adventurer->SetLevel(Data.Level);
        Adventurer->AdjustXP(Data.Experience);

        for (int32 J = 0; J < Data.InventoryItems.Num(); ++J)
        {
            Adventurer->AddItem(Data.InventoryItems[J].ToString());
        }

        for (int32 J = 0; J < Data.EquippedItems.Num(); ++J)
        {
            Adventurer->EquipItem(Data.EquippedItems[J]);
        }
    }

    if (Party.Num() > 0)
    {
        UAdventurer* Adventurer = Player->GetAdventurer(0);
        Adventurer->AddGold(Gold);
        Adventurer->TransferGoldToParty();
    }
}