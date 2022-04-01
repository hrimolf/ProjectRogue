// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FireBall.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTROGUE_API AFireBall : public AActor
{
	GENERATED_BODY()
private:
	FVector StartPos;
	FVector EndPos;
	float CurrentLerp;
	int Damage;
	bool bIsMoving;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed;

protected:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override final;
	virtual void Tick(float DeltaTime) override;
	void Move(float DeltaTime);
public:
	AFireBall();
	void FireAt(FVector Target, int InDamage);
	UFUNCTION(BluePrintCallable)
	void Explode();
	bool IsMoving()const { return bIsMoving; }
};
