// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DIAVOLO/DIAVOLOCharacter.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "Niagara/Public/NiagaraFunctionLibrary.h"
#include "Char_BEAST.generated.h"

/**
 * 
 */
UCLASS()
class DIAVOLO_API AChar_BEAST : public ADIAVOLOCharacter
{
	GENERATED_BODY()
	
public:

	// SKILL 1
	UPROPERTY(EditAnywhere, Category = "Character Info | [Q] Claw // Teleport")
	TSubclassOf<class ABaseProjectile> ClawProjectile;

	UPROPERTY(EditAnywhere, Category = "Character Info | [Q] Claw // Teleport")
	UAnimMontage* ClawAnimation;

	UPROPERTY(EditAnywhere, Category = "Character Info | [Q] Claw // Teleport")
	UNiagaraSystem* TeleportFX;

	UPROPERTY(EditAnywhere, Category = "Character Info | [Q] Claw // Teleport")
	float ClawSpawnDelayAnimation = 0;

	UPROPERTY(EditAnywhere, Category = "Character Info | [Q] Claw // Teleport")
	float ClawDamage = 600;

	UPROPERTY(EditAnywhere, Category = "Character Info | [Q] Claw // Teleport")
	float ClawVelocity = 6;

	UPROPERTY(EditAnywhere, Category = "Character Info | [Q] Claw // Teleport")
	float ClawTeleportDelay = .6;

	// SKILL 2

	// SKILL 3
	UPROPERTY(EditAnywhere, Category = "Character Info | [E] Grapple")
	TSubclassOf<class ACallBackProjectile> GrappleProjectile;

	UPROPERTY(EditAnywhere, Category = "Character Info | [E] Grapple")
	float GrappleLifetime = 3;

	UPROPERTY(EditAnywhere, Category = "Character Info | [E] Grapple")
	float GrappleVelocity = 18;
	
	UPROPERTY(EditAnywhere, Category = "Character Info | [E] Grapple")
	float GrappleSpeed = 125;

	UPROPERTY(EditAnywhere, Category = "Character Info | [E] Grapple")
	float GrappleCloseEnough = 64;
	
private:
	//Skill 1 : Q
	UPROPERTY()
	ABaseProjectile* Claw = nullptr;
	bool bSwapped = false;
	bool bDoing = false;
	float TPDelay = 0;
	FRotator LookAtRotation;

	//Skill 3 : E
	UPROPERTY()
	ACallBackProjectile* Grapple = nullptr;
	UFUNCTION()
	void onGrappleHit(FVector HitImpact, AEnemy* EnemyHit);
	bool Grappling = false;
	FVector toLoc = FVector::ZeroVector;
	
	float GrappleOutT = 0;
	bool GrappleOut = false;

	UFUNCTION(NetMulticast,Reliable)
	void PlayClawTeleportFX(FVector Location);

	virtual void Tick(float DeltaSeconds) override;
	
protected:
	virtual void onSkill1(FVector Location, AEnemy* Enemy) override;
	virtual void onSkill3(FVector Location, AEnemy* Enemy) override;
	
	/*
	virtual void onSkill2(FVector Location, AEnemy* Enemy) override;
	
	virtual void onUltimate(FVector Location, AEnemy* Enemy) override;


	virtual void endSkill1() override;
	virtual void endSkill2() override;
	virtual void endSkill3() override;
	virtual void endUltimate() override;
	*/
};