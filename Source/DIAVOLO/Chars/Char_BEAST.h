// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
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
	AChar_BEAST();
	
	// SKILL 1
	UPROPERTY(EditAnywhere, Category = "Character Info | [Q] Claw // Teleport")
	TSubclassOf<class ACallBackProjectile> ClawProjectile;

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
	UPROPERTY(EditAnywhere, Category = "Character Info | [W] Spin")
	USphereComponent* SpinHitbox;

	UPROPERTY(EditAnywhere, Category = "Character Info | [W] Spin")
	UStaticMeshComponent* SpinVFX;

	UPROPERTY(EditAnywhere, Category = "Character Info | [W] Spin")
	UStaticMeshComponent* SpinVFX2;

	UPROPERTY(EditAnywhere, Category = "Character Info | [W] Spin")
	FRotator SpinSpeed;

	UPROPERTY(EditAnywhere, Category = "Character Info | [W] Spin")
	float SpinDamage = 600;

	UPROPERTY(EditAnywhere, Category = "Character Info | [W] Spin")
	float SpinDamageTick = 0.025f;

	UPROPERTY(EditAnywhere, Category = "Character Info | [W] Spin")
	float MINSpinDuration = 0.05f;
	
	UPROPERTY(EditAnywhere, Category = "Character Info | [W] Spin")
	float MAXSpinDuration = 1.25f;

	// SKILL 3
	UPROPERTY(EditAnywhere, Category = "Character Info | [E] Grapple")
	TSubclassOf<class ACallBackProjectile> GrappleProjectile;

	UPROPERTY(EditAnywhere, Category = "Character Info | [E] Grapple")
	float GrappleLifetime = 3;

	UPROPERTY(EditAnywhere, Category = "Character Info | [E] Grapple")
	float GrappleVelocity = 18;
	
	UPROPERTY(EditAnywhere, Category = "Character Info | [E] Grapple")
	float GrapplePullSpeed = 125;

	UPROPERTY(EditAnywhere, Category = "Character Info | [E] Grapple")
	float GrappleCloseEnough = 64;

	UPROPERTY(EditAnywhere, Category = "Character Info | [E] Grapple")
	UAnimMontage* GrappleStart;
	
	UPROPERTY(EditAnywhere, Category = "Character Info | [E] Grapple")
	UAnimMontage* GrapplePull;

	//Skill 4 : R (Ultimate)
	UPROPERTY(EditAnywhere, Category = "Character Info | [R] Summon: Razors")
	float UltRadius = 300;

	UPROPERTY(EditAnywhere, Category = "Character Info | [R] Summon: Razors")
	float UltDamage = 120;

	UPROPERTY(EditAnywhere, Category = "Character Info | [R] Summon: Razors")
	float DamageTick = 0.1;

	UPROPERTY(EditAnywhere, Category = "Character Info | [R] Summon: Razors")
	float UltLength = 12;

	UPROPERTY(EditAnywhere, Category = "Character Info | [R] Summon: Razors")
	FRotator UltSpinSpeed = FRotator(0,80,0);

	UPROPERTY(EditAnywhere, Category = "Character Info | [R] Summon: Razors")
	FRotator UltRazorSpinSpeed = FRotator(0,800,0);

	UPROPERTY(EditAnywhere, Category = "Character Info | [R] Summon: Razors", meta=(AllowPrivateAccess=true))
	USceneComponent* RazorParent;

	UPROPERTY(EditAnywhere, Category = "Character Info | [R] Summon: Razors")
	UStaticMeshComponent* UltRazor1;
	UPROPERTY(EditAnywhere, Category = "Character Info | [R] Summon: Razors")
	UStaticMeshComponent* UltRazor2;
	UPROPERTY(EditAnywhere, Category = "Character Info | [R] Summon: Razors")
	UStaticMeshComponent* UltRazor3;
	UPROPERTY(EditAnywhere, Category = "Character Info | [R] Summon: Razors")
	UStaticMeshComponent* UltRazor4;
	
private:
	//Skill 1 : Q
	UPROPERTY()
	ACallBackProjectile* Claw = nullptr;
	bool bSwapped = false;
	bool bDoing = false;
	float TPDelay = 0;
	FRotator LookAtRotation;
	bool hasDoneCD = true;
	bool bSpawned = false;
	UFUNCTION()
	void EndGrappleCallback();
	UFUNCTION(NetMulticast,Reliable)
	void PlayClawTeleportFX(FVector Location);

	//Skill 2 : W
	UPROPERTY(Replicated)
	bool SpinActive = false;
	bool bEndedEarly = false;
	float DelayPerTickSpin = 0;
	float WaitMinSpin = 0;
	bool StopOnMinSpin = false;

	//Skill 3 : E
	UPROPERTY()
	ACallBackProjectile* Grapple = nullptr;
	UFUNCTION()
	void onGrappleHit(FVector HitImpact, AEnemy* EnemyHit);
	bool Grappling = false;
	FVector toLoc = FVector::ZeroVector;
	float GrappleOutT = 0;
	bool GrappleOut = false;

	//Skill 4 : R (ULTIMATE)
	UFUNCTION(Server,Reliable)
	void ChangeRazorLocation();
	void CheckCollisions();
	UPROPERTY(Replicated)
	bool RazorsActive = false;
	float UltTick = 0;
	
	// Other Funcs
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
protected:
	virtual void onSkill1(FVector Location, AEnemy* Enemy) override;
	virtual void onSkill2(FVector Location, AEnemy* Enemy) override;
	virtual void endSkill2() override;
	virtual void onSkill3(FVector Location, AEnemy* Enemy) override;
	virtual void onUltimate(FVector Location, AEnemy* Enemy) override;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;
	
	/*
	virtual void onSkill2(FVector Location, AEnemy* Enemy) override;
	
	virtual void onUltimate(FVector Location, AEnemy* Enemy) override;


	virtual void endSkill1() override;
	virtual void endSkill2() override;
	virtual void endSkill3() override;
	virtual void endUltimate() override;
	*/
};
