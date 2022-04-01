#include "CharacterData.h"

#include "Engine/World.h"
#include "Item.h"
#include "ItemManager.h"
#include "EffectItem.h"
#include "Quiver.h"
#include "Spell.h"

constexpr int32 kEquipmentSlots = 2;

// M: TODO: Move this to Player (Make a data)
constexpr int32 kMaxArrows = 10;

UCharacterData::UCharacterData()
    : InventorySize(1)
    , Level(0)
    , FreeSpace(InventorySize)
    , AttackDelay(10)
{
	EquippedItems.Init(INDEX_NONE, kEquipmentSlots);
    EquipmentSlot.Add(EItemType::Armor, 0);
    EquipmentSlot.Add(EItemType::Weapon, 1);
}

float UCharacterData::GetRemainingAttackDelay(FTimerManager& TimerManager) const
{
    return TimerManager.GetTimerRemaining(AttackHandle);
}

int32 UCharacterData::GetArmorIndex() const
{
    return EquippedItems[EquipmentSlot.FindRef(EItemType::Armor)];
}

int32 UCharacterData::GetWeaponIndex() const
{
    return EquippedItems[EquipmentSlot.FindRef(EItemType::Weapon)];
}

UItem* UCharacterData::GetArmor() const
{
    int32 Slot = EquippedItems[EquipmentSlot.FindRef(EItemType::Armor)];
    //T: default value is -1, so we have to check for that
    if (Slot < 0)
        return nullptr;
    UItem* Item = InventoryItems[Slot];
    return Item;
}

UItem* UCharacterData::GetWeapon() const
{
    int32 Slot = EquippedItems[EquipmentSlot.FindRef(EItemType::Weapon)];
    //T: default value is -1, so we have to check for that
    if (Slot < 0)
        return nullptr;
    UItem* Item = InventoryItems[Slot];
    return Item;
}

unsigned int UCharacterData::GetGold()
{
    //T: we return the character's gold here, because this is used in the
    //character creation screen and is then added to the party. It will
    //also be used when monsters are killed
    return Gold;
}

void UCharacterData::OnTakeDamage(int32 Amount)
{
    CurrentHealth -= Amount;
    if (CurrentHealth < 0)
        CurrentHealth = 0;
}

void UCharacterData::RestoreMana(int32 Amount)
{
    if (Amount >= MaxMana)
    {
        CurrentMana = MaxMana;
        return;
    }

    CurrentMana += Amount;
    if (CurrentMana > MaxMana)
        CurrentMana = MaxMana;
}

void UCharacterData::AddInventorySlots(int32 Amount)
{
    InventorySize += Amount;
    FreeSpace += Amount;
}

void UCharacterData::OnSpellCast(ASpell* Spell)
{
    check(Spell);
    int32 ManaCost = Spell->GetManaCost();
    int32 NewMana = CurrentMana - ManaCost;
    if (NewMana >= 0)
    {
        CurrentMana = NewMana;
    }
}

void UCharacterData::OnAttack(FTimerManager& TimerManager, FTimerDelegate AttackDelegate)
{
    TimerManager.SetTimer(AttackHandle, AttackDelegate, TimerDelay, false);
}

void UCharacterData::IncreaseMaxHealth(int32 Amount)
{
    MaxHealth += Amount; 
    RestoreHealth(MaxHealth);
}

void UCharacterData::IncreaseMaxMana(int32 Amount)
{
    MaxMana += Amount;
    RestoreMana(MaxMana);
}

void UCharacterData::RestoreHealth(int32 Amount)
{
    if (Amount >= MaxHealth)
    {
        CurrentHealth = MaxHealth;
        return;
    }

    CurrentHealth += Amount;
    if (CurrentHealth > MaxHealth)
        CurrentHealth = MaxHealth;
}

