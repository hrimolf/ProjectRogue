// Fill out your copyright notice in the Description page of Project Settings.


#include "Healthbar.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UHealthbar::SetName(FText InName)
{
    NameText->SetText(InName);
}

void UHealthbar::Update(float InCurrentHealth, float InMaxHealth)
{
    FString Health = FString::FromInt(InCurrentHealth) + " / " + FString::FromInt(InMaxHealth);
    HealthText->SetText(FText::FromString(Health));
    Healthbar->SetPercent(InCurrentHealth / InMaxHealth);
}

void UHealthbar::SetVisibility(ESlateVisibility InVisibility)
{
    NameText->SetVisibility(InVisibility);
    HealthText->SetVisibility(InVisibility);
    Healthbar->SetVisibility(InVisibility);
}