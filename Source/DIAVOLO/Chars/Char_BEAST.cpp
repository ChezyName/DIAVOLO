// Fill out your copyright notice in the Description page of Project Settings.

#include "Char_BEAST.h"

#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "PhysXInterfaceWrapperCore.h"
#include "DIAVOLO/Projectiles/CallBackProjectile.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void AChar_BEAST::PlayClawTeleportFX_Implementation(FVector Location)
{
	if(IsValid(TeleportFX))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),TeleportFX,Location,FRotator::ZeroRotator);
	}
}

void AChar_BEAST::Tick(float DeltaSeconds)
{
	TPDelay -= DeltaSeconds;

	if(Grappling)
	{
		FVector Direction = (toLoc - GetActorLocation()).GetSafeNormal();
		FVector Velocity = Direction * GrappleSpeed;
		GetCharacterMovement()->Launch(Velocity);

		DrawDebugDirectionalArrow(GetWorld(),GetActorLocation(),
					Velocity,5,FColor::Cyan,false,-1,0,5);
		
		if(FVector::Dist(GetActorLocation(),toLoc) < GrappleCloseEnough)
		{
			Grappling = false;
			GrappleOut = false;
			Grappling = false;
			toLoc = FVector::ZeroVector;
			CharState = EPlayerStates::E_IDLE;
		}
	}
	
	if(GrappleOut && !Grappling)
	{
		GrappleOutT -= DeltaSeconds;
		if(GrappleOutT < 0)
		{
			GrappleOut = false;
			Grappling = false;
			toLoc = FVector::ZeroVector;
			CharState = EPlayerStates::E_IDLE;
		}
	}
	
	Super::Tick(DeltaSeconds);
}

void AChar_BEAST::onSkill1(FVector Location, AEnemy* Enemy)
{
	if(Mana < AttackManaConsumption.Skill1) return;
	GEngine->AddOnScreenDebugMessage(-1,60,FColor::Emerald, GetActorLocation().ToString() + " // " + Location.ToString());

	FVector MyLoc = Location;
	MyLoc.Z = GetActorLocation().Z;
	FVector PDir = MyLoc - GetActorLocation();
	PDir.Normalize();

	LookAtRotation = FRotationMatrix::MakeFromX(PDir).Rotator();

	if(IsValid(Claw) && bSwapped == false && Claw->Destroying == false && TPDelay < 0)
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
		Mana -= AttackManaConsumption.Skill1;
		ManaCD = ManaCDOnSkillUse;
	}
	else
	{
		if(Skill1CD > 0 || bDoing) return;
		bDoing = true;
		CharState = EPlayerStates::E_ABILITY;
		
		SetActorRotation(LookAtRotation);
		
		if(ClawAnimation) PlayAnimationServer(ClawAnimation);
		GetMovementComponent()->SetActive(false);

		//Wait Windup To Deal Damage
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([&]
		{
			bSwapped = false;
		
			//Create Projectile Facing Enemy
			Claw = Cast<ABaseProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this,ClawProjectile,FTransform(LookAtRotation,GetActorLocation()),ESpawnActorCollisionHandlingMethod::AlwaysSpawn,this));
			if(Claw != nullptr)
			{
				//Finalizing Create Projecitle
				Claw->ProjectileOwner = this;
				Claw->InitVelocity = ClawVelocity;
				Claw->Damage = ClawDamage * DamageMultiplier;
				Claw->SetOwner(this);
						
				//Spawn The Actor
				UGameplayStatics::FinishSpawningActor(Claw, FTransform(LookAtRotation,GetActorLocation()));
			}

			TPDelay = ClawTeleportDelay;
			Skill1CD = AttackCooldowns.Skill1;
			Mana -= AttackManaConsumption.Skill1;
			ManaCD = ManaCDOnSkillUse;
			GetMovementComponent()->SetActive(true);
			bDoing = false;
			CharState = EPlayerStates::E_IDLE;
		});
		
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, ClawSpawnDelayAnimation, false);
	}
	
	Super::onSkill1_Implementation(Location, Enemy);
}

void AChar_BEAST::onSkill3(FVector Location, AEnemy* Enemy)
{
	if(Mana < AttackManaConsumption.Skill3 || Skill3CD > 0) return;
	FVector MyLoc = Location;
	MyLoc.Z = GetActorLocation().Z;
	FVector PDir = MyLoc - GetActorLocation();
	PDir.Normalize();

	FRotator NewLookAtRotation = FRotationMatrix::MakeFromX(PDir).Rotator();
	
	Grapple = Cast<ACallBackProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this,GrappleProjectile,FTransform(NewLookAtRotation,GetActorLocation()),ESpawnActorCollisionHandlingMethod::AlwaysSpawn,this));
	if(Grapple != nullptr)
	{
		//Finalizing Create Projecitle
		Grapple->ProjectileOwner = this;
		Grapple->InitVelocity = GrappleVelocity;
		Grapple->Damage = 0;
		Grapple->SetOwner(this);
		Grapple->InitialLifeSpan = GrappleLifetime;

		GrappleOut = true;
		GrappleOutT = GrappleLifetime;

		Grapple->FunctionOnOverlap.BindUFunction(this,FName("onGrappleHit"));

		CharState = EPlayerStates::E_ABILITY;

		Skill3CD = AttackCooldowns.Skill3;
		Mana -= AttackManaConsumption.Skill3;
		ManaCD = ManaCDOnSkillUse;
						
		//Spawn The Actor
		UGameplayStatics::FinishSpawningActor(Grapple, FTransform(NewLookAtRotation,GetActorLocation()));
	}
	Super::onSkill3(Location, Enemy);
}

void AChar_BEAST::onGrappleHit(FVector HitImpact, AEnemy* EnemyHit)
{
	GEngine->AddOnScreenDebugMessage(-1,25,FColor::Orange,
		"Grapple Hit At Location!");
	
	DrawDebugSphere(GetWorld(),HitImpact,50,32,FColor::Orange,
		false,25,0,2);
	
	DrawDebugLine(GetWorld(),GetActorLocation(),HitImpact,
		FColor::Orange, false, 25, 0, 2);

	//Grappling
	Grappling = true;
	toLoc = HitImpact;
	
	CharState = EPlayerStates::E_ABILITY;
	Grapple->Destroy();
}
