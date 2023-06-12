// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "Components/CapsuleComponent.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MouseOver = CreateDefaultSubobject<UCapsuleComponent>("MouseOver");
	MouseOver->SetCapsuleSize(75,125);
	MouseOver->SetupAttachment(RootComponent);
	MouseOver->SetCollisionProfileName("Enemy");
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::Damage_Implementation(float DamageAmount)
{
	Health -= DamageAmount;
	GEngine->AddOnScreenDebugMessage(-1,5,FColor::Red,GetName() + " Has Taken " +
		FString::SanitizeFloat(DamageAmount) + " HP!");
	if(Health <= 0)
	{
		GEngine->AddOnScreenDebugMessage(-1,1,FColor::Red,GetName() + " Has Died!");
	}
}

