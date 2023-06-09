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
#include "Materials/Material.h"
#include "Engine/World.h"

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
}

void ADIAVOLOCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void ADIAVOLOCharacter::onBasicSkill()
{
}
void ADIAVOLOCharacter::onSkill1()
{
}
void ADIAVOLOCharacter::onSkill2()
{
}
void ADIAVOLOCharacter::onSkill3()
{
}
void ADIAVOLOCharacter::onSkill4()
{
}
void ADIAVOLOCharacter::onUltimate()
{
}

void ADIAVOLOCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Basic",IE_Pressed,this,&ADIAVOLOCharacter::onBasicSkill);
	PlayerInputComponent->BindAction("Skill1",IE_Pressed,this,&ADIAVOLOCharacter::onSkill1);
	PlayerInputComponent->BindAction("Skill2",IE_Pressed,this,&ADIAVOLOCharacter::onSkill2);
	PlayerInputComponent->BindAction("Skill3",IE_Pressed,this,&ADIAVOLOCharacter::onSkill3);
	PlayerInputComponent->BindAction("Skill4",IE_Pressed,this,&ADIAVOLOCharacter::onSkill4);
	PlayerInputComponent->BindAction("Ultimate",IE_Pressed,this,&ADIAVOLOCharacter::onUltimate);

	PlayerInputComponent->BindAxis("Zoom",this,&ADIAVOLOCharacter::ZoomCamera);
}

void ADIAVOLOCharacter::ZoomCamera(float Speed)
{
	CameraBoom->TargetArmLength = FMath::Clamp(CameraBoom->TargetArmLength - (Speed*ZoomSpeed),ZoomMin,ZoomMax);
	//GEngine->AddOnScreenDebugMessage(-1,1,FColor::Green,FString::SanitizeFloat(FMath::Clamp(CameraBoom->TargetArmLength - (Speed*ZoomSpeed),ZoomMin,ZoomMax)));
}