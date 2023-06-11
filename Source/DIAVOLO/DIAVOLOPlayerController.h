// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DIAVOLOPlayerController.generated.h"

UENUM()
enum EPlayerStates
{
	E_MOVE = 0 UMETA(DisplayName="Moving"),
	E_AUTOATTACK = 1 UMETA(DisplayName="Auto Attack")
};

UCLASS()
class ADIAVOLOPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADIAVOLOPlayerController();
	
	//Client ONLY FUNC
	FVector getMousePosition();
	FVector getMousePositionGround();
	FVector getMousePositionEnemy();
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

	//Basic Move To Location
	UFUNCTION(Client,Unreliable)
	void ClientMove(FVector NewLoc);
	UFUNCTION(Server,Unreliable)
	void ServerMove(FVector NewLoc);
	//Move To Location / Attack After
	UFUNCTION(Client,Unreliable)
	void ClientAttackMove(FVector NewLoc,float Range);
	UFUNCTION(Server,Unreliable)
	void ServerAttackMove(FVector NewLoc,float Range);

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();
};


