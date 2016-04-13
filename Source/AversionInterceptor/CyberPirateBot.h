// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "AversionInterceptor.h"
#include "AversionInterceptorCharacter.h"
#include "CyberPirateBot.generated.h"

/**
 * 
 */
UCLASS()
class AVERSIONINTERCEPTOR_API ACyberPirateBot : public AAversionInterceptorCharacter
{
	GENERATED_UCLASS_BODY()
		FName GetInventoryAttachPoint(EInventorySlot Slot) const;

		UPROPERTY(EditAnywhere, Category = Behavior)
	class UBehaviorTree* BotBehavior;

	virtual bool IsFirstPerson() const;

	FName GetWeaponAttachPoint() const;


	void SpawnDefaultInventory();

	virtual void FaceRotation(FRotator NewRotation, float DeltaTime = 0.f) override;


	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TArray<TSubclassOf<class AAversionWeaponBase> > DefaultInventoryClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Category=Inventory")
		FName WeaponAttachPoint;

	/* Attachpoint for items carried on the belt/pelvis. */
	UPROPERTY(EditDefaultsOnly, Category = "Category=Inventory")
		FName PelvisAttachPoint;

	void PostInitializeComponents();
	/* Attachpoint for primary weapons */
	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName SpineAttachPoint;


	//bool IsFirstPerson();
	void UpdatePawnMeshes();
};
