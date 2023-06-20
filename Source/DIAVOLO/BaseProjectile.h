// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BaseProjectile.generated.h"

UENUM(BlueprintType)
enum class ECollisionType : uint8
{
	E_SPHERE = 0 UMETA(DisplayName="Sphere Collision Comp"),
	E_BOX = 1 UMETA(DisplayName="Box Collision Comp"),
};

UCLASS(config=Game)
class DIAVOLO_API ABaseProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseProjectile();
	
	class ADIAVOLOCharacter* ProjectileOwner;

	

	float InitVelocity;
	float Damage = 0;
	
	UPROPERTY(EditDefaultsOnly,Category="Projectile Defaults")
	int TotalWallsCanHit = 0;

	UPROPERTY(EditDefaultsOnly,Category="Projectile Defaults")
	ECollisionType CollisionType = ECollisionType::E_SPHERE;

	bool HasHit = false;
	bool Destroying = false;
	//Call Destroy On The Function [SERVER ONLY]
	UFUNCTION(BlueprintImplementableEvent)
	void SetupForDestroy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION(Server,Reliable)
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(Server,Reliable)
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile, meta = (AllowPrivateAccess = "true",
		EditCondition="CollisionType == ECollisionType::E_SPHERE", EditConditionHides))
	USphereComponent* CollisionCompS;

	UPROPERTY(VisibleDefaultsOnly, Category=Projectile, meta = (AllowPrivateAccess = "true",
		EditCondition="CollisionType == ECollisionType::E_BOX", EditConditionHides))
	UBoxComponent* CollisionCompB;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	void OnHitWall();
	void OnHitEnemy(AEnemy* EnemyHit);

	FVector lastPosition;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	int WallsHit = 0;
};
