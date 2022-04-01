#pragma once
#include "CoreMinimal.h"
#include "Item.h"
#include "CharacterData.generated.h"

class FTimerManager;
class ASpell;

UENUM(Blueprintable)
enum class ECombatDataType : uint8
{
    HitPoints,
    SpellPoints,
    PrayerPoints,
    Count,
};

/**
 * Contains inventory and combat data
 */
 UCLASS(Blueprintable)
class PROJECTROGUE_API UCharacterData : public UObject
{
	GENERATED_BODY()

protected:
    int InventorySize;

    UPROPERTY()
    FText Name;

    int32 CurrentHealth;

    UPROPERTY()
    int32 MaxHealth;

    int32 CurrentMana;

    UPROPERTY()
    int32 MaxMana;

    int32 Gold;

    UPROPERTY()
    int32 Level;
    UPROPERTY()
    int32 Experience;

    TMap<EItemType, int32> EquipmentSlot;
    UPROPERTY()
    TArray<int32> EquippedItems;
    UPROPERTY()
    TArray<UItem*> InventoryItems;
    int32 FreeSpace;

    float AttackDelay;
    float TimerDelay;
    FTimerHandle AttackHandle;

public:
	UCharacterData();

    unsigned int GetGold();
    void RenameCharacter(FText InName) { Name = InName; }
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FText GetCharacterName() const { return Name; }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetCurrentHealth()   const { return CurrentHealth; }
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetMaxHealth()       const { return MaxHealth; }
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetCurrentMana()     const { return CurrentMana; }
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetMaxMana()         const { return MaxMana; }
    float GetRemainingAttackDelay(FTimerManager& TimerManager) const;
    bool GetIsAlive() const { return CurrentHealth > 0; }

    virtual void OnAttack(FTimerManager& TimerManager, FTimerDelegate AttackDelegate);
    virtual void OnTakeDamage(int32 Amount);
    virtual void OnSpellCast(ASpell* Spell);
    virtual bool OnEquip(const UItem* Item) { return false; } // return false if you dont want to equip the item

    int32 GetArmorIndex() const;
    int32 GetWeaponIndex() const;
    UItem* GetArmor() const;
    UItem* GetWeapon() const;
    TArray<int32> GetEquippedItems() const { return EquippedItems; }

    UFUNCTION(BlueprintCallable)
    UItem* AddItem(const FString& ItemName, int32 Count = 1);
    UItem* AddItem(UItem* Item); // Required for Quivers
    void EquipItem(int32 Index);
    void UnequipItem(int32 Index);
   
    void DropItem(int32 Index);
    void GiveItem(int32 Index, UCharacterData* Recipient);

    const TArray<UItem*>& GetInventory() const { return InventoryItems; }
    int32 GetInventoryFreeSlots() const { return FreeSpace; }
    int32 GetLevel() const { return Level; }
    int32 GetXP() const { return Experience; }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    FTimerHandle& GetAttackHandle() { return AttackHandle; }
 
    void AddGold(const int32 Amount) { Gold += Amount; }
    void AddLevel() { Level += 1; }
    // If Enemies ever have levels, this will be useful
    void SetLevel(int32 InLevel) { Level = InLevel; }
    void AdjustXP(int32 Amount) { Experience += Amount; }
    void IncreaseMaxHealth(int32 Amount);
    void IncreaseMaxMana(int32 Amount);
    void SetAttackDelay(float Delay) { AttackDelay = Delay; }
    
    UFUNCTION(BlueprintCallable)
    void RestoreHealth(int32 Amount);

    UFUNCTION(BlueprintCallable)
    void RestoreMana(int32 Amount);

    UFUNCTION(BlueprintCallable)
    void AddInventorySlots(int32 Amount);

private:
    void EquipItem(UItem* Item, int32 Index);
    void UnequipItem(UItem* Item, int32 Index);
};
