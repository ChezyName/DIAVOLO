// Copyright Epic Games, Inc. All Rights Reserved.

#include "DIAVOLOCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"


void ADIAVOLOCharacter::DodgeRoll_Implementation(FVector MouseLocation)
{
	if(bisDodging || bUsingAbility) return;
	bisDodging = true;
	bUsingAbility = true;
	PlayAnimationClient(DodgeAnim);
	PlaySoundSingle(DodgeSound);

	//FVector TargetDirection = MouseLocation - GetActorLocation();
	FVector TargetDirection = MouseLocation;
	TargetDirection.Normalize();
	FRotator TargetRotation = TargetDirection.Rotation();
	SetActorRotation(TargetRotation);
	DodgeDirection = TargetDirection;

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([&]
	{
		bisDodging = false;
	});

	FTimerDelegate TimerDelegateB;
	TimerDelegateB.BindLambda([&]
	{
		StopAnimationClient(DodgeAnim);
		bisDodging = false;
		bUsingAbility = false;
	});

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, DodgeFrames, false);

	FTimerHandle TimerHandleB;
	GetWorld()->GetTimerManager().SetTimer(TimerHandleB, TimerDelegateB, DodgeFrames+RestFrames, false);
}

ADIAVOLOCharacter::ADIAVOLOCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	//GetCharacterMovement()->bOrientRotationToMovement = false; // Rotate character to moving direction
	//GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	//GetCharacterMovement()->bConstrainToPlane = true;
	//GetCharacterMovement()->bSnapToPlaneAtStart = true;

	//GetCharacterMovement()->GravityScale = 2.f;
	//GetCharacterMovement()->JumpZVelocity = 650;
	//GetCharacterMovement()->AirControl = 0.65f;
	//GetCharacterMovement()->bRunPhysicsWithNoController = true;
	
	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 2000.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	EmotePlayer = CreateDefaultSubobject<UAudioComponent>("EmoteSFX");
	EmotePlayer->SetupAttachment(GetMesh());
	EmotePlayer->SetAutoActivate(false);
	EmotePlayer->SetIsReplicated(true);

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	bReplicates = true;
	//GetMovementComponent()->SetIsReplicated(true);
	GetMesh()->SetIsReplicated(true);
}

void ADIAVOLOCharacter::CharacterTakeDamage_Implementation(float DamageAmount, bool IgnoreIFrames)
{
	if((bisDodging || IFRAMES) && !IgnoreIFrames)
	{
		//Do Effect
		return;
	}
	Health -= DamageAmount;
	if(Health <= 0)
	{
		GEngine->AddOnScreenDebugMessage(-1,25,FColor::Red,"Character Died, Running Back Numbers!");
		OnDeathFunction.ExecuteIfBound();
		onDeath();
		ClientDeathNonBP();
	}
}

void ADIAVOLOCharacter::ClientDeathNonBP_Implementation()
{
	ClientDeath();
}

void ADIAVOLOCharacter::BeginPlay()
{
	Super::BeginPlay();
	if(HasAuthority())
	{
		Health = MaxHealth;
		Mana = MaxMana;
	}
}

void ADIAVOLOCharacter::PossessedBy(AController* NewController)
{
	if(NewController)
	{
		GEngine->AddOnScreenDebugMessage(-1,60,FColor::Red,"This Pawn Controller: " + NewController->GetName());
	}
	Super::PossessedBy(NewController);
}

void ADIAVOLOCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
	
	if(HasAuthority())
	{
		BasicCD -= DeltaSeconds;
		Skill1CD -= DeltaSeconds;
		Skill2CD -= DeltaSeconds;
		Skill3CD -= DeltaSeconds;
		UltimateCD -= DeltaSeconds;
		ManaCD -= DeltaSeconds;
		if(ManaCD < 0 && Mana < MaxMana)
		{
			Mana += FMath::Max(AttackManaConsumption.ManaRechargeRate * (1-(Mana/MaxMana)) * 2,
				AttackManaConsumption.ManaRechargeRate) * DeltaSeconds;
		}
		GEngine->AddOnScreenDebugMessage(-1,0,FColor::Magenta,"Mana: " +
			FString::SanitizeFloat(Mana) + ". CD: " + FString::SanitizeFloat(ManaCD));

		if(bisDodging)
		{
			GetCharacterMovement()->Launch(DodgeDirection * 1500);
			FVector TargetDirection = DodgeDirection;
			TargetDirection.Z = 0.0f;
			TargetDirection.Normalize();

			// Calculate the yaw angle between the character's forward vector and the target direction
			FRotator TargetYawRotation = TargetDirection.Rotation();
			FRotator CharacterYawRotation = GetActorForwardVector().Rotation();
			float YawAngle = TargetYawRotation.Yaw - CharacterYawRotation.Yaw;

			// Set the character's yaw rotation to face the target direction
			FRotator NewRotation = FRotator(0.0f, YawAngle, 0.0f);
			AddActorLocalRotation(NewRotation);
		}

		FRotator CharRot = GetActorRotation();
		CharRot.Pitch = 0;
		CharRot.Roll = 0;
		SetActorRotation(CharRot);
	}

	//if(HasAuthority()) GEngine->AddOnScreenDebugMessage(-1,0,FColor::Red, "SERVER ON " + GetController()->GetName());
}

