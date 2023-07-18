// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DiavoloPS.generated.h"

UENUM(BlueprintType)
enum class EPlayerStates : uint8
{
	E_IDLE = 0 UMETA(DisplayName="Idle"),
	E_MOVE = 1 UMETA(DisplayName="Moving"),
	E_MOVE_ATTACK = 2 UMETA(DisplayName="Moving For Attack"),
	E_ATTACK_WINDUP = 3 UMETA(DisplayName="Charging Auto Attack"),
	E_ATTACK_FULL = 3 UMETA(DisplayName="After Attack Hit"),
	E_ABILITY = 4 UMETA(DisplayName="Ability"),
	E_EMOTE = 5 UMETA(DisplayName="Emote        "),
};

/**
 * 
 */
UCLASS()
class DIAVOLO_API ADiavoloPS : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void CopyProperties(APlayerState* PlayerState) override;
	virtual void BeginPlay() override;

	UPROPERTY(Replicated,BlueprintReadOnly,Category="Character State")
	EPlayerStates CharState = EPlayerStates::E_IDLE;

	UPROPERTY(Replicated,BlueprintReadOnly,Category="Character State")
	FString CharName = "[UNKNOWN]";

	UPROPERTY(Replicated,BlueprintReadOnly,Category="Character State")
	TSubclassOf<class ADIAVOLOCharacter> CharacterToSpawn = nullptr;

	UPROPERTY(Replicated,BlueprintReadOnly,Category="Character State")
	UTexture2D* ClassIcon = nullptr;
	
	UFUNCTION(Server,Reliable)
	void ChangeCharState(EPlayerStates NewState);

	UFUNCTION(Server,Reliable,BlueprintCallable)
	void SetSpawnable(TSubclassOf<class ADIAVOLOCharacter> NewSpawnable);

	UFUNCTION(Server,Reliable,BlueprintCallable)
	void ChangeReadyState(bool isReady);

	UFUNCTION(Server,Reliable,BlueprintCallable)
	void SetClassIcon(UTexture2D* NewIcon);

	UPROPERTY(Replicated,BlueprintReadOnly,Category="Lobby")
	bool bCharReady = false;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
