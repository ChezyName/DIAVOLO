// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseProjectile.h"
#include "DIAVOLOCharacter.h"
#include "DrawDebugHelpers.h"

// Sets default values
ABaseProjectile::ABaseProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("OverlapAllDynamic");
	CollisionComp->SetGenerateOverlapEvents(true);
	CollisionComp->OnComponentHit.AddDynamic(this,&ABaseProjectile::OnHit);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABaseProjectile::OnOverlap);// set up a notification for when this component hits something blocking
	
	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = InitVelocity * 100;
	ProjectileMovement->MaxSpeed = 100000000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->SetIsReplicated(true);

	// Die after 3 seconds by default
	InitialLifeSpan = 15.0;
	bReplicates = true;
	SetReplicateMovement(true);

	lastPosition = GetActorLocation();
}

// Called when the game starts or when spawned
void ABaseProjectile::BeginPlay()
{
	ProjectileMovement->Velocity = GetActorForwardVector() * (InitVelocity*100);
	lastPosition = GetActorLocation();

	CollisionComp->IgnoreActorWhenMoving(ProjectileOwner,true);
	Super::BeginPlay();
}

void ABaseProjectile::OnHit_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(!HasAuthority()) return;
	if(OtherActor == ProjectileOwner) return;
	if(OtherActor->GetClass() == this->GetClass()) return;
	DrawDebugBox(GetWorld(),Hit.ImpactPoint,FVector(5,5,5),FColor::Cyan,false,5);
}

void ABaseProjectile::OnOverlap_Implementation(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!HasAuthority()) return;
	if(OtherActor == ProjectileOwner) return;
	if(OtherActor->GetClass() == this->GetClass()) return;
	DrawDebugBox(GetWorld(),SweepResult.ImpactPoint,FVector(5,5,5),FColor::Cyan,false,5);
}

// Called every frame
void ABaseProjectile::Tick(float DeltaTime)
{
	DrawDebugLine(GetWorld(),lastPosition,GetActorLocation(),FColor::Cyan,false,1.5f,-1);
	lastPosition = GetActorLocation();
	Super::Tick(DeltaTime);
}