float ADIAVOLOCharacter::GetHealthPercent(){ return Health / MaxHealth; }
float ADIAVOLOCharacter::GetManaPercent(){ return Mana / MaxMana; }

void ADIAVOLOCharacter::MoveToRange(FVector Position, float Range)
{
	const FVector Player = GetActorLocation();
	FVector Dir = Position - Player;
	Dir.Normalize();

	const FVector Target = Position - (Dir * Range);
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), Target);
}

void ADIAVOLOCharacter::onDeath()
{
	GetCharacterMovement()->DisableMovement();

	GetCapsuleComponent()->SetEnableGravity(false);
	GetCapsuleComponent()->SetCollisionProfileName("OverlapAll");
	GetCapsuleComponent()->Deactivate();

	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	isDead = true;
}

void ADIAVOLOCharacter::PlaySoundSingle_Implementation(USoundWave* SFX)
{
	UGameplayStatics::PlaySound2D(GetWorld(),SFX);
}
void ADIAVOLOCharacter::PlaySound_Implementation(const TArray<USoundWave*>& SFXs)
{
	if(SFXs.Num() > 0)
	{
		USoundWave* RandomSound = SFXs[
			FMath::RandRange(0,SFXs.Num() - 1)];
		UGameplayStatics::PlaySound2D(GetWorld(),RandomSound);
	}
} 

void ADIAVOLOCharacter::ServerSetState_Implementation(EPlayerStates State)
{
	CharState = State;
}

void ADIAVOLOCharacter::PlayAnimationServer_Implementation(UAnimMontage* Animation)
{
	//GEngine->AddOnScreenDebugMessage(-1,80,FColor::Yellow,"Server Play Animation");
	PlayAnimMontage(Animation);
	PlayAnimationClient(Animation);
}

void ADIAVOLOCharacter::StopAnimationServer_Implementation(UAnimMontage* Animation)
{
	//GEngine->AddOnScreenDebugMessage(-1,80,FColor::Yellow,"Server Stop Animation");
	StopAnimMontage(Animation);
	StopAnimationClient(Animation);
}

void ADIAVOLOCharacter::PlayAnimationClient_Implementation(UAnimMontage* Animation)
{
	//GEngine->AddOnScreenDebugMessage(-1,80,FColor::Yellow,"Client Play Animation");
	PlayAnimMontage(Animation);
}

void ADIAVOLOCharacter::StopAnimationClient_Implementation(UAnimMontage* Animation)
{
	//GEngine->AddOnScreenDebugMessage(-1,80,FColor::Yellow,"Client Stop Animation");
	StopAnimMontage(Animation);
}

void ADIAVOLOCharacter::onBasicSkill_Implementation(AEnemy* Enemy)
{
	if(bUsingAbility) return;
	bUsingAbility = true;
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Enemy->GetActorLocation()));
	PlaySoundSingle(AutoAttack.AttackSFX);
	if(AutoAttack.AutoType == EAutoType::E_MELEE)
	{
		Enemy->Damage(AutoAttack.AttackDamage * DamageMultiplier);
	}
	else
	{
		//Create Projectile Facing Enemy
		ABaseProjectile* Projectile = Cast<ABaseProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this,AutoAttack.Projectile,FTransform(FRotator::ZeroRotator,GetActorLocation()),ESpawnActorCollisionHandlingMethod::AlwaysSpawn,this));
		if(Projectile != nullptr)
		{
			//Finalizing Create Projecitle
			Projectile->ProjectileOwner = this;
			Projectile->InitVelocity = AutoAttack.ProjectileVelocity;
			Projectile->Damage = AutoAttack.AttackDamage * DamageMultiplier;
			Projectile->SetOwner(this);

			//Calculate Rotation
			FVector Direction = Enemy->GetActorLocation() - GetActorLocation();
			Direction.Normalize();

			FRotator LookAtRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
						
			//Spawn The Actor
			UGameplayStatics::FinishSpawningActor(Projectile, FTransform(LookAtRotation,GetActorLocation()));
		}
	}

	//Reset After Time After For Basic Attack
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([&]
	{
		bUsingAbility = false;
	});
		
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, AutoAttack.TimeAfterAttack, false);
}
void ADIAVOLOCharacter::onSkill1_Implementation(FVector Location,AEnemy* Enemy)
{
	if(isDead) return;
}
void ADIAVOLOCharacter::onSkill2_Implementation(FVector Location,AEnemy* Enemy)
{
	if(isDead) return;
}
void ADIAVOLOCharacter::onSkill3_Implementation(FVector Location,AEnemy* Enemy)
{
	if(isDead) return;
}
void ADIAVOLOCharacter::onUltimate_Implementation(FVector Location,AEnemy* Enemy)
{
	if(isDead) return;
}

