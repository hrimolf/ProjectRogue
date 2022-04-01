// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ProjectRogueHUD.generated.h"

UCLASS()
class AProjectRogueHUD : public AHUD
{
	GENERATED_BODY()

public:
	AProjectRogueHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

