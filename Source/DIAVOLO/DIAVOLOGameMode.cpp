// Copyright Epic Games, Inc. All Rights Reserved.

#include "DIAVOLOGameMode.h"
#include "DIAVOLOPlayerController.h"
#include "DIAVOLOCharacter.h"
#include "Kismet/KismetMathLibrary.h"
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
		CreateRoom(Start,FVector::ZeroVector,FRotator::ZeroRotator,0,0);
	}
}

void ADIAVOLOGameMode::CreateRoom(TSubclassOf<ARoom> Room, FVector Locaiton, FRotator Rotation,int currentCount,int sinceLastSpawn)
{
	currentCount++;
	const ARoom* NewRoom = GetWorld()->SpawnActor<ARoom>(Room,Locaiton,Rotation);
	UE_LOG(LogTemp, Display, TEXT("%s"), *NewRoom->GetName());
	for(int i = 0; i < NewRoom->Exits.Num(); i++)
	{
		//END
		if(currentCount > MaxRooms)
		{
			//END ROOM IE END -> TELEPORT or END / END
			CreateRoom(HasEnd ? End : EndWTeleport,NewRoom->Exits[i]->GetComponentLocation(),NewRoom->Exits[i]->GetComponentRotation(),currentCount,sinceLastSpawn);
			HasEnd = true;
		}
		//SPAWNERS / HALLS
		else
		{
			//SPAWNERS
			if(UKismetMathLibrary::RandomBool())
			{
				//Spawner / Large Room
				CreateRoom(GetRandomSpawn(),NewRoom->Exits[i]->GetComponentLocation(),NewRoom->Exits[i]->GetComponentRotation(),currentCount,sinceLastSpawn);
			}
			//HALL
			else
			{
				sinceLastSpawn++;
				//END
				if(currentCount > 1 && UKismetMathLibrary::RandomBool())
				{
					CreateRoom(HasEnd ? End : EndWTeleport,NewRoom->Exits[i]->GetComponentLocation(),NewRoom->Exits[i]->GetComponentRotation(),currentCount,sinceLastSpawn);
					HasEnd = true;
				}
				else
				{
					CreateRoom(GetRandomHall(),NewRoom->Exits[i]->GetComponentLocation(),NewRoom->Exits[i]->GetComponentRotation(),currentCount,sinceLastSpawn);
					/*
					if(UKismetMathLibrary::RandomBool())
					{
						CreateRoom(GetRandomTurn(),NewRoom->Exits[i]->GetComponentLocation(),NewRoom->Exits[i]->GetComponentRotation(),currentCount,sinceLastSpawn);
					}
					else
					{
						CreateRoom(GetRandomHall(),NewRoom->Exits[i]->GetComponentLocation(),NewRoom->Exits[i]->GetComponentRotation(),currentCount,sinceLastSpawn);
					}
					*/
				}
			}
		}
	}
}

TSubclassOf<ARoom> ADIAVOLOGameMode::GetRandomTurn()
{
	return Turns[FMath::RandRange(0,Turns.Num()-1)];
}

TSubclassOf<ARoom> ADIAVOLOGameMode::GetRandomHall()
{
	return Halls[FMath::RandRange(0,Halls.Num()-1)];
}

TSubclassOf<ARoom> ADIAVOLOGameMode::GetRandomSpawn()
{
	return Spawners[FMath::RandRange(0,Spawners.Num()-1)];
}
