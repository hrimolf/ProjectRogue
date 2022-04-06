// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Containers/Array.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/World.h"
#include "ItemManager.h"
#include "TileBase.h"
#include "Building.h"
#include "ProjectRogueCharacter.h"
#include "Healthbar.h"
#include "Spell.h"
#include "LootObject.h"

// Sets default values
AMonsterBase::AMonsterBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->InitCapsuleSize(30.f, 60.0f);

	bDoesMove = false;
	Healthbar = CreateDefaultSubobject<UWidgetComponent>(TEXT("Healthbar"));
	Healthbar->SetupAttachment(RootComponent);
	GetCapsuleComponent()->SetSimulatePhysics(true);
}

// Called when the game starts or when spawned
void AMonsterBase::BeginPlay()
{
	Super::BeginPlay();

	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);

	HealthbarWidget = Cast<UHealthbar>(Healthbar->GetUserWidgetObject());
	if (!HealthbarWidget)
		return;

	HealthbarWidget->SetName(Name);
	//create monster data from blueprint values provided
	//based on GDD
	EnemyData = NewObject<UCharacterData>(this);
	EnemyData->AdjustXP(Experience);
	EnemyData->RenameCharacter(Name);
	DiceBag = NewObject<UDiceBag>();
	TArray<int32> HealthDice;
	HealthDice.Init(HealthDieFaces, HealthDiceRolled);
	DiceBag->SetDice(HealthDice);
	DiceBag->Roll();
	EnemyData->IncreaseMaxHealth(DiceBag->GetTotal());
	HealthbarWidget->Update(EnemyData->GetCurrentHealth(), EnemyData->GetMaxHealth());
	HideHealthbar();

	LineTraceDown(HitResult, -200);
	ATileBase* Tile = Cast<ATileBase>(HitResult.Actor);
	if (Tile)
	{
		Tile->SetActorOnTile(this);
	}

	if (bDoesMove)
	{
		TakeAction();
	}
}

// Called every frame
void AMonsterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMonsterBase::TakeWeaponDamage(int32 Damage)
{
	EnemyData->OnTakeDamage(Damage);
	OnTakeDamage();
}

bool AMonsterBase::TakeSpellDamage(ASpell* Spell)
{
	check(Spell);
	if (Spell->RunCallback(EnemyData))
	{
		OnTakeDamage();
		return true;
	}
	return false;
}

bool AMonsterBase::CheckPlayer()
{
	FHitResult Hit;
	if (LineTraceFront(Hit, 1000))
	{
		auto P = Cast<AProjectRogueCharacter>(Hit.Actor);
		if (P)
		{
			Player = P;
			return true;
		}
		else
		{
			Player = nullptr;
			return false;
		}
	}
	return false;
}

void AMonsterBase::Roam()
{
	if (CheckPlayer())
	{
		if (!TryMove(true))
		{
			Attack();
		}
	}
	else
	{
		if (FMath::RandRange(1, 100) <= 50)
		{
			if (!TryMove(true))
			{
				Rotate();
			}
		}
		else
		{
			Rotate();
		}
	}
}

