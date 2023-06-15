// Copyright Epic Games, Inc. All Rights Reserved.

#include "DIAVOLOCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

ADIAVOLOCharacter::ADIAVOLOCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 1700.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	bReplicates = true;
	GetMovementComponent()->SetIsReplicated(true);
	GetMesh()->SetIsReplicated(true);
}

void ADIAVOLOCharacter::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
	Mana = MaxMana;
}

void ADIAVOLOCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
	
	BasicCD -= DeltaSeconds;
	Skill1CD -= DeltaSeconds;
	Skill2CD -= DeltaSeconds;
	Skill3CD -= DeltaSeconds;
	Skill4CD -= DeltaSeconds;
	UltimateCD -= DeltaSeconds;

	//if(HasAuthority()) GEngine->AddOnScreenDebugMessage(-1,0,FColor::Red, "SERVER ON " + GetController()->GetName());
}

void ADIAVOLOCharacter::MoveToRange(FVector Position, float Range)
{
	const FVector Player = GetActorLocation();
	FVector Dir = Position - Player;
	Dir.Normalize();

	const FVector Target = Position - (Dir * Range);
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), Target);
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
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Enemy->GetActorLocation()));
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
}
void ADIAVOLOCharacter::onSkill1_Implementation(AEnemy* Enemy)
{
}
void ADIAVOLOCharacter::onSkill2_Implementation(AEnemy* Enemy)
{
}
void ADIAVOLOCharacter::onSkill3_Implementation(AEnemy* Enemy)
{
}
void ADIAVOLOCharacter::onSkill4_Implementation(AEnemy* Enemy)
{
}
void ADIAVOLOCharacter::onUltimate_Implementation(AEnemy* Enemy)
{
}

void ADIAVOLOCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(ADIAVOLOCharacter,Health);
	DOREPLIFETIME(ADIAVOLOCharacter,Mana);

	DOREPLIFETIME(ADIAVOLOCharacter,CharState);
	
	DOREPLIFETIME(ADIAVOLOCharacter,BasicCD);
	DOREPLIFETIME(ADIAVOLOCharacter,Skill1CD);
	DOREPLIFETIME(ADIAVOLOCharacter,Skill2CD);
	DOREPLIFETIME(ADIAVOLOCharacter,Skill3CD);
	DOREPLIFETIME(ADIAVOLOCharacter,Skill4CD);
	DOREPLIFETIME(ADIAVOLOCharacter,UltimateCD);
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ADIAVOLOCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Zoom",this,&ADIAVOLOCharacter::ZoomCamera);
}

void ADIAVOLOCharacter::ZoomCamera(float Speed)
{
	CameraBoom->TargetArmLength = FMath::Clamp(CameraBoom->TargetArmLength - (Speed*ZoomSpeed),ZoomMin,ZoomMax);
	//GEngine->AddOnScreenDebugMessage(-1,1,FColor::Green,FString::SanitizeFloat(FMath::Clamp(CameraBoom->TargetArmLength - (Speed*ZoomSpeed),ZoomMin,ZoomMax)));
}