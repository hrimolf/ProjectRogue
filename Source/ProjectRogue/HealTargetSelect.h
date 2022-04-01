// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealTargetSelect.generated.h"

class UVerticalBox;
class UAdventurer;
class AProjectRogueCharacter;
class ASpell;

/**
 * 
 */
UCLASS()
class PROJECTROGUE_API UHealTargetSelect : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UVerticalBox* TargetBox;

public:
	void Populate(AProjectRogueCharacter* Player, UAdventurer* Source, ASpell* Spell);

	UFUNCTION(BlueprintImplementableEvent)
	void AddPartyMember(UAdventurer* Adventurer, AProjectRogueCharacter* Player, ASpell* Spell);
};
