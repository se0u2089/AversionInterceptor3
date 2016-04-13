// Fill out your copyright notice in the Description page of Project Settings.

#include "AversionInterceptor.h"
#include "InterceptorCharacter.h"
#include "AversionInterceptorCharacter.h"




AInterceptorCharacter::AInterceptorCharacter(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

									   // are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	Health = 100;

}


void AInterceptorCharacter::BeginPlay()
{
	Super::BeginPlay();


}


void AInterceptorCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bWantsToRun && !IsSprinting())
	{
		SetSprinting(true);
	}


}

//////////////////////////////////////////////////////////////////////////
// Input

void AInterceptorCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAxis("MoveForward", this, &AAversionInterceptorCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AAversionInterceptorCharacter::MoveRight);
	InputComponent->BindAction("Targeting", IE_Pressed, this, &AAversionInterceptorCharacter::OnStartTargeting);
	InputComponent->BindAction("Targeting", IE_Released, this, &AAversionInterceptorCharacter::OnEndTargeting);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AAversionInterceptorCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AAversionInterceptorCharacter::LookUpAtRate);


	InputComponent->BindAction("SprintHold", IE_Pressed, this, &AInterceptorCharacter::OnStartSprinting);
	InputComponent->BindAction("SprintHold", IE_Released, this, &AInterceptorCharacter::OnStopSprinting);
	InputComponent->BindAction("CrouchToggle", IE_Released, this, &AInterceptorCharacter::OnCrouchToggle);

	InputComponent->BindAction("Fire", IE_Pressed, this, &AAversionInterceptorCharacter::OnStartFire);
	InputComponent->BindAction("Fire", IE_Released, this, &AAversionInterceptorCharacter::OnStopFire);

	InputComponent->BindAction("Reload", IE_Pressed, this, &AAversionInterceptorCharacter::OnReload);

	InputComponent->BindAction("NextWeapon", IE_Pressed, this, &AAversionInterceptorCharacter::OnNextWeapon);
	InputComponent->BindAction("PrevWeapon", IE_Pressed, this, &AAversionInterceptorCharacter::OnPrevWeapon);

	//InputComponent->BindAction("EquipPrimaryWeapon", IE_Pressed, this, &AAversionInterceptorCharacter::OnEquipPrimaryWeapon);
	//InputComponent->BindAction("EquipSecondaryWeapon", IE_Pressed, this, &AAversionInterceptorCharacter::OnEquipSecondaryWeapon);

	// handle touch devices
	InputComponent->BindTouch(IE_Pressed, this, &AInterceptorCharacter::TouchStarted);
	InputComponent->BindTouch(IE_Released, this, &AInterceptorCharacter::TouchStopped);
}




void AInterceptorCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	// jump, but only on the first touch
	if (FingerIndex == ETouchIndex::Touch1)
	{
		Jump();
	}
}

void AInterceptorCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (FingerIndex == ETouchIndex::Touch1)
	{
		StopJumping();
	}
}

void AInterceptorCharacter::OnDeath(float KillingDamage, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser)
{



	Super::OnDeath(KillingDamage, DamageEvent, PawnInstigator, DamageCauser);
}


void AInterceptorCharacter::OnStartSprinting()
{


	SetSprinting(true);
}


void AInterceptorCharacter::SetSprinting(bool NewSprinting)
{
	if (bWantsToRun)
	{
		//	StopWeaponFire();
	}

	Super::SetSprinting(NewSprinting);
}

void AInterceptorCharacter::OnStopSprinting()
{
	SetSprinting(false);
}


