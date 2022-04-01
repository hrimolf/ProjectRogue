// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpecialRequestPopup.generated.h"

class AProjectRogueCharacter;
class UEditableTextBox;
/**
 * 
 */
UCLASS()
class PROJECTROGUE_API USpecialRequestPopup : public UUserWidget
{
	GENERATED_BODY()

private:
	AProjectRogueCharacter* Player;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* OrderTextBox;
public:

	void SetPlayer(AProjectRogueCharacter* InPlayer) { Player = InPlayer; }

	UFUNCTION(BlueprintCallable)
	bool OrderComplete(FString Order);

	void SetTextBoxFocus();
};
