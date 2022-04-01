// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomType.h"
#include "Direction.h"
#include "Building.generated.h"

class ARoom;
UCLASS()
class PROJECTROGUE_API ABuilding : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Doors)
	bool NorthDoor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Doors)
	bool EastDoor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Doors)
	bool SouthDoor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Doors)
	bool WestDoor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Room)
	ERoomType RoomType;

public:	
	// Sets default values for this actor's properties
	ABuilding();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void LinkRoom(ARoom* Room);

	static ARoom* GetRoom();
	bool HasDoor(EDirection Dir)const;

	bool Enter(AActor* Actor);
};
