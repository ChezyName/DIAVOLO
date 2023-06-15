// Copyright Epic Games, Inc. All Rights Reserved.

#include "DIAVOLOGameMode.h"
#include "DIAVOLOPlayerController.h"
#include "DIAVOLOCharacter.h"
#include "DrawDebugHelpers.h"
#include "Algo/RandomShuffle.h"
#include "Kismet/KismetArrayLibrary.h"
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
		//CreateRoom(Start,FVector::ZeroVector,FRotator::ZeroRotator,0,0);
		CreateRoomGrid(Start,FVector::ZeroVector,FRotator::ZeroRotator,0,0,0);
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

bool ADIAVOLOGameMode::CanPlaceRoom(int X, int Y)
{
	for(int i = 0; i < TileMap.Num(); i++)
	{
		if(TileMap[i].X == X && TileMap[i].Y == Y)
		{
			/*
			GEngine->AddOnScreenDebugMessage(-1,25,FColor::Red,"ERR: "+
				FString::SanitizeFloat(TileMap[i].X) + "," + FString::SanitizeFloat(TileMap[i].Y) + " / " +
				FString::SanitizeFloat(X) + "," + FString::SanitizeFloat(Y));
			*/
			return false;
		}
	}
	return true;
}

//GRID BASED MAP GEN
void ADIAVOLOGameMode::CreateRoomGrid(TSubclassOf<ARoom> Room, FVector Location, FRotator Rotation, int X, int Y,int Dist)
{
	UE_LOG(LogTemp, Warning, TEXT("(%s,%s)"), *FString::SanitizeFloat(X), *FString::SanitizeFloat(Y));
	ARoom* NewRoom = GetWorld()->SpawnActor<ARoom>(Room,Location,Rotation);
	FGridItem NewGridItem;
	NewGridItem.Room = NewRoom;
	NewGridItem.X = X;
	NewGridItem.Y = Y;
	TileMap.Add(NewGridItem);
	Dist++;
	CRooms++;

	/*
	GEngine->AddOnScreenDebugMessage(-1,3,FColor::Cyan,"PLACED Room At ("+
			FString::SanitizeFloat(X) + "," + FString::SanitizeFloat(Y) + "). Creating " +
			FString::SanitizeFloat(NewRoom->Exits.Num()) + " Rooms.");
	*/

	UArrowComponent* Exit = NewRoom->Exits[FMath::RandRange(0,NewRoom->Exits.Num()-1)];
	
	//X = FORWARD / BACKWARD (1,-1)
	//Y = RIGHT / LEFT (-1,1)
	float DirectionX = Exit->GetForwardVector().X + X;
	float DirectionY = Exit->GetForwardVector().Y + Y;

	/*
	GEngine->AddOnScreenDebugMessage(-1,8,FColor::Yellow,"TESTING Placing Room At ("+
		FString::SanitizeFloat(DirectionX) + "," + FString::SanitizeFloat(DirectionY) + ").");
	*/
	
	//CanPlaceRoom(DirectionX,DirectionY) == true
	if(Dist < 15)
	{
		//GEngine->AddOnScreenDebugMessage(-1,3,FColor::Green,"CREATED ROOM!");
		CreateRoomGrid(GridBasedRooms[FMath::RandRange(0,GridBasedRooms.Num()-1)],
			Exit->GetComponentLocation(),Exit->GetComponentRotation(),
			DirectionX,DirectionY,Dist);
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1,3,FColor::Red,"NO ROOM MADE!");
		DrawDebugSphere(GetWorld(),Exit->GetComponentLocation(),25,32,FColor::Red,false,80,0,1.25);
	}
}