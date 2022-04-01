// Fill out your copyright notice in the Description page of Project Settings.


#include "TileBase.h"
#include "Components/BoxComponent.h"

// Sets default values
ATileBase::ATileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultScentRoot"));
	DefaultSceneRoot->SetMobility(EComponentMobility::Static);
	//the root is a scene component so that the relative position of the tile can be modified
	// in case it needs to be raised or lowered
	SetRootComponent(DefaultSceneRoot);	

	//create static mesh that will be used as the tile floor
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(DefaultSceneRoot);
	StaticMesh->SetRelativeLocation(FVector(0.f, 0.f, -50.f));
	StaticMesh->SetMobility(EComponentMobility::Static);

	//create the collision box that will handle line trace collision for objects interacting
	// between tiles
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionDetection"));
	CollisionBox->SetupAttachment(DefaultSceneRoot);
	CollisionBox->SetRelativeLocation(FVector(0.f, 0.f, 30.f));
	CollisionBox->SetBoxExtent(FVector(32.f, 32.f, 60.f));
	CollisionBox->SetMobility(EComponentMobility::Static);

	//create a helper component to keep track of the tile center
	TileCenter = CreateDefaultSubobject<USceneComponent>(TEXT("TileCenter"));
	TileCenter->SetupAttachment(StaticMesh);
	TileCenter->SetRelativeLocation(FVector(0.f, 0.f, 60.f));

	//all components of the tile are static
	DefaultSceneRoot->SetMobility(EComponentMobility::Static);
	StaticMesh->SetMobility(EComponentMobility::Static);
	CollisionBox->SetMobility(EComponentMobility::Static);
	TileCenter->SetMobility(EComponentMobility::Static);
}

// Called when the game starts or when spawned
void ATileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATileBase::NotifyActorBeginOverlap(AActor* OtherActor)
{
	//keep track of any objects that enters this tile
	ActorOnTile = OtherActor;
}

void ATileBase::NotifyActorEndOverlap(AActor* OtherActor)
{
	//if the object leaving is the object we're keeping track of
	if (OtherActor == ActorOnTile)
	{
		//do not keep track of objects that are no longer on this tile
		ActorOnTile = nullptr;
	}
}

// Called every frame
void ATileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ATileBase::GetCenterOfTile() const
{
	return TileCenter->GetComponentLocation();
}