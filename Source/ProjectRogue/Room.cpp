// Fill out your copyright notice in the Description page of Project Settings.


#include "Room.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectRogueGameMode.h"
#include "Building.h"

// Sets default values
ARoom::ARoom()
	: bHasFrontDoor{false}
	, bHasBackDoor{false}
	, bHasRightDoor{false}
	, bHasLeftDoor{false}
	, RoomType{ERoomType::Undefined}
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ARoom::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARoom::InitRoom(FVector InBuildingPos, ERoomType InType, EDirection InEnteredFrom)
{
	BuildingPos = InBuildingPos;
	RoomType = InType;
	EnteredFrom = InEnteredFrom;
	bHasBackDoor = true;
}

void ARoom::OnEnter(ABuilding* Building, EDirection Dir, ERoomType Type)
{
	RoomType = Type;

	BuildingPos = Building->GetActorLocation();
	UE_LOG(LogTemp, Warning, TEXT("Building at (%f,%f)"), BuildingPos.X, BuildingPos.Y);
	EnteredFrom = Dir;
	bHasBackDoor = true;
	bHasFrontDoor = false;
	bHasRightDoor = false;
	bHasLeftDoor = false;
	switch (Dir)
	{
	case EDirection::North:
	{
		if (Building->HasDoor(EDirection::West))
			bHasRightDoor = true;
		if (Building->HasDoor(EDirection::South))
			bHasFrontDoor = true;
		if (Building->HasDoor(EDirection::East))
			bHasLeftDoor = true;
		break;
	}
	case EDirection::East:
	{
		if (Building->HasDoor(EDirection::North))
			bHasRightDoor = true;
		if (Building->HasDoor(EDirection::West))
			bHasFrontDoor = true;
		if (Building->HasDoor(EDirection::South))
			bHasLeftDoor = true;
		break;
	}
	case EDirection::South:
	{
		if (Building->HasDoor(EDirection::East))
			bHasRightDoor = true;
		if (Building->HasDoor(EDirection::North))
			bHasFrontDoor = true;
		if (Building->HasDoor(EDirection::West))
			bHasLeftDoor = true;
		break;
	}
	case EDirection::West:
	{
		if (Building->HasDoor(EDirection::South))
			bHasRightDoor = true;
		if (Building->HasDoor(EDirection::East))
			bHasFrontDoor = true;
		if (Building->HasDoor(EDirection::North))
			bHasLeftDoor = true;
		break;
	}
	}

	SetRoom();
	if (RoomType == ERoomType::Normal)
	{
		SpawnNPC();
	}
}

void ARoom::SpawnNPC()
{
	if (FMath::RandRange(1, 100) > 20)
		return;

	//NPC pos = room actor position - 100y
	UWorld* World = GetWorld();
	AProjectRogueGameMode* GameMode = Cast<AProjectRogueGameMode>(UGameplayStatics::GetGameMode(World));
	check(GameMode);
	TSubclassOf<AMonsterBase> RandomMonster = GameMode->GetRandomTownMonster();
	RandomMonster->IsValidLowLevel();
	FVector SpawnPosition = GetActorLocation();
	SpawnPosition.Y -= 100;

	ActiveMonster = World->SpawnActor<AMonsterBase>(RandomMonster, SpawnPosition, FRotator(0, 100, 0));
	ActiveMonster->SetIsInRoom();
}

bool ARoom::Leave(EDirection InDir, FVector& OutExitTo, EDirection& OutDir)
{
	bool bCanLeave = false;;
	FVector DeltaPos; 
	switch (InDir)
	{
	case EDirection::East:
	{
		if (bHasRightDoor)
		{
			UE_LOG(LogTemp, Warning, TEXT("leaving right door!"));

			switch (EnteredFrom)
			{
			case EDirection::East: DeltaPos = FVector(0, -100, 0); OutDir = EDirection::North; break;
			case EDirection::South: DeltaPos = FVector(100, 0, 0); OutDir = EDirection::East; break;
			case EDirection::West: DeltaPos = FVector(0, 100, 0); OutDir = EDirection::South; break;
			case EDirection::North: DeltaPos = FVector(-100, 0, 0); OutDir = EDirection::West; break;
			}

			OutExitTo = BuildingPos + DeltaPos;
			bCanLeave = true;
		}
		break;
	}
	case EDirection::North:
	{
		if (bHasFrontDoor)
		{
			UE_LOG(LogTemp, Warning, TEXT("leaving front door!"));

			switch (EnteredFrom)
			{
			case EDirection::East: DeltaPos = FVector(-100, 0, 0); OutDir = EDirection::West; break;
			case EDirection::South: DeltaPos = FVector(0, -100, 0); OutDir = EDirection::North; break;
			case EDirection::West: DeltaPos = FVector(100, 0, 0); OutDir = EDirection::East; break;
			case EDirection::North: DeltaPos = FVector(0, 100, 0); OutDir = EDirection::South; break;
			}

			OutExitTo = BuildingPos + DeltaPos;
			bCanLeave = true;
		}
		break;
	}
	case EDirection::South:
	{
		if (bHasBackDoor)
		{
			UE_LOG(LogTemp, Warning, TEXT("leaving back door!"));
			switch (EnteredFrom)
			{
			case EDirection::East: DeltaPos = FVector(100, 0, 0); OutDir = EDirection::East; break;
			case EDirection::South: DeltaPos = FVector(0, 100, 0); OutDir = EDirection::South; break;
			case EDirection::West: DeltaPos = FVector(-100, 0, 0); OutDir = EDirection::West; break;
			case EDirection::North: DeltaPos = FVector(0, -100, 0); OutDir = EDirection::North; break;
			}
			OutExitTo = BuildingPos + DeltaPos;
			bCanLeave = true;
		}
		break;
	}
	case EDirection::West:
	{
		if (bHasLeftDoor)
		{
			UE_LOG(LogTemp, Warning, TEXT("leaving left door!"));
			switch (EnteredFrom)
			{
			case EDirection::East: DeltaPos = FVector(0, 100, 0); OutDir = EDirection::South; break;
			case EDirection::South: DeltaPos = FVector(-100, 0, 0); OutDir = EDirection::West; break;
			case EDirection::West: DeltaPos = FVector(0, -100, 0); OutDir = EDirection::North; break;
			case EDirection::North: DeltaPos = FVector(100, 0, 0); OutDir = EDirection::East; break;
			}
			OutExitTo = BuildingPos + DeltaPos;
			bCanLeave = true;
		}
		break;
	}
	default: break;
	}

	if (IsValid(ActiveMonster))
	{
		ActiveMonster->Destroy();
		ActiveMonster = nullptr;
	}
	else
	{
		if (bCanLeave)
		{
			ResetRoom();
		}
	}	
	return bCanLeave;
}

