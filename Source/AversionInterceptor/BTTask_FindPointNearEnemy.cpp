// Fill out your copyright notice in the Description page of Project Settings.

#include "AversionInterceptor.h"
#include "AversionInterceptorCharacter.h"
#include "BTTask_FindPointNearEnemy.h"
#include "CyberBotAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"



UBTTask_FindPointNearEnemy::UBTTask_FindPointNearEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


EBTNodeResult::Type UBTTask_FindPointNearEnemy::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ACyberBotAIController* MyController = Cast<ACyberBotAIController>(OwnerComp.GetAIOwner());
	if (MyController == NULL)
	{
		return EBTNodeResult::Failed;
	}

	APawn* MyBot = MyController->GetPawn();
	AAversionInterceptorCharacter* Enemy = MyController->GetEnemy();
	if (Enemy && MyBot)
	{
		const float SearchRadius = 200.0f;
		const FVector SearchOrigin = Enemy->GetActorLocation() + 600.0f * (MyBot->GetActorLocation() - Enemy->GetActorLocation()).GetSafeNormal();
		const FVector Loc = UNavigationSystem::GetRandomReachablePointInRadius(MyController, SearchOrigin, SearchRadius);
		if (Loc != FVector::ZeroVector)
		{
			
			OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID(), Loc);
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
