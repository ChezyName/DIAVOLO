// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "DIAVOLO/BaseProjectile.h"
#include "ExplosiveProjectile.generated.h"

/**
 * 
 */
UCLASS()
class DIAVOLO_API AExplosiveProjectile : public ABaseProjectile
{
	GENERATED_BODY()
private:
	AExplosiveProjectile();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	USphereComponent* ExplosionRange;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UAudioComponent* ExplosionSFX;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* ExplosionVFX;
	
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	void Explode();
};