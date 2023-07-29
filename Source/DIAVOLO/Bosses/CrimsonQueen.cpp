// Fill out your copyright notice in the Description page of Project Settings.


#include "CrimsonQueen.h"

#include "DIAVOLO/DIAVOLOCharacter.h"
#include "DIAVOLO/HelperFuncs.h"
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
	/*
	int Random = FMath::RandRange(1,1);

	switch (Random)
	{
		case 0:
			Ability1();
	}
	*/
	Ability1();
}

void ACrimsonQueen::CallRandomAbilityNonMoveNeed_Implementation()
{
	Super::CallRandomAbilityNonMoveNeed_Implementation();

	int Random = FMath::RandRange(1,3);

	switch (Random)
	{
		case 1:
			Ability2();
		case 2:
			Ability3();
		case 3:
			Ability4();
	}
}

void ACrimsonQueen::CallRandomUltimate_Implementation()
{
	Super::CallRandomUltimate_Implementation();

	Ultimate1();
}

void ACrimsonQueen::BeginPlay()
{
	StartingPostion = GetActorLocation();
	Super::BeginPlay();
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
		FVector RandomDirection = FVector(FMath::FRandRange(-1,1),FMath::FRandRange(-1,1),0) * SummonDistAway;
		AChaseEnemy* NewHound = Cast<AChaseEnemy>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this,Hounds,FTransform(GetActorRotation(),GetActorLocation() + RandomDirection),ESpawnActorCollisionHandlingMethod::AlwaysSpawn,this));
		if(NewHound != nullptr)
		{
			//Spawn The Actor
			NewHound->AttackDamage = HoundBiteDamage;
			UGameplayStatics::FinishSpawningActor(NewHound, FTransform(GetActorRotation(),GetActorLocation() + RandomDirection));
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

void ACrimsonQueen::Ability4_Implementation()
{
	if(bUsingAbility) return;
	if(HellStrike.Num() < 1) return;
	bUsingAbility = true;

	TArray<FVector> RandomPositions;

	int NumPositions = FMath::RandRange(MinMaxSummonHell.X,MinMaxSummonHell.Y);
	for (int32 i = 0; i < NumPositions; ++i)
	{
		FVector NewPosition;
		bool Overlaps = true;
		int32 NumAttempts = 0;
        
		while (Overlaps && NumAttempts < 100) // Add a maximum number of attempts to avoid infinite loops
			{
			// Generate a random position within the specified range
			float RandomX = FMath::RandRange(-Range, Range);
			float RandomY = FMath::RandRange(-Range, Range);
			NewPosition = StartingPostion + FVector(RandomX, RandomY, 0.0f);
            
			// Check for overlap with existing positions
			Overlaps = false;
			for (const FVector& ExistingPosition : RandomPositions)
			{
				if (FVector::DistSquared2D(NewPosition, ExistingPosition) < FMath::Square(TooCloseRange))
				{
					Overlaps = true;
					break;
				}
			}

			++NumAttempts;
			}

		if (!Overlaps)
		{
			RandomPositions.Add(NewPosition);
			//Spawn New Object Here
			FRotator RandRot = UHelperFuncs::GetRandomRotator();
            RandRot.Pitch = 0;
            RandRot.Roll = 0;
			AActor* BeamAttack = Cast<AActor>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this,
				HellStrike[FMath::RandRange(0,HellStrike.Num()-1)],FTransform(RandRot,NewPosition),ESpawnActorCollisionHandlingMethod::AlwaysSpawn,this));
			if(BeamAttack != nullptr)
			{
				//Spawn The Actor
				UGameplayStatics::FinishSpawningActor(BeamAttack, FTransform(RandRot,NewPosition));
			}
		}
	}

	bUsingAbility = false;
}

void ACrimsonQueen::Ultimate1_Implementation()
{
	if(bUsingAbility) return;
	bUsingAbility = true;

	AActor* BeamAttack = Cast<AActor>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this,
				DeathRay,FTransform(FRotator::ZeroRotator,FVector::ZeroVector),ESpawnActorCollisionHandlingMethod::AlwaysSpawn,this));
	if(BeamAttack != nullptr)
	{
		//Spawn The Actor
		FRotator RandRot = UHelperFuncs::GetRandomRotator();
		RandRot.Pitch = 0;
		RandRot.Roll = 0;
		UGameplayStatics::FinishSpawningActor(BeamAttack, FTransform(RandRot,FVector::ZeroVector));
	}

	bUsingAbility = false;
}