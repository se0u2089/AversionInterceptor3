// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "AversionInterceptor.h"
#include "AversionInterceptorGameMode.h"
#include "AversionInterceptorCharacter.h"
#include "MyPlayerController.h"
#include "APlayerCameraManager.h"	
#include "InterceptorCharacter.h"
#include "AversionWeaponBase.h"

AAversionInterceptorGameMode::AAversionInterceptorGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter2"));
if (PlayerPawnBPClass.Class != NULL)
{

	DefaultPawnClass = PlayerPawnBPClass.Class;

	}	
		PlayerControllerClass = AMyPlayerController::StaticClass();
	//	for (FConstControllerIterator Iterator = GetWorld()->GetControllerIterator(); Iterator; ++Iterator)
	//	{
	//		//SetPlayerDefaults;
	//		AController* Controller = *Iterator;
	//		APlayerController* PlayerController = Cast<APlayerController>(Controller);
	//		if ((PlayerController->GetPawn() == NULL) && PlayerCanRestart(PlayerController))
	//		{
	//			RestartPlayer(PlayerController);
	//		}
	//	}
}	



void AAversionInterceptorGameMode::SpawnDefaultInventory(APawn* PlayerPawn)
{
	AInterceptorCharacter* MyPawn = Cast<AInterceptorCharacter>(PlayerPawn);
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

void AAversionInterceptorGameMode::SetPlayerDefaults(APawn* PlayerPawn)
{
	Super::SetPlayerDefaults(PlayerPawn);

	SpawnDefaultInventory(PlayerPawn);
}
