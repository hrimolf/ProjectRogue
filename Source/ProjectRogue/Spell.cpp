// Fill out your copyright notice in the Description page of Project Settings.


#include "Spell.h"
#include "ProjectRogueCharacter.h"
#include "MonsterBase.h"

// Sets default values
ASpell::ASpell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

//TODO: can i use move operator here? very low priority
void ASpell::Init(AProjectRogueCharacter* InPlayer, ESpellType InType, const int32 InLevel, const int32 InManaCost, const int32 InRange, const FString& InName, const FString& InDescription, const TFunction<bool(UCharacterData*)> InCallback)
{
	Type = InType;
	Player = InPlayer;
	Level = InLevel;
	ManaCost = InManaCost;
	Range = InRange;
	Name = FText::FromString(InName);
	Description = FText::FromString(InDescription);
	Callback = InCallback;
}

bool ASpell::RunCallback(UCharacterData* Character)
{
	checkf(Character, TEXT("Cannot run spell callback: Character is nullptr"));
	return Callback(Character);
}

void ASpell::Cast()
{
	Player->OnSpellCasted(this);
}

// Called when the game starts or when spawned
void ASpell::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

