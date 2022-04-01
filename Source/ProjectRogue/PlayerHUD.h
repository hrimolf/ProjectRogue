// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectRogueCharacter.h"
#include "PlayerHUD.generated.h"

class UBorder;
class UCanvasPanel;
class UPartyInterface;
class UCharacterCreationScreen;
class UAdventurersGuildScreen;
class UEditPartyScreen;
class URenameAdventurer;
class UCharacterInfoScreen;
class USpellSelect;
class UInventory;
class UAdventurer;
class UHealTargetSelect;
class UMiniMap;
class UItemMenu;
class USpecialRequestPopup;

enum class EInputMode : uint8
{
	UIOnly,
	GameAndUI
};

/**
 * 
 */
UCLASS()
class PROJECTROGUE_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()

private:
	AProjectRogueCharacter* Player;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UCanvasPanel* Canvas;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UBorder* BackgroundBorder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UPartyInterface* PartyInterface;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UAdventurersGuildScreen* AdventurersGuildScreen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UCharacterCreationScreen* CreateAdventurerScreen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UEditPartyScreen* EditPartyScreen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class URenameAdventurer* RenameAdventurer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UCharacterInfoScreen* CharacterInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class USpellSelect* SpellSelect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UInventory* InventoryScreen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UInventory* MerchantScreen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UHealTargetSelect* TargetSelect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UMiniMap* MiniMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UItemMenu* ItemMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class USpecialRequestPopup* SpecialRequest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage* BloodSpatter;

	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
	class UWidgetAnimation* BloodFade;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	//class UEditableText* TextBox;

	FTimerHandle BloodTimer;

public:
	UFUNCTION(BlueprintCallable)
	void Init(AProjectRogueCharacter* InPlayer);

	void ShowAdventurersGuild();
	void EnterTown();

	UFUNCTION(BlueprintCallable)
	void PopulatePartyInterface(TArray<UAdventurer*> InParty);

	UFUNCTION(BlueprintCallable)
	void UpdatePartyMember(UAdventurer* Adventurer);

	UFUNCTION(BlueprintCallable)
	void ShowCreateAdventurer();

	UFUNCTION(BlueprintCallable)
	void ShowEditParty(EPartyEditType EditType);

	UFUNCTION(BlueprintCallable)
	void HideEditParty();

	UFUNCTION(BlueprintCallable)
	void RenameClicked(int32 Index);

	UFUNCTION(BlueprintCallable)
	void InfoButtonClicked(UAdventurer* Adventurer);

	UFUNCTION(BlueprintCallable)
	void InventoryButtonClicked(UAdventurer* Adventurer);
	void RefreshInventory(UCharacterData* Character);

	UFUNCTION(BlueprintCallable)
	void CastButtonClicked(UAdventurer* Adventurer);

	UFUNCTION(BlueprintCallable)
	void ContextButtonClicked(UAdventurer* Adventurer);

	void RenameConfirmed(UAdventurer* Adventurer);

	void ShowTargetMenu(UAdventurer* Adventurer, ASpell* Spell);
	void HealCasted();
	void HideTargetMenu();
	void HideItemMenu();
	bool IsHealMenuVisible() const;
	
	void ItemSlotClicked(const FItemSlotInfo& InItemSlot, FItemSlotInfo* OldSlot);
	void ItemDropped(const FItemSlotInfo& ItemSlot);
	void ItemUsed(const FItemSlotInfo& ItemSlot);
	void OpenMerchant(UCharacterData* Merchant);
	void CloseMerchant();

	void EnterSpecialRequest();
	void ConfirmSpecialRequest();
	void PartyDied();

	void ShowAttackButton(UAdventurer* Adventurer);
	void HideAttackButton(UAdventurer* Adventurer);
	void SetInputMode(EInputMode InputMode);

	void ShowBlood();

private:
	void OpenInfo(UAdventurer* Adventurer, bool bIsReviewboard);
};
