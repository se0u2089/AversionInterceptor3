// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "AversionInterceptor.h"
#include "AIController.h"
#include "InterceptorCharacter.h"
#include "DroneWayPoint.h"
#include "DroneBot.h"
#include "DroneAIController.generated.h"

/**
 * 
 */
UCLASS()
class AVERSIONINTERCEPTOR_API ADroneAIController : public AAIController
{
	GENERATED_BODY()
	
		ADroneAIController(const class FObjectInitializer& ObjectInitializer);

	/* Called whenever the controller possesses a character bot */
	virtual void Possess(class APawn* InPawn) override;

	virtual void UnPossess() override;

	UBehaviorTreeComponent* BehaviorComp;

	UBlackboardComponent* BlackboardComp;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
		FName TargetEnemyKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
		FName PatrolLocationKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
		FName CurrentWaypointKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
		FName BotTypeKeyName;

public:

	ADroneWayPoint* GetWaypoint();

	AAversionInterceptorCharacter* GetTargetEnemy();

	void SetWaypoint(ADroneWayPoint* NewWaypoint);

	void SetTargetEnemy(APawn* NewTarget);

	void SetBlackboardBotType(EBotBehaviorType NewType);

	/** Returns BehaviorComp subobject **/
	FORCEINLINE UBehaviorTreeComponent* GetBehaviorComp() const { return BehaviorComp; }

	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return BlackboardComp; }
	
	
};
