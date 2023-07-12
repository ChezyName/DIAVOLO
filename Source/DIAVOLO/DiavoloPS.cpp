// Fill out your copyright notice in the Description page of Project Settings.


#include "DiavoloPS.h"

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

void ADiavoloPS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(ADiavoloPS,CharState);
	DOREPLIFETIME(ADiavoloPS,bCharReady);
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

