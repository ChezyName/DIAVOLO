// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "DIAVOLO/Enemy.h"
#include "ChaseEnemy.generated.h"

/**
 * 
 */
UCLASS()
class DIAVOLO_API AChaseEnemy : public AEnemy
{
	GENERATED_BODY()
public:
	AChaseEnemy();
	
	float AttackDamage = 0;

	UPROPERTY(EditAnywhere)
	UBoxComponent* AttackArea;

	UFUNCTION(Server,Reliable,BlueprintCallable)
	void BiteEnemy();
};
