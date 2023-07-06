// Fill out your copyright notice in the Description page of Project Settings.


#include "Char_Moss.h"
#include "Kismet/GameplayStatics.h"

void AChar_Moss::onSkill1(FVector Location, AEnemy* Enemy)
{
	if(Skill1CD < 0 && Mana >= AttackManaConsumption.Skill1)
	{
		FVector MyLoc = Location;
		MyLoc.Z = GetActorLocation().Z;
		FVector PDir = MyLoc - GetActorLocation();
		PDir.Normalize();

		FRotator LookAtRotation = FRotationMatrix::MakeFromX(PDir).Rotator();
		
		CharState = EPlayerStates::E_ABILITY;
		PlayAnimationServer(RPGAnimation);
		SetActorRotation(LookAtRotation);
		
		//Launch Rocket
		FTimerDelegate TimerBefore;
		TimerBefore.BindLambda([&]
		{
			//Shoot Rocket HERE
			Rocket = Cast<ABaseProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this,RPGProjectile,FTransform(LookAtRotation,GetActorLocation()),ESpawnActorCollisionHandlingMethod::AlwaysSpawn,this));
			if(RPGProjectile != nullptr)
			{
				//Finalizing Create Projecitle
				Rocket->ProjectileOwner = this;
				Rocket->InitVelocity = RPGSpeed;
				Rocket->Damage = RPGDamage * DamageMultiplier;
				Rocket->SetOwner(this);
						
				//Spawn The Actor
				UGameplayStatics::FinishSpawningActor(Rocket, FTransform(LookAtRotation,GetActorLocation()));
			}
			
			//After Rocket Launched
			FTimerDelegate TimerAfter;
			TimerAfter.BindLambda([&]
			{
				Skill1CD = AttackCooldowns.Skill1;
				Mana -= AttackManaConsumption.Skill1;
				bUsingAbility = false;
				ManaCD = ManaCDOnSkillUse;
				CharState = EPlayerStates::E_IDLE;
			});
			FTimerHandle TimerHandle2;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle2, TimerAfter, RPGTimeAfter, false);
		});
		

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerBefore, RPGTimeBefore, false);
	}
	Super::onSkill1(Location, Enemy);
}
