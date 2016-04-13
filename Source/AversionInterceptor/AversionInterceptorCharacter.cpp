// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "AversionInterceptor.h"
#include "AversionInterceptorCharacter.h"
#include "AversionInterceptorGameMode.h"
#include "AversionWeaponBase.h"
#include "SPlayerController.h"
#include "AversionPlayerState.h"
#include "DroneDamageType.h"

//////////////////////////////////////////////////////////////////////////
// AAversionInterceptorCharacter

AAversionInterceptorCharacter::AAversionInterceptorCharacter(const class FObjectInitializer& ObjectInitializer)
{
	Health = 100;

	TargetingSpeedModifier = 0.5f;
	SprintingSpeedModifier = 2.0f;

	/* Noise emitter for both players and enemies. This keeps track of MakeNoise data and is used by the pawnsensing component in our SZombieCharacter class */
//	NoiseEmitterComp = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("NoiseEmitterComp"));

	/* Don't collide with camera checks to keep 3rd person camera at position when zombies or other players are standing behind us */
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		
	WeaponAttachPoint = TEXT("WeaponSocket");
	PelvisAttachPoint = TEXT("PelvisSocket");
	SpineAttachPoint = TEXT("SpineSocket");

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



	Mesh1P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("PawnMesh1P"));
	Mesh1P->AttachParent = GetCapsuleComponent();
	Mesh1P->bOnlyOwnerSee = true;
	Mesh1P->bOwnerNoSee = false;
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	Mesh1P->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	Mesh1P->SetCollisionObjectType(ECC_Pawn);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);

	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = false;
	GetMesh()->bReceivesDecals = false;
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);


	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
}



float AAversionInterceptorCharacter::GetHealth() const
{
	return Health;
}


float AAversionInterceptorCharacter::GetMaxHealth() const
{
	// Retrieve the default value of the health property that is assigned on instantiation.
	return GetClass()->GetDefaultObject<AAversionInterceptorCharacter>()->Health;
}


FName AAversionInterceptorCharacter::GetWeaponAttachPoint() const
{
	return WeaponAttachPoint;
}

void AAversionInterceptorCharacter::SpawnDefaultInventory()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	//
	int32 NumWeaponClasses = DefaultInventoryClasses.Num();
	for (int32 i = 0; i < NumWeaponClasses; i++)
	{
		if (DefaultInventoryClasses[i])
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AAversionWeaponBase* NewWeapon = GetWorld()->SpawnActor<AAversionWeaponBase>(DefaultInventoryClasses[i], SpawnInfo);
			AddWeapon(NewWeapon);
	}
	}

	// equip first weapon in inventory
	if (Inventory.Num() > 0)
	{
		EquipWeapon(Inventory[0]);
	}
}

bool AAversionInterceptorCharacter::IsAlive() const
{
	return Health > 0;
}

int32 AAversionInterceptorCharacter::GetInventoryCount() const
{
	return Inventory.Num();
}

AAversionWeaponBase* AAversionInterceptorCharacter::GetInventoryWeapon(int32 index) const
{
	return Inventory[index];
}

bool AAversionInterceptorCharacter::IsEnemyFor(AController* TestPC) const
{
	if (TestPC == Controller || TestPC == NULL)
	{
		return false;
	}

	AAversionPlayerState* TestPlayerState = Cast<AAversionPlayerState>(TestPC->PlayerState);
	AAversionPlayerState* MyPlayerState = Cast<AAversionPlayerState>(PlayerState);

	bool bIsEnemy = true;
	if (GetWorld()->GameState && GetWorld()->GameState->GameModeClass)
	{
		const AAversionInterceptorGameMode* DefGame = GetWorld()->GameState->GameModeClass->GetDefaultObject<AAversionInterceptorGameMode>();
		if (DefGame)
		{
			bIsEnemy = DefGame->CanDealDamage(TestPlayerState, MyPlayerState);
		}
	}

	return bIsEnemy;
}

float AAversionInterceptorCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	if (Health <= 0.f)
	{
		return 0.f;
	}

	/* Modify based based on gametype rules */
