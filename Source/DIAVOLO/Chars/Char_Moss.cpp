// Fill out your copyright notice in the Description page of Project Settings.


#include "Char_Moss.h"

void AChar_Moss::onSkill1(FVector Location, AEnemy* Enemy)
{
	if(Skill1CD < 0 && Mana >= AttackManaConsumption.Skill1)
	{
		CharState = EPlayerStates::E_ABILITY;

		//After Rocket Launched
		FTimerDelegate TimerAfter;
		TimerAfter.BindLambda([&]
		{
			Skill1CD = AttackCooldowns.Skill1;
			Mana -= AttackManaConsumption.Skill1;
			bUsingAbility = false;
			ManaCD = ManaCDOnSkillUse;
			CharState = EPlayerStates::E_IDLE;
		});
		
		//Launch Rocket
		FTimerDelegate TimerBefore;
		TimerBefore.BindLambda([&]
		{
			//Shoot Rocket HERE
			
			FTimerHandle TimerHandle2;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle2, TimerAfter, RPGTimeAfter, false);
		});
		

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerBefore, RPGTimeBefore, false);
	}
	Super::onSkill1(Location, Enemy);
}
