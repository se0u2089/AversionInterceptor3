// Fill out your copyright notice in the Description page of Project Settings.

#include "AversionInterceptor.h"
#include "AversionInterceptorCharacter.h"
#include "CyberPirateBot.h"
#include "CyberBotAIController.h"
#include "AversionWeaponBase.h"
#include "AversionInterceptorGameMode.h"



ACyberPirateBot::ACyberPirateBot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AIControllerClass = ACyberBotAIController::StaticClass();

	UpdatePawnMeshes();

	bUseControllerRotationYaw = true;

	WeaponAttachPoint = TEXT("WeaponSocket");
	PelvisAttachPoint = TEXT("PelvisSocket");
	SpineAttachPoint = TEXT("SpineSocket");

}

void ACyberPirateBot::PostInitializeComponents()
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

void ACyberPirateBot::UpdatePawnMeshes()
{
	bool const bFirstPerson = IsFirstPerson();

	Mesh1P->MeshComponentUpdateFlag = !bFirstPerson ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	Mesh1P->SetOwnerNoSee(!bFirstPerson);

	GetMesh()->MeshComponentUpdateFlag = bFirstPerson ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	GetMesh()->SetOwnerNoSee(bFirstPerson);
}

FName ACyberPirateBot::GetInventoryAttachPoint(EInventorySlot Slot) const
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
void ACyberPirateBot::SpawnDefaultInventory()
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

bool ACyberPirateBot::IsFirstPerson() const
{
	return false;
}

void ACyberPirateBot::FaceRotation(FRotator NewRotation, float DeltaTime)
{
	FRotator CurrentRotation = FMath::RInterpTo(GetActorRotation(), NewRotation, DeltaTime, 8.0f);

	Super::FaceRotation(CurrentRotation, DeltaTime);
}


FName ACyberPirateBot::GetWeaponAttachPoint() const
{
	return WeaponAttachPoint;
}