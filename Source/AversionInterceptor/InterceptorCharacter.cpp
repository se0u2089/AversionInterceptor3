// Fill out your copyright notice in the Description page of Project Settings.

#include "AversionInterceptor.h"
#include "AversionInterceptorCharacter.h"
#include "InterceptorCharacter.h"
#include "SPlayerController.h"
#include "AversionWeaponBase.h"


AInterceptorCharacter::AInterceptorCharacter(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;



	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	// Adjust jump to make it less floaty
	MoveComp->GravityScale = 1.5f;

	MoveComp->bCanWalkOffLedgesWhenCrouching = true;
	MoveComp->MaxWalkSpeedCrouched = 200;
	MoveComp->bOrientRotationToMovement = true; // Character moves in the direction of input...	

	/* Ignore this channel or it will absorb the trace impacts instead of the skeletal mesh */
	//GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	// Enable crouching
	MoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
	CameraBoomComp = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	CameraBoomComp->SocketOffset = FVector(0, 35, 0);
	CameraBoomComp->TargetOffset = FVector(0, 0, 55);
	CameraBoomComp->bUsePawnControlRotation = true;
	CameraBoomComp->AttachParent = GetRootComponent();

	CameraComp = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("Camera"));
	CameraComp->AttachParent = CameraBoomComp;
												   // Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	TargetingSpeedModifier = 0.5f;
	SprintingSpeedModifier = 2.5f;

	Health = 100;											   // are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	WeaponAttachPoint = TEXT("WeaponSocket");
	PelvisAttachPoint = TEXT("PelvisSocket");
	SpineAttachPoint = TEXT("SpineSocket");
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

	InputComponent->BindAxis("MoveForward", this, &AInterceptorCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AInterceptorCharacter::MoveRight);
	InputComponent->BindAction("Targeting", IE_Pressed, this, &AInterceptorCharacter::OnStartTargeting);
	InputComponent->BindAction("Targeting", IE_Released, this, &AInterceptorCharacter::OnEndTargeting);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AInterceptorCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AInterceptorCharacter::LookUpAtRate);


	InputComponent->BindAction("SprintHold", IE_Pressed, this, &AInterceptorCharacter::OnStartSprinting);
	InputComponent->BindAction("SprintHold", IE_Released, this, &AInterceptorCharacter::OnStopSprinting);
	InputComponent->BindAction("CrouchToggle", IE_Released, this, &AInterceptorCharacter::OnCrouchToggle);

	InputComponent->BindAction("Fire", IE_Pressed, this, &AInterceptorCharacter::OnStartFire);
	InputComponent->BindAction("Fire", IE_Released, this, &AInterceptorCharacter::OnStopFire);

	InputComponent->BindAction("Reload", IE_Pressed, this, &AInterceptorCharacter::OnReload);

	InputComponent->BindAction("NextWeapon", IE_Pressed, this, &AInterceptorCharacter::OnNextWeapon);
	InputComponent->BindAction("PrevWeapon", IE_Pressed, this, &AInterceptorCharacter::OnPrevWeapon);

	InputComponent->BindAction("EquipPrimaryWeapon", IE_Pressed, this, &AInterceptorCharacter::OnEquipPrimaryWeapon);
	InputComponent->BindAction("EquipSecondaryWeapon", IE_Pressed, this, &AInterceptorCharacter::OnEquipSecondaryWeapon);

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

void AInterceptorCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AInterceptorCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AInterceptorCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AInterceptorCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AInterceptorCharacter::OnStartTargeting()
{

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	SetTargeting(true);
}


