// Copyright Epic Games, Inc. All Rights Reserved.

#include "DIAVOLOPlayerController.h"

#include "BaseProjectile.h"
#include "CharacterProxy.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "DIAVOLOCharacter.h"
#include "DrawDebugHelpers.h"
#include "CharacterProxy.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
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
			setEnemy(EnemyAttacking);
			return Hit.ImpactPoint;
		}
	}

	return FVector::ZeroVector;
}

ADiavoloPS* ADIAVOLOPlayerController::GetCharState()
{
	return GetPlayerState<ADiavoloPS>();
}

bool ADIAVOLOPlayerController::CloseEnough()
{
	if(GetProxy())
	{
		FVector newPlr = GetProxy()->GetActorLocation();
		newPlr.Z = 0;

		FVector newDest = newMoveToLocation;
		newDest.Z = 0;
		GEngine->AddOnScreenDebugMessage(-1,0,FColor::Yellow,FString::SanitizeFloat(FVector::Dist( newDest,newPlr)));
		DrawDebugLine(GetWorld(),newPlr,newDest,FColor::Emerald,false,-1,0,5);
		
		return FVector::Dist(newPlr,newDest) < 64;
	}
	return false;
}

void ADIAVOLOPlayerController::setEnemy_Implementation(AEnemy* Enemy)
{
	EnemyAttacking = Enemy;
}

void ADIAVOLOPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if(GetCharState())
	{
		GEngine->AddOnScreenDebugMessage(-1,0,FColor::Green, GetName() + ": " +
			(GetCharState()->CharState == EPlayerStates::E_IDLE ? "IDLE" :
			GetCharState()->CharState == EPlayerStates::E_MOVE ? "MOVE" :
			GetCharState()->CharState == EPlayerStates::E_ATTACK_WINDUP ? "ATTACK WINDUP" :
			GetCharState()->CharState == EPlayerStates::E_ATTACK_FULL ? "ATTACK HIT" :
			GetCharState()->CharState == EPlayerStates::E_MOVE_ATTACK ? "MOVE -> ATTACK" :
			GetCharState()->CharState == EPlayerStates::E_ABILITY ? "ABILITY" : "N/A"));
	}

	if(EnemyAttacking)
	{
		FVector TempLoc = EnemyAttacking->GetActorLocation();
		TempLoc.Z = 0;
		DrawDebugCylinder(GetWorld(),TempLoc,TempLoc + FVector(0,0,1),GetProxy()->Character->AutoAttack.AttackRange,
			32,FColor::Emerald,false,-1,0,2);
	}
	
	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		if(getMousePositionEnemy() != FVector::ZeroVector)
		{
			if(GetCharState() && GetCharState()->CharState != EPlayerStates::E_ATTACK_WINDUP && 
			GetCharState()->CharState != EPlayerStates::E_ATTACK_FULL)
			{
				ServerAttackMove(getMousePositionEnemy(),GetProxy()->Character->AutoAttack.AttackRange);
			}
			//GEngine->AddOnScreenDebugMessage(-1,30,FColor::Green,"Move To Enemy!");
		}
		else MoveToMouseCursor();
	}

	ServerUpdateState();
}

void ADIAVOLOPlayerController::ServerUpdateState_Implementation()
{
	if(GetCharState())
	{
		switch (GetCharState()->CharState)
		{
		case EPlayerStates::E_MOVE:
			if(CloseEnough())
			{
				ChangeCharState(EPlayerStates::E_IDLE);
				GEngine->AddOnScreenDebugMessage(-1,0,FColor::Green, GetName() + "MOVE -> IDLE");
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1,0,FColor::Green, GetName() + " STILL MOVING");
			}
			break;
		case EPlayerStates::E_MOVE_ATTACK:
			if(CloseEnough())
			{
				DoAutoAttack();
				GEngine->AddOnScreenDebugMessage(-1,0,FColor::Green, GetName() + "MOVE/ATTACK -> ATTACK");
			}
			else if(EnemyAttacking)
			{
				ChangeCharState(EPlayerStates::E_MOVE_ATTACK);
				ServerAttackMove(EnemyAttacking->GetActorLocation(),GetProxy()->Character->AutoAttack.AttackRange);
				GEngine->AddOnScreenDebugMessage(-1,0,FColor::Green, GetName() + "MOVE//ATTACK");
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
	Proxy = GetProxy();
	Super::BeginPlay();
}


void ADIAVOLOPlayerController::MoveToMouseCursor()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_GameTraceChannel1, false, Hit);

	if(GetCharState() && GetCharState()->CharState == EPlayerStates::E_ATTACK_WINDUP)
	{
		WindUpCanceled = true;
		GetProxy()->Character->StopAnimMontage(GetProxy()->Character->AutoAttack.Animation);
		ChangeCharState(EPlayerStates::E_IDLE);
	}
	
	if (Hit.bBlockingHit) // && GetCharState() && GetCharState()->CharState != EPlayerStates::E_ABILITY && GetCharState()->CharState != EPlayerStates::E_ATTACK_FULL)
	{
		// We hit something, move there
		GEngine->AddOnScreenDebugMessage(-1,5,FColor::Orange,"Moving!");
		EnemyAttacking = nullptr;
		ChangeCharState(EPlayerStates::E_MOVE);
		ServerMove(Hit.ImpactPoint);
	}
}

