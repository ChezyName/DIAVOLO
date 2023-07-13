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
