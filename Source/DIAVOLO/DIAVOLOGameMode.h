// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Room.h"
#include "GameFramework/GameModeBase.h"
#include "DIAVOLOGameMode.generated.h"

UCLASS(minimalapi)
class ADIAVOLOGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADIAVOLOGameMode();
	virtual void BeginPlay() override;

	void GenerateDungeon();
	void CreateRoom(TSubclassOf<ARoom> Room,FVector Locaiton,FRotator Rotation);

	int RoomCount = 0;
	int MaxRooms = 15;
	
	TSubclassOf<ARoom> GetRandmRoom();

	int Seed = 123456789;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<ARoom>> RoomGeneration;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ARoom> End;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ARoom> Start;
};