void ADIAVOLOCharacter::endSkill1_Implementation()
{
}
void ADIAVOLOCharacter::endSkill2_Implementation()
{
}
void ADIAVOLOCharacter::endSkill3_Implementation()
{
}
void ADIAVOLOCharacter::endUltimate_Implementation()
{
}


void ADIAVOLOCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(ADIAVOLOCharacter,Health);
	DOREPLIFETIME(ADIAVOLOCharacter,Mana);

	DOREPLIFETIME(ADIAVOLOCharacter,CharState);
	DOREPLIFETIME(ADIAVOLOCharacter,bUsingAbility);
	
	DOREPLIFETIME(ADIAVOLOCharacter,BasicCD);
	DOREPLIFETIME(ADIAVOLOCharacter,Skill1CD);
	DOREPLIFETIME(ADIAVOLOCharacter,Skill2CD);
	DOREPLIFETIME(ADIAVOLOCharacter,Skill3CD);
	DOREPLIFETIME(ADIAVOLOCharacter,UltimateCD);

	DOREPLIFETIME(ADIAVOLOCharacter,Skill1Active);
	DOREPLIFETIME(ADIAVOLOCharacter,Skill2Active);
	DOREPLIFETIME(ADIAVOLOCharacter,Skill3Active);
	DOREPLIFETIME(ADIAVOLOCharacter,UltimateActive);
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ADIAVOLOCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Zoom",this,&ADIAVOLOCharacter::ZoomCamera);
	PlayerInputComponent->BindAxis("UpMovement",this,&ADIAVOLOCharacter::MoveForward);
	PlayerInputComponent->BindAxis("RightMovement",this,&ADIAVOLOCharacter::MoveRight);
}

void ADIAVOLOCharacter::MoveForward(float Value)
{
	if (Value != 0.0f && !bUsingAbility && !bisDodging)
	{
		// add movement in that direction
		AddMovementInput(FVector::ForwardVector, Value);
		StopEmote();
	}
}

void ADIAVOLOCharacter::MoveRight(float Value)
{
	if (Value != 0.0f && !bUsingAbility && !bisDodging)
	{
		// add movement in that direction
		AddMovementInput(FVector::RightVector, Value);
		StopEmote();
	}
}

void ADIAVOLOCharacter::ZoomCamera(float Speed)
{
	CameraBoom->TargetArmLength = FMath::Clamp(CameraBoom->TargetArmLength - (Speed*ZoomSpeed),ZoomMin,ZoomMax);
	//GEngine->AddOnScreenDebugMessage(-1,1,FColor::Green,FString::SanitizeFloat(FMath::Clamp(CameraBoom->TargetArmLength - (Speed*ZoomSpeed),ZoomMin,ZoomMax)));
}

void ADIAVOLOCharacter::StartEmote_Implementation()
{
	startEmoteSound();
	PlayAnimationServer(Emote);
}
void ADIAVOLOCharacter::StopEmote_Implementation()
{
	stopEmoteSound();
	StopAnimationServer(Emote);
}

void ADIAVOLOCharacter::startEmoteSound_Implementation()
{
	EmotePlayer->Play();
}


void ADIAVOLOCharacter::stopEmoteSound_Implementation()
{
	EmotePlayer->Stop();
}

//===========================================================================================
//                                MOVEMENT

/*
void ADIAVOLOCharacter::SetUpMovement(float Value)
{
	GEngine->AddOnScreenDebugMessage(-1,0,FColor::Magenta,"Movement Y: " + FString::SanitizeFloat(Value));
	if(Value != 0.0f)
	{
		AddMovement(FVector::ForwardVector * Value);
	}
	//SetRightMovementS(Value);
}

void ADIAVOLOCharacter::SetRightMovement(float Value)
{
	GEngine->AddOnScreenDebugMessage(-1,0,FColor::Magenta,"Movement X: " + FString::SanitizeFloat(Value));
	if(Value != 0.0f)
	{
		AddMovement(FVector::RightVector * Value);
	}
		
}

void ADIAVOLOCharacter::AddMovement_Implementation(FVector Direction)
{
	if (HasAuthority()) // Only execute this on the server
	{
		GEngine->AddOnScreenDebugMessage(-1,0,FColor::Green,"Velocity: " + Direction.ToString());
		GetCharacterMovement()->AddInputVector(Direction);
	}
}
*/