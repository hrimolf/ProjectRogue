// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"
#include "Room.h"
#include "Direction.h"
#include "ProjectRogueCharacter.h"

static ARoom* s_pRoom = nullptr;

// Sets default values
ABuilding::ABuilding()
	:RoomType{ERoomType::Normal}
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ABuilding::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABuilding::LinkRoom(ARoom* Room)
{
	s_pRoom = Room;
	if (s_pRoom)
	{
		UE_LOG(LogTemp, Warning, TEXT("Room Set!"));
	}
}

ARoom* ABuilding::GetRoom()
{
	return s_pRoom;
}

bool ABuilding::HasDoor(EDirection Dir) const
{
	switch (Dir)
	{
	case EDirection::East: return EastDoor;
	case EDirection::North: return NorthDoor;
	case EDirection::South: return SouthDoor;
	case EDirection::West: return WestDoor;
	}
	return false;
}

bool ABuilding::Enter(AActor* Actor)
{
	if (!s_pRoom)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Room!"));
		check(false);
		return false;
	}

	auto Player = Cast<AProjectRogueCharacter>(Actor);
	if (!Player)
		return false;

	auto PlayerDir = Player->GetDirection();

	if (PlayerDir == EDirection::West)
	{
		if (EastDoor)
		{
			UE_LOG(LogTemp, Warning, TEXT("Entering from East door!"));
			s_pRoom->OnEnter(this,EDirection::East, RoomType);
			Actor->SetActorLocation(FVector(0,2000,50));
			Actor->SetActorRotation(FRotator(0,-90,0));
		}
		
	}
	else if (PlayerDir == EDirection::East)
	{
		if (WestDoor)
		{
			UE_LOG(LogTemp, Warning, TEXT("Entering from West door!"));
			s_pRoom->OnEnter(this, EDirection::West, RoomType);
			Actor->SetActorLocation(FVector(0, 2000, 50));
			Actor->SetActorRotation(FRotator(0, -90, 0));
		}
	}
	else if (PlayerDir == EDirection::North)
	{
		if (SouthDoor)
		{
			UE_LOG(LogTemp, Warning, TEXT("Entering from South door!"));
			s_pRoom->OnEnter(this, EDirection::South, RoomType);
			Actor->SetActorLocation(FVector(0, 2000, 50));
			Actor->SetActorRotation(FRotator(0, -90, 0));
		}
	}
	else if (PlayerDir == EDirection::South)
	{
		if (NorthDoor)
		{
			UE_LOG(LogTemp, Warning, TEXT("Entering from North door!"));
			s_pRoom->OnEnter(this, EDirection::North, RoomType);
			Actor->SetActorLocation(FVector(0, 2000, 50));
			Actor->SetActorRotation(FRotator(0, -90, 0));
		}
	}

	return true;
}

