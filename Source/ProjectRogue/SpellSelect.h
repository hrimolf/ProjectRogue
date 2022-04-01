// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Adventurer.h"
#include "SpellSelect.generated.h"

class UVerticalBox;
class ASpell;

/**
 * 
 */
UCLASS()
class PROJECTROGUE_API USpellSelect : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UAdventurer* Adventurer;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UVerticalBox* SpellBox;

public:
	UFUNCTION(BlueprintCallable)
	void Update(UAdventurer* InAdventurer);

	UFUNCTION(BlueprintImplementableEvent)
	void SpellAdded(ASpell* Spell, const int32 Index, bool bCanUse);

	UAdventurer* GetAdventurer() const { return Adventurer; }
};
