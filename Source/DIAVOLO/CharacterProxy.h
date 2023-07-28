// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "CharacterProxy.generated.h"

UCLASS()
class ACharacterProxy : public APawn
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,Category="CameraZoom")
	float ZoomSpeed = 50;
	UPROPERTY(EditAnywhere,Category="CameraZoom")
	float ZoomMin = 500;
	UPROPERTY(EditAnywhere,Category="CameraZoom")
	float ZoomMax = 800;
	
	// Sets default values for this character's properties
	ACharacterProxy(const FObjectInitializer& ObjectInitializer);

	// Needed so we can pick up the class in the constructor and spawn it elsewhere
	UPROPERTY(EditDefaultsOnly,Category="Class Info")
	TSubclassOf<AActor> CharacterClass;

	UPROPERTY(EditDefaultsOnly,Category="Class Info")
	TSubclassOf<APawn> SpectatorClass;

	// pointer to actual character
	UPROPERTY(Replicated,BlueprintReadOnly)
	class ADIAVOLOCharacter* Character;

	// used by controller to get the moving to work
	void MoveToLocation(const FVector& DestLocation);

	UFUNCTION(Server,Reliable)
	void onCharacterDeath();

	UPROPERTY(BlueprintReadOnly)
	bool Died = false;

	UFUNCTION(Client,Reliable)
	void DestroyMyHUD();

	UFUNCTION(BlueprintNativeEvent)
	void DestroyMyHUDClient();
	
protected:
	// AIController we will use
	class AAIController* PlayerAIController;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void ZoomCamera(float Speed);

	UFUNCTION(Server,Reliable)
	void onStartSetChar(AController* NewController);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void Destroyed() override;

	bool SpawnedNew = false;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// top down camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* TopDownCameraComponent;

	// camera boom
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* CameraBoom;
};
