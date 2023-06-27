// Fill out your copyright notice in the Description page of Project Settings.

#include "Char_BEAST.h"

#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "PhysXInterfaceWrapperCore.h"
#include "DIAVOLO/CharacterProxy.h"
#include "DIAVOLO/Projectiles/CallBackProjectile.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

AChar_BEAST::AChar_BEAST()
{
	SpinHitbox = CreateDefaultSubobject<USphereComponent>("SpinHitbox");
	SpinHitbox->SetupAttachment(RootComponent);
	SpinHitbox->InitSphereRadius(250);

	SpinVFX = CreateDefaultSubobject<UStaticMeshComponent>("SpinVFX");
	SpinVFX->SetupAttachment(RootComponent);
	SpinVFX->SetCollisionProfileName("NoCollision");
	SpinVFX->SetIsReplicated(true);

	SpinVFX2 = CreateDefaultSubobject<UStaticMeshComponent>("SpinVFX2");
	SpinVFX2->SetupAttachment(RootComponent);
	SpinVFX2->SetCollisionProfileName("NoCollision");
	SpinVFX2->SetIsReplicated(true);

	//Ultimate Creations
	UltSpinParent = CreateDefaultSubobject<USceneComponent>("UltParent");
	UltSpinParent->SetupAttachment(RootComponent);

	//Children
	UltRazor1 = CreateDefaultSubobject<UStaticMeshComponent>("Razor 1");
	UltRazor1->SetupAttachment(UltSpinParent);

	UltRazor2 = CreateDefaultSubobject<UStaticMeshComponent>("Razor 2");
	UltRazor2->SetupAttachment(UltSpinParent);

	UltRazor3 = CreateDefaultSubobject<UStaticMeshComponent>("Razor 3");
	UltRazor3->SetupAttachment(UltSpinParent);

	UltRazor4 = CreateDefaultSubobject<UStaticMeshComponent>("Razor 4");
	UltRazor4->SetupAttachment(UltSpinParent);

	//Collision (CHILDREN)
	UltRazor1->SetCollisionProfileName("OverlapAll");
	UltRazor2->SetCollisionProfileName("OverlapAll");
	UltRazor3->SetCollisionProfileName("OverlapAll");
	UltRazor4->SetCollisionProfileName("OverlapAll");
	
	UltRazor1->SetGenerateOverlapEvents(true);
	UltRazor1->OnComponentBeginOverlap.AddDynamic(this, &AChar_BEAST::UltimateOverlap);
	UltRazor2->SetGenerateOverlapEvents(true);
	UltRazor2->OnComponentBeginOverlap.AddDynamic(this, &AChar_BEAST::UltimateOverlap);
	UltRazor3->SetGenerateOverlapEvents(true);
	UltRazor3->OnComponentBeginOverlap.AddDynamic(this, &AChar_BEAST::UltimateOverlap);
	UltRazor4->SetGenerateOverlapEvents(true);
	UltRazor4->OnComponentBeginOverlap.AddDynamic(this, &AChar_BEAST::UltimateOverlap);

	ChangeRazorLocation();
}

void AChar_BEAST::ChangeRazorLocation_Implementation()
{
	UltRazor1->SetRelativeLocation(FVector(1*UltRadius,0,0));
	UltRazor2->SetRelativeLocation(FVector(-1*UltRadius,0,0));
	UltRazor3->SetRelativeLocation(FVector(0,1*UltRadius,0));
	UltRazor4->SetRelativeLocation(FVector(0,-1*UltRadius,0));
}

void AChar_BEAST::PlayClawTeleportFX_Implementation(FVector Location)
{
	if(IsValid(TeleportFX))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),TeleportFX,Location,FRotator::ZeroRotator);
	}
}

void AChar_BEAST::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AChar_BEAST,SpinActive);
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AChar_BEAST::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	ChangeRazorLocation();
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void AChar_BEAST::BeginPlay()
{
	SpinVFX->SetVisibility(false);
	SpinVFX2->SetVisibility(false);
	Super::BeginPlay();
}

