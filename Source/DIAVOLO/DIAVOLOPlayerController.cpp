// Copyright Epic Games, Inc. All Rights Reserved.

#include "DIAVOLOPlayerController.h"

#include "BaseProjectile.h"
#include "CharacterProxy.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "DIAVOLOCharacter.h"
#include "DrawDebugHelpers.h"
#include "DiavoloPS.h"
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

EPlayerStates ADIAVOLOPlayerController::GetCharState()
{
	if(SpawnedCharacter) return SpawnedCharacter->CharState;
	return EPlayerStates::E_IDLE;
}

ADiavoloPS* ADIAVOLOPlayerController::GetState()
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
		//GEngine->AddOnScreenDebugMessage(-1,0,FColor::Yellow,FString::SanitizeFloat(FVector::Dist( newDest,newPlr)));
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

	if(IsLocalController() && !bController)
	{
		LookAtMouse();
	}

	/*
	GEngine->AddOnScreenDebugMessage(-1,0,FColor::Green, GetName() + ": " +
		(GetCharState() == EPlayerStates::E_IDLE ? "IDLE" :
		GetCharState() == EPlayerStates::E_MOVE ? "MOVE" :
		GetCharState() == EPlayerStates::E_ATTACK_WINDUP ? "ATTACK WINDUP" :
		GetCharState() == EPlayerStates::E_ATTACK_FULL ? "ATTACK HIT" :
		GetCharState() == EPlayerStates::E_MOVE_ATTACK ? "MOVE -> ATTACK" :
		GetCharState() == EPlayerStates::E_ABILITY ? "ABILITY" : "N/A"));
	*/

	/*
	if(EnemyAttacking)
	{
		FVector TempLoc = EnemyAttacking->GetActorLocation();
		TempLoc.Z = 0;
		DrawDebugCylinder(GetWorld(),TempLoc,TempLoc + FVector(0,0,1),Character->AutoAttack.AttackRange,
			32,FColor::Emerald,false,-1,0,2);
	}
	
	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		GEngine->AddOnScreenDebugMessage(-1,0,FColor::Orange,"Mouse is Active!");
		if(Character && !Character->bUsingAbility)
		{
			
			if(getMousePositionEnemy() != FVector::ZeroVector)
			{
				if(GetCharState() != EPlayerStates::E_ATTACK_WINDUP && 
				GetCharState() != EPlayerStates::E_ATTACK_FULL)
				{
					ServerAttackMove(getMousePositionEnemy(),Character->AutoAttack.AttackRange);
				}
				//GEngine->AddOnScreenDebugMessage(-1,30,FColor::Green,"Move To Enemy!");
			}
			else MoveToMouseCursor();
		}
	}
	*/

	ServerUpdateState();
}

void ADIAVOLOPlayerController::ServerUpdateState_Implementation()
{
	switch (GetCharState())
	{
	case EPlayerStates::E_MOVE:
		if(CloseEnough())
		{
			ChangeCharState(EPlayerStates::E_IDLE);
			//GEngine->AddOnScreenDebugMessage(-1,0,FColor::Green, GetName() + "MOVE -> IDLE");
		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(-1,0,FColor::Green, GetName() + " STILL MOVING");
		}
		break;
	case EPlayerStates::E_MOVE_ATTACK:
		if(CloseEnough())
		{
			DoAutoAttack();
			//->AddOnScreenDebugMessage(-1,0,FColor::Green, GetName() + "MOVE/ATTACK -> ATTACK");
		}
		else if(EnemyAttacking)
		{
			ChangeCharState(EPlayerStates::E_MOVE_ATTACK);
			ServerAttackMove(EnemyAttacking->GetActorLocation(),SpawnedCharacter->AutoAttack.AttackRange);
			//GEngine->AddOnScreenDebugMessage(-1,0,FColor::Green, GetName() + "MOVE//ATTACK");
		}
		break;
	}
}

void ADIAVOLOPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ADIAVOLOPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ADIAVOLOPlayerController::OnSetDestinationReleased);

	InputComponent->BindAction("Skill1",IE_Pressed,this,&ADIAVOLOPlayerController::onSkill1C);
	InputComponent->BindAction("Skill2",IE_Pressed,this,&ADIAVOLOPlayerController::onSkill2C);
	InputComponent->BindAction("Skill3",IE_Pressed,this,&ADIAVOLOPlayerController::onSkill3C);
	InputComponent->BindAction("Ultimate",IE_Pressed,this,&ADIAVOLOPlayerController::onUltimateC);

	InputComponent->BindAction("Skill1",IE_Released,this,&ADIAVOLOPlayerController::endSkill1C);
	InputComponent->BindAction("Skill2",IE_Released,this,&ADIAVOLOPlayerController::endSkill2C);
	InputComponent->BindAction("Skill3",IE_Released,this,&ADIAVOLOPlayerController::endSkill3C);
	InputComponent->BindAction("Ultimate",IE_Released,this,&ADIAVOLOPlayerController::endUltimateC);

	InputComponent->BindAction("Dance",IE_Released,this,&ADIAVOLOPlayerController::startEmoteC);
	InputComponent->BindAction("Dodge",IE_Pressed,this,&ADIAVOLOPlayerController::onDodgeC);

	InputComponent->BindAxis("UpMovement",this,&ADIAVOLOPlayerController::SetUpMovementC);
	InputComponent->BindAxis("RightMovement",this,&ADIAVOLOPlayerController::SetRightMovementC);

	InputComponent->BindAxis("LookX",this,&ADIAVOLOPlayerController::SetControllerX);
	InputComponent->BindAxis("LookY",this,&ADIAVOLOPlayerController::SetControllerY);
}

void ADIAVOLOPlayerController::BeginPlay()
{
	onStartSetChar();
	Super::BeginPlay();
}

void ADIAVOLOPlayerController::onStartSetChar_Implementation()
{
	// get current location of player proxy
	TArray<AActor*> FoundEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), FoundEnemies);
	for(AActor* Enemy : FoundEnemies)
	{
		AEnemy* BossChar = Cast<AEnemy>(Enemy);
		if(BossChar->bIsBoss)
		{
			BossCharacter = BossChar;
			break;
		}
	}
	
	AActor* StartLoc = nullptr;
	if(GetWorld() && GetWorld()->GetAuthGameMode())
		StartLoc = GetWorld()->GetAuthGameMode()->FindPlayerStart(this,FString("None"));
	FVector Location;
	FRotator Rotation;

	if(StartLoc != nullptr)
	{
		Location = StartLoc->GetActorLocation();
		Rotation = StartLoc->GetActorRotation();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.bNoFail = true;

	// spawn actual player
	if(GetState())
	{
		SpawnedCharacter = Cast<ADIAVOLOCharacter>(GetWorld()->SpawnActor(GetState()->CharacterToSpawn, &Location, &Rotation, SpawnParams));

		//GEngine->AddOnScreenDebugMessage(-1,8,FColor::Turquoise,Character ? "Spawned!" : "Not Spawned?!?");

		// we use AI to control the player character for navigation
		Possess(SpawnedCharacter);
		//Character->ParentProxy = this;
		///Character->OnDeathFunction.BindUFunction(this,FName("onCharacterDeath"));
	}
}

void ADIAVOLOPlayerController::MoveToMouseCursor()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_GameTraceChannel1, false, Hit);

	if(GetCharState() == EPlayerStates::E_ATTACK_WINDUP)
	{
		WindUpCanceled = true;
		SpawnedCharacter->StopAnimationServer(SpawnedCharacter->AutoAttack.Animation);
		ChangeCharState(EPlayerStates::E_IDLE);
	}
	
	if (Hit.bBlockingHit) // && GetCharState() && GetCharState()->CharState != EPlayerStates::E_ABILITY && GetCharState()->CharState != EPlayerStates::E_ATTACK_FULL)
	{
		// We hit something, move there
		GEngine->AddOnScreenDebugMessage(-1,0,FColor::Orange,"Moving!");
		EnemyAttacking = nullptr;
		ChangeCharState(EPlayerStates::E_MOVE);
		ServerMove(Hit.ImpactPoint);
	}
}

