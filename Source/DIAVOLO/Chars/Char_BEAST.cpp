// Fill out your copyright notice in the Description page of Project Settings.

#include "NiagaraFunctionLibrary.h"
#include "Char_BEAST.h"

#include "Kismet/GameplayStatics.h"

void AChar_BEAST::PlayClawTeleportFX_Implementation(FVector Location)
{
	if(IsValid(TeleportFX))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),TeleportFX,Location,FRotator::ZeroRotator);
	}
}

void AChar_BEAST::onSkill1(FVector Location, AEnemy* Enemy)
{
	if(Mana < AttackManaConsumption.Skill1) return;
	//GEngine->AddOnScreenDebugMessage(-1,60,FColor::Emerald,"BEAST SKILL #1 -> " + FString::SanitizeFloat(Skill1CD));

	if(IsValid(Claw) && bSwapped == false && Claw->Destroying == false)
	{
		//Swap Spots With Claw
		const FTransform ClawTransform = Claw->GetActorTransform();
		const FTransform PlrTransform = GetActorTransform();
		
		SetActorTransform(ClawTransform);
		bSwapped = true;

		PlayClawTeleportFX(ClawTransform.GetLocation());
		PlayClawTeleportFX(PlrTransform.GetLocation());
		Claw->Destroy();
		Claw = nullptr;
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
			Claw->Damage = ClawDamage * DamageMultiplier;
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
		Mana -= AttackManaConsumption.Skill1;
		ManaCD = ManaCDOnSkillUse;
	}
	
	Super::onSkill1_Implementation(Location, Enemy);
}
