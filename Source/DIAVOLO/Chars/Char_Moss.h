// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DIAVOLO/DIAVOLOCharacter.h"
#include "DIAVOLO/BaseProjectile.h"
#include "Char_Moss.generated.h"

/**
 * 
 */
UCLASS()
class DIAVOLO_API AChar_Moss : public ADIAVOLOCharacter
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Character Info | [Q] RPG")
	UAnimMontage* RPGAnimation;

	UPROPERTY(EditAnywhere, Category = "Character Info | [Q] RPG")
	TSubclassOf<ABaseProjectile> RPGProjectile;
	
	UPROPERTY(EditAnywhere, Category = "Character Info | [Q] RPG")
	float RPGTimeBefore = 0.25;

	UPROPERTY(EditAnywhere, Category = "Character Info | [Q] RPG")
	float RPGTimeAfter = 0.25;

	UPROPERTY(EditAnywhere, Category = "Character Info | [Q] RPG")
	float RPGDamage = 600;

	UPROPERTY(EditAnywhere, Category = "Character Info | [Q] RPG")
	float RPGSpeed = 8;


private:
	virtual void onSkill1(FVector Location, AEnemy* Enemy) override;
	ABaseProjectile* Rocket;
};
