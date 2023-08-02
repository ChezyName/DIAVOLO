// Fill out your copyright notice in the Description page of Project Settings.


#include "Char_Moss.h"

#include "DIAVOLO/CharacterProxy.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void AChar_Moss::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(bHealEnabled && Mana >= AttackManaConsumption.Skill2){
		cHealTick--;
		if(cHealTick < 0 && HealthGained < MaxHealthGain)
		{
			Mana -= AttackManaConsumption.Skill2;
			ManaCD = ManaCDOnSkillUse;
			CharState = EPlayerStates::E_ABILITY;
			cHealTick = HealTick;
			Health += HealPerTick;
			HealthGained += HealPerTick;
			if(Health >= MaxHealth)
			{
				Health = MaxHealth;
				endSkill2();
			}
		}
		else if(!Canceled) endSkill2();
	}
	else if(!Canceled) endSkill2();
}

void AChar_Moss::onSkill1_Implementation(FVector Location, AEnemy* Enemy)
{
	Super::onSkill1_Implementation(Location, Enemy);
	if(Skill1CD < 0 && Mana >= AttackManaConsumption.Skill1 && CharState != EPlayerStates::E_ABILITY && !bUsingAbility)
	{
		FVector MyLoc = Location;
		MyLoc.Z = GetActorLocation().Z;
		FVector PDir = MyLoc - GetActorLocation();
		PDir.Normalize();

		FRotator LookAtRotation = FRotationMatrix::MakeFromX(PDir).Rotator();
		
		CharState = EPlayerStates::E_ABILITY;
		PlayAnimationServer(RPGAnimation);
		//SetActorRotation(LookAtRotation);
		//ParentProxy->MoveToLocation(GetActorLocation());
		GetMovementComponent()->Deactivate();
		
		//Launch Rocket
		FTimerDelegate TimerBefore;
		TimerBefore.BindLambda([&]
		{
			//Shoot Rocket HERE
			Rocket = Cast<AExplosiveProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this,RPGProjectile,FTransform(GetActorRotation(),GetActorLocation()),ESpawnActorCollisionHandlingMethod::AlwaysSpawn,this));
			if(Rocket != nullptr)
			{
				//Finalizing Create Projecitle
				Rocket->ProjectileOwner = this;
				Rocket->InitVelocity = RPGSpeed;
				Rocket->Damage = RPGDamage * DamageMultiplier;
				Rocket->SetOwner(this);
						
				//Spawn The Actor
				UGameplayStatics::FinishSpawningActor(Rocket, FTransform(GetActorRotation(),GetActorLocation()));
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
				GetMovementComponent()->Activate();
			});
			FTimerHandle TimerHandle2;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle2, TimerAfter, RPGTimeAfter, false);
		});
		

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerBefore, RPGTimeBefore, false);
	}
}

void AChar_Moss::onSkill2_Implementation(FVector Location, AEnemy* Enemy)
{
	Super::onSkill2_Implementation(Location, Enemy);
	if(Skill2CD < 0 && Mana >= AttackManaConsumption.Skill2 && CharState != EPlayerStates::E_ABILITY && !bUsingAbility
		&& Health < MaxHealth)
	{
		bHealEnabled = true;
		Canceled = false;
		HealthGained = 0;
		CharState = EPlayerStates::E_ABILITY;
		cHealTick = -1;
	}
}

void AChar_Moss::endSkill2_Implementation()
{
	Super::endSkill2_Implementation();
	if(bHealEnabled)
	{
		bHealEnabled = false;
		CharState = EPlayerStates::E_IDLE;
		bUsingAbility = false;
		Skill2CD = AttackCooldowns.Skill2;
		Canceled = true;
	}
}

