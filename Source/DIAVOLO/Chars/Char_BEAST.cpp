// Fill out your copyright notice in the Description page of Project Settings.


#include "Char_BEAST.h"

#include "Kismet/GameplayStatics.h"

void AChar_BEAST::onSkill1(FVector Location, AEnemy* Enemy)
{
	GEngine->AddOnScreenDebugMessage(-1,60,FColor::Emerald,"BEAST SKILL #1 -> " + FString::SanitizeFloat(Skill1CD));
	if(Claw != nullptr && bSwapped == false && Claw->Destroying == false)
	{
		//Swap Spots With Claw
		FTransform ClawTransform = Claw->GetActorTransform();
		FTransform PlrTransform = GetActorTransform();

		Claw->SetActorTransform(PlrTransform);
		SetActorTransform(ClawTransform);
		bSwapped = true;
	}
	else
	{
		if(Skill1CD > 0) return;
		
		bSwapped = false;
		
		//Create Projectile Facing Enemy
		Claw = Cast<ABaseProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this,AutoAttack.Projectile,FTransform(FRotator::ZeroRotator,GetActorLocation()),ESpawnActorCollisionHandlingMethod::AlwaysSpawn,this));
		if(Claw != nullptr)
		{
			//Finalizing Create Projecitle
			Claw->ProjectileOwner = this;
			Claw->InitVelocity = AutoAttack.ProjectileVelocity;
			Claw->Damage = AutoAttack.AttackDamage * DamageMultiplier;
			Claw->SetOwner(this);

			//Calculate Rotation
			Location.Z = GetActorLocation().Z;
			FVector Direction = Location - GetActorLocation();
			Direction.Normalize();

			FRotator LookAtRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
						
			//Spawn The Actor
			UGameplayStatics::FinishSpawningActor(Claw, FTransform(LookAtRotation,GetActorLocation()));
		}

		Skill1CD = AttackCooldowns.Skill1;
	}
	
	Super::onSkill1_Implementation(Location, Enemy);
}
