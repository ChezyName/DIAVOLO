// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Room.h"
#include "GameFramework/GameModeBase.h"
#include "DIAVOLOGameMode.generated.h"

USTRUCT()
struct FGridItem
{
public:
	GENERATED_BODY()
	int X = 0;
	int Y = 0;
	UPROPERTY()
	ARoom* Room;
};


UCLASS(minimalapi)
class ADIAVOLOGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADIAVOLOGameMode();
	virtual void BeginPlay() override;

	void GenerateDungeon();
	void CreateRoom(TSubclassOf<ARoom> Room,FVector Locaiton,FRotator Rotation,int currentCount,int sinceLastSpawn);
	TSubclassOf<ARoom> GetRandomSpawn();
	TSubclassOf<ARoom> GetRandomTurn();
	TSubclassOf<ARoom> GetRandomHall();
	
	int MaxRooms = 3;
	int SpawnerSpawnPercent = 3;
	bool HasEnd = false;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<ARoom>> Halls;
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<ARoom>> Turns;
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<ARoom>> Spawners;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ARoom> End;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ARoom> EndWTeleport;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ARoom> Start;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<ARoom>> GridBasedRooms;

	bool CanPlaceRoom(int X, int Y);
	void CreateRoomGrid(TSubclassOf<ARoom> Room, FVector Location, FRotator Rotation, int X, int Y);
	TArray<FGridItem> TileMap;
};



