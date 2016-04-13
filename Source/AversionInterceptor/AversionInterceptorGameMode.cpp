// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "AversionInterceptor.h"
#include "AversionInterceptorGameMode.h"
#include "AversionInterceptorCharacter.h"
#include "MyPlayerController.h"
#include "APlayerCameraManager.h"	
#include "InterceptorCharacter.h"
#include "AversionWeaponBase.h"
#include "AversionPlayerState.h"

AAversionInterceptorGameMode::AAversionInterceptorGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	/* Assign the class types used by this gamemode */
	PlayerControllerClass = AMyPlayerController::StaticClass();
	PlayerStateClass = AAversionPlayerState::StaticClass();
	
	



}


bool AAversionInterceptorGameMode::CanDealDamage(class AAversionPlayerState* DamageInstigator, class AAversionPlayerState* DamagedPlayer) const
{
	return true;
}


void AAversionInterceptorGameMode::SetPlayerDefaults(APawn* PlayerPawn)
{
//	Super::SetPlayerDefaults(PlayerPawn);

	SpawnDefaultInventory(PlayerPawn);
}


void AAversionInterceptorGameMode::SpawnDefaultInventory(APawn* PlayerPawn)
{
	AAversionInterceptorCharacter* MyPawn = Cast<AAversionInterceptorCharacter>(PlayerPawn);
	if (MyPawn)
	{
		for (int32 i = 0; i < DefaultInventoryClasses.Num(); i++)
		{
			if (DefaultInventoryClasses[i])
			{
				FActorSpawnParameters SpawnInfo;
				SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				AAversionWeaponBase* NewWeapon = GetWorld()->SpawnActor<AAversionWeaponBase>(DefaultInventoryClasses[i], SpawnInfo);

				MyPawn->AddWeapon(NewWeapon);
			}
		}
	}
}

void AAversionInterceptorGameMode::Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType)
{
	AAversionPlayerState* KillerPlayerState = Killer ? Cast<AAversionPlayerState>(Killer->PlayerState) : NULL;
	AAversionPlayerState* VictimPlayerState = KilledPlayer ? Cast<AAversionPlayerState>(KilledPlayer->PlayerState) : NULL;

	if (KillerPlayerState && KillerPlayerState != VictimPlayerState)
	{
	//	KillerPlayerState->ScoreKill(VictimPlayerState, KillScore);
		KillerPlayerState->InformAboutKill(KillerPlayerState, DamageType, VictimPlayerState);
	}

	if (VictimPlayerState)
	{
		//VictimPlayerState->ScoreDeath(KillerPlayerState, DeathScore);
		VictimPlayerState->BroadcastDeath(KillerPlayerState, DamageType, VictimPlayerState);
	}
}

void AAversionInterceptorGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	/* Set timer to run every second */
//	GetWorldTimerManager().SetTimer(TimerHandle_DefaultTimer, this, &AAversionInterceptorGameMode::DefaultTimer, GetWorldSettings()->GetEffectiveTimeDilation(), true);
}

