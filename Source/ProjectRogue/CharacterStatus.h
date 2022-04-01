// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectRogueCharacter.h"
#include "CharacterStatus.generated.h"

class UTextBlock;
class UButton;
class UBorder;
class UHorizontalBox;
class UAdventurer;

/**
 * 
 */
UCLASS()
class PROJECTROGUE_API UCharacterStatus : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn=true))
	UAdventurer* Adventurer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UBorder* MainBorder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UHorizontalBox* Container;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* NameText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* HealthText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ManaText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* LevelText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ExpText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* InfoButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* InventoryButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* AttackButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* CastButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* ContextButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ContextSensitiveText;

public:
	UCharacterStatus(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void UpdateStatus(UAdventurer* InAdventurer, EContext Context);

	UFUNCTION(BlueprintCallable)
	void CollapseButtons();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetAttackCooldown();

	void ShowAttackButton();
	void HideAttackButton();

	UAdventurer* GetAdventurer() const { return Adventurer; }
};
