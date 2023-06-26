// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Projectilev2.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(config=Game)
class AProjectilev2 : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	float ProjectileDebugLifetime = 1.5f;

	//Debug
	FVector lastPosition;
	FVector startingPosition;

	int numWallsHit = 1;

	float CalculateDamage(FVector HitLocation,USkeletalMeshComponent* Skeleton);

public:
	AProjectilev2();

	/** called when projectile hits something */
	UFUNCTION(Server,Reliable)
	void OnHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Returns CollisionComp subobject **/
	UBoxComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;
	
	class UWeapon* Weapon;

	TArray<class ADIAVOLOCharacter*> CharactersHit;

	UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	ADIAVOLOCharacter* ProjectileOwner;

	UPROPERTY(EditAnywhere, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float InitVelocity = 0;
};
