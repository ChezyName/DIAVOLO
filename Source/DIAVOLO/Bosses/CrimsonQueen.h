// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "DIAVOLO/Enemy.h"
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

	
	//Functions
	//========================================================================
	virtual void CallRandomAbility_Implementation() override;

	UFUNCTION(Server,Reliable)
	void Ability1();
};
