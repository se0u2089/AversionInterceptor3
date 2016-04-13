// Fill out your copyright notice in the Description page of Project Settings.

#include "AversionInterceptor.h"
#include "AversionPlayerState.h"


AAversionPlayerState::AAversionPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	TeamNumber = 0;
	NumKills = 0;
	NumDeaths = 0;
	NumBulletsFired = 0;
	NumRocketsFired = 0;
	bQuitter = false;
}

void AAversionPlayerState::Reset()
{
	Super::Reset();

	//PlayerStates persist across seamless travel.  Keep the same teams as previous match.
	//SetTeamNum(0);
	NumKills = 0;
	NumDeaths = 0;
	NumBulletsFired = 0;
	NumRocketsFired = 0;
	bQuitter = false;
}

void AAversionPlayerState::UnregisterPlayerWithSession()
{
	if (!bFromPreviousLevel)
	{
		Super::UnregisterPlayerWithSession();
	}
}

void AAversionPlayerState::ClientInitialize(AController* InController)
{
	Super::ClientInitialize(InController);

	UpdateTeamColors();
}

void AAversionPlayerState::SetTeamNum(int32 NewTeamNumber)
{
	TeamNumber = NewTeamNumber;

	UpdateTeamColors();
}

void AAversionPlayerState::OnRep_TeamColor()
{
	UpdateTeamColors();
}

void AAversionPlayerState::AddBulletsFired(int32 NumBullets)
{
	NumBulletsFired += NumBullets;
}

void AAversionPlayerState::AddRocketsFired(int32 NumRockets)
{
	NumRocketsFired += NumRockets;
}

void AAversionPlayerState::SetQuitter(bool bInQuitter)
{
	bQuitter = bInQuitter;
}

void AAversionPlayerState::CopyProperties(APlayerState* PlayerState)
{
	//Super::CopyProperties(PlayerState);

	AAversionPlayerState* ShooterPlayer = Cast<AAversionPlayerState>(PlayerState);
	if (ShooterPlayer)
	{
		ShooterPlayer->TeamNumber = TeamNumber;
	}
}

void AAversionPlayerState::UpdateTeamColors()
{
	AController* OwnerController = Cast<AController>(GetOwner());
	if (OwnerController != NULL)
	{
	//	AAversionPlayerState* ShooterCharacter = Cast<AAversionInterceptorCharacter>(OwnerController->GetCharacter());
	//	if (ShooterCharacter != NULL)
	//	{
	//		ShooterCharacter->UpdateTeamColorsAllMIDs();
	//	}
	}
}

int32 AAversionPlayerState::GetTeamNum() const
{
	return TeamNumber;
}

int32 AAversionPlayerState::GetKills() const
{
	return NumKills;
}

int32 AAversionPlayerState::GetDeaths() const
{
	return NumDeaths;
}

float AAversionPlayerState::GetScore() const
{
	return Score;
}

int32 AAversionPlayerState::GetNumBulletsFired() const
{
	return NumBulletsFired;
}

int32 AAversionPlayerState::GetNumRocketsFired() const
{
	return NumRocketsFired;
}

bool AAversionPlayerState::IsQuitter() const
{
	return bQuitter;
}

void AAversionPlayerState::ScoreKill(AAversionPlayerState* Victim, int32 Points)
{
	NumKills++;
	ScorePoints(Points);
}

void AAversionPlayerState::ScoreDeath(AAversionPlayerState* KilledBy, int32 Points)
{
	NumDeaths++;
	ScorePoints(Points);
}

void AAversionPlayerState::ScorePoints(int32 Points)
{
//	AShooterGameState* const MyGameState = Cast<AShooterGameState>(GetWorld()->GameState);
//	if (MyGameState && TeamNumber >= 0)
//	{
//		if (TeamNumber >= MyGameState->TeamScores.Num())
//		{
	//		MyGameState->TeamScores.AddZeroed(TeamNumber - MyGameState->TeamScores.Num() + 1);
	//	}

	//	MyGameState->TeamScores[TeamNumber] += Points;
//	}

	Score += Points;
}

void AAversionPlayerState::InformAboutKill_Implementation(class AAversionPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class AAversionPlayerState* KilledPlayerState)
{
	//id can be null for bots
	if (KillerPlayerState->UniqueId.IsValid())
	{
		//search for the actual killer before calling OnKill()	
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
		//	AMyPlayerController* TestPC = Cast<AMyPlayerController>(*It);
		//	if (TestPC && TestPC->IsLocalController())
		//	{
		//		// a local player might not have an ID if it was created with CreateDebugPlayer.
			//	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(TestPC->Player);
			//	TSharedPtr<const FUniqueNetId> LocalID = LocalPlayer->GetCachedUniqueNetId();
			//	if (LocalID.IsValid() && *LocalPlayer->GetCachedUniqueNetId() == *KillerPlayerState->UniqueId)
			//	{
			//		TestPC->OnKill();
			//	}
		//	}
		}
	}
}

void AAversionPlayerState::BroadcastDeath_Implementation(class AAversionPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class AAversionPlayerState* KilledPlayerState)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		// all local players get death messages so they can update their huds.
	//	AMyPlayerController* TestPC = Cast<AMyPlayerController>(*It);
	//	if (TestPC && TestPC->IsLocalController())
	//	{
	//		TestPC->OnDeathMessage(KillerPlayerState, this, KillerDamageType);
	//	}
	}
}

void AAversionPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAversionPlayerState, TeamNumber);
	DOREPLIFETIME(AAversionPlayerState, NumKills);
	DOREPLIFETIME(AAversionPlayerState, NumDeaths);
}

FString AAversionPlayerState::GetShortPlayerName() const
{

	return PlayerName;
}



