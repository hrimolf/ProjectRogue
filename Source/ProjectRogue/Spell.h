// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spell.generated.h"

class UDiceBag;
class UCharacterData;
class AProjectRogueCharacter;

UENUM()
enum class ESpellType : uint8
{
	Spell,
	Prayer,
};

UENUM()
enum class ETargetType : uint8
{
	Single,
	Area,
};

UCLASS()
class PROJECTROGUE_API ASpell : public AActor
{
	GENERATED_BODY()
	
private:
	AProjectRogueCharacter* Player;
	ESpellType Type;
	ETargetType TargetType;
	int32 Level;
	int32 ManaCost;
	int32 Range;
	FText Name;
	FText Description;
	TFunction<bool(UCharacterData*)> Callback;

public:	
	// Sets default values for this actor's properties
	ASpell();

	void Init(AProjectRogueCharacter* InPlayer, ESpellType InSpellType, ETargetType InTargetType, const int32 InLevel, const int32 InManaCost, const int32 InRange, const FString& InName, const FString& InDescription, const TFunction<bool(UCharacterData*)> InCallback);
	bool RunCallback(UCharacterData* Character = nullptr);

	ESpellType GetSpellType() const { return Type; }
	ETargetType GetTargetType() const { return TargetType; }
	const int32& GetLevel() const { return Level; }
	const int32& GetManaCost() const { return ManaCost; }
	const int32& GetRange() const { return Range; }
	const FText& GetName() const { return Name; }
	const FText& GetDescription() const { return Description; }
	const TFunction<bool(UCharacterData*)>& GetCallback() const { return Callback; }

	UFUNCTION(BlueprintCallable)
	void Cast();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