void AChar_BEAST::Tick(float DeltaSeconds)
{
	TPDelay -= DeltaSeconds;
	WaitMinSpin -= DeltaSeconds;

	UltSpinParent->AddLocalRotation(UltSpinSpeed*DeltaSeconds);
	UltRazor1->AddLocalRotation(UltRazorSpinSpeed*DeltaSeconds);
	UltRazor2->AddLocalRotation(UltRazorSpinSpeed*DeltaSeconds);
	UltRazor3->AddLocalRotation(UltRazorSpinSpeed*DeltaSeconds);
	UltRazor4->AddLocalRotation(UltRazorSpinSpeed*DeltaSeconds);

	SpinVFX->AddLocalRotation(SpinSpeed*DeltaSeconds);
	SpinVFX->SetVisibility(SpinActive);

	SpinVFX2->AddLocalRotation(SpinSpeed*(-DeltaSeconds));
	SpinVFX2->SetVisibility(SpinActive);

	if(StopOnMinSpin && WaitMinSpin < 0) endSkill2();

	if(SpinActive)
	{
		//Spinning, Deal Damage
		DelayPerTickSpin -= DeltaSeconds;
		if(DelayPerTickSpin < 0)
		{
			if(Mana >= AttackManaConsumption.Skill2)
			{
				Mana -= AttackManaConsumption.Skill2;
				ManaCD = ManaCDOnSkillUse;
				DelayPerTickSpin = SpinDamageTick;
				TArray<AActor*> OverlappingActors;
				SpinHitbox->GetOverlappingActors(OverlappingActors);
				for (AActor* Actor : OverlappingActors)
				{
					// Do something with each overlapping actor
					AEnemy* myEnemy = Cast<AEnemy>(Actor);
					if(myEnemy) myEnemy->Damage(SpinDamage);
				}
			}
			else endSkill2();
		}
	}

	if(Grappling && Grapple != nullptr)
	{
		FVector Direction = (toLoc - GetActorLocation()).GetSafeNormal();
		FVector Velocity = Direction * (GrapplePullSpeed*100);
		GetCharacterMovement()->Launch(Velocity);

		DrawDebugDirectionalArrow(GetWorld(),GetActorLocation(),
					Velocity,5,FColor::Cyan,false,-1,0,5);

		//Look at GrappleHook
		FVector TargetDirection = Grapple->GetActorLocation() - GetActorLocation();
		TargetDirection.Normalize();
		FRotator TargetRotation = TargetDirection.Rotation();
		SetActorRotation(TargetRotation);
		
		if(FVector::Dist(GetActorLocation(),toLoc) < GrappleCloseEnough)
		{
			if(GrappleStart) StopAnimationServer(GrappleStart);
			if(GrapplePull) StopAnimationServer(GrapplePull);
			Grappling = false;
			GrappleOut = false;
			Grappling = false;
			toLoc = FVector::ZeroVector;
			CharState = EPlayerStates::E_IDLE;
			bUsingAbility = false;
			if(Grapple) Grapple->Destroy();
			Skill3CD = AttackCooldowns.Skill3;
		}
	}
	
	if(GrappleOut && Grapple != nullptr)
	{
		GrappleOutT -= DeltaSeconds;

		//Look at GrappleHook
		FVector TargetDirection = Grapple->GetActorLocation() - GetActorLocation();
		TargetDirection.Normalize();
		FRotator TargetRotation = TargetDirection.Rotation();
		SetActorRotation(TargetRotation);
		
		if(GrappleOutT < 0)
		{
			if(GrappleStart) StopAnimationServer(GrappleStart);
			if(GrapplePull) StopAnimationServer(GrapplePull);
			GrappleOut = false;
			Grappling = false;
			toLoc = FVector::ZeroVector;
			CharState = EPlayerStates::E_IDLE;
			bUsingAbility = false;
			if(Grapple) Grapple->Destroy();
			Skill3CD = AttackCooldowns.Skill3;
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
		//Stop Movement
		ParentProxy->MoveToLocation(GetActorLocation());
		
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
		//Skill1CD = AttackCooldowns.Skill1;
		hasDoneCD = true;
	}
	else
	{
		if(Skill1CD > 0 || bDoing) return;
		bDoing = true;
		CharState = EPlayerStates::E_ABILITY;
		bUsingAbility = true;
		hasDoneCD = false;
		
		//Stop Movement
		ParentProxy->MoveToLocation(GetActorLocation());
		
		SetActorRotation(LookAtRotation);
		
		if(ClawAnimation) PlayAnimationServer(ClawAnimation);
		GetMovementComponent()->SetActive(false);

		//Wait Windup To Deal Damage
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([&]
		{
			bSwapped = false;
		
			//Create Projectile Facing Enemy
			Claw = Cast<ACallBackProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this,ClawProjectile,FTransform(LookAtRotation,GetActorLocation()),ESpawnActorCollisionHandlingMethod::AlwaysSpawn,this));
			if(Claw != nullptr)
			{
				//Finalizing Create Projecitle
				Claw->ProjectileOwner = this;
				Claw->InitVelocity = ClawVelocity;
				Claw->Damage = ClawDamage * DamageMultiplier;
				Claw->SetOwner(this);
				Claw->FunctionOnDestroy.BindUFunction(this,FName("EndGrappleCallback"));
						
				//Spawn The Actor
				UGameplayStatics::FinishSpawningActor(Claw, FTransform(LookAtRotation,GetActorLocation()));
				bSpawned = true;
			}

			TPDelay = ClawTeleportDelay;
			Mana -= AttackManaConsumption.Skill1;
			ManaCD = ManaCDOnSkillUse;
			GetMovementComponent()->SetActive(true);
			bDoing = false;
			CharState = EPlayerStates::E_IDLE;
			bUsingAbility = false;
		});
		
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, ClawSpawnDelayAnimation, false);
	}
	
	Super::onSkill1_Implementation(Location, Enemy);
}

