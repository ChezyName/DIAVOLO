// Copyright Epic Games, Inc. All Rights Reserved.

#include "DIAVOLOPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "DIAVOLOCharacter.h"
#include "Engine/World.h"

ADIAVOLOPlayerController::ADIAVOLOPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

FVector ADIAVOLOPlayerController::getMousePosition()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	if(Hit.bBlockingHit)
	{
		return Hit.ImpactPoint;
	}
	return FVector::ZeroVector;
}

FVector ADIAVOLOPlayerController::getMousePositionGround()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_GameTraceChannel1, false, Hit);
	if(Hit.bBlockingHit)
	{
		return Hit.ImpactPoint;
	}
	return FVector::ZeroVector;
}

FVector ADIAVOLOPlayerController::getMousePositionEnemy()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_GameTraceChannel2, false, Hit);
	if(Hit.bBlockingHit)
	{
		return Hit.ImpactPoint;
	}
	return FVector::ZeroVector;
}

void ADIAVOLOPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		if(getMousePositionEnemy() != FVector::ZeroVector)
		{
			ClientAttackMove(getMousePositionEnemy(),250);
			ServerAttackMove(getMousePositionEnemy(),250);
			GEngine->AddOnScreenDebugMessage(-1,30,FColor::Green,"Move To Enemy!");
		}
		else MoveToMouseCursor();
	}
}

void ADIAVOLOPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ADIAVOLOPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ADIAVOLOPlayerController::OnSetDestinationReleased);
}


void ADIAVOLOPlayerController::MoveToMouseCursor()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_GameTraceChannel1, false, Hit);

	if (Hit.bBlockingHit)
	{
		// We hit something, move there
		ClientMove(Hit.ImpactPoint);
		ServerMove(Hit.ImpactPoint);
	}
}

void ADIAVOLOPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if ((Distance > 120.0f))
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void ADIAVOLOPlayerController::ClientMove_Implementation(FVector NewLoc)
{
	SetNewMoveDestination(NewLoc);
}

void ADIAVOLOPlayerController::ServerMove_Implementation(FVector NewLoc)
{
	SetNewMoveDestination(NewLoc);
}

void ADIAVOLOPlayerController::ClientAttackMove_Implementation(FVector NewLoc,float Range)
{
	const FVector plr = GetPawn()->GetActorLocation();
	FVector Dir = NewLoc - plr;
	Dir.Normalize();

	const FVector Target = NewLoc - (Dir * Range);
	SetNewMoveDestination(Target);
}

void ADIAVOLOPlayerController::ServerAttackMove_Implementation(FVector NewLoc,float Range)
{
	const FVector plr = GetPawn()->GetActorLocation();
	FVector Dir = NewLoc - plr;
	Dir.Normalize();

	const FVector Target = NewLoc - (Dir * Range);
	SetNewMoveDestination(Target);
}

void ADIAVOLOPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void ADIAVOLOPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}