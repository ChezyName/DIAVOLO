// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DIAVOLO/DIAVOLOCharacter.h"
#include "DIAVOLO/BaseProjectile.h"
#include "DIAVOLO/Projectiles/ExplosiveProjectile.h"
#include "Char_Moss.generated.h"

/**
 * 
 */
UCLASS()
class DIAVOLO_API AChar_Moss : public ADIAVOLOCharacter
{
	GENERATED_BODY()
public:
	//Skill 1 : [Q] RPG (Rocket Propelled GUN?)
	UPROPERTY(EditAnywhere, Category = "Character Info | [Q] RPG")
	UAnimMontage* RPGAnimation;

	UPROPERTY(EditAnywhere, Category = "Character Info | [Q] RPG")
	TSubclassOf<AExplosiveProjectile> RPGProjectile;
	
	UPROPERTY(EditAnywhere, Category = "Character Info | [Q] RPG")
	float RPGTimeBefore = 0.25;

	UPROPERTY(EditAnywhere, Category = "Character Info | [Q] RPG")
	float RPGTimeAfter = 0.25;

	UPROPERTY(EditAnywhere, Category = "Character Info | [Q] RPG")
	float RPGDamage = 600;

	UPROPERTY(EditAnywhere, Category = "Character Info | [Q] RPG")
	float RPGSpeed = 8;

	//Skill 2 : Stim Shot
	UPROPERTY(EditAnywhere, Category = "Character Info | [W] Medical Stim")
	float HealPerTick = 10;

	UPROPERTY(EditAnywhere, Category = "Character Info | [W] Medical Stim")
	float HealTick = 0.1;
	
	UPROPERTY(EditAnywhere, Category = "Character Info | [W] Medical Stim")
	float MaxHealthGain = 350;

	//Skill 3 : Shotgun
	UPROPERTY(EditAnywhere, Category = "Character Info | [E] Shotgun")
	float ShotgunDamage = 80;

	UPROPERTY(EditAnywhere, Category = "Character Info | [E] Shotgun")
	float TotalAngles = 8;

	UPROPERTY(EditAnywhere, Category = "Character Info | [E] Shotgun")
	float AngleInBetween = 10;

	UPROPERTY(EditAnywhere, Category = "Character Info | [E] Shotgun")
	UAnimMontage* ShotgunAnimation;

	UPROPERTY(EditAnywhere, Category = "Character Info | [E] Shotgun")
	float ShotgunBeforeDelay = 0.15;

	UPROPERTY(EditAnywhere, Category = "Character Info | [E] Shotgun")
	float ShotgunEndDelay = 0.15;

	UPROPERTY(EditAnywhere, Category = "Character Info | [E] Shotgun")
	USoundWave* ShotgunSFX;
	

private:
	virtual void Tick(float DeltaSeconds) override;
	
	//Skill 1
	virtual void onSkill1(FVector Location, AEnemy* Enemy) override;
	AExplosiveProjectile* Rocket;

	//Skill 2
	virtual void onSkill2(FVector Location, AEnemy* Enemy) override;
	virtual void endSkill2() override;
	bool bHealEnabled = false;
	float cHealTick = 0;
	bool Canceled = false;
	float HealthGained = 0;

	//Skill 3
	virtual void onSkill3(FVector Location, AEnemy* Enemy) override;
};
