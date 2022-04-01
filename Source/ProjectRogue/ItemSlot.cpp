// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSlot.h"
#include "Components/TextBlock.h"
#include "Adventurer.h"
#include "Quiver.h"
#include "ProjectRogueCharacter.h"
#include "Kismet/GameplayStatics.h"

void UItemSlot::Update(FItemSlotInfo InItemSlot)
{
    //FText Value = FText::FromString(FString::FromInt());
    const auto& Inventory = InItemSlot.Character->GetInventory();
    check(Inventory.Num() > 0);
    UItem* Item = Inventory[InItemSlot.InventoryIndex];
    check(Item);
    if (Item->GetType() == EItemType::Container)
    {
        UQuiver* Quiver = Cast<UQuiver>(Item);
        check(Quiver);
        //get item contained by this
        UItem* ContainerItem = Quiver->GetContainedItem();
        check(ContainerItem);
        FString QuiverString;
        int32 Count = Quiver->GetCount();
        QuiverString.Append(FString::FromInt(Count));

        // M: This was extracted from the below append because it was crashing.
        //  We shall watch these values closely.
        int32 MaxCount = Quiver->GetMaxCount();
        FString MaxCountString = FString::FromInt(MaxCount);
        QuiverString.Append(" / " + MaxCountString);

        FString ItemNameString = ContainerItem->GetItemName().ToString();
        //QuiverString.Append(" " + ItemNameString + "s");
        
        float Value = ContainerItem->GetValue() * Count;
        FString ValueString = FString::SanitizeFloat(Value);

        NameText->SetText(FText::FromString(ItemNameString + "s"));
        ValueText->SetText(FText::FromString(ValueString));
        DescriptionText->SetText(FText::FromString(QuiverString));
    }
    else
    {
        NameText->SetText(InItemSlot.Name);
        ValueText->SetText(InItemSlot.Value);
        DescriptionText->SetText(InItemSlot.Description);
    }
}

bool UItemSlot::BuyOrUseItem(UAdventurer* InAdventurer, FItemSlotInfo InItemSlot)
{
    switch (InItemSlot.SlotType)
    {
        case ESlotType::Inventory: return InAdventurer->UseItem(InItemSlot.InventoryIndex);
        case ESlotType::Merchant:
        case ESlotType::Tavern: InAdventurer->AddItem(InItemSlot.Name.ToString()); break;
    }

    return true;
}

void UItemSlot::ItemSlotClicked(FItemSlotInfo InItemSlot)
{
    ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    check(Character);
    AProjectRogueCharacter* Player = Cast<AProjectRogueCharacter>(Character);
    check(Player);
    Player->ItemSlotClicked(InItemSlot);
}