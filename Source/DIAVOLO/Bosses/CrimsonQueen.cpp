// Fill out your copyright notice in the Description page of Project Settings.


#include "CrimsonQueen.h"

#include "DIAVOLO/DIAVOLOCharacter.h"
#include "Kismet/GameplayStatics.h"

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
	int Random = FMath::RandRange(0,2);

	switch (Random)
	{
		case 0:
			Ability1();
		case 1:
			Ability2();
		case 2:
			Ability3();
	}
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

void ACrimsonQueen::Ability2_Implementation()
{
	if(bUsingAbility) return;
	bUsingAbility = true;

	int spawnCount = FMath::RandRange(MinMaxSpawnHounds.X,MinMaxSpawnHounds.Y);
	for(int i = 0; i < spawnCount; i++)
	{
		AChaseEnemy* NewHound = Cast<AChaseEnemy>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this,Hounds,FTransform(GetActorRotation(),GetActorLocation()),ESpawnActorCollisionHandlingMethod::AlwaysSpawn,this));
		if(NewHound != nullptr)
		{
			//Spawn The Actor
			NewHound->AttackDamage = HoundBiteDamage;
			UGameplayStatics::FinishSpawningActor(NewHound, FTransform(GetActorRotation(),GetActorLocation()));
		}
	}

	bUsingAbility = false;
}

void ACrimsonQueen::Ability3_Implementation()
{
	if(bUsingAbility) return;
	bUsingAbility = true;
	for(int i = 0; i < TotalAngles; i++)
	{
		float ShotAngle = i * AngleInBetween;
		FRotator ShootAngle = GetActorRotation();
		ShootAngle.Yaw += ShotAngle;
				
		ABaseProjectile* Bullet = Cast<ABaseProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this,BulletProjectile,FTransform(ShootAngle,GetActorLocation()),ESpawnActorCollisionHandlingMethod::AlwaysSpawn,this));
		if(Bullet != nullptr)
		{
			//Finalizing Create Projecitle
			Bullet->InitVelocity = BulletVelocity;
			Bullet->bEnemyProjectile = true;
			Bullet->Damage = BulletDamage;
			Bullet->SetOwner(this);
							
			//Spawn The Actor
			UGameplayStatics::FinishSpawningActor(Bullet, FTransform(ShootAngle,GetActorLocation()));
		}
	}
	bUsingAbility = false;
}