void AInterceptorCharacter::OnEndTargeting()
{

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SetTargeting(false);
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



void AInterceptorCharacter::OnCrouchToggle()
{
	if (IsSprinting())
	{
		SetSprinting(false);
	}

	// If we are crouching then CanCrouch will return false. If we cannot crouch then calling Crouch() wont do anything
	if (CanCrouch())
	{
		Crouch();
	}
	else
	{
		UnCrouch();
	}
}


void AInterceptorCharacter::OnDeath(float KillingDamage, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser)
{
	if (bIsDying)
	{
		return;
	}



	Super::OnDeath(KillingDamage, DamageEvent, PawnInstigator, DamageCauser);
}


void AInterceptorCharacter::KilledBy(class APawn* EventInstigator)
{

}


bool AInterceptorCharacter::CanFire() const
{
	/* Add your own checks here, for example non-shooting areas or checking if player is in an NPC dialogue etc. */
	return IsAlive();
}


bool AInterceptorCharacter::CanReload() const
{
	return IsAlive();
}


bool AInterceptorCharacter::IsFiring() const
{
	return CurrentWeapon && CurrentWeapon->GetCurrentState() == EWeaponState::Firing;
}


FName AInterceptorCharacter::GetInventoryAttachPoint(EInventorySlot Slot) const
{
	/* Return the socket name for the specified storage slot */
	switch (Slot)
	{
	case EInventorySlot::Hands:
		return WeaponAttachPoint;
	case EInventorySlot::Primary:
		return SpineAttachPoint;
	case EInventorySlot::Secondary:
		return PelvisAttachPoint;
	default:
		// Not implemented.
		return "";
	}
}


void AInterceptorCharacter::DestroyInventory()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	for (int32 i = Inventory.Num() - 1; i >= 0; i--)
	{
		AAversionWeaponBase* Weapon = Inventory[i];
		if (Weapon)
		{
			RemoveWeapon(Weapon, true);
		}
	}
}


void AInterceptorCharacter::SetCurrentWeapon(class AAversionWeaponBase* NewWeapon, class AAversionWeaponBase* LastWeapon)
{
	/* Maintain a reference for visual weapon swapping */
	PreviousWeapon = LastWeapon;

	AAversionWeaponBase* LocalLastWeapon = nullptr;
	if (LastWeapon)
	{
		LocalLastWeapon = LastWeapon;
	}
	else if (NewWeapon != CurrentWeapon)
	{
		LocalLastWeapon = CurrentWeapon;
	}

	// UnEquip the current
	bool bHasPreviousWeapon = false;
	if (LocalLastWeapon)
	{
		LocalLastWeapon->OnUnEquip();
		bHasPreviousWeapon = true;
	}

	CurrentWeapon = NewWeapon;

	if (NewWeapon)
	{
		NewWeapon->SetOwningPawn(this);
		/* Only play equip animation when we already hold an item in hands */
		NewWeapon->OnEquip(bHasPreviousWeapon);
	}

	/* NOTE: If you don't have an equip animation w/ animnotify to swap the meshes halfway through, then uncomment this to immediately swap instead */
	//SwapToNewWeaponMesh();
}


void AInterceptorCharacter::OnRep_CurrentWeapon(AAversionWeaponBase* LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}


AAversionWeaponBase* AInterceptorCharacter::GetCurrentWeapon() const
{
	return CurrentWeapon;
}


void AInterceptorCharacter::EquipWeapon(AAversionWeaponBase* Weapon)
{
	if (Weapon)
	{
		/* Ignore if trying to equip already equipped weapon */
		if (Weapon == CurrentWeapon)
			return;

		if (Role == ROLE_Authority)
		{
			SetCurrentWeapon(Weapon, CurrentWeapon);
		}
		else
		{
			ServerEquipWeapon(Weapon);
		}
	}
}


bool AInterceptorCharacter::ServerEquipWeapon_Validate(AAversionWeaponBase* Weapon)
{
	return true;
}


void AInterceptorCharacter::ServerEquipWeapon_Implementation(AAversionWeaponBase* Weapon)
{
	EquipWeapon(Weapon);
}


void AInterceptorCharacter::AddWeapon(class AAversionWeaponBase* Weapon)
{
	if (Weapon && Role == ROLE_Authority)
	{
		Weapon->OnEnterInventory(this);
		Inventory.AddUnique(Weapon);

		// Equip first weapon in inventory
		if (Inventory.Num() > 0 && CurrentWeapon == nullptr)
		{
			EquipWeapon(Inventory[0]);
		}
	}
}


