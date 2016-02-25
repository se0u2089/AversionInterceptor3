// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AversionInterceptorCharacter.h"
#include "InterceptorCharacter.generated.h"


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

/**
 * 
 */
UCLASS()
class AVERSIONINTERCEPTOR_API AInterceptorCharacter : public AAversionInterceptorCharacter
{
	GENERATED_BODY()
		AInterceptorCharacter(const class FObjectInitializer& ObjectInitializer);
	virtual void PawnClientRestart() override;
		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	virtual void BeginPlay() override;

	/* Called every frame */
	virtual void Tick(float DeltaSeconds) override;
	/** Follow camera */

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

public:




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

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
private:
	/* Boom to handle distance to player mesh. */
	UPROPERTY(VisibleAnywhere, Category = "Camera")
		USpringArmComponent* CameraBoomComp;

	/* Primary camera of the player*/
	UPROPERTY(VisibleAnywhere, Category = "Camera")
		UCameraComponent* CameraComp;

public:
		


	FORCEINLINE UCameraComponent* GetCameraComponent()
	{
		return CameraComp;
	}




		bool bWantsToFire;

	void OnStartTargeting();

	void OnEndTargeting();

	/* Client mapped to Input */
	void OnStartSprinting();


	/* Client mapped to Input */
	void OnStopSprinting();



	virtual void SetSprinting(bool NewSprinting) override;



	/* Client mapped to Input */
	void OnCrouchToggle();

	/************************************************************************/
	/* Damage & Death                                                       */
	/************************************************************************/

	virtual void OnDeath(float KillingDamage, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser) override;


	virtual void KilledBy(class APawn* EventInstigator);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
		FName WeaponAttachPoint;

	/* Attachpoint for items carried on the belt/pelvis. */
	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
		FName PelvisAttachPoint;

	/* Attachpoint for primary weapons */
	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
		FName SpineAttachPoint;
	





	/* Distance away from character when dropping inventory items. */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
		float DropWeaponMaxDistance;

	void OnReload();

	/* Mapped to input */
	void OnStartFire();

	/* Mapped to input */
	void OnStopFire();

	/* Mapped to input */
	void OnNextWeapon();

	/* Mapped to input */
	void OnPrevWeapon();

	/* Mapped to input */
	void OnEquipPrimaryWeapon();

	/* Mapped to input */
	void OnEquipSecondaryWeapon();

	void StartWeaponFire();

	void StopWeaponFire();

	void DestroyInventory();

	/* Mapped to input. Drops current weapon */
	void DropWeapon();

	UFUNCTION(Reliable, Server, WithValidation)
		void ServerDropWeapon();

	void ServerDropWeapon_Implementation();

	bool ServerDropWeapon_Validate();

public:

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		AAversionWeaponBase* GetCurrentWeapon() const;

	/* Check if the specified slot is available, limited to one item per type (primary, secondary) */
	bool WeaponSlotAvailable(EInventorySlot CheckSlot);

	/* Check if pawn is allowed to fire weapon */
	bool CanFire() const;

	bool CanReload() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		bool IsFiring() const;

	/* Return socket name for attachments (to match the socket in the character skeleton) */
	FName GetInventoryAttachPoint(EInventorySlot Slot) const;

	/* All weapons/items the player currently holds */
	UPROPERTY()
		TArray<AAversionWeaponBase*> Inventory;

	void SetCurrentWeapon(class AAversionWeaponBase* newWeapon, class AAversionWeaponBase* LastWeapon = nullptr);

	void EquipWeapon(AAversionWeaponBase* Weapon);

	UFUNCTION(Reliable, Server, WithValidation)
		void ServerEquipWeapon(AAversionWeaponBase* Weapon);

	void ServerEquipWeapon_Implementation(AAversionWeaponBase* Weapon);

	bool ServerEquipWeapon_Validate(AAversionWeaponBase* Weapon);

	/* OnRep functions can use a parameter to hold the previous value of the variable. Very useful when you need to handle UnEquip etc. */
	UFUNCTION()
		void OnRep_CurrentWeapon(AAversionWeaponBase* LastWeapon);

	void AddWeapon(class AAversionWeaponBase* Weapon);

	void RemoveWeapon(class AAversionWeaponBase* Weapon, bool bDestroy);

	UPROPERTY()
	class AAversionWeaponBase* CurrentWeapon;

	class AAversionWeaponBase* PreviousWeapon;

	/* Update the weapon mesh to the newly equipped weapon, this is triggered during an anim montage.
	NOTE: Requires an AnimNotify created in the Equip animation to tell us when to swap the meshes. */
	UFUNCTION(BlueprintCallable, Category = "Animation")
		void SwapToNewWeaponMesh();

};
