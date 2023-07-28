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
struct FAbilityIcons
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UTexture2D* Skill1Icon;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UTexture2D* Skill2Icon;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UTexture2D* Skill3Icon;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UTexture2D* ULTIMATEIcon;
};

USTRUCT(BlueprintType)
struct FCooldowns
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float Skill1 = 0;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float Skill2 = 0;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float Skill3 = 0;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float Ultimate = 0;
};

USTRUCT(BlueprintType)
struct FManaConsumption
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float Skill1 = 0;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float Skill2 = 0;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float Skill3 = 0;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float Ultimate = 0;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float ManaRechargeRate = 15;
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

	UPROPERTY(EditAnywhere)
	USoundWave* AttackSFX;
	
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
public:
	UPROPERTY(EditAnywhere,Category="CameraZoom")
	float ZoomSpeed = 50;
	UPROPERTY(EditAnywhere,Category="CameraZoom")
	float ZoomMin = 500;
	UPROPERTY(EditAnywhere,Category="CameraZoom")
	float ZoomMax = 800;

	UPROPERTY(EditAnywhere,Category="Character Info | Health // Mana ")
	float MaxHealth = 1500;
	UPROPERTY(EditAnywhere,Category="Character Info | Health // Mana ")
	float MaxMana = 800;

	UPROPERTY(EditAnywhere,Category="Character Info | Emote ")
	UAnimMontage* Emote;
	UPROPERTY(EditAnywhere,Category="Character Info | Emote ")
	UAudioComponent* EmotePlayer;

	UPROPERTY(EditAnywhere,Category="Character Info | DodgeRoll ")
	UAnimMontage* DodgeAnim;
	UPROPERTY(EditAnywhere,Category="Character Info | DodgeRoll ")
	USoundWave* DodgeSound;
	UPROPERTY(EditAnywhere,Category="Character Info | DodgeRoll ")
	float DodgeFrames = 0.2;
	UPROPERTY(EditAnywhere,Category="Character Info | DodgeRoll ")
	float RestFrames = 0.12;
	
	UFUNCTION(Server,Reliable)
	void DodgeRoll(FVector MouseLocation);

	UFUNCTION(Client,Reliable)
	void ClientDeathNonBP();

	UFUNCTION(BlueprintImplementableEvent)
	void ClientDeath();

	bool IFRAMES = false;
	bool bisDodging = false;
	FVector DodgeDirection;

	UFUNCTION(Server,Reliable)
	void StartEmote();
	UFUNCTION(Server,Reliable)
	void StopEmote();

	UFUNCTION(NetMulticast,Reliable)
	void startEmoteSound();

	UFUNCTION(NetMulticast,Reliable)
	void stopEmoteSound();
	
	ADIAVOLOCharacter();

	UFUNCTION(Server,Reliable,BlueprintCallable)
	void CharacterTakeDamage(float DamageAmount, bool IgnoreIFrames = false);

	UPROPERTY()
	class ACharacterProxy* ParentProxy;

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	
	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	float DamageMultiplier = 1.f;

	float ManaCD = 0;
	float ManaCDOnSkillUse = 3;
	
	UPROPERTY(Replicated,BlueprintReadOnly)
	float BasicCD = 0;
	UPROPERTY(Replicated,BlueprintReadOnly)
	float Skill1CD = 0;
	UPROPERTY(Replicated,BlueprintReadOnly)
	float Skill2CD = 0;
	UPROPERTY(Replicated,BlueprintReadOnly)
	float Skill3CD = 0;
	UPROPERTY(Replicated,BlueprintReadOnly)
	float UltimateCD = 0;
	
	UPROPERTY(Replicated,BlueprintReadOnly)
	bool Skill1Active = false;
	UPROPERTY(Replicated,BlueprintReadOnly)
	bool Skill2Active = false;
	UPROPERTY(Replicated,BlueprintReadOnly)
	bool Skill3Active = false;
	UPROPERTY(Replicated,BlueprintReadOnly)
	bool UltimateActive = false;
	
	UPROPERTY(Replicated,BlueprintReadOnly)
	float Health = 0;
	UPROPERTY(Replicated,BlueprintReadOnly)
	float Mana = 0;

	UFUNCTION(BlueprintCallable)
	float GetHealthPercent();
	UFUNCTION(BlueprintCallable)
	float GetManaPercent();
	
	void MoveToRange(FVector Position,float Range);

	UFUNCTION(NetMulticast,Reliable)
	void PlaySound(const TArray<USoundWave*>& SFXs);

	UFUNCTION(NetMulticast,Reliable)
	void PlaySoundSingle(USoundWave* SFX);

	UFUNCTION(Server,Reliable)
	void ServerSetState(EPlayerStates State);

	UFUNCTION(Server,Reliable)
	void PlayAnimationServer(UAnimMontage* Animation);
	UFUNCTION(Server,Reliable)
	void StopAnimationServer(UAnimMontage* Animation);

	UFUNCTION(NetMulticast,Reliable)
	void PlayAnimationClient(UAnimMontage* Animation);
	UFUNCTION(NetMulticast,Reliable)
	void StopAnimationClient(UAnimMontage* Animation);

	DECLARE_DELEGATE(DOnDeath)
	DOnDeath OnDeathFunction;

	void onDeath();
	bool isDead = false;

	UPROPERTY(Replicated,BlueprintReadOnly)
	EPlayerStates CharState = EPlayerStates::E_IDLE;

	UPROPERTY(Replicated,BlueprintReadOnly)
	bool bUsingAbility = false;

	//ALL ATTACKS
	UPROPERTY(EditAnywhere,Category="Character Info | Attacks",BlueprintReadOnly)
	FAutoAttack AutoAttack;

	//Cooldowns
	UPROPERTY(EditAnywhere,Category="Character Info | Attacks",BlueprintReadOnly)
	FCooldowns AttackCooldowns;
	
	UPROPERTY(EditAnywhere,Category="Character Info | Attacks",BlueprintReadOnly)
	FManaConsumption AttackManaConsumption;

	UPROPERTY(EditAnywhere,Category="Character Info | Attacks",BlueprintReadOnly)
	FAbilityIcons AbilityIcons;
	
	UFUNCTION(Server,Reliable)
	virtual void onBasicSkill(AEnemy* Enemy);
	UFUNCTION(Server,Reliable)
	virtual void onSkill1(FVector Location,AEnemy* Enemy);
	UFUNCTION(Server,Reliable)
	virtual void onSkill2(FVector Location,AEnemy* Enemy);
	UFUNCTION(Server,Reliable)
	virtual void onSkill3(FVector Location,AEnemy* Enemy);
	UFUNCTION(Server,Reliable)
	virtual void onUltimate(FVector Location,AEnemy* Enemy);

	UFUNCTION(Server,Reliable)
	virtual void endSkill1();
	UFUNCTION(Server,Reliable)
	virtual void endSkill2();
	UFUNCTION(Server,Reliable)
	virtual void endSkill3();
	UFUNCTION(Server,Reliable)
	virtual void endUltimate();

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