void AInterceptorCharacter::RemoveWeapon(class AAversionWeaponBase* Weapon, bool bDestroy)
{
	if (Weapon && Role == ROLE_Authority)
	{
		bool bIsCurrent = CurrentWeapon == Weapon;

		if (Inventory.Contains(Weapon))
		{
			Weapon->OnLeaveInventory();
		}
		Inventory.RemoveSingle(Weapon);

		/* Replace weapon if we removed our current weapon */
		if (bIsCurrent && Inventory.Num() > 0)
		{
			SetCurrentWeapon(Inventory[0]);
		}

		/* Clear reference to weapon if we have no items left in inventory */
		if (Inventory.Num() == 0)
		{
			SetCurrentWeapon(nullptr);
		}

		if (bDestroy)
		{
			Weapon->Destroy();
		}
	}
}


void AInterceptorCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	/* Equip the weapon on the client side. */
	SetCurrentWeapon(CurrentWeapon);
}


void AInterceptorCharacter::OnReload()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartReload();
	}
}


void AInterceptorCharacter::OnStartFire()
{
	if (IsSprinting())
	{
		SetSprinting(false);
	}

	//if (CarriedObjectComp->GetIsCarryingActor())
//	{
//		StopWeaponFire();

//		CarriedObjectComp->Throw();
//		return;
//	}

	StartWeaponFire();
}


void AInterceptorCharacter::OnStopFire()
{
	StopWeaponFire();
}


void AInterceptorCharacter::StartWeaponFire()
{
	if (!bWantsToFire)
	{
		bWantsToFire = true;
		if (CurrentWeapon)
		{
			CurrentWeapon->StartFire();
		}
	}
}


void AInterceptorCharacter::StopWeaponFire()
{
	if (bWantsToFire)
	{
		bWantsToFire = false;
		if (CurrentWeapon)
		{
			CurrentWeapon->StopFire();
		}
	}
}


void AInterceptorCharacter::OnNextWeapon()
{
	//if (CarriedObjectComp->GetIsCarryingActor())
	//{
	//	CarriedObjectComp->Rotate(0.0f, 1.0f);
	//	return;
	//}

	if (Inventory.Num() >= 2) // TODO: Check for weaponstate.
	{
		const int32 CurrentWeaponIndex = Inventory.IndexOfByKey(CurrentWeapon);
		AAversionWeaponBase* NextWeapon = Inventory[(CurrentWeaponIndex + 1) % Inventory.Num()];
		EquipWeapon(NextWeapon);
	}
}


void AInterceptorCharacter::OnPrevWeapon()
{
//	if (CarriedObjectComp->GetIsCarryingActor())
//	{
//		CarriedObjectComp->Rotate(0.0f, -1.0f);
//		return;
//	}

	if (Inventory.Num() >= 2) // TODO: Check for weaponstate.
	{
		const int32 CurrentWeaponIndex = Inventory.IndexOfByKey(CurrentWeapon);
		AAversionWeaponBase* PrevWeapon = Inventory[(CurrentWeaponIndex - 1 + Inventory.Num()) % Inventory.Num()];
		EquipWeapon(PrevWeapon);
	}
}


