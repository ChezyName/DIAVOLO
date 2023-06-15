// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DiavoloPS.h"
#include "Enemy.h"
#include "GameFramework/PlayerController.h"
#include "DIAVOLOPlayerController.generated.h"

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

	//Auto Attack
	bool WindUpCanceled;
protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	ADiavoloPS* GetState();
	EPlayerStates GetCharState();

	UPROPERTY()
	class ACharacterProxy* Proxy;

	FVector newMoveToLocation = FVector::ZeroVector;
	AEnemy* EnemyAttacking;
	bool CloseEnough();

	UFUNCTION(Server,Reliable)
	void setEnemy(AEnemy* Enemy);
	
	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	
	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();
	
	/** Navigate player to the given world location. */
	UFUNCTION(Server,Reliable)
	void SetNewMoveDestination(const FVector DestLocation);

	ACharacterProxy* GetProxy();

	//Basic Move To Location
	UFUNCTION(Server,Unreliable)
	void ServerMove(FVector NewLoc);
	
	//Move To Location / Attack After
	UFUNCTION(Server,Unreliable)
	void ServerAttackMove(FVector NewLoc,float Range);

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();

	UFUNCTION(Server,Reliable)
	void ServerUpdateState();
	
	UFUNCTION(Server,Reliable)
	void DoAutoAttack();

	UFUNCTION(Server,Reliable)
	void ChangeCharState(EPlayerStates NewState);
};


