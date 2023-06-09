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
		UE_LOG(LogTemp, Display, TEXT("Creating Map..."));
		CreateRoom(Start,FVector::ZeroVector,FRotator::ZeroRotator);
	}
}

void ADIAVOLOGameMode::CreateRoom(TSubclassOf<ARoom> Room, FVector Locaiton, FRotator Rotation)
{
	RoomCount++;
	if(RoomCount > MaxRooms) return;
	const ARoom* NewRoom = GetWorld()->SpawnActor<ARoom>(Room,Locaiton,Rotation);
	UE_LOG(LogTemp, Display, TEXT("%s"), *NewRoom->GetName());
	for(int i = 0; i < NewRoom->Exits.Num(); i++)
	{
		CreateRoom(GetRandomRoom(),NewRoom->Exits[i]->GetComponentLocation(),NewRoom->Exits[i]->GetComponentRotation());
	}
}

TSubclassOf<ARoom> ADIAVOLOGameMode::GetRandomRoom()
{
	return RoomGeneration[FMath::RandRange(0,RoomGeneration.Num()-1)];
}
