// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Direction.h"
#include "Driver.generated.h"

class ATileBase;
class APawnPlaceHolder;
struct FHitResult;

UCLASS()
class PROJECTROGUE_API ADriver : public ACharacter
{
	GENERATED_BODY()
protected:
	TArray<FVector> UnavailableLocations;
	bool bIsMoving;
	bool bIsTurning;
	bool bIsInRoom;

	float CurrentLerp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TurnSpeed;

	AActor* ActorImFacing;

	FVector StartPos;
	FVector EndPos;
	FRotator StartRot;
	FRotator EndRot;
	FHitResult HitResult;

	
	APawnPlaceHolder* PlaceHolder;

public:	
	// Sets default values for this actor's properties
	ADriver();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// para: true to move forward, false to move backwards
	// returns if it actually can move
	UFUNCTION(BlueprintCallable)
	bool TryMove(bool bGoForward);

	// para: true to turn right, false to turn left
	// returns if it actually can turn
	UFUNCTION(BlueprintCallable)
	bool TryTurn(bool bTurnRight);

	UFUNCTION(BlueprintCallable)
	bool IsMoving() const { return bIsMoving; }

	UFUNCTION(BlueprintCallable)
	bool IsTurning() const { return bIsTurning; }

	// Line trace frontally with certain distance
	// Returns whether we hit something
	// FHitResult contains info on the thing we hit
	// Usage:
	// FHitResult.Component					//gets the component we hit
	// FHitResult.Component->GetOwner()		//gets the owning actor we hit
	// FHitResult.Actor						//same as above
	UFUNCTION(BlueprintCallable)
	bool LineTraceFront(FHitResult& Hit, int Distance = 150);
	bool LineTraceDown(FHitResult& Hit, int Distance = -150);

	virtual void Interact();

	AActor* GetActorImFacing() const { return ActorImFacing; }
	EDirection GetDirection();
	void SetRotation(EDirection Dir);
	FRotator GetRotatorFromDirection(EDirection Dir);

	UFUNCTION(BlueprintCallable)
	FVector2D GetPosition2D();

	float GetMiniMapRot();

	//Anything this actor needs to do before moving
	virtual void PreMove();
	//Anything this actor needs to do after moving
	virtual void PostMove();
	//Anything this actor needs to do before moving
	virtual void PreTurn();
	//Anything this actor needs to do before moving
	virtual void PostTurn();
private:
	void Move(float DeltaTime);
	void Turn(float DeltaTime);
	//Return True if there is an obstacle at target location
	bool CheckObstacle(FVector Target);
};
