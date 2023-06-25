// Copyright Epic Games, Inc. All Rights Reserved.

#include "Projectilev2.h"

#include "DrawDebugHelpers.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "DIAVOLOCharacter.h"
#include "Net/UnrealNetwork.h"

AProjectilev2::AProjectilev2() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Hitbox"));
	CollisionComp->InitBoxExtent(FVector(5,5,5));
	CollisionComp->BodyInstance.SetCollisionProfileName("OverlapAll");
	CollisionComp->SetGenerateOverlapEvents(true);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectilev2::OnHit);// set up a notification for when this component hits something blocking

	PrimaryActorTick.bStartWithTickEnabled = false;
	
	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = InitVelocity;
	ProjectileMovement->MaxSpeed = 100000000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.75f;
	lastPosition = GetActorLocation();

	// Die after 3 seconds by default
	InitialLifeSpan = 12.0f;
	bReplicates = true;
	SetReplicateMovement(true);
}

void AProjectilev2::BeginPlay()
{
	PrimaryActorTick.bCanEverTick = true;
	//Bloom
	ProjectileMovement->Velocity = GetActorForwardVector() * (InitVelocity*100);
	lastPosition = GetActorLocation();
	startingPosition = lastPosition;

	/*
	else
	{
		if(ProjectileOwner != nullptr)
		{
			SetActorLocation(ProjectileOwner->GetFirstPersonCameraComponent()->GetComponentLocation());
		}
	}
	*/
	Super::BeginPlay();
}


void AProjectilev2::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	DrawDebugLine(GetWorld(),lastPosition,GetActorLocation(),FColor::Red,false,ProjectileDebugLifetime,-1);
	lastPosition = GetActorLocation();
}

void AProjectilev2::OnHit_Implementation(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Check if server ONLY
	if(!HasAuthority()) return;
	if(ProjectileOwner == OtherActor) return;
	if(OtherActor->GetClass() == this->GetClass()) return;

	DrawDebugBox(GetWorld(),SweepResult.ImpactPoint,
		FVector(5,5,5),FColor::Red,0,25,0,5);

	lastPosition = SweepResult.ImpactPoint;
}

void AProjectilev2::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AProjectilev2,ProjectileOwner);

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}