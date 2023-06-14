// Copyright Epic Games, Inc. All Rights Reserved.

#include "DIAVOLOPlayerController.h"

#include "BaseProjectile.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "DIAVOLOCharacter.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
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
		EnemyAttacking = nullptr;
		return Hit.ImpactPoint;
	}
	return FVector::ZeroVector;
}

FVector ADIAVOLOPlayerController::getMousePositionEnemy()
{
	FVector WorldLocation, WorldDirection;
	DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(Hit, WorldLocation,
		WorldLocation + WorldDirection * 5000, ECC_GameTraceChannel2, Params);

	if (Hit.bBlockingHit)
	{
		EnemyAttacking = Cast<AEnemy>(Hit.Actor.Get());
		if (EnemyAttacking)
		{
			return Hit.ImpactPoint;
		}
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
		
		return FVector::Dist(newPlr,newDest) < 64;
	}
	return false;
}

void ADIAVOLOPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	/*
	GEngine->AddOnScreenDebugMessage(-1,0,FColor::Yellow,
		(CharState == EPlayerStates::E_IDLE ? "IDLE" :
		CharState == EPlayerStates::E_MOVE ? "MOVE" :
		CharState == EPlayerStates::E_ATTACK_WINDUP ? "ATTACK WINDUP" :
		CharState == EPlayerStates::E_ATTACK_FULL ? "ATTACK HIT" :
		CharState == EPlayerStates::E_MOVE_ATTACK ? "MOVE -> ATTACK" :
		CharState == EPlayerStates::E_ABILITY ? "ABILITY" : "N/A"));
	*/

	if(EnemyAttacking)
	{
		
		FVector TempLoc = EnemyAttacking->GetActorLocation();
		TempLoc.Z = 0;
		DrawDebugCylinder(GetWorld(),TempLoc,TempLoc + FVector(0,0,1),CharacterClass->AutoAttack.AttackRange,
			32,FColor::Emerald,false,-1,0,2);
	}

	//Draw Debug Lines
	if(GetPawn()) DrawDebugLine(GetWorld(),GetPawn()->GetActorLocation(),newMoveToLocation,FColor::Emerald,false,-1,0,5);
	
	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		if(getMousePositionEnemy() != FVector::ZeroVector)
		{
			if(CharState != EPlayerStates::E_ATTACK_WINDUP && 
			CharState != EPlayerStates::E_ATTACK_FULL)
			{
				ClientAttackMove(getMousePositionEnemy(),CharacterClass->AutoAttack.AttackRange);
				ServerAttackMove(getMousePositionEnemy(),CharacterClass->AutoAttack.AttackRange);
			}
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
				if(CloseEnough()) DoAutoAttack();
				else if(EnemyAttacking)
				{
					CharState = EPlayerStates::E_ATTACK_WINDUP;
					ClientAttackMove(EnemyAttacking->GetActorLocation(),CharacterClass->AutoAttack.AttackRange);
					ServerAttackMove(EnemyAttacking->GetActorLocation(),CharacterClass->AutoAttack.AttackRange);
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

void ADIAVOLOPlayerController::BeginPlay()
{
	CharacterClass = Cast<ADIAVOLOCharacter>(GetPawn());
	Super::BeginPlay();
}


void ADIAVOLOPlayerController::MoveToMouseCursor()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_GameTraceChannel1, false, Hit);

	if(CharState == EPlayerStates::E_ATTACK_WINDUP)
	{
		WindUpCanceled = true;
		CharacterClass->StopAnimMontage(CharacterClass->AutoAttack.Animation);
		CharState = EPlayerStates::E_IDLE;
	}
	
	if (Hit.bBlockingHit && CharState != EPlayerStates::E_ABILITY && CharState != EPlayerStates::E_ATTACK_FULL)
	{
		// We hit something, move there
		EnemyAttacking = nullptr;
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

	FVector Target;
	float Distance = FVector::Dist(plr, NewLoc);
	if (Distance > Range)
	{
		Target = NewLoc - (Dir * Range);
	}
	else
	{
		Target = plr; // Player is already within range, return current position
	}
	Target.Z = 0;
	SetNewMoveDestination(Target);
}

void ADIAVOLOPlayerController::ServerAttackMove_Implementation(FVector NewLoc,float Range)
{
	CharState = EPlayerStates::E_MOVE_ATTACK;
	const FVector plr = GetPawn()->GetActorLocation();
	FVector Dir = NewLoc - plr;
	Dir.Normalize();

	FVector Target;
	float Distance = FVector::Dist(plr, NewLoc);
	if (Distance > Range)
	{
		Target = NewLoc - (Dir * Range);
	}
	else
	{
		Target = plr; // Player is already within range, return current position
	}
	Target.Z = 0;
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

void ADIAVOLOPlayerController::DoAutoAttack_Implementation()
{
	if(EnemyAttacking == nullptr) return;
	UAnimMontage* AttackAnim = CharacterClass->AutoAttack.Animation;
	if(AttackAnim)
	{
		CharacterClass->PlayAnimMontage(AttackAnim);
	}
	CharState = EPlayerStates::E_ATTACK_WINDUP;
	WindUpCanceled = false;

	//Wait Windup To Deal Damage
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([&]
	{
		if(!WindUpCanceled)
		{
			if(EnemyAttacking){
				ADIAVOLOCharacter* Char = Cast<ADIAVOLOCharacter>(GetPawn());
				if(Char) Char->onBasicSkill(EnemyAttacking);
				CharState = EPlayerStates::E_IDLE;
			}
			else
			{
				CharState = EPlayerStates::E_IDLE;
				return;
			}
		}
	});

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, CharacterClass->AutoAttack.TimeBeforeAttack, false);
}
