// Fill out your copyright notice in the Description page of Project Settings.

#include "AversionInterceptor.h"
#include "DroneAIController.h"
#include "DroneBot.h"


/* AI Specific includes */
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

ADroneAIController::ADroneAIController(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BehaviorComp = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));
	BlackboardComp = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComp"));

	/* Match with the AI/ZombieBlackboard */
	PatrolLocationKeyName = "PatrolLocation";
	CurrentWaypointKeyName = "CurrentWaypoint";
	BotTypeKeyName = "BotType";
	TargetEnemyKeyName = "TargetEnemy";

	/* Initializes PlayerState so we can assign a team index to AI */
	bWantsPlayerState = true;
}
	


void ADroneAIController::Possess(class APawn* InPawn)
{
	Super::Possess(InPawn);

	ADroneBot* ZombieBot = Cast<ADroneBot>(InPawn);
	if (ZombieBot)
	{
		if (ZombieBot->BehaviorTree->BlackboardAsset)
		{
			BlackboardComp->InitializeBlackboard(*ZombieBot->BehaviorTree->BlackboardAsset);

			/* Make sure the Blackboard has the type of bot we possessed */
			SetBlackboardBotType(ZombieBot->BotType);
		}

		BehaviorComp->StartTree(*ZombieBot->BehaviorTree);
	}
}


void ADroneAIController::UnPossess()
{
	Super::UnPossess();

	/* Stop any behavior running as we no longer have a pawn to control */
	BehaviorComp->StopTree();
}


void ADroneAIController::SetWaypoint(ADroneWayPoint* NewWaypoint)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(CurrentWaypointKeyName, NewWaypoint);
	}
}


void ADroneAIController::SetTargetEnemy(APawn* NewTarget)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(TargetEnemyKeyName, NewTarget);
	}
}


ADroneWayPoint* ADroneAIController::GetWaypoint()
{
	if (BlackboardComp)
	{
		return Cast<ADroneWayPoint>(BlackboardComp->GetValueAsObject(CurrentWaypointKeyName));
	}

	return nullptr;
}


AAversionInterceptorCharacter* ADroneAIController::GetTargetEnemy()
{
	if (BlackboardComp)
	{
		return Cast<AAversionInterceptorCharacter>(BlackboardComp->GetValueAsObject(TargetEnemyKeyName));
	}

	return nullptr;
}


void ADroneAIController::SetBlackboardBotType(EBotBehaviorType NewType)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsEnum(BotTypeKeyName, (uint8)NewType);
	}
}


