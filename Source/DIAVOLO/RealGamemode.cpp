// Fill out your copyright notice in the Description page of Project Settings.


#include "RealGamemode.h"

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
	UGameplayStatics::OpenLevel(GetWorld(), *GetWorld()->GetName(), false);
	GetWorld()->ServerTravel(*GetWorld()->GetName(),false,false);
}
