// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealTarget.generated.h"

class UTextBlock;
class UAdventurer;
class AProjectRogueCharacter;
class ASpell;

/**
 * 
 */
UCLASS()
class PROJECTROGUE_API UHealTarget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn=true))
	UAdventurer* Adventurer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	AProjectRogueCharacter* Player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn=true))
	ASpell* Spell;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* NameText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* HealthText;

public:
	UFUNCTION(BlueprintCallable)
	void Update(UAdventurer* InAdventurer);

	UFUNCTION(BlueprintCallable)
	void Selected();

	UAdventurer* GetAdventurer() const { return Adventurer; }
};
