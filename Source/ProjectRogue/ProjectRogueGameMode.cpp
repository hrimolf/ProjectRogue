// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectRogueGameMode.h"
#include "ProjectRogueHUD.h"
#include "ProjectRogueCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"

#include "TileBase.h"
#include "Building.h"
#include "WarLock.h"

AProjectRogueGameMode::AProjectRogueGameMode()
	: Super()
{
	//THIS LINE OF CODE IS CURSED! NEVER USE THIS!
	// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	//USE THIS INSTEAD
	//DefaultPawnClass = PlayerActor;

	// use our custom HUD class
	//HUDClass = AProjectRogueHUD::StaticClass();
}


void AProjectRogueGameMode::GenerateAreaWithMonsters(TSubclassOf<AActor> TileClass, FVector StartLocation, const uint8 Width, const uint8 Height, const FVector TileSize, const TArray<TSubclassOf<AMonsterBase>>& Monsters, int32 MonstersToSpawn, bool bDoMonstersMove)
{
	UWorld* World = GetWorld();
	FRotator Rotation{ 0,0,0 };
	FVector InitialLocation = StartLocation;
	FVector NewLocation = InitialLocation;
	TArray<ATileBase*> Tiles;

	for (uint8 y = 0; y < Height; ++y)
	{
		for (uint8 x = 0; x < Width; ++x)
		{
			NewLocation.X = InitialLocation.X + (TileSize.X * x);
			NewLocation.Y = InitialLocation.Y + (TileSize.Y * y);

			ATileBase* Tile = World->SpawnActor<ATileBase>(TileClass, NewLocation, Rotation);
			Tiles.Add(Tile);
		}
	}

	GenerateMonsters(StartLocation, Tiles, Monsters, MonstersToSpawn, bDoMonstersMove);
}

void AProjectRogueGameMode::GenerateMonsters(const FVector& StartLocation, const TArray<ATileBase*>& Tiles, const TArray<TSubclassOf<AMonsterBase>>& Monsters, int32 MonstersToSpawn, bool bDoMonstersMove)
{
	UWorld* World = GetWorld();
	AActor* ActorOnTile = nullptr;
	TSubclassOf<AMonsterBase> MonsterClass = nullptr;
	AMonsterBase* Monster = nullptr;
	int32 EnemiesSpawned = 0;
	int32 RandomTileIndex = 0;
	int32 RandomMonsterIndex = 0;
	FVector TilePosition;
	FVector TraceDistance{ 0.f, 0.f, 150.f };
	FVector TraceEnd;
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	FCollisionObjectQueryParams Params;
	Params.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
	Params.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);

	TArray<ATileBase*> AvailableTiles;
	//T: cache available tiles so they're easier to randomly select
	//and can be removed from the array
	for (auto& Tile : Tiles)
	{
		//line trace up from tile
		TilePosition = Tile->GetActorLocation();
		TraceEnd = TilePosition + TraceDistance;
		QueryParams.AddIgnoredActor(Tile);
		World->LineTraceSingleByObjectType(HitResult, TilePosition, TraceEnd, Params, QueryParams);
		ActorOnTile = Cast<AActor>(HitResult.Actor);

		//check if location is available
		if (!CheckIsAvailable(TilePosition))
			continue;

		if (!ActorOnTile)
		{
			AvailableTiles.Add(Tile);
		}
		QueryParams.ClearIgnoredActors();
	}

	ATileBase* Tile = nullptr;
	int32 MonsterCount = Monsters.Num() - 1;
	FActorSpawnParameters SpawnParams;
	SpawnParams.bNoFail = true;

	while (EnemiesSpawned < MonstersToSpawn)
	{
		//find a random tile
		RandomTileIndex = FMath::RandRange(0, AvailableTiles.Num() - 1);
		Tile = AvailableTiles[RandomTileIndex];

		//line trace up from tile
		TilePosition = Tile->GetActorLocation();
		TraceEnd = TilePosition + TraceDistance;
		QueryParams.AddIgnoredActor(Tile);
		World->LineTraceSingleByObjectType(HitResult, TilePosition, TraceEnd, Params, QueryParams);
		ActorOnTile = Cast<AActor>(HitResult.Actor);

		//if there is no building or monster, spawn monster
		if (!ActorOnTile)
		{
			//get random monster class
			RandomMonsterIndex = FMath::RandRange(0, MonsterCount);
			MonsterClass = Monsters[RandomMonsterIndex];

			//lower monster spawn positions so they line up with tiles
			TraceEnd.Z = FMath::TruncToInt(StartLocation.Z + MonsterSpawnOffset);
			//create monster from class
			Monster = World->SpawnActor<AMonsterBase>(MonsterClass, TraceEnd, FRotator(0.f, 0.f, 0.f), SpawnParams);
			Monster->SetDeathCallback([this, StartLocation, AvailableTiles, Monsters, bDoMonstersMove]
				{ SpawnNewMonster(StartLocation, AvailableTiles, Monsters, bDoMonstersMove); });
			
			if (bDoMonstersMove)
			{
				Monster->SetDoesMove(true);
			}


			++EnemiesSpawned;
		}

		//remove the tile so we dont randomly select it again
		AvailableTiles.Remove(Tile);
		QueryParams.ClearIgnoredActors();
	}
}

bool AProjectRogueGameMode::CheckIsAvailable(const FVector& TraceEnd)
{
	for (const auto& Location : UnavailableTileLocations)
	{
		if (TraceEnd.Equals(Location, 50.f))
		{
			return false;
		}
	}
	return true;
}

void AProjectRogueGameMode::SpawnNewMonster(const FVector& StartLocation, const TArray<ATileBase*>& Tiles, const TArray<TSubclassOf<AMonsterBase>>& Monsters, bool bDoMonstersMove)
{
	GenerateMonsters(StartLocation, Tiles, Monsters, 1, bDoMonstersMove);
}

TSubclassOf<AMonsterBase> AProjectRogueGameMode::GetRandomTownMonster() const
{
	check(TownMonsters.Num() > 0);
	return TownMonsters[FMath::RandRange(0, TownMonsters.Num() - 1)];
}

void AProjectRogueGameMode::RespawnDungeon()
{
	FActorSpawnParameters Params;
	Params.bNoFail = true;
	if (!IsValid(TheBossRoomKey))
	{
		TheBossRoomKey = GetWorld()->SpawnActor<AActor>(BossRoomKey, FVector(500,500,-950), FRotator(0,0,0), Params);
	}
	if (!IsValid(TheBossRoomDoor))
	{
		TheBossRoomDoor = GetWorld()->SpawnActor<AActor>(BossRoomDoor, FVector(900,355,-1000), FRotator(0, 0, 0), Params);
	}
	if (!IsValid(TheBoss))
	{
		TheBoss = GetWorld()->SpawnActor<AWarLock>(Boss, FVector(900,100,-950), FRotator(0, 90, 0), Params);
	}
}
