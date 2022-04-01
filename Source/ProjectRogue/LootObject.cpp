// Fill out your copyright notice in the Description page of Project Settings.


#include "LootObject.h"

// Sets default values
ALootObject::ALootObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	//SetRootComponent(DefaultSceneRoot);
	//StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	//StaticMesh->SetupAttachment(DefaultSceneRoot);
}

void ALootObject::Init(TArray<UItem*> InItems)
{
	Items = InItems;
}