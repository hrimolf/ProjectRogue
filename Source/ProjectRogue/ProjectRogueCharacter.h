// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blueprint/UserWidget.h"
#include "Driver.h"
#include "PartyController.h"
#include "ItemSlot.h"
#include "ProjectRogueCharacter.generated.h"

class UInputComponent;
class UStaticMeshComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;
class UDiceBag;
class UPlayerHUD;
class ASpell;
class AMonsterBase;

UENUM(Blueprintable)
enum class EPartyEditType : uint8
{
	Rename,
	Delete,
	Reorder,
};

UENUM(Blueprintable)
enum class EContext : uint8
{
	None,
	Guild,
	CreateAdventurer,
	RenameAdventurer,
	RenameScreen,
	DeleteScreen,
	ReorderScreen,
	GameMenu,
	Shop,
	Review,
	Tavern,
	Loot
};

USTRUCT(Blueprintable)
struct FCombatDice
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DiceThrown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DieFaces;
};

//T: use an enum so we know what each index is just by reading the code
//M: Updated. "CombatDataType" enum moved to CharacterData.
USTRUCT(Blueprintable)
struct FCombatData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<ECombatDataType, FCombatDice> Data;
};

UCLASS(config=Game)
class AProjectRogueCharacter : public ADriver
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Party Controller handles the logical adventurer party */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG", meta = (AllowPrivateAccess = "true"))
	UPartyController* PartyController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG", meta = (AllowPrivateAccess = "true"))
	int32 BowRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG", meta = (AllowPrivateAccess = "true"))
	int32 MeleeRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG", meta = (AllowPrivateAccess = "true"))
	int32 StartingInventorySize;

	bool bHasEnteredDungeon;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UUserWidget> HUD_Class;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	UPlayerHUD* HUD;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EContext Context;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 KeyboardSelectedAdventurer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<ERace, FCombatData> RaceCombatData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EClass, FCombatData> ClassCombatData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector DungeonStartLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TownStartLocation;

	UAdventurer* SpellCaster;
	UCharacterData* Target;
	TMap<EClass, TArray<ASpell*>> Spells;
	FItemSlotInfo ClickedSlot;
	
	UPROPERTY()
	UCharacterData* Shop;
	UPROPERTY()
	UCharacterData* Tavern;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 MerchantInventoryIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> SlitherObject;

	AActor* SlitherBlocker;

public:
	AProjectRogueCharacter();

	/** Returns Mesh1P subobject **/
	UStaticMeshComponent* GetStaticMesh() const { return StaticMesh; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	const TMap<ECombatDataType, FCombatDice>& GetRaceCombatData(ERace Race) { return RaceCombatData[Race].Data; }
	const TMap<ECombatDataType, FCombatDice>& GetClassCombatData(EClass Class) { return ClassCombatData[Class].Data; }

protected:
	virtual void BeginPlay();

	// Helper functions
	void MoveOneUnit();
	void ReverseOneUnit();
	void TurnRight();
	void TurnLeft();

	virtual void Interact() override final;

	virtual void PreMove() override final;
	virtual void PreTurn() override final;
	virtual void PostMove() override final;
	virtual void PostTurn() override final;
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:	
	UFUNCTION(BlueprintCallable)
	UAdventurer* GetAdventurer(int32 Index) { return PartyController->GetAdventurer(Index); }

	UFUNCTION(BlueprintCallable)
	int32 GetGold() { return PartyController->GetGold(); }

	UFUNCTION(BlueprintCallable)
	void EnterTown();

	UFUNCTION(BlueprintCallable)
	void ShowCreateAdventurer();

	UFUNCTION(BlueprintCallable)
	void ShowEditParty(EPartyEditType EditType);

	UFUNCTION(BlueprintCallable)
	void HideEditParty();

	UFUNCTION(BlueprintCallable)
	void DeleteAdventurer(int32 Index);

	UFUNCTION(BlueprintCallable)
	void RenameClicked(int32 Index);

	UFUNCTION(BlueprintCallable)
	bool RenameAdventurer(UAdventurer* Adventurer, const FText& NewName);

	UFUNCTION(BlueprintCallable)
	void SwapAdventurer(int32 FromIndex, int32 ToIndex) { PartyController->SwapAdventurer(FromIndex, ToIndex); }

	UFUNCTION(BlueprintCallable)
	const TArray<UAdventurer*>& GetParty() const;

	UFUNCTION(BlueprintCallable)
	int32 GetPartySize() const { return PartyController->GetPartySize(); }

	//T: this needs to be public so the HUD can access it
	UFUNCTION(BlueprintCallable)
	UAdventurer* AddAdventurer();

	// M: Only used for Debugging
	UFUNCTION(BlueprintCallable)
	UAdventurer* AddRandomAdventurer();

	UFUNCTION(BlueprintCallable)
	void PopulatePartyInterface();

	UFUNCTION(BlueprintCallable)
	void InfoButtonClicked(UAdventurer* Adventurer);

	UFUNCTION(BlueprintCallable)
	void InventoryButtonClicked(UAdventurer* Adventurer);

	UFUNCTION(BlueprintCallable)
	void AttackButtonClicked(UAdventurer* Adventurer);

	UFUNCTION(BlueprintCallable)
	void CastButtonClicked(UAdventurer* Adventurer);

	UFUNCTION(BlueprintCallable)
	void ContextButtonClicked(UAdventurer* Adventurer);

	UFUNCTION(BlueprintCallable)
	void SetContext(EContext NewContext);

	UFUNCTION(BlueprintCallable)
	void UpdatePartyInterface();

	UFUNCTION(BlueprintCallable)
	void SetTarget(UCharacterData* InTarget) { Target = InTarget; }

	void OnTakeDamage(UAdventurer* DamagedAdventurer, int32 Amount);
	void OnSpellCasted(ASpell* Spell);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	EContext GetContext() const { return Context; }

	TArray<UItem*> GetInventory(int32 PartyIndex);
	TArray<ASpell*> GetSpells(EClass InClass);

	void ItemSlotClicked(const FItemSlotInfo& InItemSlot);

	UFUNCTION(BlueprintCallable)
	void DropSelectedItem();

	UFUNCTION(BlueprintCallable)
	void GiveItemClicked();
	void GiveItem();

	UFUNCTION(BlueprintCallable)
	void UseItemClicked();

	void MonsterDied(AMonsterBase* Monster);

	UFUNCTION(BlueprintCallable)
	void EnterAdventurersGuild();

	void LoadGame();

	UFUNCTION(BlueprintCallable)
	void SaveGame();

	UFUNCTION(BlueprintCallable)
	void EnterDungeon();

	UFUNCTION(BlueprintCallable)
	bool CheckKey();

	UFUNCTION(BlueprintCallable)
	void SetKeyboardSelectedAdventurer(int32 Index);

	UFUNCTION(BlueprintImplementableEvent)
	void ContextChanged(EContext OutContext);

	UFUNCTION(BlueprintCallable)
	void MoveUpInMerchant();

	UFUNCTION(BlueprintCallable)
	void MoveDownInMerchant();

	UFUNCTION(BlueprintCallable)
	void RefreshInventory(UAdventurer* Adventurer);

	bool IsNameAvailable(UAdventurer* TempAdventurer, const FText& InName) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetKeyboardSelectedAdventurer() const;

private:
	bool CastSpell(ASpell* Spell, UCharacterData* SpellTarget = nullptr);
	void CreateAllItems();
	void CreateAllSpells();

	void UpdateContextSensitiveUI();
	void ShowHealthbarsForVisibleEnemies();
};