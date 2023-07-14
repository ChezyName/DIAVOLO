// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HelperFuncs.generated.h"

/**
 * 
 */
UCLASS()
class DIAVOLO_API UHelperFuncs : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, Category = "HelperClass")
	static void SetPlayerName(APlayerController *PlayerController, FString PlayerName);

	UFUNCTION(BlueprintCallable, Category = "HelperClass")
	static void SetPlayerNameC(AController *Controller, FString PlayerName);

	UFUNCTION(BlueprintCallable, Category = "HelperClass")
	static FString GetPlayerName(APlayerController *PlayerController);

	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject", AdvancedDisplay = "2", DisplayName = "ServerTravel"), Category="Game")
	static void SeamlessTravel(const UObject* WorldContextObject,const TSoftObjectPtr<UWorld> Level);
};
