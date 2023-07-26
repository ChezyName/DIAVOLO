// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BaseProjectile.generated.h"

UCLASS(config=Game)
class ABaseProjectile : public AActor
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

public:
	ABaseProjectile();

	void EnableProjectileMovement(bool canMove);

	bool HasHit;
	bool Destroying;
	float Damage = 0;
	bool bEnemyProjectile = false;
	
	UFUNCTION(BlueprintImplementableEvent)
	void SetupForDestroy();
	
	/** called when projectile hits something */
	UFUNCTION()
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Returns CollisionComp subobject **/
	UBoxComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;
	
	class UWeapon* Weapon;

	TArray<class ABRM6Character*> CharactersHit;
	
	void OnHitEnemy(class AEnemy* EnemyHit);

	UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class ADIAVOLOCharacter* ProjectileOwner;

	UPROPERTY(EditAnywhere, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float InitVelocity = 0;
};
