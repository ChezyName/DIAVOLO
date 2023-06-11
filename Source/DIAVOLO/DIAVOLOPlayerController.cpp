// Copyright Epic Games, Inc. All Rights Reserved.

#include "DIAVOLOPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "DIAVOLOCharacter.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

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
		EnemyAttacking = Cast<AEnemy>(Hit.Actor);
		return Hit.ImpactPoint;
	}
	return FVector::ZeroVector;
}

void ADIAVOLOPlayerController::ChangeCharState_Implementation(EPlayerStates newState)
{
	CharState = newState;
}

EPlayerStates ADIAVOLOPlayerController::getCharState()
{
	return CharState;
}

bool ADIAVOLOPlayerController::CloseEnough()
{
	if(GetPawn())
	{
		FVector newPlr = GetPawn()->GetActorLocation();
		newPlr.Z = 0;

		FVector newDest = newMoveToLocation;
		newDest.Z = 0;
		GEngine->AddOnScreenDebugMessage(-1,0,FColor::Yellow,FString::SanitizeFloat(FVector::Dist( newDest,newPlr)));
		DrawDebugLine(GetWorld(),newPlr,newDest,FColor::Emerald,false,-1,0,5);
		
		return FVector::Dist(newPlr,newDest) <64;
	}
	return false;
}

void ADIAVOLOPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	GEngine->AddOnScreenDebugMessage(-1,0,FColor::Yellow,
		(CharState == EPlayerStates::E_IDLE ? "IDLE" :
		CharState == EPlayerStates::E_MOVE ? "MOVE" :
		CharState == EPlayerStates::E_ATTACK ? "ATTACK" :
		CharState == EPlayerStates::E_MOVE_ATTACK ? "MOVE -> ATTACK" :
		CharState == EPlayerStates::E_ABILITY ? "ABILITY" : "N/A"));

	if(EnemyAttacking)
	{
		FVector TempLoc = EnemyAttacking->GetActorLocation();
		TempLoc.Z = 0;
		DrawDebugSphere(GetWorld(),TempLoc,125,2,FColor::Emerald,false,-1,0,2);
	}
	
	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		if(getMousePositionEnemy() != FVector::ZeroVector)
		{
			ClientAttackMove(getMousePositionEnemy(),125);
			ServerAttackMove(getMousePositionEnemy(),125);
			//GEngine->AddOnScreenDebugMessage(-1,30,FColor::Green,"Move To Enemy!");
		}
		else MoveToMouseCursor();
	}

	if(HasAuthority())
	{
		switch (CharState)
		{
			case EPlayerStates::E_MOVE:
				if(CloseEnough()) CharState = EPlayerStates::E_IDLE;
				break;
			case EPlayerStates::E_MOVE_ATTACK:
				if(CloseEnough()) CharState = EPlayerStates::E_ATTACK;
				else if(EnemyAttacking)
				{
					ClientAttackMove(EnemyAttacking->GetActorLocation(),125);
					ServerAttackMove(EnemyAttacking->GetActorLocation(),125);
				}
				break;
		}
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
		if ((Distance > 32.0f))
		{
			newMoveToLocation = DestLocation;
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
	CharState = EPlayerStates::E_MOVE;
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
	CharState = EPlayerStates::E_MOVE_ATTACK;
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

void ADIAVOLOPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(ADIAVOLOPlayerController,CharState);
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
