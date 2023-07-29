// Fill out your copyright notice in the Description page of Project Settings.


#include "HelperFuncs.h"

#include "GameFramework/PlayerState.h"

void UHelperFuncs::SetPlayerName(APlayerController* PlayerController, FString PlayerName)
{
	if (PlayerController != nullptr && PlayerController->PlayerState != nullptr)
	{
		PlayerController->PlayerState->SetPlayerName(PlayerName);
	}
}

void UHelperFuncs::SetPlayerNameC(AController* Controller, FString PlayerName)
{
	if(Controller != nullptr && Controller->PlayerState != nullptr)
	{
		Controller->PlayerState->SetPlayerName(PlayerName);
	}
}

FString UHelperFuncs::GetPlayerName(APlayerController* PlayerController)
{
	if (PlayerController != nullptr && PlayerController->PlayerState != nullptr)
	{
		return PlayerController->PlayerState->GetPlayerName();
	}
	return FString("");
}

void UHelperFuncs::SeamlessTravel(const UObject* WorldContextObject,const TSoftObjectPtr<UWorld> Level)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	const FString LevelName = FString(*FPackageName::ObjectPathToPackageName(Level.ToString()));
	if (World == nullptr)
	{
		return;
	}

	if(World->IsServer())
	{
		World->ServerTravel(LevelName,false,false);
		//GEngine->AddOnScreenDebugMessage(-1,25,FColor::Magenta,"Server Travel C++!");
	}
}

FRotator UHelperFuncs::GetRandomRotator()
{
	float pitch = FMath::RandRange(-180.0f, 180.0f);
	float yaw = FMath::RandRange(-180.0f, 180.0f);
	float roll = FMath::RandRange(-180.0f, 180.0f);

	return FRotator(pitch, yaw, roll);
}
