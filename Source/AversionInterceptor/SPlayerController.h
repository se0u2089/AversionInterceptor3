// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AversionInterceptor.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class AVERSIONINTERCEPTOR_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()
		ASPlayerController(const FObjectInitializer& ObjectInitializer);

	
	/* Flag to respawn or start spectating upon death */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
		bool bRespawnImmediately;

	/* Respawn or start spectating after dying */
	virtual void UnFreeze() override;

	

public:

	

	/* Start spectating. Should be called only on server */
	void StartSpectating();
};