// Fill out your copyright notice in the Description page of Project Settings.


#include "ChaseEnemy.h"

#include "DIAVOLO/DIAVOLOCharacter.h"

void AChaseEnemy::BiteEnemy_Implementation()
{
	//Bite Enemy IN Front
	TArray<AActor*> Actors;
	AttackArea->GetOverlappingActors(Actors);
	for(AActor* Actor : Actors)
	{
		ADIAVOLOCharacter* CharacterHit = Cast<ADIAVOLOCharacter>(Actor);
		if(CharacterHit) CharacterHit->CharacterTakeDamage(AttackDamage);
	}

	Destroy();
}

AChaseEnemy::AChaseEnemy()
{
	AttackArea = CreateDefaultSubobject<UBoxComponent>("AttackArea");
	AttackArea->InitBoxExtent(FVector(5,5,5));
	AttackArea->SetupAttachment(RootComponent);
}
