
#pragma once

#include "CoreMinimal.h"
#include "CharacterData.h"
#include "Adventurer.generated.h"

class UPartyController;
class ASpell;
struct FCombatDice;

/**
 * Data for our adventurer characters
 */

UENUM()
enum class ERace : uint8
{
	Human	UMETA(DisplayName = "Human"),
	Dwarf	UMETA(DisplayName = "Dwarf"),
	Elf		UMETA(DisplayName = "Elf"),
	Orc		UMETA(DisplayName = "Orc"),
};

UENUM()
enum class EClass : uint8
{
	Warrior		UMETA(DisplayName = "Warrior"),
	Magician	UMETA(DisplayName = "Magician"),
	Cleric		UMETA(DisplayName = "Cleric"),
};

UENUM()
enum class EStat : uint8
{
	Strength		UMETA(DisplayName = "Strength"),
	Intelligence	UMETA(DisplayName = "Intelligence"),
	Wisdom			UMETA(DisplayName = "Wisdom"),
	Dexterity		UMETA(DisplayName = "Dexterity"),
	Constitution	UMETA(DisplayName = "Constitution"),
};

 UCLASS(Blueprintable)
class PROJECTROGUE_API UAdventurer : public UCharacterData
{
	GENERATED_BODY()

	friend UPartyController;

private:
	UPartyController* PartyController;

	UPROPERTY()
	ERace Race;

	UPROPERTY()
	EClass Class;

	UPROPERTY()
	FSlateBrush Icon;
	//Model?
	//T: we use a map to easily access the stat we need
	//we already have an enum for stats, so lets use it
	UPROPERTY()
	TMap<EStat, int32> Stats;
	TArray<FString> AllowedEquipment;
	TArray<ASpell*> Spells;

	UPROPERTY()
	int32 PartyIndex;

	//T; cached for character creation
	// M: Moved to Adventurer, renamed
	int32 TentativeSpellPoints;
	int32 TentativePrayerPoints;

public:
	UAdventurer();
	~UAdventurer();

	UFUNCTION(BlueprintCallable)
	void Update(FText InName, ERace InRace, EClass InClass, FSlateBrush InIcon);

	FSlateBrush GetIcon() const { return Icon; }

	UFUNCTION(BlueprintCallable)
	int32 GetStatValue(const EStat Stat) { return Stats.FindOrAdd(Stat); }

	UFUNCTION(BlueprintCallable)
	int32 GetStatModifier(EStat Stat);

	TMap<EStat, int32> GetStats() const { return Stats; }
	void SetStats(TMap<EStat, int32> InStats) { Stats = InStats; }

	TArray<ASpell*> GetSpells() const { return Spells; }

	ERace GetRace() const { return Race; }
	EClass GetClass() const { return Class; }
	void AddSpell(ASpell* Spell);

	virtual void OnAttack(FTimerManager& TimerManager, FTimerDelegate AttackDelegate) override;
	virtual bool OnEquip(const UItem* InItem) override;

	// Maybe this is added during character creation?
	UFUNCTION(BlueprintCallable) 
	void AddAllowedEquipment(const FString& InName);

	UFUNCTION(BlueprintCallable)
	void ClearAllowedEquipment() { AllowedEquipment.Empty(); }

	UFUNCTION(BlueprintCallable)
	void TransferGoldToParty();

	UFUNCTION(BlueprintCallable)
	void RandomizeStats();

	UFUNCTION(BlueprintCallable)
	void SetPartyIndex(int32 NewIndex) { PartyIndex = NewIndex; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetPartyIndex() const { return PartyIndex; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CanLevelUp();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetExpToLevel();

	bool LevelUp();


	UFUNCTION(BlueprintCallable)
	void AddStatPoint(EStat Stat);

	UFUNCTION(BlueprintCallable)
	bool UseItem(const int32 InventoryIndex);

private:
	int32 RollManaIncrease(const TMap<ECombatDataType, FCombatDice>& Data, ECombatDataType DataType, UDiceBag* DiceBag);
	void SetPartyController(UPartyController* InPartyController) { PartyController = InPartyController; }
	const TMap<ECombatDataType, FCombatDice>& GetCombatData(ERace InRace);
	const TMap<ECombatDataType, FCombatDice>& GetCombatData(EClass InClass);
	int32 RollManaPoints(ECombatDataType ManaType, EStat Stat);
};
