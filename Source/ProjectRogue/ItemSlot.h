// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemSlot.generated.h"

class UTextBlock;
class UCharacterData;

UENUM(Blueprintable)
enum class ESlotType : uint8
{
	Inventory,
	Merchant,
	Tavern
};

USTRUCT(Blueprintable)
struct FItemSlotInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCharacterData* Character;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FText Name;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FText Value;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FText Description;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 InventoryIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ESlotType SlotType;
};

/**
 * 
 */
UCLASS()
class PROJECTROGUE_API UItemSlot : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* NameText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ValueText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* DescriptionText;

public:
	UFUNCTION(BlueprintCallable)
	void Update(FItemSlotInfo InItemSlot);

	UFUNCTION(BlueprintCallable)
	bool BuyOrUseItem(UAdventurer* InAdventurer, FItemSlotInfo InItemSlot);

	UFUNCTION(BlueprintCallable)
	void ItemSlotClicked(FItemSlotInfo InItemSlot);
};