void ADIAVOLOPlayerController::startEmoteC_Implementation() { startEmoteS(); }
void ADIAVOLOPlayerController::startEmoteS_Implementation()
{
	if(SpawnedCharacter && !SpawnedCharacter->isDead)
	{
		GetProxy()->MoveToLocation(SpawnedCharacter->GetActorLocation());
		SpawnedCharacter->StartEmote();
		SpawnedCharacter->CharState = EPlayerStates::E_EMOTE;
	}
}

void ADIAVOLOPlayerController::endEmoteC_Implementation() { endEmoteS(); }
void ADIAVOLOPlayerController::endEmoteS_Implementation()
{
	if(SpawnedCharacter) SpawnedCharacter->StopEmote();
}

ACharacterProxy* ADIAVOLOPlayerController::GetProxy()
{
	if(Proxy) return Proxy;
	Proxy = Cast<ACharacterProxy>(GetPawn());
	return Proxy;
}

void ADIAVOLOPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(ADIAVOLOPlayerController,EnemyAttacking);
	DOREPLIFETIME(ADIAVOLOPlayerController,MousePosition);
	DOREPLIFETIME(ADIAVOLOPlayerController,SpawnedCharacter);
	DOREPLIFETIME(ADIAVOLOPlayerController,BossCharacter);
	DOREPLIFETIME(ADIAVOLOPlayerController,bController);
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ADIAVOLOPlayerController::SetNewMoveDestination_Implementation(const FVector DestLocation)
{
	/*
	if(GetProxy()) GEngine->AddOnScreenDebugMessage(-1,5,FColor::Red,"Pawn Moving > " +
		*(GetProxy() ? "PROXY" : " ___ ") + *(Character ? "CHARACTER" : " ___ "));
	else GEngine->AddOnScreenDebugMessage(-1,5,FColor::Red,"PROXY DONT EXIST!");
	*/
	endEmoteS();
	if (SpawnedCharacter)
	{
		float const Distance = FVector::Dist(DestLocation, SpawnedCharacter->GetActorLocation());
		UE_LOG(LogTemp, Warning, TEXT("%s is doing an Auto Attack."),*GetName());
		// We need to issue move command only if far enough in order for walk animation to play correctly
		if ((Distance > 32.0f))
		{
			newMoveToLocation = DestLocation;
			GetProxy()->MoveToLocation(DestLocation);
			endEmoteS();
		}
	}
}

void ADIAVOLOPlayerController::ServerMove_Implementation(FVector NewLoc)
{
	//GEngine->AddOnScreenDebugMessage(-1,5,FColor::Orange,"SERVER Moving!");
	ChangeCharState(EPlayerStates::E_MOVE);
	SetNewMoveDestination(NewLoc);
}

void ADIAVOLOPlayerController::ServerAttackMove_Implementation(FVector NewLoc,float Range)
{
	if(GetProxy() == nullptr || SpawnedCharacter == nullptr) return;
	ChangeCharState(EPlayerStates::E_MOVE_ATTACK);
	const FVector plr = SpawnedCharacter->GetActorLocation();
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
	if(GetState() && HasAuthority())
	{
		GetState()->ChangeCharState(NewState);
		SpawnedCharacter->ServerSetState(NewState);
	}
}

