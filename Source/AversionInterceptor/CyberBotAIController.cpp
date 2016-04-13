// Fill out your copyright notice in the Description page of Project Settings.

#include "AversionInterceptor.h"
#include "AversionWeaponBase.h"
#include "AversionInterceptorCharacter.h"
#include "CyberBotAIController.h"
#include "CyberPirateBot.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"



ACyberBotAIController::ACyberBotAIController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	BlackboardComp = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackBoardComp"));

	BrainComponent = BehaviorComp = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));

	bWantsPlayerState = true;
}



void ACyberBotAIController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);

	ACyberPirateBot* Bot = Cast<ACyberPirateBot>(InPawn);

	// start behavior
	if (Bot && Bot->BotBehavior)
	{
		if (Bot->BotBehavior->BlackboardAsset)
		{
			BlackboardComp->InitializeBlackboard(*Bot->BotBehavior->BlackboardAsset);
		}

		EnemyKeyID = BlackboardComp->GetKeyID("Enemy");
		NeedAmmoKeyID = BlackboardComp->GetKeyID("NeedAmmo");
	
		BehaviorComp->StartTree(*(Bot->BotBehavior));
	}
}

void ACyberBotAIController::BeginInactiveState()
{
	Super::BeginInactiveState();

	//AGameState* GameState = GetWorld()->GameState;

	//const float MinRespawnDelay = (GameState && GameState->GameModeClass) ? GetDefault<AGameMode>(GameState->GameModeClass)->MinRespawnDelay : 1.0f;

	//GetWorldTimerManager().SetTimer(TimerHandle_Respawn, this, &ACyberBotAIController::Respawn, MinRespawnDelay);
}

void ACyberBotAIController::Respawn()
{
	GetWorld()->GetAuthGameMode()->RestartPlayer(this);
}

void ACyberBotAIController::FindClosestEnemy()
{
	APawn* MyBot = GetPawn();
	if (MyBot == NULL)
	{
		return;
	}

	const FVector MyLoc = MyBot->GetActorLocation();
	float BestDistSq = MAX_FLT;
	AAversionInterceptorCharacter* BestPawn = NULL;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		AAversionInterceptorCharacter* TestPawn = Cast<AAversionInterceptorCharacter>(*It);
		if (TestPawn && TestPawn->IsAlive() && TestPawn->IsEnemyFor(this))
		{
			const float DistSq = (TestPawn->GetActorLocation() - MyLoc).SizeSquared();
			if (DistSq < BestDistSq)
			{
				BestDistSq = DistSq;
				BestPawn = TestPawn;
			}
		}
	}

	if (BestPawn)
	{
		SetEnemy(BestPawn);
	}
}

bool ACyberBotAIController::FindClosestEnemyWithLOS(AAversionInterceptorCharacter* ExcludeEnemy)
{
	bool bGotEnemy = false;
	APawn* MyBot = GetPawn();
	if (MyBot != NULL)
	{
		const FVector MyLoc = MyBot->GetActorLocation();
		float BestDistSq = MAX_FLT;
		AAversionInterceptorCharacter* BestPawn = NULL;

		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
		{
			AAversionInterceptorCharacter* TestPawn = Cast<AAversionInterceptorCharacter>(*It);
			if (TestPawn && TestPawn != ExcludeEnemy && TestPawn->IsAlive() && TestPawn->IsEnemyFor(this))
			{
				if (HasWeaponLOSToEnemy(TestPawn, true) == true)
				{
					const float DistSq = (TestPawn->GetActorLocation() - MyLoc).SizeSquared();
					if (DistSq < BestDistSq)
					{
						BestDistSq = DistSq;
						BestPawn = TestPawn;
					}
				}
			}
		}
		if (BestPawn)
		{
			SetEnemy(BestPawn);
			bGotEnemy = true;
		}
	}
	return bGotEnemy;
}

