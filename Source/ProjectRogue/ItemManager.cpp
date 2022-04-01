// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemManager.h"
#include "Item.h"
#include "ProjectRogueCharacter.h"
#include "Templates/UnrealTemplate.h"

AItemManager* AItemManager::Instance = nullptr;

void AItemManager::Init(AItemManager* InInstance)
{
    check(InInstance);
    Instance = InInstance;
}

UItem* AItemManager::GetItem(const FString& Name)
{
    if (FindItem(Name))
    {
        return Instance->Items[Name];
    }
    return nullptr;
}

UItem* AItemManager::GetRandomItemFromType(EItemType InType, ETypeSearchType SearchType)
{
    if(InType == EItemType::None)
        return nullptr;

    TArray<UItem*> SearchPool;
    for (const TPair<FString, UItem*> Pair : Instance->Items)
    {
        UItem* Item = Pair.Value;
        EItemType Type = Item->GetType();
        EItemType SubType = Item->GetSubType();

        if(Type == EItemType::None)
            continue;

        if (SearchType == ETypeSearchType::TypeOnly || SearchType == ETypeSearchType::TypeAndSubType)
        {
            if (Type == InType)
            {
                SearchPool.Add(Item);
                continue;
            }
        }

        if (SubType == EItemType::None)
            continue;

        if (SearchType == ETypeSearchType::SubTypeOnly || SearchType == ETypeSearchType::TypeAndSubType)
        {
            if (SubType == InType)
            {
                SearchPool.Add(Item);
                continue;
            }
        }
    }

    int32 Size = SearchPool.Num() - 1;
    if (Size == 0)
        return nullptr;
    UItem* RandomItem = SearchPool[ FMath::RandRange(0, Size) ];
    return RandomItem;
}

void AItemManager::AddItem(UItem* Item)
{
    //T: TODO this needs to be polished
    Instance->Items.Add(Item->GetItemName().ToString(), MoveTemp(Item));
}

bool AItemManager::FindItem(const FString& Name)
{
    if(!Instance->Items.Contains(Name))
        return false;

    UItem** Item = Instance->Items.Find(Name);

    if (!Item)
        return false;

    return true;
}
