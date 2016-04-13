
#pragma once

#include "AversionInterceptor.h"
#include "GameFramework/GameMode.h"
#include "AversionInterceptorGameMode.generated.h"

class ACyberBotAIController;
class AAversionPlayerState;


UCLASS()
class AAversionInterceptorGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	/* Handle for efficient management of DefaultTimer timer */
	FTimerHandle TimerHandle_DefaultTimer;

		AAversionInterceptorGameMode(const FObjectInitializer& ObjectInitializer);


		virtual void PreInitializeComponents() override;

		virtual void SpawnDefaultInventory(APawn* PlayerPawn);

		/**
		* Make sure pawn properties are back to default
		* Also a good place to modify them on spawn
		*/
		virtual void SetPlayerDefaults(APawn* PlayerPawn) override;

public:
	/** can players damage each other? */
	virtual bool CanDealDamage(AAversionPlayerState* DamageInstigator, AAversionPlayerState* DamagedPlayer) const;

	virtual void Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType);

	/* The enemy pawn class */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
		TSubclassOf<class APawn> BotPawnClass;
	/* The default weapons to spawn with */
	UPROPERTY(EditDefaultsOnly, Category = "Player")
		TArray<TSubclassOf<class AAversionWeaponBase>> DefaultInventoryClasses;


	







};


