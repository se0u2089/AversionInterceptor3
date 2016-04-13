// Fill out your copyright notice in the Description page of Project Settings.

#include "AversionInterceptor.h"
#include "MyPlayerController.h"
#include "APlayerCameraManager.h"





AMyPlayerController::AMyPlayerController(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	PlayerCameraManagerClass = AAPlayerCameraManager::StaticClass();

}