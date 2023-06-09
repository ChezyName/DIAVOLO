// Copyright Epic Games, Inc. All Rights Reserved.

#include "DIAVOLOGameMode.h"
#include "DIAVOLOPlayerController.h"
#include "DIAVOLOCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADIAVOLOGameMode::ADIAVOLOGameMode(){
}

void ADIAVOLOGameMode::BeginPlay()
{
	Super::BeginPlay();

	//Start Dungeon Generation Using SEED
	GenerateDungeon();

}

void ADIAVOLOGameMode::GenerateDungeon()
{
	if(HasAuthority() && IsValid(GetWorld()))
	{
		CreateRoom(Start,FVector::ZeroVector,FRotator::ZeroRotator);
	}
}

void ADIAVOLOGameMode::CreateRoom(TSubclassOf<ARoom> Room, FVector Locaiton, FRotator Rotation)
{
	RoomCount++;
	if(RoomCount > MaxRooms) return;
	const ARoom* NewRoom = GetWorld()->SpawnActor<ARoom>(Start,Locaiton,Rotation);
	for(int i = 0; i < NewRoom->Exits.Num(); i++)
	{
		CreateRoom(Start,NewRoom->Exits[i]->GetComponentLocation(),NewRoom->Exits[i]->GetComponentRotation());
	}
}

TSubclassOf<ARoom> ADIAVOLOGameMode::GetRandmRoom()
{
	return RoomGeneration[FMath::RandRange(0,RoomGeneration.Num())];
}
