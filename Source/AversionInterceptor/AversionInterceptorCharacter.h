// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "AversionInterceptor.h"
//#include "GameFramework/Character.h"
#include "AversionInterceptorCharacter.generated.h"



UENUM()
enum class EInventorySlot : uint8
{
	/* For currently equipped items/weapons */
	Hands,

	/* For primary weapons on spine bone */
	Primary,

	/* Storage for small items like flashlight on pelvis */
	Secondary,
};

UCLASS(Abstract)
class AAversionInterceptorCharacter : public ACharacter
{
	GENERATED_BODY()

		/** Camera boom positioning the camera behind the character */
public:
	AAversionInterceptorCharacter(const class FObjectInitializer& ObjectInitializer);


	virtual void PawnClientRestart() override;


	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;


	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);



	/** check if pawn can fire weapon */
	bool CanFire() const;

	/** check if pawn can reload weapon */
	bool CanReload() const;



	bool IsEnemyFor(AController* TestPC) const;



	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		USoundCue* SoundTakeHit;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		USoundCue* SoundDeath;

	/************************************************************************/
	/* Health                                                               */
	/************************************************************************/

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
		float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
		float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
		bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
		virtual bool IsSprinting() const;

	/* Client/local call to update sprint state  */
	virtual void SetSprinting(bool NewSprinting);

	float GetSprintingSpeedModifier() const;
	/* Character wants to run, checked during Tick to see if allowed */
	UPROPERTY(Transient, Replicated)
		bool bWantsToRun;

	UPROPERTY(Transient, Replicated)
		bool bIsTargeting;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition", Replicated)
		float Health;


	UPROPERTY(EditDefaultsOnly, Category = "Movement")
		float SprintingSpeedModifier;


	/* Server side call to update actual sprint state */
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetSprinting(bool NewSprinting);

	void ServerSetSprinting_Implementation(bool NewSprinting);

	bool ServerSetSprinting_Validate(bool NewSprinting);

	/************************************************************************/
	/* Targeting                                                            */
	/************************************************************************/

	void SetTargeting(bool NewTargeting);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetTargeting(bool NewTargeting);

	void ServerSetTargeting_Implementation(bool NewTargeting);

	bool ServerSetTargeting_Validate(bool NewTargeting);

	

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
		float TargetingSpeedModifier;


	/* Is player aiming down sights */
	UFUNCTION(BlueprintCallable, Category = "Targeting")
		bool IsTargeting() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	float GetTargetingSpeedModifier() const;

	/* Retrieve Pitch/Yaw from current camera */
	UFUNCTION(BlueprintCallable, Category = "Targeting")
		FRotator GetAimOffsets() const;

	/** get camera view type */
	UFUNCTION(BlueprintCallable, Category = Mesh)
		virtual bool IsFirstPerson() const;
	/** get mesh component */
	USkeletalMeshComponent* GetPawnMesh() const;
	/** get mesh component */
	USkeletalMeshComponent* GetSpecifcPawnMesh(bool WantFirstPerson) const;
		/** pawn mesh: 1st person view */
		//UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	//		USkeletalMeshComponent* Mesh1P;
	

	/************************************************************************/
	/* Damage & Death                                                       */
	/************************************************************************/


	

	/* Take damage & handle death */
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	virtual bool CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const;

	virtual bool Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser);

	virtual void OnDeath(float KillingDamage, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser);
	/** animation played on death */

	UPROPERTY(EditDefaultsOnly, Category = Animation)
		UAnimMontage* DeathAnim;

	/** sound played on death, local player only */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		USoundCue* DeathSound;

	virtual void FellOutOfWorld(const class UDamageType& DmgType) override;

	void SetRagdollPhysics();

	virtual void PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled);

	void ReplicateHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled);

	/* Holds hit data to replicate hits and death to clients */
//	UPROPERTY(Transient, ReplicatedUsing = OnRep_LastTakeHitInfo)
//	struct FTakeHitInfo LastTakeHitInfo;

	//UFUNCTION()
		void OnRep_LastTakeHitInfo();

	bool bIsDying;
	bool bInRagDoll;

	/** [server] spawns default inventory */
	void SpawnDefaultInventory();


	class AAversionWeaponBase* GetInventoryWeapon(int32 index) const;

	