void AMonsterBase::OnTakeDamage()
{
	checkf(EnemyData, TEXT("Monster can't take damage: EnemyData is nullptr"));
	UpdateHealthbar();
	int32 Health = EnemyData->GetCurrentHealth();

	if (Health <= 0)
	{
		if (FMath::FRandRange(0, 1) < GoldChance)
		{
			EnemyData->AddGold(FMath::RandRange(1, MaxGold));
		}

		UItem* Item = nullptr;
		//T: roll is successfull if it is less than chance
		//otherwise a 25% chance has a 75% chance of success

		// M: Your original choice was 0-1, but your values are 10-40.
		//	We want to test 0-99 where 10.0 = 10%
		//	We also want this roll to be performed once for all items.
		float Choice = FMath::FRandRange(0, 99);
		
		//randomize inventory contents based on loot table
		for (const auto& Entry : LootTable)
		{
			//if choice > chance, roll was not successful
			// M: subtract the current % and check the next item.
			//	If none of the items are hit, we reached the 
			//	"no item" chance, being whatever is leftover.
			if (Choice > Entry.Chance)
			{
				Choice -= Entry.Chance;
				continue;
			}

			if (Entry.Name != "Random")
			{
				Item = AItemManager::GetItem(Entry.Name);
			}
			else
			{
				if (Entry.ItemType == EItemType::Special || Entry.ItemType == EItemType::Potion)
				{
					Item = AItemManager::GetRandomItemFromType(Entry.ItemType, ETypeSearchType::SubTypeOnly);
				}
				else
				{
					Item = AItemManager::GetRandomItemFromType(Entry.ItemType, ETypeSearchType::TypeOnly);
				}
				
			}
			if (Item)
			{
				GEngine->AddOnScreenDebugMessage(2, 2, FColor::Green, "Monster has " + Item->GetItemName().ToString());
				EnemyData->AddItem(Item);
			}

			//break on successfully adding an item
			//because monsters can only drop one item
			break;
		}

		const auto& Inventory = EnemyData->GetInventory();
		if (Inventory.Num() > 0)
		{
			ALootObject* LootObject = GetWorld()->SpawnActorDeferred<ALootObject>(LootObjectClass, FTransform());

			LineTraceDown(HitResult, -200);
			ATileBase* Tile = Cast<ATileBase>(HitResult.Actor);
			check(Tile);
			Tile->SetActorOnTile(LootObject);

			//spawn a loot object where this monter is
			check(LootObject);
			LootObject->Init(Inventory);
			FVector Location = GetActorLocation();
			Location.Z -= 60;

			LootObject->FinishSpawning(FTransform());
			LootObject->SetActorLocation(Location);
		}
		else
		{
			LineTraceDown(HitResult, -200);
			ATileBase* Tile = Cast<ATileBase>(HitResult.Actor);
			check(Tile);
			Tile->SetActorOnTile(nullptr);
		}

		//tell the player that this monster died to get gold and exp
		Player->MonsterDied(this);
		GetWorld()->GetTimerManager().ClearTimer(EnemyData->GetAttackHandle());
		GetWorld()->GetTimerManager().ClearTimer(MovementTimer);
		Destroy();

		//T: the death callback to respawn another monster should only happen
		//if the monster is not in a room. Monsters in rooms dont have a death
		//callback so that would cause an error anyways
		if (!bIsInRoom)
		{
			DeathCallback();
		}
		return;
	}
	else
	{
		ShowHealthbar();
	}

	//quickly rotate toward player
	if (Player)
	{
		FHitResult Hit;
		LineTraceFront(Hit);
		auto P = Cast<AProjectRogueCharacter>(Hit.Actor);
		if (P == nullptr)
		{
			bCounterAttack = true;
			TurnToPlayer(Player);
		}
		else if(!bCounterAttack)
		{
			bCounterAttack = true;
			TakeAction();
		}
	}
	else
	{
		check(false);
	}
}

void AMonsterBase::TakeAction()
{
	//check if player is within 10 tiles in front of the monster
	if (CheckPlayer())
	{
		//if this is an dungeon monster
		if (bDoesMove)
		{
			//prevent the 
			for (const auto& Location : UnavailableLocations)
			{
				if (EndPos == Location)
				{
					return;
				}
			}
			//move up to player
			if (TryMove(true))
			{

			}
			//if it can not move , it means it's right infront of player
			else
			{
				Attack();
			}
		}
		//if this is a town monster, simply try attack
		else
		{
			Attack();
		}
	}
	// if there is no player, try roam if it's a dungeon monster
	else if(bDoesMove)
	{
		//make a random decision; move or rotate

		GetWorld()->GetTimerManager().SetTimer(MovementTimer, [this] { Roam(); }, MovementRate, false);
	}
	else if (bCounterAttack)
	{
		bCounterAttack = false;
	}
}

