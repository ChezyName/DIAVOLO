// Fill out your copyright notice in the Description page of Project Settings.
#include "CharacterProxy.h"
#include "DIAVOLOCharacter.h"
#include "DIAVOLOPlayerController.h"
#include "AIController.h"
#include "Net/UnrealNetwork.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"


// Sets default values
ACharacterProxy::ACharacterProxy(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("root"));

	// don't rotate character tp camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// config character movement
	/*GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;*/

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

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ACharacterProxy::onStartSetChar_Implementation()
{
	// get current location of player proxy
	FVector Location = GetActorLocation();
	FRotator Rotation = GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.bNoFail = true;

	// spawn actual player
	Character = Cast<ADIAVOLOCharacter>(GetWorld()->SpawnActor(CharacterClass, &Location, &Rotation, SpawnParams));

	//GEngine->AddOnScreenDebugMessage(-1,8,FColor::Turquoise,Character ? "Spawned!" : "Not Spawned?!?");

	// we use AI to control the player character for navigation
	PlayerAIController = GetWorld()->SpawnActor<AAIController>(GetActorLocation(), GetActorRotation());
	PlayerAIController->Possess(Character);
	Character->ParentProxy = this;
}

// Called when the game starts or when spawned
void ACharacterProxy::BeginPlay()
{
	if(GetLocalRole() == ENetRole::ROLE_Authority) onStartSetChar();
	Super::BeginPlay();
}

// Called every frame
void ACharacterProxy::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (Character)
	{
		// keep proxy in sync with real character
		FTransform CharTransform = Character->GetTransform();
		FTransform MyTransform = GetTransform();

		FTransform Transform;
		// NOTE:: ScalarRegister(0.5f) is causing camera jitter / vibrations like crazy, hence this low value
		Transform.LerpTranslationScale3D(CharTransform, MyTransform, ScalarRegister(0.01f));
		SetActorTransform(Transform);
	}
}

void ACharacterProxy::MoveToLocation(const FVector& DestLocation)
{
	/**
	*  Engage AI to path find
	*  The AI will then "route" the character correctly.
	*  The Proxy (and with it the camera), on each tick, moves to the location of the real character
	*
	*  And thus, we get the illusion of moving with the Player Character
	*/
	PlayerAIController->MoveToLocation(DestLocation);
}

void ACharacterProxy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// replicated to everyone
	DOREPLIFETIME(ACharacterProxy, Character);
}
