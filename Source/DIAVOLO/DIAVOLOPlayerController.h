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

	UPROPERTY(Replicated)
	AEnemy* EnemyAttacking;

	UPROPERTY(Replicated)
	FVector MousePosition;
	
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

	UFUNCTION(Client,Reliable)
	void onSkill1C();
	UFUNCTION(Client,Reliable)
	void onSkill2C();
	UFUNCTION(Client,Reliable)
	void onSkill3C();
	UFUNCTION(Client,Reliable)
	void onUltimateC();

	UFUNCTION(Server,Reliable)
	void onSkill1S(FVector MouseLoc,AEnemy* Enemy);
	UFUNCTION(Server,Reliable)
	void onSkill2S(FVector MouseLoc,AEnemy* Enemy);
	UFUNCTION(Server,Reliable)
	void onSkill3S(FVector MouseLoc,AEnemy* Enemy);
	UFUNCTION(Server,Reliable)
	void onUltimateS(FVector MouseLoc,AEnemy* Enemy);

	UFUNCTION(Client,Reliable)
	void endSkill1C();
	UFUNCTION(Client,Reliable)
	void endSkill2C();
	UFUNCTION(Client,Reliable)
	void endSkill3C();
	UFUNCTION(Client,Reliable)
	void endUltimateC();

	UFUNCTION(Server,Reliable)
	void endSkill1();
	UFUNCTION(Server,Reliable)
	void endSkill2();
	UFUNCTION(Server,Reliable)
	void endSkill3();
	UFUNCTION(Server,Reliable)
	void endUltimate();

	ACharacterProxy* GetProxy();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

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


