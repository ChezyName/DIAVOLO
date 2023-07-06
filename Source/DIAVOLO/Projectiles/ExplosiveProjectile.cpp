// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveProjectile.h"

#include "Components/AudioComponent.h"
#include "DIAVOLO/DIAVOLOCharacter.h"

AExplosiveProjectile::AExplosiveProjectile()
{
	ExplosionRange = CreateDefaultSubobject<USphereComponent>("ExplosionRange");
	ExplosionRange->InitSphereRadius(50);
	ExplosionRange->SetupAttachment(RootComponent);

	ExplosionSFX = CreateDefaultSubobject<UAudioComponent>("Explosion SFX");
	ExplosionSFX->SetupAttachment(RootComponent);
	ExplosionSFX->SetAutoActivate(false);

	ExplosionVFX = CreateDefaultSubobject<UNiagaraComponent>("Explosion VFX");
	ExplosionVFX->SetupAttachment(RootComponent);
	ExplosionVFX->SetAutoActivate(false);
}

void AExplosiveProjectile::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if(!HasAuthority()) return;
	if(OtherActor == ProjectileOwner) return;
	if(OtherActor->GetClass() == this->GetClass()) return;
	Explode();
}

void AExplosiveProjectile::Explode_Implementation()
{
	GetProjectileMovement()->SetActive(false);
	ExplosionSFX->Play();
	ExplosionVFX->BeginPlay();

	TArray<AActor*> OverlapingActors;
	ExplosionRange->GetOverlappingActors(OverlapingActors);
	for (AActor* OverlappingActor : OverlapingActors)
	{
		//Check if Overlapping is Enemy
		AEnemy* Enemy = Cast<AEnemy>(OverlappingActor);
		if(Enemy)
		{
			float Distance = FVector::Dist(GetActorLocation(), Enemy->GetActorLocation());
			float NormalizedDistance = FMath::Clamp(Distance / ExplosionRange->GetScaledSphereRadius(), 0.0f, 1.0f);
			float DealingDamage = Damage * (1.0f - NormalizedDistance);
			Enemy->Damage(DealingDamage);
		}
	}
}