//	AAversionInterceptorGameMode* MyGameMode = Cast<AAversionInterceptorGameMode>(GetWorld()->GetAuthGameMode());
//	Damage = MyGameMode ? MyGameMode->ModifyDamage(Damage, this, DamageEvent, EventInstigator, DamageCauser) : Damage;

	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;
		if (Health <= 0)
		{
			bool bCanDie = true;

			/* Check the damagetype, always allow dying if the cast fails, otherwise check the property if player can die from damagetype */
			if (DamageEvent.DamageTypeClass)
			{
			//	USDamageType* DmgType = Cast<USDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
			//	bCanDie = (DmgType == nullptr || (DmgType && DmgType->GetCanDieFrom()));
			}

			if (bCanDie)
			{
				Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
			}
			else
			{
				/* Player cannot die from this damage type, set hitpoints to 1.0 */
				Health = 1.0f;
			}
		}
		else
		{
			/* Shorthand for - if x != null pick1 else pick2 */
			APawn* Pawn = EventInstigator ? EventInstigator->GetPawn() : nullptr;
			PlayHit(ActualDamage, DamageEvent, Pawn, DamageCauser, false);
		}
	}

	return ActualDamage;
}


bool AAversionInterceptorCharacter::CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const
{
	/* Check if character is already dying, destroyed or if we have authority */
	if (bIsDying ||
		IsPendingKill() ||
		Role != ROLE_Authority ||
		GetWorld()->GetAuthGameMode() == NULL ||
		GetWorld()->GetAuthGameMode()->GetMatchState() == MatchState::LeavingMap)
	{
		return false;
	}

	return true;
}


void AAversionInterceptorCharacter::FellOutOfWorld(const class UDamageType& DmgType)
{
	Die(Health, FDamageEvent(DmgType.GetClass()), NULL, NULL);
}


bool AAversionInterceptorCharacter::Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser)
{
	if (!CanDie(KillingDamage, DamageEvent, Killer, DamageCauser))
	{
		return false;
	}

	Health = FMath::Min(0.0f, Health);

	// if this is an environmental death then refer to the previous killer so that they receive credit (knocked into lava pits, etc)
	UDamageType const* const DamageType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
	Killer = GetDamageInstigator(Killer, *DamageType);

	AController* const KilledPlayer = (Controller != NULL) ? Controller : Cast<AController>(GetOwner());
	GetWorld()->GetAuthGameMode<AAversionInterceptorGameMode>()->Killed(Killer, KilledPlayer, this, DamageType);

	NetUpdateFrequency = GetDefault<AAversionInterceptorGameMode>()->NetUpdateFrequency;
	GetCharacterMovement()->ForceReplicationUpdate();

	OnDeath(KillingDamage, DamageEvent, Killer ? Killer->GetPawn() : NULL, DamageCauser);
	return true;
}


void AAversionInterceptorCharacter::OnDeath(float KillingDamage, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser)
{

	if (bIsDying)
	{
		return;
	}

	bReplicateMovement = false;
	bTearOff = true;
	bIsDying = true;

	if (Role == ROLE_Authority)
	{
		ReplicateHit(KillingDamage, DamageEvent, PawnInstigator, DamageCauser, true);

		// play the force feedback effect on the client player controller
		APlayerController* PC = Cast<APlayerController>(Controller);
		if (PC && DamageEvent.DamageTypeClass)
		{
			UDroneDamageType *DamageType = Cast<UDroneDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
		//	if (DamageType && DamageType->KilledForceFeedback)
	//		{
	//			PC->ClientPlayForceFeedback(DamageType->KilledForceFeedback, false, "Damage");
	//		}
		}
	}

	// cannot use IsLocallyControlled here, because even local client's controller may be NULL here
	if (GetNetMode() != NM_DedicatedServer && DeathSound && Mesh1P && Mesh1P->IsVisible())
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	// remove all weapons
	DestroyInventory();

	// switch back to 3rd person view
	UpdatePawnMeshes();

	DetachFromControllerPendingDestroy();
	//StopAllAnimMontages();


	if (GetMesh())
	{
		static FName CollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetCollisionProfileName(CollisionProfileName);
	}
	SetActorEnableCollision(true);

	// Death anim
	float DeathAnimDuration = PlayAnimMontage(DeathAnim);

	// Ragdoll
	if (DeathAnimDuration > 0.f)
	{
		// Use a local timer handle as we don't need to store it for later but we don't need to look for something to clear
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AAversionInterceptorCharacter::SetRagdollPhysics, FMath::Min(0.1f, DeathAnimDuration), false);
	}
	else
	{
		SetRagdollPhysics();
	}

	// disable collisions on capsule
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
}


