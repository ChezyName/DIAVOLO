// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "DIAVOLO/BaseProjectile.h"
#include "DIAVOLO/Enemy.h"
#include "DIAVOLO/OtherEnemies/ChaseEnemy.h"
#include "CrimsonQueen.generated.h"

/**
 * 
 */
UCLASS()
class DIAVOLO_API ACrimsonQueen : public AEnemy
{
	GENERATED_BODY()

	ACrimsonQueen();

	//Spin Attack
	//========================================================================
	UPROPERTY(EditAnywhere, Category = "Character Attacks | Spin")
	USphereComponent* SpinAttackRadius;

	UPROPERTY(EditAnywhere, Category = "Character Attacks | Spin")
	UAnimMontage* SpinAnimation;

	UPROPERTY(EditAnywhere, Category = "Character Attacks | Spin")
	float SpinAttackLength = 0;

	UPROPERTY(EditAnywhere, Category = "Character Attacks | Spin")
	float SpinAttackDamage = 300;

	//Summon Hounds Attack
	//========================================================================
	UPROPERTY(EditAnywhere, Category = "Character Attacks | Hounds")
	TSubclassOf<AChaseEnemy> Hounds;

	UPROPERTY(EditAnywhere, Category = "Character Attacks | Hounds")
	float SummonDistAway = 500;

	UPROPERTY(EditAnywhere, Category = "Character Attacks | Hounds")
	FVector2D MinMaxSpawnHounds = FVector2D(2,8);

	UPROPERTY(EditAnywhere, Category = "Character Attacks | Hounds")
	float HoundBiteDamage = 800;

	//Multi Shoot Attack
	//========================================================================
	UPROPERTY(EditAnywhere, Category = "Character Attacks | Multi Shot")
	float BulletDamage = 120;

	UPROPERTY(EditAnywhere, Category = "Character Attacks | Multi Shot")
	float BulletVelocity = 12;
	
	UPROPERTY(EditAnywhere, Category = "Character Attacks | Multi Shot")
	float TotalAngles = 8;

	UPROPERTY(EditAnywhere, Category = "Character Attacks | Multi Shot")
	float AngleInBetween = 10;

	UPROPERTY(EditAnywhere, Category = "Character Attacks | Multi Shot")
	TSubclassOf<ABaseProjectile> BulletProjectile;

	//Summon Hellstrike Attack
	//========================================================================
	UPROPERTY(EditAnywhere, Category = "Character Attacks | HellStrike")
	FVector2D MinMaxSummonHell = FVector2D(6,12);

	UPROPERTY(EditAnywhere, Category = "Character Attacks | HellStrike")
	TArray<TSubclassOf<AActor>> HellStrike;

	UPROPERTY(EditAnywhere, Category = "Character Attacks | HellStrike")
	float TooCloseRange = 500;

	UPROPERTY(EditAnywhere, Category = "Character Attacks | HellStrike")
	float Range = 500;

	//Summon Large Beam
	//========================================================================
	UPROPERTY(EditAnywhere, Category = "Character Attacks | DEATHRAY")
	TSubclassOf<AActor> DeathRay;
	
	//Functions
	//========================================================================
	virtual void CallRandomAbility_Implementation() override;
	virtual void CallRandomAbilityNonMoveNeed_Implementation() override;
	virtual void CallRandomUltimate_Implementation() override;
	virtual void BeginPlay() override;

	FVector StartingPostion;

	UFUNCTION(Server,Reliable)
	void Ability1();

	UFUNCTION(Server,Reliable)
	void Ability2();
	
	UFUNCTION(Server,Reliable)
	void Ability3();

	UFUNCTION(Server,Reliable)
	void Ability4();

	UFUNCTION(Server,Reliable)
	void Ultimate1();
};
