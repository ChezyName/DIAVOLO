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

	UPROPERTY(EditDefaultsOnly,Category="Class Info")
	TSubclassOf<APawn> SpectatorClass;

	UFUNCTION(Server,Reliable)
	void onDeath();

	//Auto Attack
	bool WindUpCanceled;

	UPROPERTY(BlueprintReadWrite,Replicated)
	bool bController = false;
protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	ADiavoloPS* GetState();
	EPlayerStates GetCharState();

	UPROPERTY()
	class ACharacterProxy* Proxy;
	
	UPROPERTY(Replicated,BlueprintReadOnly)
	class ADIAVOLOCharacter* SpawnedCharacter;
	
	FVector newMoveToLocation = FVector::ZeroVector;

	UPROPERTY(Replicated)
	AEnemy* EnemyAttacking;

	UPROPERTY(Replicated)
	AEnemy* BossCharacter;

	UPROPERTY(Replicated)
	FVector MousePosition;

	FVector MouseLookDir = FVector::ForwardVector;

	UFUNCTION(Server,Reliable)
	void onStartSetChar();
	
	void LookAtMouse();

	FVector2D ControllerAimDir;
	void LookAtController();

	void SetControllerX(float X);
	void SetControllerY(float Y);
	
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
	void SetUpMovementC(float Value);
	UFUNCTION(Client,Reliable)
	void SetRightMovementC(float Value);
	UFUNCTION(Server,Reliable)
	void SetUpMovementS(float Value);
	UFUNCTION(Server,Reliable)
	void SetRightMovementS(float Value);

	UFUNCTION(Client,Reliable)
	void onSkill1C();
	UFUNCTION(Client,Reliable)
	void onSkill2C();
	UFUNCTION(Client,Reliable)
	void onSkill3C();
	UFUNCTION(Client,Reliable)
	void onUltimateC();
	UFUNCTION(Client,Reliable)
	void onDodgeC();

	UFUNCTION(Server,Reliable)
	void onSkill1S(FVector MouseLoc,AEnemy* Enemy);
	UFUNCTION(Server,Reliable)
	void onSkill2S(FVector MouseLoc,AEnemy* Enemy);
	UFUNCTION(Server,Reliable)
	void onSkill3S(FVector MouseLoc,AEnemy* Enemy);
	UFUNCTION(Server,Reliable)
	void onUltimateS(FVector MouseLoc,AEnemy* Enemy);
	UFUNCTION(Server,Reliable)
	void onDodgeS(FVector MouseLoc);

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

	UFUNCTION(Reliable,Client)
	void startEmoteC();
	UFUNCTION(Reliable,Server)
	void startEmoteS();
	UFUNCTION(Reliable,Client)
	void endEmoteC();
	UFUNCTION(Reliable,Server)
	void endEmoteS();
	

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


