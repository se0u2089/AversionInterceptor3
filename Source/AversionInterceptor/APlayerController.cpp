// Fill out your copyright notice in the Description page of Project Settings.

#include "AversionInterceptor.h"
#include "APlayerController.h"
#include "APlayerCameraManager.h"


AAPlayerController::AAPlayerController(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/* Assign the class types we wish to use */
	PlayerCameraManagerClass = AAPlayerCameraManager::StaticClass();

	
}