protected:

	/** default inventory list */

	//UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TArray<TSubclassOf<class AAversionWeaponBase> > DefaultInventoryClasses;
		/* The default weapons to spawn with */

	

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(Transient, Replicated)
		TArray<AAversionWeaponBase*> Inventory;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon)
	class AAversionWeaponBase* CurrentWeapon;

	class AAversionWeaponBase* PreviousWeapon;



	//UPROPERTY(EditDefaultsOnly, Category = "Category=Inventory")
FName WeaponAttachPoint;

	/* Attachpoint for items carried on the belt/pelvis. */
//	UPROPERTY(EditDefaultsOnly, Category = "Category=Inventory")
		FName PelvisAttachPoint;

	/* Attachpoint for primary weapons */
	//UPROPERTY(EditDefaultsOnly, Category = "Sockets")
		FName SpineAttachPoint;

	public:

	bool bWantsToFire;




	/* Distance away from character when dropping inventory items. */
	//UPROPERTY(EditDefaultsOnly, Category = "Inventory")
		float DropWeaponMaxDistance;

	void OnReload();

	/* Mapped to input */

	void OnStartFire();


	void OnStopFire();

	/* Mapped to input */
	void OnNextWeapon();

	/* Mapped to input */
	void OnPrevWeapon();


	void StartWeaponFire();

	void StopWeaponFire();


	void DestroyInventory();

	/* Mapped to input. Drops current weapon */
	void DropWeapon();


	/** get weapon attach point */
	FName GetWeaponAttachPoint() const;

	/** get total number of inventory items */
	int32 GetInventoryCount() const;

	UFUNCTION(Reliable, Server, WithValidation)
		void ServerDropWeapon();

	/** get currently equipped weapon */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	class AAversionWeaponBase* GetWeapon() const;

	void PostInitializeComponents();


	void ServerDropWeapon_Implementation();

	bool ServerDropWeapon_Validate();



	/** sound played on respawn */
	//UPROPERTY(EditDefaultsOnly, Category = Pawn)
		USoundCue* RespawnSound;

	/** sound played when health is low */
//	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		USoundCue* LowHealthSound;
		/** pawn mesh: 1st person view */
	
		UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
			USkeletalMeshComponent* Mesh1P;
	/** sound played when running */
//	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		USoundCue* RunLoopSound;

	/** sound played when stop running */
	//UPROPERTY(EditDefaultsOnly, Category = Pawn)
		USoundCue* RunStopSound;

	/** sound played when targeting state changes */
	//UPROPERTY(EditDefaultsOnly, Category = Pawn)
		USoundCue* TargetingSound;

	/** used to manipulate with run loop sound */


	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
		AAversionWeaponBase* GetCurrentWeapon() const;


	bool IsFirstPerson();
	void UpdatePawnMeshes();

	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
		bool IsFiring() const;

	



	/* All weapons/items the player currently holds */






	void SetCurrentWeapon(class AAversionWeaponBase* newWeapon, class AAversionWeaponBase* LastWeapon = nullptr);


public:
	void AddWeapon(class AAversionWeaponBase* Weapon);

	void RemoveWeapon(class AAversionWeaponBase* Weapon, bool bDestroy);




	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;



	void EquipWeapon(AAversionWeaponBase* Weapon);
	/* Return socket name for attachments (to match the socket in the character skeleton) */
	FName GetInventoryAttachPoint(EInventorySlot Slot) const;

	
	UFUNCTION(Reliable, Server, WithValidation)
		void ServerEquipWeapon(AAversionWeaponBase* Weapon);

	void ServerEquipWeapon_Implementation(AAversionWeaponBase* Weapon);

	bool ServerEquipWeapon_Validate(AAversionWeaponBase* Weapon);

	/* OnRep functions can use a parameter to hold the previous value of the variable. Very useful when you need to handle UnEquip etc. */
	UFUNCTION()
		void OnRep_CurrentWeapon(AAversionWeaponBase* LastWeapon);

	
		/* Boom to handle distance to player mesh. */
		UPROPERTY(VisibleAnywhere, Category = "Camera")
			USpringArmComponent* CameraBoomComp;

		/* Primary camera of the player*/
		UPROPERTY(VisibleAnywhere, Category = "Camera")
			UCameraComponent* CameraComp;





	FORCEINLINE UCameraComponent* GetCameraComponent()
	{
		return CameraComp;
	}





	void OnStartTargeting();

	void OnEndTargeting();




	/* Client mapped to Input */
	void OnCrouchToggle();


	virtual void KilledBy(class APawn* EventInstigator);




};

