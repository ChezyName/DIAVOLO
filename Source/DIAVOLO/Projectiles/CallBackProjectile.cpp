// Fill out your copyright notice in the Description page of Project Settings.


#include "CallBackProjectile.h"
#include "DIAVOLO/DIAVOLOCharacter.h"

void ACallBackProjectile::OnOverlap_Implementation(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!HasAuthority()) return;
	if(OtherActor == ProjectileOwner) return;
	if(OtherActor->GetClass() == this->GetClass()) return;
	
	AEnemy* IfHitEnemy = Cast<AEnemy>(OtherActor);
	
	/*
	GEngine->AddOnScreenDebugMessage(-1,25,FColor::Yellow,"[OLP] Recieved Hit Information @ " +
		GetActorLocation().ToString());
	*/
	
	FunctionOnOverlap.ExecuteIfBound(GetActorLocation(),IfHitEnemy);
	Super::OnOverlap_Implementation(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void ACallBackProjectile::OnHit_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(!HasAuthority()) return;
	if(OtherActor == ProjectileOwner) return;
	if(OtherActor->GetClass() == this->GetClass()) return;
	
	AEnemy* IfHitEnemy = Cast<AEnemy>(OtherActor);
	FunctionOnOverlap.ExecuteIfBound(GetActorLocation(),IfHitEnemy);
	Super::OnHit_Implementation(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}
