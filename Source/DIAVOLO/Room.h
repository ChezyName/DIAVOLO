// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Actor.h"
#include "Room.generated.h"

UCLASS(BlueprintType)
class DIAVOLO_API ARoom : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoom();

	UPROPERTY(EditAnywhere)
	UArrowComponent* Entrence;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="Exits")
	TArray<UArrowComponent*> Exits;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