void ADIAVOLOPlayerController::DoAutoAttack_Implementation()
{
	if(EnemyAttacking == nullptr || SpawnedCharacter == nullptr || SpawnedCharacter->bUsingAbility
		|| SpawnedCharacter->isDead || SpawnedCharacter->bUsingAbility) return;
	//GEngine->AddOnScreenDebugMessage(-1,25,FColor::Magenta,GetName() + " USING BASIC ATTACK!");

	//Face Enemy
	if(SpawnedCharacter) SpawnedCharacter->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(Proxy->Character->GetActorLocation(),
		EnemyAttacking->GetActorLocation()));

	UAnimMontage* AttackAnim = SpawnedCharacter->AutoAttack.Animation;
	if(AttackAnim)
	{
		SpawnedCharacter->PlayAnimationServer(AttackAnim);
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
				if(SpawnedCharacter) SpawnedCharacter->onBasicSkill(EnemyAttacking);
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
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, SpawnedCharacter->AutoAttack.TimeBeforeAttack, false);
}

//===========================================================================================
//                                CLIENT

void ADIAVOLOPlayerController::onSkill1C_Implementation()
{
	if(SpawnedCharacter) onSkill1S(SpawnedCharacter->GetActorForwardVector(),BossCharacter);
	endEmoteC();
}
void ADIAVOLOPlayerController::onSkill2C_Implementation()
{
	if(SpawnedCharacter) onSkill2S(SpawnedCharacter->GetActorForwardVector(),BossCharacter);
	endEmoteC();
}
void ADIAVOLOPlayerController::onSkill3C_Implementation()
{
	if(SpawnedCharacter) onSkill3S(SpawnedCharacter->GetActorForwardVector(),BossCharacter);
	endEmoteC();
}
void ADIAVOLOPlayerController::onUltimateC_Implementation()
{
	if(SpawnedCharacter) onUltimateS(SpawnedCharacter->GetActorForwardVector(),BossCharacter);
	endEmoteC();
}

void ADIAVOLOPlayerController::onDodgeC_Implementation()
{
	if(SpawnedCharacter)
	{
		if(bController)
		{
			FVector Direction = SpawnedCharacter->GetVelocity().GetSafeNormal();
			if(Direction == FVector::ZeroVector)
			{
				Direction = SpawnedCharacter->GetActorForwardVector();
			}
			onDodgeS(Direction);
		}
		else
		{
			onDodgeS(SpawnedCharacter->GetActorForwardVector());
		}
	}
	endEmoteC();
}

//===========================================================================================
//                                SERVER

void ADIAVOLOPlayerController::onSkill1S_Implementation(FVector MouseLoc,AEnemy* Enemy)
{
	if(SpawnedCharacter && !SpawnedCharacter->isDead) SpawnedCharacter->onSkill1(MouseLoc,Enemy);
}
void ADIAVOLOPlayerController::onSkill2S_Implementation(FVector MouseLoc,AEnemy* Enemy)
{
	if(SpawnedCharacter && !SpawnedCharacter->isDead) SpawnedCharacter->onSkill2(MouseLoc,Enemy);
}
void ADIAVOLOPlayerController::onSkill3S_Implementation(FVector MouseLoc,AEnemy* Enemy)
{
	if(SpawnedCharacter && !SpawnedCharacter->isDead) SpawnedCharacter->onSkill3(MouseLoc,Enemy);
}
void ADIAVOLOPlayerController::onUltimateS_Implementation(FVector MouseLoc,AEnemy* Enemy)
{
	if(SpawnedCharacter && !SpawnedCharacter->isDead) SpawnedCharacter->onUltimate(MouseLoc,Enemy);
}

void ADIAVOLOPlayerController::onDodgeS_Implementation(FVector MouseLoc)
{
	if(SpawnedCharacter && !SpawnedCharacter->isDead) SpawnedCharacter->DodgeRoll(MouseLoc);
}

//===========================================================================================
//                          END ABILITY CLIENT

