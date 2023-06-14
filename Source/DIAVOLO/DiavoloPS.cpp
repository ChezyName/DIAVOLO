// Fill out your copyright notice in the Description page of Project Settings.


#include "DiavoloPS.h"

#include "Net/UnrealNetwork.h"

void ADiavoloPS::ChangeCharState_Implementation(EPlayerStates NewState)
{
	GEngine->AddOnScreenDebugMessage(-1,1,FColor::Emerald,"Changed State On PS!");
	CharState = NewState;
}

void ADiavoloPS::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ADiavoloPS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(ADiavoloPS,CharState);
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

