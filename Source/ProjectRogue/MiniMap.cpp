// Fill out your copyright notice in the Description page of Project Settings.

#include "MiniMap.h"
#include "ProjectRogueCharacter.h"



void UMiniMap::Update()
{
    auto playerLocation = Player->GetPosition2D();
    auto lerpX = playerLocation.X / ActualMapSize;
    auto lerpY = playerLocation.Y / ActualMapSize;
    
    auto localX = MiniMapSize * lerpX;
    auto localY = MiniMapSize * lerpY;

    PlayerIcon->SetRenderTranslation(FVector2D{ localX - (MiniMapSize - PlayerIconSize),localY });
    PlayerIcon->SetRenderTransformAngle(Player->GetMiniMapRot());
}

bool UMiniMap::ChangeMap()
{
    bool bOldState = bIsInTown;
    bIsInTown = Player->GetActorLocation().Z > 0 ? true : false;

    if (bIsInTown != bOldState)
    {
        return true;
        UE_LOG(LogTemp,Warning,TEXT("Map Changed!"));
    }
    else
    {
        return false;
    }
}
