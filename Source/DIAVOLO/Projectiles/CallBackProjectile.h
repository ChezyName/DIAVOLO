// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DIAVOLO/BaseProjectile.h"
#include "CallBackProjectile.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class DIAVOLO_API ACallBackProjectile : public ABaseProjectile
{
	GENERATED_BODY()
	
public:
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOverlapCallback, FVector, HitLocation, class AEnemy*, EnemyHit);
	
	UPROPERTY()
	FOverlapCallback FunctionOnOverlap;
	
	virtual void OnOverlap_Implementation(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	//virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
};
