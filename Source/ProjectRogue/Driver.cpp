// Fill out your copyright notice in the Description page of Project Settings.

#include "Driver.h"
#include "ProjectRogueCharacter.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Engine/Engine.h"

#include "TileBase.h"
#include "PawnPlaceHolder.h"

#include "Shopkeeper.h"
#include "TavernKeeper.h"
#include "Building.h"
#include "Room.h"

constexpr int kUnitLength = 100;
constexpr int kInitialRot = 90.f;

// Sets default values
ADriver::ADriver()
	: bIsMoving{false}
	, bIsTurning{false}
	, bIsInRoom{false}
	, CurrentLerp{0}
	, MoveSpeed{2}
	, TurnSpeed{2}
	, ActorImFacing{nullptr}
	, PlaceHolder{nullptr}
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADriver::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADriver::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsMoving)
	{
		Move(DeltaTime);
	}
	if (bIsTurning)
	{
		Turn(DeltaTime);
	}
}

bool ADriver::TryMove(bool GoForward)
{
	if (bIsMoving|| bIsTurning)
		return false;

	StartPos = GetActorLocation();
	if (GoForward)
	{
		EndPos = StartPos + GetActorForwardVector() * kUnitLength;
		
	}
	else // go backward
	{
		EndPos = StartPos - GetActorForwardVector() * kUnitLength;
	}

	EndPos.X = FMath::RoundToInt(EndPos.X);
	EndPos.Y = FMath::RoundToInt(EndPos.Y);

	if(CheckObstacle(EndPos))
	{
		if (GoForward)
			Interact();
	}
	else
	{
		FRotator Rotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.bNoFail = true;
		
		bIsMoving = true;
		PreMove();

		PlaceHolder = GetWorld()->SpawnActor<APawnPlaceHolder>(EndPos, Rotation, SpawnInfo);
		return true;
	}
	return false;
}

bool ADriver::TryTurn(bool TurnRight)
{
	if (bIsMoving || bIsTurning)
		return true;
	PreTurn();

	StartRot = GetActorRotation();
	if (TurnRight)
	{
		EndRot = GetActorRotation().Add(0, 90.f, 0);
	}
	else // turn left
	{
		EndRot = GetActorRotation().Add(0, -90.f, 0);
	}

	EndRot.Yaw = FMath::RoundToInt(EndRot.Yaw);

	bIsTurning = true;
	return true;
}

bool ADriver::LineTraceFront(FHitResult& Outhit, int Distance)
{
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);
	FVector End = GetActorLocation() + GetActorForwardVector() * Distance;
	return GetWorld()->LineTraceSingleByChannel(Outhit, GetActorLocation(), End, ECC_Visibility, TraceParams);
}

bool ADriver::LineTraceDown(FHitResult& hit, int distance)
{
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);
	FVector End = GetActorLocation() + FVector(0, 0, distance);
	return GetWorld()->LineTraceSingleByChannel(hit, GetActorLocation(), End, ECC_Visibility, TraceParams);
}

void ADriver::Interact()
{
	return;
}

EDirection ADriver::GetDirection()
{
	auto Rot = GetActorRotation();
	if (FMath::IsNearlyEqual(Rot.Yaw ,0, 0.1f))
	{
		UE_LOG(LogTemp, Display, TEXT("Facing East!"));

		return EDirection::East;
	}
	if (FMath::IsNearlyEqual(Rot.Yaw, 90, 0.1f))
	{
		UE_LOG(LogTemp, Display, TEXT("Facing South!"));
		return EDirection::South;
	}
	if (FMath::IsNearlyEqual(Rot.Yaw, -90, 0.1f))
	{
		UE_LOG(LogTemp, Display, TEXT("Facing North!"));
		return EDirection::North;
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Facing West!"));
		return EDirection::West;
	}
}

void ADriver::Move(float DeltaTime)
{
	float Step = DeltaTime * MoveSpeed;
	CurrentLerp += Step;
	SetActorLocation(FMath::Lerp(StartPos, EndPos, CurrentLerp));
	if (CurrentLerp > 1)
	{
		CurrentLerp = 0;
		SetActorLocation(EndPos);
		bIsMoving = false;

		PlaceHolder->Destroy();
		PlaceHolder = nullptr;
		PostMove();
	}
}

void ADriver::Turn(float DeltaTime)
{
	float Step = DeltaTime * TurnSpeed;
	CurrentLerp += Step;
	SetActorRotation(FMath::Lerp(StartRot, EndRot, CurrentLerp));
	if (CurrentLerp > 1)
	{
		CurrentLerp = 0;
		SetActorRotation(EndRot);
		bIsTurning = false;
		PostTurn();
	}
}

bool ADriver::CheckObstacle(FVector Target)
{
	FHitResult Hit;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);
	bool result = GetWorld()->LineTraceSingleByChannel(Hit,GetActorLocation(),Target,ECC_Visibility,TraceParams);

	return result;
}

void ADriver::SetRotation(EDirection Dir)
{
	switch (Dir)
	{
	case EDirection::East: SetActorRotation(FRotator(0,0,0)); break;
	case EDirection::South: SetActorRotation(FRotator(0, 90, 0)); break;
	case EDirection::West: SetActorRotation(FRotator(0, 180, 0)); break;
	case EDirection::North: SetActorRotation(FRotator(0, -90, 0)); break;
	default: break;
	}
}

FRotator ADriver::GetRotatorFromDirection(EDirection Dir)
{
	switch (Dir)
	{
	case EDirection::East: return(FRotator(0, 0, 0)); 
	case EDirection::South: return(FRotator(0, 90, 0));
	case EDirection::West: return(FRotator(0, 180, 0)); 
	case EDirection::North: return(FRotator(0, -90, 0)); 
	default: return FRotator(0, 0, 0); 
	}
}

FVector2D ADriver::GetPosition2D()
{
	if (!bIsInRoom)
	{
		auto Pos = GetActorLocation();
		return { Pos.X, Pos.Y};
	}
	else
	{
		auto Pos = ABuilding::GetRoom()->GetBuildingPos();
		return { Pos.X ,Pos.Y};
	}
}

float ADriver::GetMiniMapRot()
{
	if (!bIsInRoom)
	{
		return GetActorRotation().Yaw + kInitialRot;
	}
	else
	{
		auto Dir = ABuilding::GetRoom()->GetEnteringDirection();
		return GetActorRotation().Yaw + GetRotatorFromDirection(Dir).Yaw;
	}
	return 0.0f;
}

void ADriver::PreMove()
{
	//Add here any code that should be executed before a movement happens
}

void ADriver::PostMove()
{
	//Add here any code that should be executed after a movement happenned
}

void ADriver::PreTurn()
{
	//Add here any code that should be executed before a movement happens
}

void ADriver::PostTurn()
{
	//Add here any code that should be executed after a movement happenned
}

