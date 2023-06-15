// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BaseProjectile.h"
#include "DIAVOLOPlayerController.h"
#include "GameFramework/Character.h"
#include "DIAVOLOCharacter.generated.h"

UENUM(BlueprintType)
enum class EAutoType : uint8
{
	E_MELEE = 0 UMETA(DisplayName="Melee"),
	E_RANGE = 1 UMETA(DisplayName="Ranged"),
};

USTRUCT(BlueprintType)
struct FAutoAttack
{
	GENERATED_BODY()

	//The Type Of Attack
	UPROPERTY(EditAnywhere)
	EAutoType AutoType = EAutoType::E_MELEE;

	//If Its A Projectile
	UPROPERTY(EditAnywhere,meta=(EditCondition="AutoType == EAutoType::E_RANGE"))
	TSubclassOf<class ABaseProjectile> Projectile;

	UPROPERTY(EditAnywhere,meta=(EditCondition="AutoType == EAutoType::E_RANGE"))
	float ProjectileVelocity = 100;

	//Animation
	UPROPERTY(EditAnywhere)
	UAnimMontage* Animation;

	//Delay Before Dealing / Calculating Damage
	UPROPERTY(EditAnywhere)
	float TimeBeforeAttack = 0;

	//Delay After Doing Anything
	UPROPERTY(EditAnywhere)
	float TimeAfterAttack = 0;

	UPROPERTY(EditAnywhere)
	float AttackDamage = 125;

	UPROPERTY(EditAnywhere)
	float AttackRange = 250;
};

UCLASS(Blueprintable)
class ADIAVOLOCharacter : public ACharacter
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,Category="CameraZoom")
	float ZoomSpeed = 50;
	UPROPERTY(EditAnywhere,Category="CameraZoom")
	float ZoomMin = 500;
	UPROPERTY(EditAnywhere,Category="CameraZoom")
	float ZoomMax = 800;

	UPROPERTY(EditAnywhere,Category="Character Info")
	float MaxHealth = 1500;

	UPROPERTY(EditAnywhere,Category="Character Info")
	float MaxMana = 800;

public:
	ADIAVOLOCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	
	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	float DamageMultiplier = 1.f;
	
	UPROPERTY(Replicated)
	float BasicCD = 0;
	UPROPERTY(Replicated)
	float Skill1CD = 0;
	UPROPERTY(Replicated)
	float Skill2CD = 0;
	UPROPERTY(Replicated)
	float Skill3CD = 0;
	UPROPERTY(Replicated)
	float Skill4CD = 0;
	UPROPERTY(Replicated)
	float UltimateCD = 0;

	UPROPERTY(Replicated,BlueprintReadOnly)
	float Health = 0;
	UPROPERTY(Replicated,BlueprintReadOnly)
	float Mana = 0;
	
	void MoveToRange(FVector Position,float Range);

	UFUNCTION(Server,Reliable)
	void ServerSetState(EPlayerStates State);

	UFUNCTION(Server,Reliable)
	void PlayAnimationServer(UAnimMontage* Animation);

	UFUNCTION(Server,Reliable)
	void StopAnimationServer(UAnimMontage* Animation);

	UPROPERTY(Replicated,BlueprintReadOnly)
	EPlayerStates CharState = EPlayerStates::E_IDLE;

	//ALL ATTACKS
	UPROPERTY(EditAnywhere,Category="Character Info")
	FAutoAttack AutoAttack;

	UFUNCTION(Server,Reliable)
	virtual void onBasicSkill(AEnemy* Enemy);
	UFUNCTION(Server,Reliable)
	virtual void onSkill1(AEnemy* Enemy);
	UFUNCTION(Server,Reliable)
	virtual void onSkill2(AEnemy* Enemy);
	UFUNCTION(Server,Reliable)
	virtual void onSkill3(AEnemy* Enemy);
	UFUNCTION(Server,Reliable)
	virtual void onSkill4(AEnemy* Enemy);
	UFUNCTION(Server,Reliable)
	virtual void onUltimate(AEnemy* Enemy);

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ZoomCamera(float Speed);
};

