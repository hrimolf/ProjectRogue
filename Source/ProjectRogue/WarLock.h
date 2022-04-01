// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MonsterBase.h"
#include "TileBase.h"

#include "WarLock.generated.h"

/**
 * 
 */

class AFireBall;
class AMonsterBase;
UCLASS()
class PROJECTROGUE_API AWarLock : public AMonsterBase
{
	GENERATED_BODY()
private:
	FVector FireBallTarget;
	FVector BossRoomEntranceLocation;
	TArray<ATileBase*> RoomTiles;
	TArray<ATileBase*> RoomCorners;
	bool bFoundPlayer;
	bool bOnCoolDown;
	FTimerHandle FireballTimer;
	AFireBall* CurrentFireBall;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CoolDown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ChannelTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TeleportThreshold;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SpawnMinionThreshold;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int NextTeleportHp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int NextSpawnMinionHp;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AFireBall> FireBall;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMonsterBase> Minion;
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void Init();
	void StandBy();
	void KeepTrackPlayer();
	void CastFireBall();
	void Fire();
	void Teleport();
	void SpawnMinion();

private:
	virtual void OnTakeDamage() final;
};
