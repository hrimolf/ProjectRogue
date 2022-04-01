// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PartyInterface.generated.h"

class UButton;
class UVerticalBox;
class UCharacterStatus;
class UAdventurer;
class AProjectRogueCharacter;

/**
 * 
 */
UCLASS()
class PROJECTROGUE_API UPartyInterface : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AProjectRogueCharacter* Player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UBorder* MainBorder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UVerticalBox* StatusList;

public:
	void SetPlayer(AProjectRogueCharacter* InPlayer);

	UFUNCTION(BlueprintCallable)
	void ClearPartyInterface();

	UFUNCTION(BlueprintCallable)
	void DeletePartyMember(const int32 index);

	UFUNCTION(BlueprintImplementableEvent)
	void PartyMemberAdded(UAdventurer* Adventurer);

	UFUNCTION(BlueprintCallable)
	void AddPartyMember(UAdventurer* Adventurer);

	UFUNCTION(BlueprintCallable)
	void UpdatePartySlot(UAdventurer* Adventurer, int32 Index);

	UFUNCTION(BlueprintCallable)
	void HideButtons();

	void ShowAttackButton(UAdventurer* Adventurer);
	void HideAttackButton(UAdventurer* Adventurer);
};
