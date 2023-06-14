// Fill out your copyright notice in the Description page of Project Settings.
#include "CharacterProxy.h"
#include "DIAVOLOCharacter.h"
#include "DIAVOLOPlayerController.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Net/UnrealNetwork.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"


// Sets default values
ACharacterProxy::ACharacterProxy(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

// Called when the game starts or when spawned
void ACharacterProxy::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetLocalRole() == ROLE_Authority)
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

		// we use AI to control the player character for navigation
		PlayerAIController = GetWorld()->SpawnActor<AAIController>(GetActorLocation(), GetActorRotation());
		PlayerAIController->Possess(Character);
	}
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

void ACharacterProxy::MoveToLocation(const class ADIAVOLOPlayerController* controller, const FVector& DestLocation)
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
