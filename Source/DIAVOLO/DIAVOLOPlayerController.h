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

	UPROPERTY(BlueprintReadWrite)
	bool bController = false;

	void IsKeyboard();

	UFUNCTION(BlueprintCallable)
	void MoveUp(float Speed);
	UFUNCTION(BlueprintCallable)
	void MoveRight(float Speed);
	UFUNCTION(BlueprintCallable)
	void ZoomCamera(float Speed);

	UPROPERTY(Replicated,BlueprintReadOnly)
	class ADIAVOLOCharacter* SpawnedCharacter;

	virtual void OnPossess(APawn* InPawn) override;
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
	AEnemy* BossCharacter;

	UPROPERTY(Replicated)
	FVector MousePosition;

	FVector MouseLookDir = FVector::ForwardVector;

	UFUNCTION(Server,Unreliable)
	void onStartSetChar();
	
	void LookAtMouse();

	FVector2D ControllerAimDir;

	UFUNCTION(Server,Unreliable)
	void SetLookAtRot(FRotator NewRot);
	
	void LookAtController();
	void LookAtBoss();

	void SetControllerX(float X);
	void SetControllerY(float Y);
	
	bool CloseEnough();

	UFUNCTION(Server,Unreliable)
	void setEnemy(AEnemy* Enemy);
	
	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	
	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();
	
	/** Navigate player to the given world location. */
	UFUNCTION(Server,Unreliable)
	void SetNewMoveDestination(const FVector DestLocation);

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

	UFUNCTION(Unreliable,Client)
	void startEmoteC();
	UFUNCTION(Unreliable,Server)
	void startEmoteS();
	UFUNCTION(Unreliable,Client)
	void endEmoteC();
	UFUNCTION(Unreliable,Server)
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

	UFUNCTION(Server,Unreliable)
	void ServerUpdateState();
	
	UFUNCTION(Server,Unreliable)
	void DoAutoAttack();

	UFUNCTION(Server,Unreliable)
	void ChangeCharState(EPlayerStates NewState);
};


