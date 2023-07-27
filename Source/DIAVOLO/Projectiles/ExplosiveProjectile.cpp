// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveProjectile.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "DIAVOLO/DIAVOLOCharacter.h"

AExplosiveProjectile::AExplosiveProjectile()
{
	ExplosionRange = CreateDefaultSubobject<USphereComponent>("ExplosionRange");
	ExplosionRange->InitSphereRadius(50);
	ExplosionRange->SetupAttachment(RootComponent);
	ExplosionRange->BodyInstance.SetCollisionProfileName("Projectile");

	ExplosionSFX = CreateDefaultSubobject<UAudioComponent>("Explosion SFX");
	ExplosionSFX->SetupAttachment(RootComponent);
	ExplosionSFX->SetAutoActivate(false);
}

void AExplosiveProjectile::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!HasAuthority()) return;
	if(OtherActor == ProjectileOwner) return;
	if(OtherActor->GetClass() == this->GetClass()) return;
	if(OtherActor->GetClass() == ProjectileOwner->GetClass()) return;
	if(!didExplode) Explode();
}

void AExplosiveProjectile::Explode()
{
	/*
	FString DataA = (ExplosionSFX != nullptr ? "FR" : "N/A");
	FString DataB = (ExplosionSFX != nullptr ? "FR" : "N/A");
	GEngine->AddOnScreenDebugMessage(-1,30,FColor::Red,
		"EXPLOSION! @ SFX:" + DataA + " VFX: " + DataB);
	*/
	
	didExplode = true;
	GetProjectileMovement()->SetActive(false);
	ExplodeFXs();
	onExplode();
	
	//GEngine->AddOnScreenDebugMessage(-1,30,FColor::Red,"EXPLOSION!");
	DrawDebugSphere(GetWorld(),GetActorLocation(),ExplosionRange->GetScaledSphereRadius(), 24,
		FColor::Red,false, 5,0,2);
	
	if(ExplosionRange)
	{
		//GEngine->AddOnScreenDebugMessage(-1,8,FColor::Red,"---------------------");
		TArray<AActor*> OverlapingActors;
		ExplosionRange->GetOverlappingActors(OverlapingActors);
		for (AActor* OverlappingActor : OverlapingActors)
		{
			//GEngine->AddOnScreenDebugMessage(-1,8,FColor::Yellow,OverlappingActor->GetName());
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
		//GEngine->AddOnScreenDebugMessage(-1,8,FColor::Red,"---------------------");


		//Destroy Actor in 3s
		SetLifeSpan(4);
	}
}

void AExplosiveProjectile::ExplodeFXs_Implementation()
{
	if(ExplosionSFX != nullptr) ExplosionSFX->Play();
	if(ExplosionVFX != nullptr) {
		UNiagaraComponent* Explo = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),ExplosionVFX,GetActorLocation());
		if(Explo) Explo->SetFloatParameter("Size",ExplosionRange->GetScaledSphereRadius()*2);
	}
}
