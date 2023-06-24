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

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("root"));

	CollisionCompB = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	CollisionCompB->InitBoxExtent(FVector(5.0f,5.0f,5.0f));
	CollisionCompB->BodyInstance.SetCollisionProfileName("OverlapAllDynamic");
	CollisionCompB->SetGenerateOverlapEvents(true);
	CollisionCompB->OnComponentHit.AddDynamic(this,&ABaseProjectile::OnHit);
	CollisionCompB->OnComponentBeginOverlap.AddDynamic(this, &ABaseProjectile::OnOverlap);// set up a notification for when this component hits something blocking
	CollisionCompB->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionCompB->CanCharacterStepUpOn = ECB_No;
	CollisionCompB->SetupAttachment(RootComponent);

	CollisionCompS = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	CollisionCompS->InitSphereRadius(5.0f);
	CollisionCompS->BodyInstance.SetCollisionProfileName("OverlapAllDynamic");
	CollisionCompS->SetGenerateOverlapEvents(true);
	CollisionCompS->OnComponentHit.AddDynamic(this,&ABaseProjectile::OnHit);
	CollisionCompS->OnComponentBeginOverlap.AddDynamic(this, &ABaseProjectile::OnOverlap);// set up a notification for when this component hits something blocking
	CollisionCompS->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionCompS->CanCharacterStepUpOn = ECB_No;
	CollisionCompS->SetupAttachment(RootComponent);

	if(CollisionType == ECollisionType::E_SPHERE) CollisionCompS->BodyInstance.SetCollisionProfileName("OverlapAllDynamic");
	else CollisionCompS->BodyInstance.SetCollisionProfileName("NoCollision");
	
	if(CollisionType == ECollisionType::E_BOX) CollisionCompB->BodyInstance.SetCollisionProfileName("OverlapAllDynamic");
	else CollisionCompB->BodyInstance.SetCollisionProfileName("NoCollision");

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->InitialSpeed = InitVelocity * 100;
	ProjectileMovement->MaxSpeed = 100000000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->UpdatedComponent = RootComponent;
	ProjectileMovement->SetIsReplicated(true);

	// Die after 3 seconds by default
	InitialLifeSpan = 15.0;
	bReplicates = true;
	SetReplicateMovement(true);

	lastPosition = GetActorLocation();
}

void ABaseProjectile::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("Changing Values"));

	switch (CollisionType)
	{
		case ECollisionType::E_SPHERE:
			// Handle sphere collision logic
			CollisionCompS->SetActive(true);
			CollisionCompB->SetActive(false);

			CollisionCompS->BodyInstance.SetCollisionProfileName("OverlapAllDynamic");
			CollisionCompB->BodyInstance.SetCollisionProfileName("NoCollision");

			CollisionCompB->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			CollisionCompS->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			CollisionCompB->SetHiddenInGame(true);
			CollisionCompB->SetHiddenInSceneCapture(true);
		UE_LOG(LogTemp, Warning, TEXT("Changing SPHERE Values"));
			break;

		case ECollisionType::E_BOX:
			// Handle box collision logic
			CollisionCompB->SetActive(true);
			CollisionCompS->SetActive(false);

			CollisionCompB->BodyInstance.SetCollisionProfileName("OverlapAllDynamic");
			CollisionCompS->BodyInstance.SetCollisionProfileName("NoCollision");

			CollisionCompB->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			CollisionCompS->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					
			CollisionCompS->SetHiddenInGame(true);
			CollisionCompS->SetHiddenInSceneCapture(true);
		UE_LOG(LogTemp, Warning, TEXT("Changing BOX Values"));
			break;

		default:
			// Handle other collision types or an invalid value
			break;
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void ABaseProjectile::EnableProjectileMovement(bool canMove)
{
	ProjectileMovement->SetActive(canMove);
}

// Called when the game starts or when spawned
void ABaseProjectile::BeginPlay()
{
	ProjectileMovement->Velocity = GetActorForwardVector() * (InitVelocity*100);
	lastPosition = GetActorLocation();

	CollisionCompS->IgnoreActorWhenMoving(ProjectileOwner,true);
	CollisionCompB->IgnoreActorWhenMoving(ProjectileOwner,true);

	switch (CollisionType)
	{
		case ECollisionType::E_SPHERE:
			// Handle sphere collision logic
			CollisionCompS->SetActive(true);
			CollisionCompB->SetActive(false);
			break;

		case ECollisionType::E_BOX:
			// Handle box collision logic
			CollisionCompB->SetActive(true);
			CollisionCompS->SetActive(false);
			break;
	}
	Super::BeginPlay();
}

void ABaseProjectile::OnHit_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(!HasAuthority()) return;
	if(OtherActor == ProjectileOwner) return;
	if(OtherActor->GetClass() == this->GetClass()) return;

	DrawDebugBox(GetWorld(),Hit.ImpactPoint,FVector(5,5,5),FColor::Cyan,false,5);
	
	AEnemy* HitEnemy = Cast<AEnemy>(OtherActor);
	if(HitEnemy) OnHitEnemy(HitEnemy);
	//else OnHitWall();
}

void ABaseProjectile::OnOverlap_Implementation(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!HasAuthority()) return;
	if(OtherActor == ProjectileOwner) return;
	if(OtherActor->GetClass() == this->GetClass()) return;

	DrawDebugBox(GetWorld(),SweepResult.ImpactPoint,FVector(5,5,5),FColor::Cyan,false,5);

	AEnemy* HitEnemy = Cast<AEnemy>(OtherActor);
	if(HitEnemy) OnHitEnemy(HitEnemy);
	//else OnHitWall();
}

void ABaseProjectile::OnHitWall()
{
	WallsHit++;
	//if(WallsHit > TotalWallsCanHit) Destroy();
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

