// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "CharacterCreationScreen.generated.h"

class UButton;
class UBorder;
class UTextBlock;
class UEditableText;
class UVerticalBox;
class UHorizontalBox;
class USizeBox;
class UComboBoxString;
class UPlayerHUD;
class UAdventurer;
class UCharacterInfoScreen;
class AProjectRogueCharacter;
enum class ERace : uint8;
enum class EClass : uint8;

/**
 * 
 */
UCLASS()
class PROJECTROGUE_API UCharacterCreationScreen : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AProjectRogueCharacter* Player;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAdventurer* Adventurer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 PartyIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERace Race;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EClass Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Gold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> Stats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UBorder* MainBorder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UComboBoxString* RaceSelector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UComboBoxString* ClassSelector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UComboBoxString* ModelSelector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UCharacterInfoScreen* CharacterInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* GoldText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UEditableText* NameTextField;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* ConfirmButton;

public:
	void SetPlayer(AProjectRogueCharacter* InPlayer) { Player = InPlayer; }

	void Open(UAdventurer* InAdventurer, int32 InPartyIndex);

	UFUNCTION(BlueprintCallable)
	void Update();

	UFUNCTION(BlueprintCallable)
	void ConfirmAdventurer();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsNameAvailable() const;
};