ACharacterProxy* ADIAVOLOPlayerController::GetProxy()
{
	if(Proxy) return Proxy;
	Proxy = Cast<ACharacterProxy>(GetPawn());
	return Proxy;
}

void ADIAVOLOPlayerController::SetNewMoveDestination_Implementation(const FVector DestLocation)
{
	if(GetProxy()) GEngine->AddOnScreenDebugMessage(-1,5,FColor::Red,"Pawn Moving > " +
		*(GetProxy() ? "PROXY" : " ___ ") + *(GetProxy()->Character ? "CHARACTER" : " ___ "));
	else GEngine->AddOnScreenDebugMessage(-1,5,FColor::Red,"PROXY DONT EXIST!");
	if (GetProxy() && GetProxy()->Character)
	{
		float const Distance = FVector::Dist(DestLocation, GetProxy()->Character->GetActorLocation());
		UE_LOG(LogTemp, Warning, TEXT("%s is doing an Auto Attack."),*GetName());
		// We need to issue move command only if far enough in order for walk animation to play correctly
		if ((Distance > 32.0f))
		{
			newMoveToLocation = DestLocation;
			GetProxy()->MoveToLocation(this, DestLocation);
		}
	}
}

void ADIAVOLOPlayerController::ServerMove_Implementation(FVector NewLoc)
{
	GEngine->AddOnScreenDebugMessage(-1,5,FColor::Orange,"SERVER Moving!");
	ChangeCharState(EPlayerStates::E_MOVE);
	SetNewMoveDestination(NewLoc);
}

void ADIAVOLOPlayerController::ServerAttackMove_Implementation(FVector NewLoc,float Range)
{
	if(GetProxy() == nullptr || GetProxy()->Character == nullptr) return;
	ChangeCharState(EPlayerStates::E_MOVE_ATTACK);
	const FVector plr = GetProxy()->Character->GetActorLocation();
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

void ADIAVOLOPlayerController::ChangeCharState_Implementation(EPlayerStates NewState)
{
	if(GetCharState() && HasAuthority())
	{
		GetCharState()->ChangeCharState(NewState);
		GetProxy()->Character->ServerSetState(NewState);
	}
}

void ADIAVOLOPlayerController::DoAutoAttack_Implementation()
{
	if(EnemyAttacking == nullptr || GetProxy()->Character == nullptr) return;
	GEngine->AddOnScreenDebugMessage(-1,25,FColor::Magenta,GetName() + " USING BASIC ATTACK!");

	//Face Enemy
	if(GetProxy()->Character) GetProxy()->Character->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(Proxy->Character->GetActorLocation(),
		EnemyAttacking->GetActorLocation()));

	UAnimMontage* AttackAnim = GetProxy()->Character->AutoAttack.Animation;
	if(AttackAnim)
	{
		GetProxy()->Character->PlayAnimMontage(AttackAnim);
	}
	ChangeCharState(EPlayerStates::E_ATTACK_WINDUP);
	WindUpCanceled = false;

	//Wait Windup To Deal Damage
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([&]
	{
		if(!WindUpCanceled)
		{
			if(EnemyAttacking){
				if(GetProxy()->Character) GetProxy()->Character->onBasicSkill(EnemyAttacking);
				ChangeCharState(EPlayerStates::E_IDLE);
			}
			else
			{
				ChangeCharState(EPlayerStates::E_IDLE);
				return;
			}
		}
	});

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, GetProxy()->Character->AutoAttack.TimeBeforeAttack, false);
}