void AMonsterBase::PostMove()
{
	//T: grab the player directly from the world because this monster may not see the player.
	// this is not the most ideal solution, but its the easiest, and there are worse solutions
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	//T: show or hide healthbar here for enemies that come into or go out of the 
	// player's view while the player is standing still. we line trace to the player 
	// manually because we dont know what direction the player is in
	FHitResult ViewResult;
	FVector Start = GetActorLocation();
	FVector End = PlayerCharacter->GetActorLocation();
	ECollisionChannel Channel = ECollisionChannel::ECC_Visibility;
	GetWorld()->LineTraceSingleByChannel(ViewResult, Start, End, Channel);
	if (Cast<AProjectRogueCharacter>(ViewResult.Actor))
	{
		ShowHealthbar();
	}
	else
	{
		HideHealthbar();
	}

	TakeAction();
}

void AMonsterBase::PostTurn()
{
	TakeAction();
}

void AMonsterBase::Rotate()
{
	//rotate a random direction
	if (FMath::RandRange(1, 100) <= 50)
	{
		TryTurn(true);
	}
	else
	{
		TryTurn(false);
	}
}

void AMonsterBase::Attack()
{
	check(Player);
	if (EnemyData->GetCurrentHealth() <= 0)
		return;

	float AttackRate = 10.f - FMath::RandRange(0.f, AttackRateModifier);
	float Distance = FMath::RoundToZero(GetDistanceTo(Player));
	if (Distance <= AttackRange)
	{
		const TArray<UAdventurer*>& Party = Player->GetParty();
		size_t PartySize = Party.Num();
		int32 Damage = 0;
		TArray<int32> DamageDice;
		DamageDice.Init(DamageDieFaces, DamageDiceRolled);
		DiceBag->SetDice(DamageDice);
		if (DamageDieFaces == 1)
		{
			Damage = 1;
		}
		else
		{
			Damage = DiceBag->Roll();
		}

		if (PartySize == 1)
		{
			Player->OnTakeDamage(Party[0], Damage);
		}
		else
		{
			//possibly target higher health party member in town
			if (FMath::RandRange(1, 100) <= 50)
			{
				//try attacking the second party member
				//if the second party member is already dead
				if (Party[1]->GetCurrentHealth() <= 0)
				{
					//attack the first one
					Player->OnTakeDamage(Party[0], Damage);
				}
				else
				{
					Player->OnTakeDamage(Party[1], Damage);
				}
			}
			else
			{
				Player->OnTakeDamage(Party[0], Damage);
			}
		}
	}

	//start attack timer
	GetWorld()->GetTimerManager().ClearTimer(MovementTimer);
	GetWorld()->GetTimerManager().SetTimer(MovementTimer, [this] { TakeAction(); }, AttackRate, false);
}

void AMonsterBase::TurnToPlayer(AProjectRogueCharacter* ThePlayer)
{
	if (bIsTurning || bIsMoving)
	{
		return;
	}

	auto playerDir = ThePlayer->GetDirection();
	EDirection endDir = EDirection::East;
	switch (playerDir)
	{
	case EDirection::East : endDir = (EDirection::West); break;
	case EDirection::West : endDir = (EDirection::East); break;
	case EDirection::North : endDir = (EDirection::South); break;
	case EDirection::South : endDir = (EDirection::North); break;
	}
	EndRot = GetRotatorFromDirection(endDir);
	StartRot = GetActorRotation();
	bIsTurning = true;
}

void AMonsterBase::UpdateHealthbar()
{
	if (!HealthbarWidget)
		return;

	int32 Health = EnemyData->GetCurrentHealth();

	if (Health <= 0)
		Destroy();

	HealthbarWidget->Update(Health, EnemyData->GetMaxHealth());
}

void AMonsterBase::ShowHealthbar()
{
	if (!HealthbarWidget)
		return;

	HealthbarWidget->SetVisibility(ESlateVisibility::Visible);
}

void AMonsterBase::HideHealthbar()
{
	if (!HealthbarWidget)
		return;

	HealthbarWidget->SetVisibility(ESlateVisibility::Hidden);
}

void AMonsterBase::KillAndRunCallBack()
{
	GetWorld()->GetTimerManager().ClearTimer(EnemyData->GetAttackHandle());
	GetWorld()->GetTimerManager().ClearTimer(MovementTimer);
	Destroy();
	DeathCallback();
}
