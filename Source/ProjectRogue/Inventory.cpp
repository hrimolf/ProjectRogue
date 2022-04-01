// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "ProjectRogueCharacter.h"
#include "Item.h"

void UInventory::RepopulateInventory(UCharacterData* InCharacter)
{
    Character = InCharacter;
    FString Title = "Inventory | " + InCharacter->GetCharacterName().ToString();
    TitleText->SetText(FText::FromString(Title));
    InventoryBox->ClearChildren();
    const auto& Inventory = InCharacter->GetInventory();
    FItemSlotInfo ItemSlot;
    int32 InventoryIndex = 0;
    for (const auto& Item : Inventory)
    {
        FText Name = Item->GetItemName();
        
        // M: Hack
        if(Item->GetType() == EItemType::Container)
            Name = FText::FromString("Quiver");

        FString EquippedText;
        if (InCharacter->GetWeaponIndex() == InventoryIndex || InCharacter->GetArmorIndex() == InventoryIndex)
        {
            EquippedText = " (Equipped)";
        }
        if (EquippedText.IsEmpty())
        {
            ItemSlot.Name = FText::FromString(Name.ToString());
        }
        else
        {
            ItemSlot.Name = FText::FromString(Name.ToString() + EquippedText);
        }
        ItemSlot.Character = InCharacter;
        ItemSlot.Value = FText::FromString(FString::FromInt(Item->GetValue()));
        ItemSlot.Description = Item->GetDescription();
        ItemSlot.InventoryIndex = InventoryIndex;
        ++InventoryIndex;

        //check to see if this is a tavern or merchant
        FText Tavern = FText::FromString("Tavern");
        FText Merchant = FText::FromString("Merchant");

        if (InCharacter->GetCharacterName().EqualTo(Merchant))
        {
            ItemSlot.SlotType = ESlotType::Merchant;
            GoldText->SetVisibility(ESlateVisibility::Hidden);
            TitleText->SetText(Merchant);
        }
        else if (InCharacter->GetCharacterName().EqualTo(Tavern))
        {
            ItemSlot.SlotType = ESlotType::Tavern;
            GoldText->SetVisibility(ESlateVisibility::Hidden);
            TitleText->SetText(Tavern);
        }
        else
        {
            GoldText->SetVisibility(ESlateVisibility::Visible);
            ItemSlot.SlotType = ESlotType::Inventory;
            FText InventoryTitle = FText::FromString("Inventory | " + InCharacter->GetCharacterName().ToString());
            TitleText->SetText(InventoryTitle);
        }

        InventorySlotAdded(ItemSlot, InCharacter);
    }
}

void UInventory::SetGoldText(int InGold)
{
    FString String = "Gold: ";
    String.Append(FString::FromInt(InGold));
    GoldText->SetText(FText::FromString(String));
}