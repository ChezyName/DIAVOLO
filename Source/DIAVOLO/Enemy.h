// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class DIAVOLO_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	UPROPERTY(Replicated,BlueprintReadOnly)
	float Health;

	UPROPERTY(EditDefaultsOnly,Category="Enemy",meta = (AllowPrivateAccess = "true"),BlueprintReadOnly)
	float MaxHealth = 10000;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UCapsuleComponent* MouseOver;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UPROPERTY(Replicated,BlueprintReadOnly)
	bool bUsingAbility = false;

	UFUNCTION(Server,Reliable,BlueprintCallable)
	virtual void CallRandomAbility();
	UFUNCTION(Server,Reliable,BlueprintCallable)
	virtual void CallRandomAbilityNonMoveNeed();
	UFUNCTION(Server,Reliable,BlueprintCallable)
	virtual void CallRandomUltimate();

	UFUNCTION(Server,Reliable)
	void PlayAnimationServer(UAnimMontage* Animation);

private:
	UFUNCTION(NetMulticast,Reliable)
	void PlayAnimationClient(UAnimMontage* Animation);

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	/** Handles moving forward/backward */
	void MoveForward(float Val);
	/** Handles stafing movement, left and right */
	void MoveRight(float Val);
	
public:	
	UFUNCTION(Server,Reliable)
	void Damage(float DamageAmount);

	UPROPERTY(EditAnywhere)
	bool bIsBoss = false;
};
