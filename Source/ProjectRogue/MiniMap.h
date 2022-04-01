// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "MiniMap.generated.h"

/**
 * 
 */

class AProjectRogueCharacter;

UCLASS()
class PROJECTROGUE_API UMiniMap : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		AProjectRogueCharacter* Player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UImage* Map;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UImage* PlayerIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	float ActualMapSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	float MiniMapSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	float PlayerIconSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly);
	bool bIsInTown;

	float PlayerInitialRotation = 90.f;
public:
	void SetPlayer(AProjectRogueCharacter* InPlayer) { Player = InPlayer; }
	UFUNCTION(BlueprintCallable)
	void Update();
	UFUNCTION(BlueprintCallable)
	bool ChangeMap();
};
