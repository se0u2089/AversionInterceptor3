
#pragma once
#include "GameFramework/GameMode.h"
#include "AversionInterceptorGameMode.generated.h"

UCLASS(minimalapi)
class AAversionInterceptorGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:

	AAversionInterceptorGameMode(const FObjectInitializer & ObjectInitializer);

protected:


	virtual void SpawnDefaultInventory(APawn* PlayerPawn);

	virtual void SetPlayerDefaults(APawn* PlayerPawn) override;

public:

	/* The default weapons to spawn with */
	UPROPERTY(EditDefaultsOnly, Category = "Player")
		TArray<TSubclassOf<class AAversionWeaponBase>> DefaultInventoryClasses;

};