void AAversionInterceptorCharacter::SetRagdollPhysics()
{
	bool bInRagdoll = false;

	if (IsPendingKill())
	{
		bInRagdoll = false;
	}
	else if (!GetMesh() || !GetMesh()->GetPhysicsAsset())
	{
		bInRagdoll = false;
	}
	else
	{
		// initialize physics/etc
		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->WakeAllRigidBodies();
		GetMesh()->bBlendPhysics = true;

		bInRagdoll = true;
	}

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);

	if (!bInRagdoll)
	{
		// hide and set short lifespan
		TurnOff();
		SetActorHiddenInGame(true);
		SetLifeSpan(1.0f);
	}
	else
	{
		SetLifeSpan(30.0f);
	}
}


void AAversionInterceptorCharacter::PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled)
{
	if (Role == ROLE_Authority)
	{
		ReplicateHit(DamageTaken, DamageEvent, PawnInstigator, DamageCauser, bKilled);
	}

	if (GetNetMode() != NM_DedicatedServer)
	{
	//	if (bKilled && SoundDeath)
	//	{
	//		UGameplayStatics::SpawnSoundAttached(SoundDeath, RootComponent, NAME_None, FVector::ZeroVector, EAttachLocation::SnapToTarget, true);
	//	}
	//	else if (SoundTakeHit)
		//{
		//	UGameplayStatics::SpawnSoundAttached(SoundTakeHit, RootComponent, NAME_None, FVector::ZeroVector, EAttachLocation::SnapToTarget, true);
		//}
	}
}


void AAversionInterceptorCharacter::ReplicateHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled)
{
	const float TimeoutTime = GetWorld()->GetTimeSeconds() + 0.5f;

	//FDamageEvent const& LastDamageEvent = LastTakeHitInfo.GetDamageEvent();
	/*if (PawnInstigator == LastTakeHitInfo.PawnInstigator.Get() && LastDamageEvent.DamageTypeClass == LastTakeHitInfo.DamageTypeClass)
	{
		// Same frame damage
		if (bKilled && LastTakeHitInfo.bKilled)
		{
			// Redundant death take hit, ignore it
			return;
		}

		DamageTaken += LastTakeHitInfo.ActualDamage;
	}

	LastTakeHitInfo.ActualDamage = DamageTaken;
	LastTakeHitInfo.PawnInstigator = Cast<AAversionInterceptorCharacter>(PawnInstigator);
	LastTakeHitInfo.DamageCauser = DamageCauser;
	LastTakeHitInfo.SetDamageEvent(DamageEvent);
	LastTakeHitInfo.bKilled = bKilled;
	LastTakeHitInfo.EnsureReplication();*/
}


void AAversionInterceptorCharacter::OnRep_LastTakeHitInfo()
{
/*	if (LastTakeHitInfo.bKilled)
	{
		OnDeath(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get());
	}
	else
	{
		PlayHit(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get(), LastTakeHitInfo.bKilled);
	}*/
}

