// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemMenu.h"
#include "ProjectRogueCharacter.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "CharacterData.h"
#include "ItemSlot.h"
#include "Item.h"
#include "Equipment.h"

void UItemMenu::Update(const FItemSlotInfo& InItemSlot)
{
    const auto& Inventory = InItemSlot.Character->GetInventory();
    const auto& Item = Inventory[InItemSlot.InventoryIndex];
    //make it visible here because it might be collapsed from a container
    UseText->SetVisibility(ESlateVisibility::Visible);
    switch (Item->GetType())
    {
        case EItemType::Armor:UpdateTextForEquipment(InItemSlot.Character->GetArmorIndex(), InItemSlot.InventoryIndex); break;
        case EItemType::Weapon: UpdateTextForEquipment(InItemSlot.Character->GetWeaponIndex(), InItemSlot.InventoryIndex); break;
        case EItemType::Consumable: UseText->SetText(FText::FromString("Use")); break;
        case EItemType::None: [[fallthrough]];
        case EItemType::Container: UseText->SetVisibility(ESlateVisibility::Collapsed); break;
    }

    if (Player->GetPartySize() == 1)
    {
        GiveButton->SetVisibility(ESlateVisibility::Collapsed);
    }
    else
    {
        GiveButton->SetVisibility(ESlateVisibility::Visible);
    }
}

void UItemMenu::UpdateTextForEquipment(int32 EquipmentIndex, int32 ItemIndex)
{
    //if item in slot is equipped
    if (EquipmentIndex == ItemIndex)
    {
        UseText->SetText(FText::FromString("Unequip"));
    }
    else
    {
        UseText->SetText(FText::FromString("Equip"));
    }
}