void ADIAVOLOPlayerController::endSkill1C_Implementation(){ endSkill1(); }
void ADIAVOLOPlayerController::endSkill2C_Implementation(){ endSkill2(); }
void ADIAVOLOPlayerController::endSkill3C_Implementation(){ endSkill3(); }
void ADIAVOLOPlayerController::endUltimateC_Implementation(){ endUltimate(); }

//===========================================================================================
//                          END ABILITY SERVER

void ADIAVOLOPlayerController::endSkill1_Implementation()
{
	if(SpawnedCharacter) SpawnedCharacter->endSkill1();
}
void ADIAVOLOPlayerController::endSkill2_Implementation()
{
	if(SpawnedCharacter) SpawnedCharacter->endSkill2();
}
void ADIAVOLOPlayerController::endSkill3_Implementation()
{
	if(SpawnedCharacter) SpawnedCharacter->endSkill3();
}
void ADIAVOLOPlayerController::endUltimate_Implementation()
{
	if(SpawnedCharacter) SpawnedCharacter->endUltimate();
}

//===========================================================================================
//                                MOVEMENT

void ADIAVOLOPlayerController::SetUpMovementC_Implementation(float Value)
{
	SetUpMovementS(Value);
}

void ADIAVOLOPlayerController::SetRightMovementC_Implementation(float Value)
{
	SetRightMovementS(Value);
}

void ADIAVOLOPlayerController::SetUpMovementS_Implementation(float Value)
{
	if(SpawnedCharacter && !SpawnedCharacter->isDead &&
		!SpawnedCharacter->bUsingAbility && !SpawnedCharacter->bisDodging)
			SpawnedCharacter->AddMovementInput(FVector::ForwardVector * Value,1,false);
}

void ADIAVOLOPlayerController::SetRightMovementS_Implementation(float Value)
{
	if(SpawnedCharacter && !SpawnedCharacter->isDead &&
		!SpawnedCharacter->bUsingAbility && !SpawnedCharacter->bisDodging)
			SpawnedCharacter->AddMovementInput(FVector::RightVector * Value,1,false);
}

void ADIAVOLOPlayerController::LookAtMouse()
{
	if (SpawnedCharacter)
	{
		// Calculate the direction the character should look at (ignoring Z-axis)
		FVector CharacterLocation = SpawnedCharacter->GetActorLocation();
		FVector LookDirection = getMousePositionGround() - CharacterLocation;
		LookDirection.Z = 0.0f;

		if (!LookDirection.IsNearlyZero())
		{
			// Calculate the rotation for the player character (only on the yaw axis)
			FRotator NewRotation = FRotationMatrix::MakeFromX(LookDirection).Rotator();
			NewRotation.Pitch = 0.0f;
			NewRotation.Roll = 0.0f;

			// Set the actor rotation with the new rotation
			SpawnedCharacter->SetActorRotation(NewRotation);
		}

		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Orange,
			"Mouse Angle: " + FString::SanitizeFloat(SpawnedCharacter->GetActorRotation().Yaw));
	}
}

void ADIAVOLOPlayerController::LookAtController()
{
	if (SpawnedCharacter)
	{
		GEngine->AddOnScreenDebugMessage(-1,0,FColor::Orange,
			"Controller Pos: " + FString::SanitizeFloat(ControllerAimDir.X) + " , " + 
			FString::SanitizeFloat(ControllerAimDir.Y) + ".");
		float AngleRadians = FMath::Atan2(ControllerAimDir.Y, ControllerAimDir.X);
		float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);

		// Set the rotation for the player character
		FRotator NewRotation = FRotator(0.0f, AngleDegrees+90, 0.0f);
		SpawnedCharacter->SetActorRotation(NewRotation);
	}
}

void ADIAVOLOPlayerController::SetControllerX(float X)
{
	ControllerAimDir.X = X;
	if(bController) LookAtController();
}

void ADIAVOLOPlayerController::SetControllerY(float Y)
{
	ControllerAimDir.Y = Y;
	if(bController) LookAtController();
}
