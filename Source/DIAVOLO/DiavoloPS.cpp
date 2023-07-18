// Fill out your copyright notice in the Description page of Project Settings.


#include "DiavoloPS.h"
#include "DIAVOLOCharacter.h"
#include "Net/UnrealNetwork.h"

void ADiavoloPS::ChangeCharState_Implementation(EPlayerStates NewState)
{
	/*
	GEngine->AddOnScreenDebugMessage(-1,18,FColor::Magenta,
	(NewState == EPlayerStates::E_IDLE ? "SC IDLE" :
	NewState == EPlayerStates::E_MOVE ? "SC MOVE" :
	NewState == EPlayerStates::E_ATTACK_WINDUP ? "SC ATTACK WINDUP" :
	NewState == EPlayerStates::E_ATTACK_FULL ? "SC ATTACK HIT" :
	NewState == EPlayerStates::E_MOVE_ATTACK ? "SC MOVE -> ATTACK" :
	NewState == EPlayerStates::E_ABILITY ? "SC ABILITY" : "SC N/A"));
	*/
	CharState = NewState;
}

void ADiavoloPS::SetSpawnable_Implementation(TSubclassOf<ADIAVOLOCharacter> NewSpawnable)
{
	CharacterToSpawn = NewSpawnable;
	CharName = CharacterToSpawn->GetPathName();
	GEngine->AddOnScreenDebugMessage(-1,25,FColor::Magenta,"Spawnable: "+NewSpawnable->GetName()+"//"+CharName);
}

void ADiavoloPS::ChangeReadyState_Implementation(bool isReady)
{
	bCharReady = isReady;
}

void ADiavoloPS::SetClassIcon_Implementation(UTexture2D* NewIcon)
{
	ClassIcon = NewIcon;
	GEngine->AddOnScreenDebugMessage(-1,60,FColor::Yellow,NewIcon->GetName());
}

void ADiavoloPS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(ADiavoloPS,CharState);
	DOREPLIFETIME(ADiavoloPS,bCharReady);
	DOREPLIFETIME(ADiavoloPS, CharName);
	DOREPLIFETIME(ADiavoloPS, CharacterToSpawn);
	DOREPLIFETIME(ADiavoloPS, ClassIcon);
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ADiavoloPS::CopyProperties(APlayerState* PlayerState)
{   
	Super::CopyProperties(PlayerState);

	if(PlayerState)
	{
		ADiavoloPS* ShooterPlayer = Cast<ADiavoloPS>(PlayerState);
		if (ShooterPlayer)
		{
			ShooterPlayer->CharName = CharName;
			ShooterPlayer->bCharReady = bCharReady;
			ShooterPlayer->CharacterToSpawn = CharacterToSpawn;
			ShooterPlayer->ClassIcon = ClassIcon;
			GEngine->AddOnScreenDebugMessage(-1,5,FColor::Turquoise,"Copied Properties!");
		}
	}
}

void ADiavoloPS::BeginPlay()
{
	Super::BeginPlay();
}
