// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "ItemSlot.h"

#include "ItemMenu.generated.h"

class UItem;
class UButton;
class UTextBlock;
class AProjectRogueCharacter;

/**
 * 
 */
UCLASS()
class PROJECTROGUE_API UItemMenu : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AProjectRogueCharacter* Player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* UseText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* GiveButton;


public:
	void SetPlayer(AProjectRogueCharacter* InPlayer) { Player = InPlayer; }

	void Update(const FItemSlotInfo& InItemSlot);

private:
	void UpdateTextForEquipment(int32 EquipmentIndex, int32 ItemIndex);
};
