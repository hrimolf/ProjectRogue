// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileBase.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class USceneComponent;

UCLASS()
class PROJECTROGUE_API ATileBase : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* DefaultSceneRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* TileCenter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* ActorOnTile;

public:
	// Sets default values for this actor's properties
	ATileBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override final;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override final;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	AActor* GetActorOnTile() const { return ActorOnTile; }
	void SetActorOnTile(AActor* Actor) { ActorOnTile = Actor; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetCenterOfTile() const;
};