void AChar_Moss::onSkill3_Implementation(FVector Location, AEnemy* Enemy)
{
	Super::onSkill3_Implementation(Location, Enemy);
	if(Skill3CD < 0 && Mana >= AttackManaConsumption.Skill3 && CharState != EPlayerStates::E_ABILITY && !bUsingAbility)
	{
		FVector MyLoc = Location;
		MyLoc.Z = GetActorLocation().Z;
		FVector PDir = MyLoc - GetActorLocation();
		PDir.Normalize();

		FRotator LookAtRotation = FRotationMatrix::MakeFromX(PDir).Rotator();

		CharState = EPlayerStates::E_ABILITY;
		PlayAnimationServer(ShotgunAnimation);
		//SetActorRotation(LookAtRotation);
		//ParentProxy->MoveToLocation(GetActorLocation());
		GetMovementComponent()->Deactivate();
		
		FTimerDelegate TimeBefore;
		TimeBefore.BindLambda([&]
		{
			PlaySoundSingle(ShotgunSFX);
			
			//Shoot Pellets
			int Parts = FMath::RoundToInt(TotalAngles/2);
			for(int i = -Parts; i < Parts; i++)
			{
				float ShotAngle = i * AngleInBetween;
				FRotator ShootAngle = GetActorRotation();
				ShootAngle.Yaw += ShotAngle;
				
				ABaseProjectile* Bullet = Cast<ABaseProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this,AutoAttack.Projectile,FTransform(ShootAngle,GetActorLocation()),ESpawnActorCollisionHandlingMethod::AlwaysSpawn,this));
				if(Bullet != nullptr)
				{
					//Finalizing Create Projecitle
					Bullet->ProjectileOwner = this;
					Bullet->InitVelocity = AutoAttack.ProjectileVelocity;
					Bullet->Damage = ShotgunDamage * DamageMultiplier;
					Bullet->SetOwner(this);
							
					//Spawn The Actor
					UGameplayStatics::FinishSpawningActor(Bullet, FTransform(ShootAngle,GetActorLocation()));
				}
			}
			
			FTimerDelegate TimerAfter;
			TimerAfter.BindLambda([&]
			{
				Skill3CD = AttackCooldowns.Skill3;
				Mana -= AttackManaConsumption.Skill3;
				bUsingAbility = false;
				ManaCD = ManaCDOnSkillUse;
				CharState = EPlayerStates::E_IDLE;
				GetMovementComponent()->Activate();
				StopAnimationServer(ShotgunAnimation);
			});
			FTimerHandle TimerHandle2;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle2, TimerAfter, ShotgunEndDelay, false);
		});
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimeBefore, ShotgunEndDelay, false);
	}
}

void AChar_Moss::onUltimate_Implementation(FVector Location, AEnemy* Enemy)
{
	Super::onUltimate_Implementation(Location, Enemy);
	// && UltimateCD < 0 && Mana >= AttackManaConsumption.Ultimate && !bUsingAbility
	if(Enemy != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1,45,FColor::Blue,"Using Moss ULTIMATE on " + Enemy->GetName());
		//FVector MyLoc = Enemy->GetActorLocation();
		/*
		MyLoc.Z = GetActorLocation().Z;
		FVector PDir = MyLoc - GetActorLocation();
		PDir.Normalize();

		FRotator LookAtRotation = FRotationMatrix::MakeFromX(PDir).Rotator();
		*/
	
		CharState = EPlayerStates::E_ABILITY;
		//PlayAnimationServer(RPGAnimation);
		//SetActorRotation(LookAtRotation);
		//ParentProxy->MoveToLocation(GetActorLocation());
		//GetMovementComponent()->Deactivate(); 

		float HalfRange = NukeRange/2;
		for(int i = 0; i < NukesCount; i++)
		{
			FVector NukeStartLoc = Enemy->GetActorLocation();
			NukeStartLoc.Z += 1500;
			NukeStartLoc.X += FMath::FRandRange(-HalfRange,HalfRange);
			NukeStartLoc.Y += FMath::FRandRange(-HalfRange,HalfRange);
			SpawnNuke(NukeStartLoc,FMath::FRandRange(MinMaxDelayPerSpawn.X,MinMaxDelayPerSpawn.Y));
		}

		//After Ability Usage
		UltimateCD = AttackCooldowns.Ultimate;
		Mana -= AttackManaConsumption.Ultimate;
		bUsingAbility = false;
		ManaCD = ManaCDOnSkillUse;
		CharState = EPlayerStates::E_IDLE;
		GetMovementComponent()->Activate();
		//StopAnimationServer(ShotgunAnimation);
	}
}

void AChar_Moss::SpawnNuke(FVector Location,float Delay)
{
	FTimerDelegate NukeTime;
	NukeTime.BindLambda([Location, this]()
	{
		AExplosiveProjectile* Nuke = Cast<AExplosiveProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this,NukeProjectile,FTransform(FRotationMatrix::MakeFromX(-GetActorUpVector()).Rotator(),Location),ESpawnActorCollisionHandlingMethod::AlwaysSpawn,this));
			if(Nuke != nullptr)
			{
				//Finalizing Create Projecitle
				Nuke->ProjectileOwner = this;
				Nuke->InitVelocity = NukeSpeed;
				Nuke->Damage = NukeDamage * DamageMultiplier;
				Nuke->SetOwner(this);
						
				//Spawn The Actor
				UGameplayStatics::FinishSpawningActor(Nuke, FTransform(FRotationMatrix::MakeFromX(-GetActorUpVector()).Rotator(),Location));
			}
	});
	FTimerHandle NukeHandleSpawner;
	GetWorld()->GetTimerManager().SetTimer(NukeHandleSpawner, NukeTime, Delay, false);
}