void AInterceptorCharacter::DropWeapon()
{
	if (Role < ROLE_Authority)
	{
		ServerDropWeapon();
		return;
	}

	if (CurrentWeapon)
	{
		FVector CamLoc;
		FRotator CamRot;

		if (Controller == nullptr)
		{
			return;
		}

		/* Find a location to drop the item, slightly in front of the player.
		Perform ray trace to check for blocking objects or walls and to make sure we don't drop any item through the level mesh */
		Controller->GetPlayerViewPoint(CamLoc, CamRot);
		FVector SpawnLocation;
		FRotator SpawnRotation = CamRot;

		const FVector Direction = CamRot.Vector();
		const FVector TraceStart = GetActorLocation();
		const FVector TraceEnd = GetActorLocation() + (Direction * DropWeaponMaxDistance);

		/* Setup the trace params, we are only interested in finding a valid drop position */
		FCollisionQueryParams TraceParams;
		TraceParams.bTraceComplex = false;
		TraceParams.bReturnPhysicalMaterial = false;
		TraceParams.AddIgnoredActor(this);

		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldDynamic, TraceParams);

		/* Find farthest valid spawn location */
		if (Hit.bBlockingHit)
		{
			/* Slightly move away from impacted object */
			SpawnLocation = Hit.ImpactPoint + (Hit.ImpactNormal * 20);
		}
		else
		{
			SpawnLocation = TraceEnd;
		}

		/* Spawn the "dropped" weapon */
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//	ASWeaponPickup* NewWeaponPickup = GetWorld()->SpawnActor<ASWeaponPickup>(CurrentWeapon->WeaponPickupClass, SpawnLocation, FRotator::ZeroRotator, SpawnInfo);

//		if (NewWeaponPickup)
//		{
//			/* Apply torque to make it spin when dropped. */
//			UStaticMeshComponent* MeshComp = NewWeaponPickup->GetMeshComponent();
//			if (MeshComp)
//			{
//				MeshComp->SetSimulatePhysics(true);
	//			MeshComp->AddTorque(FVector(1, 1, 1) * 4000000);
	//		}
	//	}

		RemoveWeapon(CurrentWeapon, true);
	}
}


void AInterceptorCharacter::ServerDropWeapon_Implementation()
{
	DropWeapon();
}


bool AInterceptorCharacter::ServerDropWeapon_Validate()
{
	return true;
}


void AInterceptorCharacter::OnEquipPrimaryWeapon()
{
//	if (CarriedObjectComp->GetIsCarryingActor())
//	{
//	CarriedObjectComp->Rotate(1.0f, 0.0f);
//		return;
	//}

	if (Inventory.Num() >= 1)
	{
		/* Find first weapon that uses primary slot. */
		for (int32 i = 0; i < Inventory.Num(); i++)
		{
			AAversionWeaponBase* Weapon = Inventory[i];
			if (Weapon->GetStorageSlot() == EInventorySlot::Primary)
			{
				EquipWeapon(Weapon);
			}
		}
	}
}


void AInterceptorCharacter::OnEquipSecondaryWeapon()
{
//	if (CarriedObjectComp->GetIsCarryingActor())
//	{
//		CarriedObjectComp->Rotate(-1.0f, 0.0f);
///		return;
//	}

	if (Inventory.Num() >= 2)
	{
		/* Find first weapon that uses secondary slot. */
		for (int32 i = 0; i < Inventory.Num(); i++)
		{
			AAversionWeaponBase* Weapon = Inventory[i];
			if (Weapon->GetStorageSlot() == EInventorySlot::Secondary)
			{
				EquipWeapon(Weapon);
			}
		}
	}
}

bool AInterceptorCharacter::WeaponSlotAvailable(EInventorySlot CheckSlot)
{
	/* Iterate all weapons to see if requested slot is occupied */
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		AAversionWeaponBase* Weapon = Inventory[i];
		if (Weapon)
		{
			if (Weapon->GetStorageSlot() == CheckSlot)
				return false;
		}
	}

	return true;

	/* Special find function as alternative to looping the array and performing if statements
	the [=] prefix means "capture by value", other options include [] "capture nothing" and [&] "capture by reference" */
	//return nullptr == Inventory.FindByPredicate([=](ASWeapon* W){ return W->GetStorageSlot() == CheckSlot; });
}



void AInterceptorCharacter::SwapToNewWeaponMesh()
{
	if (PreviousWeapon)
	{
		PreviousWeapon->AttachMeshToPawn(PreviousWeapon->GetStorageSlot());
	}

	if (CurrentWeapon)
	{
		CurrentWeapon->AttachMeshToPawn(EInventorySlot::Hands);
	}
}