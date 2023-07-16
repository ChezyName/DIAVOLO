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
	CharName = CharacterToSpawn->GetName();
	GEngine->AddOnScreenDebugMessage(-1,25,FColor::Magenta,"Spawnable: "+NewSpawnable->GetName());
}

void ADiavoloPS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(ADiavoloPS,CharState);
	DOREPLIFETIME(ADiavoloPS,CharName);
	DOREPLIFETIME(ADiavoloPS,bCharReady);
	DOREPLIFETIME(ADiavoloPS,CharacterToSpawn);
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ADiavoloPS::CopyProperties(APlayerState* PlayerState)
{   
	Super::CopyProperties(PlayerState);

	if(PlayerState)
	{
		GEngine->AddOnScreenDebugMessage(-1,25,FColor::Green,"Replacing Name: "+GetPlayerName());
		PlayerState->SetPlayerName(GetPlayerName());
		ADiavoloPS* State = Cast<ADiavoloPS>(PlayerState);
		if(State)
		{
			CharName = State->CharName;
			CharacterToSpawn = State->CharacterToSpawn;
			GEngine->AddOnScreenDebugMessage(-1,25,FColor::Orange,"Replacing Class: "+
				FString(State->CharacterToSpawn ? CharacterToSpawn->GetName() : "NULL"));
			GEngine->AddOnScreenDebugMessage(-1,25,FColor::Orange,"Class Name: "+CharName);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1,25,FColor::Red,"No Name To Change To!");
	}
}

void ADiavoloPS::BeginPlay()
{
	Super::BeginPlay();
}
