// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DIAVOLOPlayerController.generated.h"

UCLASS()
class ADIAVOLOPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADIAVOLOPlayerController();

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface
	
	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();
	
	/** Navigate player to the given world location. */
	void SetNewMoveDestination(const FVector DestLocation);

	UFUNCTION(Client,Unreliable)
	void ClientMove(FVector NewLoc);
	UFUNCTION(Server,Unreliable)
	void ServerMove(FVector NewLoc);

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();
};


