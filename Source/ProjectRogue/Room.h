// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Direction.h"
#include "RoomType.h"

#include "Room.generated.h"

class ABuilding;
class AMonsterBase;

UCLASS()
class PROJECTROGUE_API ARoom : public AActor
{
	GENERATED_BODY()
private:
	FVector BuildingPos;
	EDirection EnteredFrom;
protected:
	UPROPERTY(BlueprintReadOnly, Category = Doors)
		bool bHasFrontDoor;
	UPROPERTY(BlueprintReadOnly, Category = Doors)
		bool bHasBackDoor;
	UPROPERTY(BlueprintReadOnly, Category = Doors)
		bool bHasRightDoor;
	UPROPERTY(BlueprintReadOnly, Category = Doors)
		bool bHasLeftDoor;
	UPROPERTY(BlueprintReadOnly, Category = Room)
	ERoomType RoomType;
	AMonsterBase* ActiveMonster;

public:	
	// Sets default values for this actor's properties
	ARoom();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void InitRoom(FVector BuildingPos, ERoomType InType, EDirection EnteredFrom);

	void OnEnter(ABuilding* Building, EDirection Dir, ERoomType Type);
	
	UFUNCTION(BlueprintImplementableEvent)
	void SetRoom();
	UFUNCTION(BlueprintImplementableEvent)
	void ResetRoom();

	void SpawnNPC();
	void MonsterDied() { ActiveMonster = nullptr; }
	bool Leave(EDirection CurrentDir, FVector& OutExitTo, EDirection& OutDir);
	//Returns the position of the building of this room
	UFUNCTION(BlueprintCallable)
	FVector GetBuildingPos()const{ return BuildingPos; }
	EDirection GetEnteringDirection()const { return EnteredFrom; };
};
