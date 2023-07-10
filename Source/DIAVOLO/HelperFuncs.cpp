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