void AAversionInterceptorCharacter::OnRep_CurrentWeapon(AAversionWeaponBase* LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

void AAversionInterceptorCharacter::SetSprinting(bool NewSprinting)
{
	bWantsToRun = NewSprinting;

	if (bIsCrouched)
	{
		UnCrouch();
	}

	if (Role < ROLE_Authority)
	{
		ServerSetSprinting(NewSprinting);
	}
}


void AAversionInterceptorCharacter::ServerSetSprinting_Implementation(bool NewSprinting)
{
	SetSprinting(NewSprinting);
}


bool AAversionInterceptorCharacter::ServerSetSprinting_Validate(bool NewSprinting)
{
	return true;
}


bool AAversionInterceptorCharacter::IsSprinting() const
{
	if (!GetCharacterMovement())
	{
		return false;
	}

	return bWantsToRun && !IsTargeting() && !GetVelocity().IsZero()
		// Don't allow sprint while strafing sideways or standing still (1.0 is straight forward, -1.0 is backward while near 0 is sideways or standing still)
		&& (FVector::DotProduct(GetVelocity().GetSafeNormal2D(), GetActorRotation().Vector()) > 0.8); // Changing this value to 0.1 allows for diagonal sprinting. (holding W+A or W+D keys)
}


float AAversionInterceptorCharacter::GetSprintingSpeedModifier() const
{
	return SprintingSpeedModifier;
}



void AAversionInterceptorCharacter::SetTargeting(bool NewTargeting)
{
	bIsTargeting = NewTargeting;

	if (Role < ROLE_Authority)
	{
		ServerSetTargeting(NewTargeting);
	}
}


void AAversionInterceptorCharacter::ServerSetTargeting_Implementation(bool NewTargeting)
{
	SetTargeting(NewTargeting);
}


bool AAversionInterceptorCharacter::ServerSetTargeting_Validate(bool NewTargeting)
{
	return true;
}



bool AAversionInterceptorCharacter::IsTargeting() const
{
	return bIsTargeting;
}


float AAversionInterceptorCharacter::GetTargetingSpeedModifier() const
{
	return TargetingSpeedModifier;
}


FRotator AAversionInterceptorCharacter::GetAimOffsets() const
{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}















bool AAversionInterceptorCharacter::CanFire() const
{
	/* Add your own checks here, for example non-shooting areas or checking if player is in an NPC dialogue etc. */
	return IsAlive();
}


bool AAversionInterceptorCharacter::CanReload() const
{
	return IsAlive();
}


bool AAversionInterceptorCharacter::IsFiring() const
{
	return CurrentWeapon && CurrentWeapon->GetCurrentState() == EWeaponState::Firing;
}



FName AAversionInterceptorCharacter::GetInventoryAttachPoint(EInventorySlot Slot) const
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


void AAversionInterceptorCharacter::DestroyInventory()
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


void AAversionInterceptorCharacter::SetCurrentWeapon(class AAversionWeaponBase* NewWeapon, class AAversionWeaponBase* LastWeapon)
{
	AAversionWeaponBase* LocalLastWeapon = NULL;

	if (LastWeapon != NULL)
	{
		LocalLastWeapon = LastWeapon;
	}
	else if (NewWeapon != CurrentWeapon)
	{
		LocalLastWeapon = CurrentWeapon;
	}

	// unequip previous
	if (LocalLastWeapon)
	{
		LocalLastWeapon->OnUnEquip();
	}

	CurrentWeapon = NewWeapon;

	// equip new one
	if (NewWeapon)
	{
		NewWeapon->SetOwningPawn(this);	// Make sure weapon's MyPawn is pointing back to us. During replication, we can't guarantee APawn::CurrentWeapon will rep after AWeapon::MyPawn!

		NewWeapon->OnEquip(LastWeapon);
	}
}


AAversionWeaponBase* AAversionInterceptorCharacter::GetCurrentWeapon() const
{
	return CurrentWeapon;
}


void AAversionInterceptorCharacter::EquipWeapon(AAversionWeaponBase* Weapon)
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


bool AAversionInterceptorCharacter::ServerEquipWeapon_Validate(AAversionWeaponBase* Weapon)
{
	return true;
}


void AAversionInterceptorCharacter::ServerEquipWeapon_Implementation(AAversionWeaponBase* Weapon)
{
	EquipWeapon(Weapon);
}


void AAversionInterceptorCharacter::AddWeapon(class AAversionWeaponBase* Weapon)
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


void AAversionInterceptorCharacter::RemoveWeapon(class AAversionWeaponBase* Weapon, bool bDestroy)
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


void AAversionInterceptorCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	UpdatePawnMeshes();
	/* Equip the weapon on the client side. */
	SetCurrentWeapon(CurrentWeapon);
}


void AAversionInterceptorCharacter::OnReload()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartReload();
	}
}


void AAversionInterceptorCharacter::OnStartFire()
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


