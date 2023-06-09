// Fill out your copyright notice in the Description page of Project Settings.


#include "Room.h"

#include "Components/ArrowComponent.h"

// Sets default values
ARoom::ARoom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Entrence = CreateDefaultSubobject<UArrowComponent>("Entrence");
	RootComponent = Entrence;

	bReplicates = true;

	TArray<UArrowComponent*> AllArrows;
	GetComponents<UArrowComponent>(AllArrows);
	for(int i = 0; i < AllArrows.Num(); i++)
	{
		if(AllArrows[i] != Entrence)
		{
			Exits.Add(AllArrows[i]);
		}
	}
}

// Called when the game starts or when spawned
void ARoom::BeginPlay()
{
	Super::BeginPlay();
	TArray<UArrowComponent*> AllArrows;
	GetComponents<UArrowComponent>(AllArrows);
	for(int i = 0; i < AllArrows.Num(); i++)
	{
		if(AllArrows[i] != Entrence)
		{
			Exits.Add(AllArrows[i]);
		}
	}
}

// Called every frame
void ARoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

