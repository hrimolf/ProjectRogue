#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "Direction.generated.h"

UENUM(BlueprintType)
enum class EDirection : uint8
{
    North UMETA(DisplayName = "North"),
    East UMETA(DisplayName = "East"),
    South UMETA(DisplayName = "South"),
    West UMETA(DisplayName = "West"),
};