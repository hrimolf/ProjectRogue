// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RenameAdventurer.generated.h"

class UTextBlock;
class UAdventurer;
class AProjectRogueCharacter;

/**
 * 
 */
UCLASS()
class PROJECTROGUE_API URenameAdventurer : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AProjectRogueCharacter* Player;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAdventurer* Adventurer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText NewName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* NameLabel;

public:
	void SetPlayer(AProjectRogueCharacter* InPlayer) { Player = InPlayer; }
	void SetAdventurer(UAdventurer* InAdventurer);
	UAdventurer* GetAdventurer() { return Adventurer; }
	FText GetNewName() const { return NewName; }
	
};
