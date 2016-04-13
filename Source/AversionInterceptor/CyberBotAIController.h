// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AversionInterceptor.h"
#include "AIController.h"
#include "CyberBotAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;

UCLASS(config=Game)
class AVERSIONINTERCEPTOR_API ACyberBotAIController : public AAIController
{
	GENERATED_UCLASS_BODY()

private:
	UPROPERTY(transient)
		UBlackboardComponent* BlackboardComp;

	/* Cached BT component */
	UPROPERTY(transient)
		UBehaviorTreeComponent* BehaviorComp;
public:

	// Begin AController interface
	virtual void GameHasEnded(class AActor* EndGameFocus = NULL, bool bIsWinner = false) override;
	virtual void Possess(class APawn* InPawn) override;
	virtual void BeginInactiveState() override;
	// End APlayerController interface

	void Respawn();



	void SetEnemy(class APawn* InPawn);

	class AAversionInterceptorCharacter* GetEnemy() const;

	/* If there is line of sight to current enemy, start firing at them */
	UFUNCTION(BlueprintCallable, Category = Behavior)
		void ShootEnemy();

	void CheckAmmo(const AAversionWeaponBase * CurrentWeapon);

	/* Finds the closest enemy and sets them as current target */
	UFUNCTION(BlueprintCallable, Category = Behavior)
		void FindClosestEnemy();

	UFUNCTION(BlueprintCallable, Category = Behavior)
		bool FindClosestEnemyWithLOS(AAversionInterceptorCharacter* ExcludeEnemy);

	bool HasWeaponLOSToEnemy(AActor* InEnemyActor, const bool bAnyEnemy) const;

	// Begin AAIController interface
	/** Update direction AI is looking based on FocalPoint */
	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn = true) override;
	// End AAIController interface

protected:
	// Check of we have LOS to a character
	bool LOSTrace(AAversionInterceptorCharacter* InEnemyChar) const;

	int32 EnemyKeyID;
	int32 NeedAmmoKeyID;
	int32 DestinationKeyID;
	/** Handle for efficient management of Respawn timer */
	FTimerHandle TimerHandle_Respawn;

public:
	/** Returns BlackboardComp subobject **/
	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return BlackboardComp; }
	/** Returns BehaviorComp subobject **/
	FORCEINLINE UBehaviorTreeComponent* GetBehaviorComp() const { return BehaviorComp; }
	
	
};
