// Fill out your copyright notice in the Description page of Project Settings.


#include "WarLock.h"
#include "TileBase.h"
#include "ProjectRogueCharacter.h"
#include "FireBall.h"
#include "Kismet/KismetMathLibrary.h"


void AWarLock::BeginPlay()
{
    Super::BeginPlay();
    
    Init();
}

void AWarLock::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (!bFoundPlayer)
    {
        StandBy();
    }
    else if(!bOnCoolDown)
    {
        CastFireBall();
    }
    else
    {
        KeepTrackPlayer();
    }
}

void AWarLock::Init()
{
    FVector V = GetActorLocation();
    V.Y += 100;
    BossRoomEntranceLocation = V;

    EnemyData->IncreaseMaxHealth(600);
    UpdateHealthbar();
    NextTeleportHp = EnemyData->GetCurrentHealth() - TeleportThreshold;
    NextSpawnMinionHp = EnemyData->GetCurrentHealth() - SpawnMinionThreshold;

    TArray<int32> DamageDice;
    DamageDice.Init(DamageDieFaces, DamageDiceRolled);
    DiceBag->SetDice(DamageDice);

    FCollisionShape Cube = FCollisionShape::MakeBox(FVector{100,100,100});
    TArray<FHitResult> OutResults;
    FCollisionQueryParams TraceParams;
    TraceParams.AddIgnoredActor(this);
    GetWorld()->SweepMultiByChannel(OutResults, GetActorLocation(), GetActorLocation() - FVector{0,0,200}
    , FQuat::Identity, ECC_Visibility, Cube, TraceParams);

    for (auto Hit : OutResults)
    {
        auto* P = Cast<ATileBase>(Hit.Actor);
        if (P)
        {
            if (RoomTiles.Find(P) == INDEX_NONE)
            {
                RoomTiles.Add(P);
                UE_LOG(LogTemp, Display, TEXT("Adding Tile at (%f,%f)"), P->GetActorLocation().X, P->GetActorLocation().Y);
            }
        }
    }

    for (auto* P : RoomTiles)
    {
        int A = P->GetActorLocation().X;
        int B = P->GetActorLocation().Y;
        int C = GetActorLocation().X;
        int D = GetActorLocation().Y;

        if ((A - C) * (A - C) + (B - D) * (B - D) > 10000)
        {
            RoomCorners.Add(P);
            UE_LOG(LogTemp, Display, TEXT("Adding Corner at (%f,%f)"), P->GetActorLocation().X, P->GetActorLocation().Y);
        }
    }
}
void AWarLock::StandBy()
{
    FHitResult Hit;
    LineTraceFront(Hit,300);
    auto* p = Cast<AProjectRogueCharacter>(Hit.Actor);
    if (p)
    {
        bFoundPlayer = true;
        Player = p;
    }
}

void AWarLock::KeepTrackPlayer()
{
    if (GetDistanceTo(Player) < 500.f)
    {
        auto Rot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Player->GetActorLocation());
        SetActorRotation(Rot);
    }
    else
    {
        bFoundPlayer = false;
        Player = nullptr;
        auto EntranceLocation = BossRoomEntranceLocation;
        EntranceLocation.Z = GetActorLocation().Z;
        auto Rot2 = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), EntranceLocation);
        SetActorRotation(Rot2);
    }
}

void AWarLock::CastFireBall()
{
    //spawn fireball
    FVector BallLocation{ GetActorLocation() };
    BallLocation += GetActorForwardVector()*50;
    FRotator BallRot{};
    FActorSpawnParameters Params;
    Params.bNoFail = true;
    CurrentFireBall = GetWorld()->SpawnActor<AFireBall>(FireBall, BallLocation,BallRot, Params);
    FireBallTarget = Player->GetActorLocation();
    GetWorld()->GetTimerManager().SetTimer(FireballTimer, [this]{Fire(); }, 1,false, ChannelTime);

    bOnCoolDown = true;
}

void AWarLock::Fire()
{
    if (CurrentFireBall)
    {
        CurrentFireBall->FireAt(FireBallTarget, DiceBag->Roll());
        CurrentFireBall = nullptr;

        GetWorld()->GetTimerManager().SetTimer(FireballTimer, [this] {bOnCoolDown = false; }, 1, false, CoolDown);
    }
}

void AWarLock::Teleport()
{
    TArray<ATileBase*> EmptyTile;
    for (auto& Tile : RoomCorners)
    {
        if(Tile->GetActorOnTile()==nullptr)
        EmptyTile.Add(Tile);
    }
    auto id = UKismetMathLibrary::RandomIntegerInRange(0, EmptyTile.Num()-1);
    FVector TargetPos = EmptyTile[id]->GetActorLocation();
    TargetPos.Z += 50;

    LineTraceDown(HitResult);
    ATileBase* Tile = Cast<ATileBase>(HitResult.Actor);
    check(Tile);
    Tile->SetActorOnTile(nullptr);

    SetActorLocation(TargetPos);

    LineTraceDown(HitResult);
    Tile = Cast<ATileBase>(HitResult.Actor);
    check(Tile);
    Tile->SetActorOnTile(this);

    auto Rot = UKismetMathLibrary::FindLookAtRotation(TargetPos, Player->GetActorLocation());
    SetActorRotation(Rot);

    if (CurrentFireBall)
    {
        if (!CurrentFireBall->IsMoving())
        {
            CurrentFireBall->SetActorLocation(GetActorLocation()+GetActorForwardVector() * 50);
        }
    }
}

void AWarLock::SpawnMinion()
{
    UE_LOG(LogTemp, Display, TEXT("Warlock spawning minion!"));
    TArray<ATileBase*> EmptyTile;
    for (auto& Tile : RoomTiles)
    {
        if(Tile->GetActorOnTile() == nullptr)
            EmptyTile.Add(Tile);
    }
    auto Id = UKismetMathLibrary::RandomIntegerInRange(0, EmptyTile.Num() - 1);
    FVector TargetPos = EmptyTile[Id]->GetActorLocation();
    TargetPos.Z += 50;

    // spawn monster at targetPos
    AMonsterBase* Monster = GetWorld()->SpawnActor<AMonsterBase>(Minion, TargetPos, FRotator{});
    //prevent this monster from running death callback
    Monster->SetIsInRoom();
}

void AWarLock::OnTakeDamage()
{
    UpdateHealthbar();
    int32 Health = EnemyData->GetCurrentHealth();

    if (Health <= 0)
    {
        if (CurrentFireBall)
        {
            CurrentFireBall->Destroy();
            CurrentFireBall = nullptr;
        }
        Player->MonsterDied(this);
        GetWorld()->GetTimerManager().ClearTimer(FireballTimer);
        Destroy();
        return;
    }
    else
    {
        ShowHealthbar();
    }

    if (Health <= NextTeleportHp)
    {
        NextTeleportHp -= TeleportThreshold;
        Teleport();
    }

    if (Health <= NextSpawnMinionHp)
    {
        NextSpawnMinionHp -= SpawnMinionThreshold;
        SpawnMinion();
    }

    if (!bFoundPlayer)
    {
        TurnToPlayer(Player);
        bFoundPlayer = true;
    }
}
