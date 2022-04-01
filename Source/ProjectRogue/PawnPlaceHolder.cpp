// Fill out your copyright notice in the Description page of Project Settings.


#include "PawnPlaceHolder.h"

// Sets default values
APawnPlaceHolder::APawnPlaceHolder()
	: PlaceHolderBox{nullptr}
	, TestBox{nullptr}
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PlaceHolderBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PlaceHolder"));
	PlaceHolderBox->InitBoxExtent(FVector(50, 50, 50));
	//PlaceHolderBox->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	PlaceHolderBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PlaceHolderBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	RootComponent = PlaceHolderBox;

	//uncomment this to see the place holding cube
	/*TestBox = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube"));
	UStaticMesh* cubeMesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'")).Object;
	TestBox->SetStaticMesh(cubeMesh);
	TestBox->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	TestBox->AttachTo(RootComponent);
	TestBox->SetMobility(EComponentMobility::Static);*/

	RootComponent->SetMobility(EComponentMobility::Static);
}