bool ACyberBotAIController::HasWeaponLOSToEnemy(AActor* InEnemyActor, const bool bAnyEnemy) const
{
	static FName LosTag = FName(TEXT("AIWeaponLosTrace"));

	ACyberPirateBot* MyBot = Cast<ACyberPirateBot>(GetPawn());

	bool bHasLOS = false;
	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(LosTag, true, GetPawn());
	TraceParams.bTraceAsyncScene = true;

	TraceParams.bReturnPhysicalMaterial = true;
	FVector StartLocation = MyBot->GetActorLocation();
	StartLocation.Z += GetPawn()->BaseEyeHeight; //look from eyes

	FHitResult Hit(ForceInit);
	const FVector EndLocation = InEnemyActor->GetActorLocation();
	GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, COLLISION_WEAPON, TraceParams);
	if (Hit.bBlockingHit == true)
	{
		// Theres a blocking hit - check if its our enemy actor
		AActor* HitActor = Hit.GetActor();
		if (Hit.GetActor() != NULL)
		{
			if (HitActor == InEnemyActor)
			{
				bHasLOS = true;
			}
			else if (bAnyEnemy == true)
			{
				// Its not our actor, maybe its still an enemy ?
				ACharacter* HitChar = Cast<ACharacter>(HitActor);
				if (HitChar != NULL)
				{
				//	AShooterPlayerState* HitPlayerState = Cast<AShooterPlayerState>(HitChar->PlayerState);
				//	AShooterPlayerState* MyPlayerState = Cast<AShooterPlayerState>(PlayerState);
				//	if ((HitPlayerState != NULL) && (MyPlayerState != NULL))
				//	{
				//		if (HitPlayerState->GetTeamNum() != MyPlayerState->GetTeamNum())
				//		{
				//			bHasLOS = true;
				//		}
				//	}
				}
			}
		}
	}



	return bHasLOS;
}

void ACyberBotAIController::ShootEnemy()
{
	ACyberPirateBot* MyBot = Cast<ACyberPirateBot>(GetPawn());
	AAversionWeaponBase* MyWeapon = MyBot ? MyBot->GetWeapon() : NULL;
	if (MyWeapon == NULL)
	{
		return;
	}

	bool bCanShoot = false;
	AAversionInterceptorCharacter* Enemy = GetEnemy();
	if (Enemy && (Enemy->IsAlive()) && (MyWeapon->GetCurrentAmmo() > 0) && (MyWeapon->CanFire() == true))
	{
		if (LineOfSightTo(Enemy, MyBot->GetActorLocation()))
		{
			bCanShoot = true;
		}
	}

	if (bCanShoot)
	{
		MyBot->StartWeaponFire();
	}
	else
	{
		MyBot->StopWeaponFire();
	}
}

void ACyberBotAIController::CheckAmmo(const class AAversionWeaponBase* CurrentWeapon)
{
	if (CurrentWeapon && BlackboardComp)
	{
		const int32 Ammo = CurrentWeapon->GetCurrentAmmo();
		const int32 MaxAmmo = CurrentWeapon->GetMaxAmmo();
		const float Ratio = (float)Ammo / (float)MaxAmmo;

	BlackboardComp->SetValue<UBlackboardKeyType_Bool>(NeedAmmoKeyID, (Ratio <= 0.1f));
	}
}

void ACyberBotAIController::SetEnemy(class APawn* InPawn)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValue<UBlackboardKeyType_Object>(EnemyKeyID, InPawn);
		SetFocus(InPawn);
	}
}

class AAversionInterceptorCharacter* ACyberBotAIController::GetEnemy() const
{
	if (BlackboardComp)
	{
	return Cast<AAversionInterceptorCharacter>(BlackboardComp->GetValue<UBlackboardKeyType_Object>(EnemyKeyID));
	}

	return NULL;
}


void ACyberBotAIController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
	// Look toward focus
	FVector FocalPoint = GetFocalPoint();
	if (!FocalPoint.IsZero() && GetPawn())
	{
		FVector Direction = FocalPoint - GetPawn()->GetActorLocation();
		FRotator NewControlRotation = Direction.Rotation();

		NewControlRotation.Yaw = FRotator::ClampAxis(NewControlRotation.Yaw);

		SetControlRotation(NewControlRotation);

		APawn* const P = GetPawn();
		if (P && bUpdatePawn)
		{
			P->FaceRotation(NewControlRotation, DeltaTime);
		}

	}
}

void ACyberBotAIController::GameHasEnded(AActor* EndGameFocus, bool bIsWinner)
{
	// Stop the behaviour tree/logic
	BehaviorComp->StopTree();

	// Stop any movement we already have
	StopMovement();

	// Cancel the repsawn timer
	GetWorldTimerManager().ClearTimer(TimerHandle_Respawn);

	// Clear any enemy
	SetEnemy(NULL);

	// Finally stop firing
	ACyberPirateBot* MyBot = Cast<ACyberPirateBot>(GetPawn());
	AAversionWeaponBase* MyWeapon = MyBot ? MyBot->GetWeapon() : NULL;
	if (MyWeapon == NULL)
	{
		return;
	}
	MyBot->StopWeaponFire();
}

