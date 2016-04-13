// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "AversionInterceptor.h"
#include "AversionInterceptorCharacter.h"
#include "InterceptorCharacter.generated.h"


UCLASS(Abstract)
class AVERSIONINTERCEPTOR_API AInterceptorCharacter : public AAversionInterceptorCharacter
{


	GENERATED_BODY()
		AInterceptorCharacter(const class FObjectInitializer& ObjectInitializer);
public:
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	virtual void BeginPlay() override;

	/* Called every frame */
	virtual void Tick(float DeltaSeconds) override;
	/** Follow camera */

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface



	/* The default weapons to spawn with */
		UPROPERTY(VisibleAnywhere, Category = "Inventory")

			TArray<TSubclassOf<class AAversionWeaponBase>> DefaultInventoryClasses;





	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	/************************************************************************/
	/* Damage & Death                                                       */
	/************************************************************************/

	virtual void OnDeath(float KillingDamage, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser) override;


	/* Client mapped to Input */
	void OnStartSprinting();


	/* Client mapped to Input */
	void OnStopSprinting();



	virtual void SetSprinting(bool NewSprinting) override;


};
