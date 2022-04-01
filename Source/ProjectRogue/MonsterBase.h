// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Engine/EngineTypes.h"
#include "Driver.h"
#include "CharacterData.h"
#include "ItemSlot.h"
#include "MonsterBase.generated.h"

class UItem;
class USkeletalMeshComponent;
class UCapsuleComponent;
class UCharacterData;
class UDiceBag;
class UHealthbar;
class UWidgetComponent;
class AProjectRogueCharacter;
class ASpell;
class ALootObject;

USTRUCT(Blueprintable)
struct FLootSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemType ItemType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Chance;
};

UCLASS()
class PROJECTROGUE_API AMonsterBase : public ADriver
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	UCharacterData* EnemyData;
	UPROPERTY()
	UDiceBag* DiceBag;
	FTimerHandle MovementTimer;
	FTimerDelegate MovementDelegate;

	FTimerHandle PlayerVisibilityTimer;
	FTimerDelegate PlayerVisibilityDelegate;
	TFunction<void()> DeathCallback;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AProjectRogueCharacter* Player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDoesMove;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HealthDiceRolled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HealthDieFaces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DamageDiceRolled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DamageDieFaces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Experience;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackRateModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RetaliateDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetComponent* Healthbar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UHealthbar* HealthbarWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FLootSlot> LootTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ALootObject> LootObjectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GoldChance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxGold;

	bool bCounterAttack = false;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Sets default values for this actor's properties
	AMonsterBase();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PostMove() override final;
	virtual void PostTurn() override final;

	void TakeWeaponDamage(int32 Damage);
	bool TakeSpellDamage(ASpell* Spell);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCurrentHealth() const { return EnemyData->GetCurrentHealth(); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMaxHealth() const { return EnemyData->GetMaxHealth(); }

	int32 GetExperience() const { return EnemyData->GetXP(); }

	const TArray<UItem*>& GetInventory() const { return EnemyData->GetInventory(); }

	unsigned int GetGold() const { return EnemyData->GetGold(); }

	UFUNCTION(BlueprintCallable)
	void UpdateHealthbar();

	UFUNCTION(BlueprintCallable)
	void ShowHealthbar();
	UFUNCTION(BlueprintCallable)
	void HideHealthbar();
	UFUNCTION(BlueprintCallable)
	void KillAndRunCallBack();

	void TakeAction();
	void SetPlayer(AProjectRogueCharacter* InPlayer) { Player = InPlayer; }
	void SetDeathCallback(TFunction<void()>&& InCallback) { DeathCallback = MoveTemp(InCallback); }
	void SetIsInRoom() { bIsInRoom = true; }
	void SetDoesMove(bool bInDoesMove) { bDoesMove = bInDoesMove; }

	bool CheckPlayer();
	void Roam();
	void TurnToPlayer(AProjectRogueCharacter* ThePlayer);
private:
	virtual void OnTakeDamage();
	void Rotate();
	void Attack();
};
