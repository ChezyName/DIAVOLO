// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "RealGamemode.generated.h"

/**
 * 
 */
UCLASS()
class DIAVOLO_API ARealGamemode : public AGameMode
{
	GENERATED_BODY()
public:
	void onCharacterDeath();
	
private:
	void ResetGame();

	//virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	//UClass* GetDefaultPawnClassForController(AController* InController) override;
	//virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
};
