// Fill out your copyright notice in the Description page of Project Settings.


#include "CallBackProjectile.h"
#include "DIAVOLO/DIAVOLOCharacter.h"

void ACallBackProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	FunctionOnDestroy.ExecuteIfBound();
}

void ACallBackProjectile::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
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
	Super::OnOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}
