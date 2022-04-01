// Fill out your copyright notice in the Description page of Project Settings.


#include "FireBall.h"
#include "ProjectRogueCharacter.h"

AFireBall::AFireBall()
    : bIsMoving{false}
{
    PrimaryActorTick.bCanEverTick = true;
}


void AFireBall::NotifyActorBeginOverlap(AActor* OtherActor)
{
    auto* Player = Cast<AProjectRogueCharacter>(OtherActor);
    if (Player)
    {
        const TArray<UAdventurer*>& Party = Player->GetParty();
        for (auto& Adventurer : Party)
        {
            Player->OnTakeDamage(Adventurer,Damage);
        }
        UE_LOG(LogTemp,Warning,TEXT("Adventurers took %d damage!"), Damage);
    }
}

void AFireBall::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (bIsMoving)
    {
        Move(DeltaTime);
    }
}



void AFireBall::Move(float DeltaTime)
{
    float Step = DeltaTime * Speed;
    CurrentLerp += Step;
    SetActorLocation(FMath::Lerp(StartPos, EndPos, CurrentLerp));
    if (CurrentLerp > 1)
    {
        CurrentLerp = 0;
        SetActorLocation(EndPos);
        bIsMoving = false;
        Explode();
    }
}

void AFireBall::FireAt(FVector Target, int InDamage)
{
    StartPos = GetActorLocation();
    EndPos = Target;
    bIsMoving = true;

    Damage = InDamage;
}

void AFireBall::Explode()
{
    //
    Destroy();
}


