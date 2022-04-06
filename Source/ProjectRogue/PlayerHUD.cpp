// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "Components/Border.h"

#include "PartyInterface.h"
#include "ProjectRogueCharacter.h"
#include "CharacterCreationScreen.h"
#include "AdventurersGuildScreen.h"
#include "EditPartyScreen.h"
#include "RenameAdventurer.h"
#include "CharacterInfoScreen.h"
#include "Inventory.h"
#include "ItemManager.h"
#include "SpellSelect.h"
#include "HealTargetSelect.h"
#include "MiniMap.h"
#include "ItemSlot.h"
#include "ItemMenu.h"
#include "SpecialRequestPopup.h"

void UPlayerHUD::Init(AProjectRogueCharacter* InPlayer)
{
    Player = InPlayer;
    PartyInterface->SetPlayer(InPlayer);
    RenameAdventurer->SetPlayer(InPlayer);
    AdventurersGuildScreen->SetPlayer(InPlayer);
    CreateAdventurerScreen->SetPlayer(InPlayer);
    MiniMap->SetPlayer(InPlayer);
    ItemMenu->SetPlayer(InPlayer);
    SpecialRequest->SetPlayer(InPlayer);
}

void UPlayerHUD::ShowAdventurersGuild()
{
    AdventurersGuildScreen->SetVisibility(ESlateVisibility::Visible);
    AdventurersGuildScreen->EnterAdventurersGuild();
    CharacterInfo->SetVisibility(ESlateVisibility::Hidden);
    InventoryScreen->SetVisibility(ESlateVisibility::Hidden);
    SpellSelect->SetVisibility(ESlateVisibility::Hidden);
    TargetSelect->SetVisibility(ESlateVisibility::Hidden);
    ItemMenu->SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerHUD::EnterTown()
{
    AdventurersGuildScreen->SetVisibility(ESlateVisibility::Hidden);
    SetInputMode(EInputMode::GameAndUI);
}

void UPlayerHUD::PopulatePartyInterface(TArray<UAdventurer*> InParty)
{
    PartyInterface->ClearPartyInterface();
    for (const auto& Adventurer : InParty)
    {
        PartyInterface->AddPartyMember(Adventurer);
    }
    //T:update the party after adding the members to the interface
    //to avoid read access violation
    for (const auto& Adventurer : InParty)
    {
        UpdatePartyMember(Adventurer);
    }
}

void UPlayerHUD::UpdatePartyMember(UAdventurer* Adventurer)
{
    PartyInterface->UpdatePartySlot(Adventurer, Adventurer->GetPartyIndex());
}

void UPlayerHUD::ShowCreateAdventurer()
{
    if (Player->GetPartySize() == 4)
        return;
    
    UAdventurer* Adventurer = Player->AddAdventurer();
    Adventurer->RandomizeStats();
    CreateAdventurerScreen->Open(Adventurer, Player->GetPartySize() - 1);
}

void UPlayerHUD::ShowEditParty(EPartyEditType EditType)
{
    BackgroundBorder->SetVisibility(ESlateVisibility::Visible);
    EditPartyScreen->SetVisibility(ESlateVisibility::Visible);
    EditPartyScreen->PopulatePartyInterface(Player, EditType);
}

void UPlayerHUD::HideEditParty()
{
    BackgroundBorder->SetVisibility(ESlateVisibility::Hidden);
    EditPartyScreen->SetVisibility(ESlateVisibility::Hidden);
    InventoryScreen->SetVisibility(ESlateVisibility::Hidden);
    CharacterInfo->SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerHUD::RenameClicked(int32 Index)
{
    const auto& Party = Player->GetParty();
    UAdventurer* Adventurer = Party[Index];
    if (RenameAdventurer->GetVisibility() == ESlateVisibility::Visible)
    {
        UAdventurer* StoredAdventurer = RenameAdventurer->GetAdventurer();
        if (Adventurer->GetPartyIndex() != StoredAdventurer->GetPartyIndex())
        {
            RenameAdventurer->SetAdventurer(Party[Index]);
        }
    }
    else
    {
        RenameAdventurer->SetVisibility(ESlateVisibility::Visible);
        RenameAdventurer->SetAdventurer(Party[Index]);
    }
}

void UPlayerHUD::RenameConfirmed(UAdventurer* Adventurer)
{
    RenameAdventurer->SetVisibility(ESlateVisibility::Hidden);
    EditPartyScreen->Update();
}

void UPlayerHUD::InfoButtonClicked(UAdventurer* Adventurer)
{
    OpenInfo(Adventurer, false);
}

void UPlayerHUD::HideItemMenu()
{
    ItemMenu->SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerHUD::CastButtonClicked(UAdventurer* Adventurer)
{
    if (SpellSelect->GetVisibility() == ESlateVisibility::Visible)
    {
        //check if its a different adventurer
        if (Adventurer != SpellSelect->GetAdventurer())
        {
            SpellSelect->Update(Adventurer);
        }
        else
        {
            //otherwise hide it
            SpellSelect->SetVisibility(ESlateVisibility::Hidden);
        }
    }
    else
    {
        SpellSelect->SetVisibility(ESlateVisibility::Visible);
        SpellSelect->Update(Adventurer);
    }
    SetInputMode(EInputMode::GameAndUI);
}

void UPlayerHUD::SetInputMode(EInputMode InputMode)
{
    APlayerController* Controller = Cast<APlayerController>(Player->GetController());
    if (Controller)
    {
        switch (InputMode)
        {
            case EInputMode::UIOnly: 
            {
                //GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, TEXT("InputMode: UI Only"));
                Controller->SetInputMode(FInputModeUIOnly());
            }
            break;
            case EInputMode::GameAndUI: 
            {
                //GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, TEXT("InputMode: Game and UI"));
                //put the player back in the game so the UI is not focused
                FInputModeGameOnly GameOnly;
                GameOnly.SetConsumeCaptureMouseDown(false);
                Controller->SetInputMode(GameOnly);
                //then reset so they can move and use the UI
                FInputModeGameAndUI GameAndUI;
                GameAndUI.SetHideCursorDuringCapture(false);
                Controller->SetInputMode(GameAndUI);
            }
            break;
        }
    }
}

void UPlayerHUD::ShowBlood()
{
    PlayAnimation(BloodFade);
}

void UPlayerHUD::ContextButtonClicked(UAdventurer* Adventurer)
{
    switch (Player->GetContext())
    {
        case EContext::Shop:
        {
            //update inventory after purchase
            InventoryScreen->RepopulateInventory(Adventurer);
            UpdatePartyMember(Adventurer);
            InventoryScreen->SetGoldText(Player->GetGold());
        }
        break;
        case EContext::Review:
        {
            //open character info and enable buttons
            OpenInfo(Adventurer, true);
        }
        break;
        case EContext::Tavern:
        {
            //update inventory after purchase
            InventoryScreen->RepopulateInventory(Adventurer);
            UpdatePartyMember(Adventurer);
            InventoryScreen->SetGoldText(Player->GetGold());
        }
        break;
    }
}

void UPlayerHUD::InventoryButtonClicked(UAdventurer* Adventurer)
{
    if (InventoryScreen->GetVisibility() == ESlateVisibility::Visible)
    {
        if (Adventurer == InventoryScreen->GetCharacter())
        {
            InventoryScreen->SetVisibility(ESlateVisibility::Hidden);
            ItemMenu->SetVisibility(ESlateVisibility::Hidden);
            TargetSelect->SetVisibility(ESlateVisibility::Hidden);
            if (MerchantScreen->GetVisibility() == ESlateVisibility::Hidden)
            {
                SetInputMode(EInputMode::GameAndUI);
            }
        }
        else
        {
            InventoryScreen->RepopulateInventory(Adventurer);
        }
    }
    else
    {
        InventoryScreen->SetVisibility(ESlateVisibility::Visible);
        InventoryScreen->RepopulateInventory(Adventurer);
        InventoryScreen->SetGoldText(Player->GetGold());
        SetInputMode(EInputMode::GameAndUI);
    }
}

void UPlayerHUD::RefreshInventory(UCharacterData* Character)
{
    if (InventoryScreen->GetCharacter() == Character)
    {
        InventoryScreen->RepopulateInventory(Character);
    }
}

void UPlayerHUD::OpenMerchant(UCharacterData* Merchant)
{
    MerchantScreen->SetVisibility(ESlateVisibility::Visible);
    MerchantScreen->RepopulateInventory(Merchant);

    // M: We want to hide the ItemMenu when we enter any shop
    ItemMenu->SetVisibility(ESlateVisibility::Hidden);

    // M: Hide the inventory so the player can choose which one to open
    InventoryScreen->SetVisibility(ESlateVisibility::Hidden);

    SetInputMode(EInputMode::GameAndUI);
}

void UPlayerHUD::CloseMerchant()
{
    MerchantScreen->SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerHUD::OpenInfo(UAdventurer* Adventurer, bool bIsReviewboard)
{
    if (CharacterInfo->GetVisibility() == ESlateVisibility::Visible)
    {
        if (Adventurer == CharacterInfo->GetAdventurer())
        {
            CharacterInfo->SetVisibility(ESlateVisibility::Hidden);
            SetInputMode(EInputMode::GameAndUI);
        }
        else
        {
            CharacterInfo->Update(Adventurer, Player);
        }
    }
    else
    {
        CharacterInfo->SetVisibility(ESlateVisibility::Visible);
        CharacterInfo->Update(Adventurer, Player);
        SetInputMode(EInputMode::GameAndUI);
    }

    //T: info is looking for ExpText visibility, so set that first if
    //we're in the review board
    if (bIsReviewboard)
    {
        CharacterInfo->OpenReviewBoard(Adventurer);
    }
}

void UPlayerHUD::HealCasted()
{
    SpellSelect->SetVisibility(ESlateVisibility::Hidden);
    HideTargetMenu();
}

void UPlayerHUD::HideTargetMenu()
{
    TargetSelect->SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerHUD::ShowTargetMenu(UAdventurer* Adventurer, ASpell* Spell)
{
    TargetSelect->SetVisibility(ESlateVisibility::Visible);
    TargetSelect->Populate(Player, Adventurer, Spell);
}

bool UPlayerHUD::IsHealMenuVisible() const
{
    return TargetSelect->GetVisibility() == ESlateVisibility::Visible;
}

void UPlayerHUD::ItemSlotClicked(const FItemSlotInfo& InItemSlot, FItemSlotInfo* OldSlot)
{
    if (InItemSlot.SlotType == ESlotType::Inventory)
    {
        EContext Context = Player->GetContext();
        if(Context == EContext::Shop || Context == EContext::Tavern)
            return;

        InventoryScreen->SlotClicked(InItemSlot.InventoryIndex);
        if (OldSlot->Character && (InItemSlot.InventoryIndex == OldSlot->InventoryIndex))
        {
            if (ItemMenu->GetVisibility() == ESlateVisibility::Visible)
            {
                ItemMenu->SetVisibility(ESlateVisibility::Hidden);
            }
            else
            {
                ItemMenu->SetVisibility(ESlateVisibility::Visible);
            }
        }
        else
        {
            ItemMenu->SetVisibility(ESlateVisibility::Visible);
            ItemMenu->Update(InItemSlot);
        }
    }
    else
    {
        MerchantScreen->SlotClicked(InItemSlot.InventoryIndex);
    }
}

void UPlayerHUD::ItemDropped(const FItemSlotInfo& ItemSlot)
{
    ItemMenu->SetVisibility(ESlateVisibility::Hidden);
    InventoryScreen->RepopulateInventory(ItemSlot.Character);
}

void UPlayerHUD::ItemUsed(const FItemSlotInfo& ItemSlot)
{
    ItemMenu->SetVisibility(ESlateVisibility::Hidden);
    InventoryScreen->RepopulateInventory(ItemSlot.Character);
}

void UPlayerHUD::EnterSpecialRequest()
{
    SpecialRequest->SetVisibility(ESlateVisibility::Visible);
    SpecialRequest->SetTextBoxFocus();
}

void UPlayerHUD::ConfirmSpecialRequest()
{
    SpecialRequest->SetVisibility(ESlateVisibility::Hidden);
    InventoryScreen->SetVisibility(ESlateVisibility::Hidden);
    MerchantScreen->SetVisibility(ESlateVisibility::Hidden);
    CharacterInfo->SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerHUD::PartyDied()
{
    AdventurersGuildScreen->SetVisibility(ESlateVisibility::Visible);
    AdventurersGuildScreen->EnterAdventurersGuild();
    AdventurersGuildScreen->PartyDied();
}

void UPlayerHUD::ShowAttackButton(UAdventurer* Adventurer)
{
    PartyInterface->ShowAttackButton(Adventurer);
}

void UPlayerHUD::HideAttackButton(UAdventurer* Adventurer)
{
    PartyInterface->HideAttackButton(Adventurer);
}