void AAversionInterceptorCharacter::OnStopFire()
{
	StopWeaponFire();
}

USkeletalMeshComponent* AAversionInterceptorCharacter::GetPawnMesh() const
{
	return IsFirstPerson() ? Mesh1P : GetMesh();
}

USkeletalMeshComponent* AAversionInterceptorCharacter::GetSpecifcPawnMesh(bool WantFirstPerson) const
{
	return WantFirstPerson == true ? Mesh1P : GetMesh();

}

bool AAversionInterceptorCharacter::IsFirstPerson() const
{
	return IsAlive() && Controller && Controller->IsLocalPlayerController();
}

void AAversionInterceptorCharacter::StartWeaponFire()
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


void AAversionInterceptorCharacter::StopWeaponFire()
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


void AAversionInterceptorCharacter::OnNextWeapon()
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


void AAversionInterceptorCharacter::OnPrevWeapon()
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


void AAversionInterceptorCharacter::DropWeapon()
{
	if (Role < ROLE_Authority)
	{
		//ServerDropWeapon();
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


AAversionWeaponBase * AAversionInterceptorCharacter::GetWeapon() const
{
	return CurrentWeapon;
}


void AAversionInterceptorCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Role == ROLE_Authority)
	{
		Health = GetMaxHealth();
		SpawnDefaultInventory();
	}

	// set initial mesh visibility (3rd person view)
	UpdatePawnMeshes();

	
	
}

bool AAversionInterceptorCharacter::IsFirstPerson()
{
	return false;
}

void AAversionInterceptorCharacter::UpdatePawnMeshes()
{
	bool const bFirstPerson = IsFirstPerson();

	Mesh1P->MeshComponentUpdateFlag = !bFirstPerson ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	Mesh1P->SetOwnerNoSee(!bFirstPerson);

	GetMesh()->MeshComponentUpdateFlag = bFirstPerson ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
GetMesh()->SetOwnerNoSee(bFirstPerson);
}


void AAversionInterceptorCharacter::ServerDropWeapon_Implementation()
{
	DropWeapon();
}



bool AAversionInterceptorCharacter::ServerDropWeapon_Validate()
{
	return true;
}







 void AAversionInterceptorCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// only to local owner: weapon change requests are locally instigated, other clients don't need it
DOREPLIFETIME_CONDITION(AAversionInterceptorCharacter, Inventory, COND_OwnerOnly);

	//everyone except local owner: flag change is locally instigated
	DOREPLIFETIME_CONDITION(AAversionInterceptorCharacter, bIsTargeting, COND_SkipOwner);
		DOREPLIFETIME_CONDITION(AAversionInterceptorCharacter, bWantsToRun, COND_SkipOwner);

	//DOREPLIFETIME_CONDITION(AAversionInterceptorCharacter, LastTakeHitInfo, COND_Custom);

	// everyone
	DOREPLIFETIME(AAversionInterceptorCharacter, CurrentWeapon);
	DOREPLIFETIME(AAversionInterceptorCharacter, Health);
}








 void AAversionInterceptorCharacter::TurnAtRate(float Rate)
 {
	 // calculate delta for this frame from the rate information
	 AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
 }

 void AAversionInterceptorCharacter::LookUpAtRate(float Rate)
 {
	 // calculate delta for this frame from the rate information
	 AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
 }

 void AAversionInterceptorCharacter::MoveForward(float Value)
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

 void AAversionInterceptorCharacter::MoveRight(float Value)
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

 void AAversionInterceptorCharacter::OnStartTargeting()
 {

	 bUseControllerRotationPitch = false;
	 bUseControllerRotationYaw = true;
	 bUseControllerRotationRoll = false;

	 SetTargeting(true);
 }


 void AAversionInterceptorCharacter::OnEndTargeting()
 {

	 bUseControllerRotationPitch = false;
	 bUseControllerRotationYaw = false;
	 bUseControllerRotationRoll = false;

	 SetTargeting(false);
 }




 void AAversionInterceptorCharacter::OnCrouchToggle()
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




 void AAversionInterceptorCharacter::KilledBy(class APawn* EventInstigator)
 {

 }
