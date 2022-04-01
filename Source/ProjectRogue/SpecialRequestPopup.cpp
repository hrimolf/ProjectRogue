// Fill out your copyright notice in the Description page of Project Settings.


#include "SpecialRequestPopup.h"
#include "ProjectRogueCharacter.h"
#include "ProjectRogueCharacter.h"
//#include "Components/Widget.h"
#include "Components/EditableTextBox.h"

bool USpecialRequestPopup::OrderComplete(FString Order)
{
    if (Order == "Devil Whiskey")
    {
        Player->EnterDungeon();
        return true;
    }

    return false;
}

void USpecialRequestPopup::SetTextBoxFocus()
{
    OrderTextBox->SetKeyboardFocus();
}
