// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Healthbar.generated.h"

class UVerticalBox;
class UTextBlock;
class UOverlay;
class UProgressBar;

/**
 * 
 */
UCLASS()
class PROJECTROGUE_API UHealthbar : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* NameText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* HealthText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* Healthbar;

public:
	void SetName(FText InName);
	void Update(float InCurrentHealth, float InMaxHealth);

	void SetVisibility(ESlateVisibility InVisibility);
};