UItem* UCharacterData::AddItem(const FString& ItemName, int32 Count)
{
    UItem* Item = AItemManager::GetItem(ItemName);

    if(!Item || Count < 1)
        return nullptr;

    for (int32 i = 0; i < Count; ++i)
    {
        //M: If the item is an EffectItem, it is consumed 
        //  immediately
        switch (Item->GetType())
        {
            case EItemType::Ammo:
            {
                // M: Find Existing Quiver and add
                bool addedToQuiver = false;
                for (UItem* InventoryItem : InventoryItems)
                {
                    if (InventoryItem->GetType() == EItemType::Container)
                    {
                        // M: Since the only container we have is Quivers,
                        //  do the easy thing
                        UQuiver* Quiver = Cast<UQuiver>(InventoryItem);
                        if (Quiver->Contains(Item) && Quiver->AddOne())
                        {
                            addedToQuiver = true;
                            break;
                        }
                    }
                }

                // M: Or Create a new Quiver if there's room
                if (!addedToQuiver && FreeSpace > 0)
                {
                    UQuiver* Quiver = NewObject<UQuiver>();
                    Quiver->Init(Item, kMaxArrows);
                    --FreeSpace;
                    InventoryItems.Add(Quiver);
                }
            }
            break;
            default:
            {
                if (FreeSpace <= 0 && Item->GetConsumesSpace())
                {
                    GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Yellow, "Unable to Add " + Item->GetItemName().ToString() + ": No free inventory.");
                    continue;
                }
                else
                {
                    if (Item->GetConsumesSpace())
                    {
                        --FreeSpace;
                    }

                    InventoryItems.Add(Item);
                }
            } //T: end of default case
        } //T: end of switch
    } //T: end of loop

    return Item;
}

UItem* UCharacterData::AddItem(UItem* Item)
{
    if (FreeSpace <= 0 && Item->GetConsumesSpace())
    {
        GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Yellow, "Unable to Add " + Item->GetItemName().ToString() + ": No free inventory.");
        return nullptr;
    }
    else
    {
        if (Item->GetConsumesSpace())
        {
            --FreeSpace;
        }

        InventoryItems.Add(Item);
    }
    return Item;
}

void UCharacterData::EquipItem(int32 Index)
{
    if (!InventoryItems.IsValidIndex(Index))
        return;

    UItem* Item = InventoryItems[Index];
    EquipItem(Item, Index);
}

void UCharacterData::UnequipItem(int32 Index)
{
    if (!InventoryItems.IsValidIndex(Index))
        return;

    UItem* Item = InventoryItems[Index];
    UnequipItem(Item, Index);
}

void UCharacterData::EquipItem(UItem* Item, int32 Index)
{
    if (!Item)
        return;

    if(!EquipmentSlot.Find(Item->GetType()))
        return;

    int32 Slot = EquipmentSlot[Item->GetType()];

    if (EquippedItems[Slot] == Index)
    {
        // M: If the slot already contains this item, toggle unequip
        UnequipItem(Item, Index);
        return;
    }
    
    if (!OnEquip(Item))
        return;

    if(EquippedItems[Slot] != INDEX_NONE)
    { 
        UnequipItem(EquippedItems[Slot]);
        if(EquippedItems[Slot] != INDEX_NONE)
            return;
    }
    
    EquippedItems[Slot] = Index;
}

void UCharacterData::UnequipItem(UItem* Item, int32 Index)
{
    if (!Item)
        return;

    if (!EquipmentSlot.Find(Item->GetType()))
        return;

    size_t Slot = EquipmentSlot[Item->GetType()];
    if (EquippedItems[Slot] == Index)
    {
        if (FreeSpace <= 0)
        {
            // Cant unequip if there's no inventory space
            GEngine->AddOnScreenDebugMessage(2, 5, FColor::Yellow, "Can't unequip without inventory space!");
            return;
        }

        EquippedItems[Slot] = INDEX_NONE;

        GEngine->AddOnScreenDebugMessage(2, 2, FColor::White, "Unequipped " + Item->GetItemName().ToString());
    }
}

void UCharacterData::DropItem(int32 Index)
{
    if(!InventoryItems.IsValidIndex(Index))
        return;

    UItem* Item = InventoryItems[Index];

    if(!Item)
        return;

    //Unequip item if we drop it
    if (EquippedItems.Contains(Index))
    {
        int32 EquipmentIndex = EquippedItems.Find(Index);
        EquippedItems[EquipmentIndex] = INDEX_NONE;
    }

    //adjust our equipped Item indeces
    for (auto& EquippedItemIndex : EquippedItems)
    {
        // This will never have the same index
        if(EquippedItemIndex > Index)
            --EquippedItemIndex;
    }

    if (Item->GetConsumesSpace())
        ++FreeSpace;

    InventoryItems.RemoveAt(Index);
}

void UCharacterData::GiveItem(int32 Index, UCharacterData* Recipient)
{
    if (!InventoryItems.IsValidIndex(Index))
        return;

    if(!Recipient || Recipient->GetInventoryFreeSlots() <= 0)
        return;

    UItem* Item = InventoryItems[Index];

    if (!Item)
        return;

    if (Item->GetConsumesSpace())
        ++FreeSpace;

    InventoryItems.RemoveAt(Index);

    Recipient->AddItem(Item);
}
