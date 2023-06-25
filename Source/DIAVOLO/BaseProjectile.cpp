// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseProjectile.h"
#include "DIAVOLOCharacter.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABaseProjectile::ABaseProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Hitbox"));
	CollisionComp->InitBoxExtent(FVector(5,5,5));
	CollisionComp->BodyInstance.SetCollisionProfileName("OverlapAll");
	CollisionComp->SetGenerateOverlapEvents(true);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABaseProjectile::OnOverlap);// set up a notification for when this component hits something blocking

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

void ABaseProjectile::EnableProjectileMovement(bool canMove)
{
	ProjectileMovement->SetActive(canMove);
}

// Called when the game starts or when spawned
void ABaseProjectile::BeginPlay()
{
	PrimaryActorTick.bCanEverTick = true;
	//Bloom
	ProjectileMovement->Velocity = GetActorForwardVector() * (InitVelocity*100);
	lastPosition = GetActorLocation();
	startingPosition = lastPosition;
	
	Super::BeginPlay();
}

void ABaseProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(ABaseProjectile,ProjectileOwner);
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ABaseProjectile::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	DrawDebugBox(GetWorld(),SweepResult.ImpactPoint,FVector(5,5,5),FColor::Red,false,5);
	
	if(!HasAuthority()) return;
	if(OtherActor == ProjectileOwner) return;
	if(OtherActor->GetClass() == this->GetClass()) return;
	if(OtherActor->GetClass() == ADIAVOLOCharacter::StaticClass()) return;

	DrawDebugBox(GetWorld(),SweepResult.ImpactPoint,FVector(7,7,7),FColor::Cyan,false,5);

	AEnemy* HitEnemy = Cast<AEnemy>(OtherActor);
	if(HitEnemy) OnHitEnemy(HitEnemy);
	//else OnHitWall();
}

void ABaseProjectile::OnHitEnemy(AEnemy* EnemyHit)
{
	if(HasHit) return;
	HasHit = true;
	EnemyHit->Damage(Damage);
	Destroying = true;
	SetupForDestroy();
}

// Called every frame
void ABaseProjectile::Tick(float DeltaTime)
{
	//DrawDebugLine(GetWorld(),lastPosition,GetActorLocation(),FColor::Cyan,false,1.5f,-1);
	lastPosition = GetActorLocation();
	Super::Tick(DeltaTime);
}