void AChar_BEAST::EndGrappleCallback()
{
	if(bSpawned && hasDoneCD == false)
	{
		GEngine->AddOnScreenDebugMessage(-1,1,FColor::Black,"Reset Cooldown SK1");
		Skill1CD = AttackCooldowns.Skill1 + 5;
		hasDoneCD = true;
		bDoing = false;
		bSpawned = false;
	}
}

void AChar_BEAST::onSkill2(FVector Location, AEnemy* Enemy)
{
	//Pre Ability
	if(Mana < AttackManaConsumption.Skill2 || Skill2CD > 0) return;
	CharState = EPlayerStates::E_ABILITY;
	bUsingAbility = true;

	//Stop Movement
	ParentProxy->MoveToLocation(GetActorLocation());
	SpinActive = true;
	GetMesh()->SetVisibility(false);
	bEndedEarly = false;
	StopOnMinSpin = false;
	WaitMinSpin = MINSpinDuration;

	//End Ability
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([&]
	{
		if(bEndedEarly) return;
		bEndedEarly = true;
		SpinActive = false;
		Skill2CD = AttackCooldowns.Skill2;
		Mana -= AttackManaConsumption.Skill2;
		CharState = EPlayerStates::E_IDLE;
		bUsingAbility = false;
		ManaCD = ManaCDOnSkillUse;
		GetMesh()->SetVisibility(true);
	});

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, MAXSpinDuration, false);
	Super::onSkill2(Location, Enemy);
}

void AChar_BEAST::endSkill2()
{
	if(bEndedEarly) return;
	if(WaitMinSpin > 0)
	{
		StopOnMinSpin = true;
	}
	else
	{
		bEndedEarly = true;
		SpinActive = false;
		Skill2CD = AttackCooldowns.Skill2;
		Mana -= AttackManaConsumption.Skill2;
		CharState = EPlayerStates::E_IDLE;
		bUsingAbility = false;
		ManaCD = ManaCDOnSkillUse;
		GetMesh()->SetVisibility(true);
	}
	Super::endSkill2();
}

void AChar_BEAST::onSkill3(FVector Location, AEnemy* Enemy)
{
	if(Mana < AttackManaConsumption.Skill3 || Skill3CD > 0) return;
	FVector MyLoc = Location;
	MyLoc.Z = GetActorLocation().Z;
	FVector PDir = MyLoc - GetActorLocation();
	PDir.Normalize();

	FRotator NewLookAtRotation = FRotationMatrix::MakeFromX(PDir).Rotator();
	SetActorRotation(NewLookAtRotation);
	ParentProxy->MoveToLocation(GetActorLocation());
	
	Grapple = Cast<ACallBackProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this,GrappleProjectile,FTransform(NewLookAtRotation,GetActorLocation()),ESpawnActorCollisionHandlingMethod::AlwaysSpawn,this));
	if(Grapple != nullptr)
	{
		if(GrappleStart) PlayAnimationServer(GrappleStart);
		
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
		bUsingAbility = true;
		
		Mana -= AttackManaConsumption.Skill3;
		ManaCD = ManaCDOnSkillUse;
						
		//Spawn The Actor
		UGameplayStatics::FinishSpawningActor(Grapple, FTransform(NewLookAtRotation,GetActorLocation()));
	}
	Super::onSkill3(Location, Enemy);
}

void AChar_BEAST::onGrappleHit(FVector HitImpact, AEnemy* EnemyHit)
{
	/*
	GEngine->AddOnScreenDebugMessage(-1,25,FColor::Orange,
		"Grapple Hit At Location!");
	
	DrawDebugSphere(GetWorld(),HitImpact,50,32,FColor::Orange,
		false,25,0,2);
	
	DrawDebugLine(GetWorld(),GetActorLocation(),HitImpact,
		FColor::Orange, false, 25, 0, 2);
	*/
	
	DrawDebugSphere(GetWorld(),HitImpact,50,32,FColor::Orange,
		false,25,0,2);

	//Grappling
	GrappleOutT = GrappleLifetime;
	Grappling = true;
	toLoc = HitImpact;
	
	if(GrappleStart) StopAnimationServer(GrappleStart);
	if(GrapplePull) PlayAnimationServer(GrapplePull);
	
	CharState = EPlayerStates::E_ABILITY;
	bUsingAbility = true;
	Grapple->EnableProjectileMovement(false);
};


void AChar_BEAST::UltimateOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	DrawDebugBox(GetWorld(),SweepResult.ImpactPoint,FVector(8,8,8),FColor::Black,
		0,60,0,4);
}
