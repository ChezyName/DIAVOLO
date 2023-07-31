// Fill out your copyright notice in the Description page of Project Settings.


#include "RealGamemode.h"
#include "GameFramework/SpectatorPawn.h"
#include "DIAVOLOCharacter.h"
#include "Kismet/GameplayStatics.h"

void ARealGamemode::onCharacterDeath()
{
	//GEngine->AddOnScreenDebugMessage(-1,30,FColor::Magenta,"SOMEONE DIED!!!");
	TArray<AActor*> Characters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADIAVOLOCharacter::StaticClass(), Characters);
	bool isDead = true;
	for(AActor* Char : Characters)
	{
		ADIAVOLOCharacter* CharDiavolo = Cast<ADIAVOLOCharacter>(Char);
		if(CharDiavolo && CharDiavolo->isDead == false)
		{
			//GEngine->AddOnScreenDebugMessage(-1,30,FColor::Magenta,CharDiavolo->GetName() + " has not died!");
			isDead = false;
			break;
		}
	}

	//Everyone Dead
	if(isDead)
	{
		ResetGame();
	}
}

void ARealGamemode::ResetGame()
{
	bUseSeamlessTravel = true;
	//GEngine->AddOnScreenDebugMessage(-1,30,FColor::Magenta,"RESET MAP HOLD ON!");
	//UGameplayStatics::OpenLevel(GetWorld(), *GetWorld()->GetName(), false);
	GetWorld()->ServerTravel(*GetWorld()->GetName(),false,false);
}

void ARealGamemode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	AActor* StartLoc = nullptr;
	if(GetWorld() && GetWorld()->GetAuthGameMode())
		StartLoc = GetWorld()->GetAuthGameMode()->FindPlayerStart(NewPlayer,FString("None"));
	FVector Location;
	FRotator Rotation;

	if(StartLoc)
	{
		Location = StartLoc->GetActorLocation();
		Rotation = StartLoc->GetActorRotation();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.bNoFail = true;

	// spawn actual player
	GEngine->AddOnScreenDebugMessage(-1,60,FColor::Red,"Spawned Character For " + NewPlayer->GetName());
	ADiavoloPS* State = NewPlayer->GetPlayerState<ADiavoloPS>();
	ADIAVOLOPlayerController* DPC = Cast<ADIAVOLOPlayerController>(NewPlayer);
	if(State && State->CharacterToSpawn)
	{
		ADIAVOLOCharacter* NewCharacter = Cast<ADIAVOLOCharacter>(GetWorld()->SpawnActor(State->CharacterToSpawn, &Location, &Rotation, SpawnParams));
		if (NewCharacter)
		{
			NewPlayer->Possess(NewCharacter);
			GEngine->AddOnScreenDebugMessage(-1, 160, FColor::Green, "Possession Successful! " + NewCharacter->GetName());
			NewCharacter->OnDeathFunction.BindUFunction(DPC,FName("onDeath"));
			DPC->SetPawn(NewCharacter);
			DPC->SpawnedCharacter = NewCharacter;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 160, FColor::Red, "SpawnedCharacter is nullptr! Possession failed!");
		}
	}
}

/*
UClass* ARealGamemode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	ADiavoloPS* PS = InController->GetPlayerState<ADiavoloPS>();
	if(PS && PS->CharacterToSpawn) return SpectatorClass;
	return DefaultPawnClass;
}
*/
