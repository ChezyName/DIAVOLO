// Fill out your copyright notice in the Description page of Project Settings.


#include "CrimsonQueen.h"

#include "DIAVOLO/DIAVOLOCharacter.h"

ACrimsonQueen::ACrimsonQueen()
{
	SpinAttackRadius = CreateDefaultSubobject<USphereComponent>("SpinHitbox");
	SpinAttackRadius->SetupAttachment(RootComponent);
	SpinAttackRadius->InitSphereRadius(250);
}

void ACrimsonQueen::CallRandomAbility_Implementation()
{
	Super::CallRandomAbility_Implementation();
	//GEngine->AddOnScreenDebugMessage(-1,30,FColor::White,"is Using Ability on Crimson Queen! [SERVER]");
	Ability1();
}

void ACrimsonQueen::Ability1_Implementation()
{
	if(bUsingAbility) return;
	bUsingAbility = true;
	PlayAnimationServer(SpinAnimation);

	TArray<AActor*> Actors;
	SpinAttackRadius->GetOverlappingActors(Actors);
	for(AActor* Actor : Actors)
	{
		ADIAVOLOCharacter* CharacterHit = Cast<ADIAVOLOCharacter>(Actor);
		if(CharacterHit) CharacterHit->CharacterTakeDamage(SpinAttackDamage);
	}

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([&]
	{
		bUsingAbility = false;
	});
		
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, SpinAttackLength, false);
}
