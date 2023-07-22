// Fill out your copyright notice in the Description page of Project Settings.


#include "CrimsonQueen.h"

ACrimsonQueen::ACrimsonQueen()
{
	SpinAttackRadius = CreateDefaultSubobject<USphereComponent>("SpinHitbox");
	SpinAttackRadius->SetupAttachment(RootComponent);
	SpinAttackRadius->InitSphereRadius(250);
}
