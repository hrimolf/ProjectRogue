#pragma once

#include "CoreMinimal.h"
#include "DiceBag.h"
#include "Item.generated.h"

class AItemManager;
class UAdventurer;

UENUM(Blueprintable)
enum class EItemType : uint8
{
    None,
    Armor,
    Weapon,
    Ammo,
    Consumable, //Potions, etc. Things useable from inventory
    Effect,     //Things applied immediately after acquiring
    Container,  //Quivers
    Potion,     //Potions (Subtype)
    Special,    //Mithril (Subtype)
};

/**
 * An object that fits into the inventory
 */
 UCLASS()
class PROJECTROGUE_API UItem : public UObject
{
    GENERATED_BODY()

    friend AItemManager;

private:
    FText Name;
    float Value;    // cost of 1 item
    bool bConsumesSpace;
    FText Description;
    EItemType Type;
    EItemType SubType;

    UPROPERTY()
    UDiceBag* DiceBag;

public:
	UItem();
    ~UItem();

    virtual void OnUse(UAdventurer* Adventurer, int32 InventoryIndex) {}

    void SetConsumesSpace(bool bInConsumesSpace) { bConsumesSpace = bInConsumesSpace; }
    bool GetConsumesSpace() const { return bConsumesSpace; }

    const FText& GetDescription() const { return Description; }
    const FText& GetItemName() const { return Name; }
    float GetValue() const { return Value; }
    //T: this cant be 'const UDiceBag*' because Roll cant be called
    UDiceBag* GetDiceBag() const { return DiceBag; }
    EItemType GetType() const { return Type; }
    EItemType GetSubType() const { return SubType; }

    //M: Don't call this unless you absolutely have to override
    //  the SubType of an item.
    void SetSubType(const EItemType InType) { SubType = InType; }

protected:
    void Init(const FString& InName, float InValue, const TArray<int32> InDiceFaces, const FString& InDescription = FString());
    void SetType(const EItemType InType) { Type = InType; }
};
