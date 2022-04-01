// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemSlot.h"
#include "Inventory.generated.h"

class UTextBlock;
class UScrollBox;
class UItem;
class UCharacterData;
class AProjectRogueCharacter;
class UAdventurer;

/**
 * 
 */
UCLASS()
class PROJECTROGUE_API UInventory : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCharacterData* Character;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* GoldText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TitleText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UScrollBox* InventoryBox;

public:
	UFUNCTION(BlueprintImplementableEvent)
	void SlotClicked(int32 Index);

	UFUNCTION(BlueprintImplementableEvent)
	void InventorySlotAdded(FItemSlotInfo ItemSlot, UCharacterData* InCharacter);

	UFUNCTION(BlueprintCallable)
	void RepopulateInventory(UCharacterData* InCharacter);

	UCharacterData* GetCharacter() const { return Character; }
	void SetGoldText(int InGold);

};
