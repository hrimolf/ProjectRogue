// Copyright Epic Games, Inc. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MonsterBase.h"
#include "ProjectRogueGameMode.generated.h"

class ATileBase;
class AWarLock;
UCLASS(minimalapi)
class AProjectRogueGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> UnavailableTileLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ATileBase*> TownTiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ATileBase*> DungeonTiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> TownTile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> DungeonTile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AMonsterBase>> TownMonsters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AMonsterBase>> DungeonMonsters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TownEnemyCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DungeonEnemyCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> BossRoomKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> BossRoomDoor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AWarLock> Boss;

	AActor* TheBossRoomKey;
	AActor* TheBossRoomDoor;
	AWarLock* TheBoss;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MonsterSpawnOffset;

public:
	AProjectRogueGameMode();

	//only uint8 and int32 can be exposed to blueprints >:(
	UFUNCTION(BlueprintCallable)
	void GenerateAreaWithMonsters(TSubclassOf<AActor> TileClass, FVector StartLocation, const uint8 Width, const uint8 Height, const FVector TileSize, const TArray<TSubclassOf<AMonsterBase>>& Monsters, int32 MonstersToSpawn, bool bDoMonstersMove);

	//T: this is used by Room to spawn a random town monster
	TSubclassOf<AMonsterBase> GetRandomTownMonster() const;

	const TArray<FVector>& GetUnavailableTileLocations() const { return UnavailableTileLocations; }

	UFUNCTION(BlueprintCallable)
	void RespawnDungeon();
private:
	void GenerateMonsters(const FVector& StartLocation, const TArray<ATileBase*>& Tiles, const TArray<TSubclassOf<AMonsterBase>>& Monsters, int32 MonstersToSpawn, bool bDoMonstersMove);
	bool CheckIsAvailable(const FVector& TraceEnd);
	void SpawnNewMonster(const FVector& StartLocation, const TArray<ATileBase*>& Tiles, const TArray<TSubclassOf<AMonsterBase>>& Monsters, bool bDoMonstersMove);
};



