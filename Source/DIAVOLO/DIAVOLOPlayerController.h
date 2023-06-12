// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DIAVOLOCharacter.h"
#include "Enemy.h"
#include "GameFramework/PlayerController.h"
#include "DIAVOLOPlayerController.generated.h"

UENUM(BlueprintType)
enum class EPlayerStates : uint8
{
	E_IDLE = 0 UMETA(DisplayName="Moving"),
	E_MOVE = 1 UMETA(DisplayName="Moving"),
	E_MOVE_ATTACK = 2 UMETA(DisplayName="Moving For Attack"),
	E_ATTACK_WINDUP = 3 UMETA(DisplayName="Charging Auto Attack"),
	E_ATTACK_FULL = 3 UMETA(DisplayName="After Attack Hit"),
	E_ABILITY = 4 UMETA(DisplayName="Ability"),
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

	//Auto Attack
	bool WindUpCanceled;

	UFUNCTION(Server,Reliable)
	void ChangeCharState(EPlayerStates newState);

	UFUNCTION(BlueprintGetter)
	EPlayerStates getCharState();
protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category="Player State")
	EPlayerStates CharState = EPlayerStates::E_IDLE;

	UPROPERTY()
	ADIAVOLOCharacter* CharacterClass;
	
	FVector newMoveToLocation = FVector::ZeroVector;
	AEnemy* EnemyAttacking;
	bool CloseEnough();
	
	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	
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

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void AutoAttack();
};


