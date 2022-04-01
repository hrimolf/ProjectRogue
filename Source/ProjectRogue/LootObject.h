// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LootObject.generated.h"

class UItem;
class UStaticMeshComponent;

UCLASS()
class PROJECTROGUE_API ALootObject : public AActor
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<UItem*> Items;
	
public:	
	// Sets default values for this actor's properties
	ALootObject();

	void Init(TArray<UItem*> InItems);
	const TArray<UItem*>& GetItems() const { return Items; }
};
