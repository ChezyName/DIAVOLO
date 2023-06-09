// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DIAVOLOCharacter.generated.h"

UCLASS(Blueprintable)
class ADIAVOLOCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,Category="CameraZoom")
	float ZoomSpeed = 50;
	UPROPERTY(EditAnywhere,Category="CameraZoom")
	float ZoomMin = 500;
	UPROPERTY(EditAnywhere,Category="CameraZoom")
	float ZoomMax = 1700;

	UPROPERTY(EditAnywhere,Category="CameraZoom")
	float MaxHealth = 1500;

	UPROPERTY(EditAnywhere,Category="CameraZoom")
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

protected:
	virtual void onBasicSkill();
	virtual void onSkill1();
	virtual void onSkill2();
	virtual void onSkill3();
	virtual void onSkill4();
	virtual void onUltimate();

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

