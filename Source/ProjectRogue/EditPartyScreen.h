// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Adventurer.h"
#include "PartyController.h"
#include "EditPartyScreen.generated.h"

class UVerticalBox;
class AProjectRogueCharacter;

/**
 * 
 */
UCLASS()
class PROJECTROGUE_API UEditPartyScreen : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AProjectRogueCharacter* Player;

	//this is the party we operate on so that we can throw out any
	//changes if the player decides to cancel
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//TArray<UAdventurer*> Party;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EPartyEditType EditType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UVerticalBox* EditSlotBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UPartyInterface* PartyInterface;

public:
	UFUNCTION(BlueprintCallable)
	void DeletePartyMember(int32 Index);

	UFUNCTION(BlueprintCallable)
	void MoveUp(int32 Index);

	UFUNCTION(BlueprintCallable)
	void MoveDown(int32 Index);

	UFUNCTION(BlueprintCallable)
	void RenameAdventurer(int32 Index);

	UFUNCTION(BlueprintCallable)
	void PopulatePartyInterface(AProjectRogueCharacter* InPlayer, EPartyEditType InEditType);

	UFUNCTION(BlueprintCallable)
	void Update();

	UFUNCTION(BlueprintImplementableEvent)
	void PopulateEditBox(int32 PartySize);

protected:
	//maybe pass the player instead so i dont have to keep 
	//calling functions 1 level up until i get there
	UFUNCTION(BlueprintCallable)
	void ConfirmParty();
};
