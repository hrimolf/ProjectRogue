// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemManager.generated.h"

class UItem;

UENUM()
enum class ETypeSearchType
{
	TypeOnly,
	SubTypeOnly,
	TypeAndSubType,
};

enum class EItemType : uint8;

/**
 * A manager for all our Items
 */
 UCLASS()
class PROJECTROGUE_API AItemManager : public AActor
{
	GENERATED_BODY()
private:
	UPROPERTY()
	TMap<FString, UItem*> Items;

	static AItemManager* Instance;

public:
	static void Init(AItemManager* InInstance);

	static void ClearItems() { Instance->Items.Empty(); }
	static UItem* GetItem(const FString& Name);
	static UItem* GetRandomItemFromType(EItemType InType, ETypeSearchType SearchType = ETypeSearchType::TypeAndSubType);
	
	//M: Items must be added in C++. See ProjectRogueCharacter BeginPlay
	template<class UType>
	static UType* AddItem(const FString& InName, float InValue = 0.f, const FString& InDescription = FString(), const TArray<int32>& InDiceFaces = {})
	{
		// Create a new object and try to cast to UItem
		UType* Item = NewObject<UType>();
		Item->Init(InName, InValue, InDiceFaces, InDescription);
		AddItem(Item);
		return Item;
	}

private:
	static void AddItem(UItem* Item);
	static bool FindItem(const FString& Name);
};
