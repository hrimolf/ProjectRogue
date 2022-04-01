// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "RoomType.generated.h"

UENUM(BlueprintType)
enum class ERoomType : uint8
{
    Normal UMETA(DisplayName = "Normal"),
    Shop UMETA(DisplayName = "Shop"),
    Tavern UMETA(DisplayName = "Tavern"),
    ReviewRoom UMETA(DisplayName = "Review Room"),
    Guild UMETA(DisplayName = "Adventurer's Guild"),
    Undefined UMETA(DisplayName = "Undefined"),
};
