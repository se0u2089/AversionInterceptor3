// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AversionWeaponBase.h"
#include "AversionWeaponInstance.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class AVERSIONINTERCEPTOR_API AAversionWeaponInstance : public AAversionWeaponBase
{
	GENERATED_BODY()
		
	

	private:

		/************************************************************************/
		/* Visual Handlers                                                      */
		/************************************************************************/

		void SimulateInstantHit(const FVector& ImpactPoint);

		void SpawnImpactEffects(const FHitResult& Impact);

		void SpawnTrailEffects(const FVector& EndPoint);

		/* Particle FX played when a surface is hit. */
		UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class ABulletImpactEffect> ImpactTemplate;

		UPROPERTY(EditDefaultsOnly)
			FName TrailTargetParam;

		UPROPERTY(EditDefaultsOnly)
			UParticleSystem* TrailFX;

		UPROPERTY(EditDefaultsOnly)
			UParticleSystem* TracerFX;

		/* Minimum firing distance before spawning tracers or trails. */
		UPROPERTY(EditDefaultsOnly)
			float MinimumProjectileSpawnDistance;

		UPROPERTY(EditDefaultsOnly)
			int32 TracerRoundInterval;

		/* Keeps track of number of shots fired */
		int32 BulletsShotCount;

	protected:

		AAversionWeaponInstance(const FObjectInitializer& ObjectInitializer);

		/************************************************************************/
		/* Damage Processing                                                    */
		/************************************************************************/

		virtual void FireWeapon() override;

		void DealDamage(const FHitResult& Impact, const FVector& ShootDir);

		bool ShouldDealDamage(AActor* TestActor) const;

		void ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir);

		void ProcessInstantHitConfirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir);

		UPROPERTY()
			FVector HitImpactNotify;

		UFUNCTION()
			void OnRep_HitLocation();

		void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

		/************************************************************************/
		/* Weapon Configuration                                                 */
		/************************************************************************/

		UPROPERTY(EditDefaultsOnly)
			float HitDamage;

		UPROPERTY(EditDefaultsOnly)
			TSubclassOf<class UDamageType> DamageType;

		UPROPERTY(EditDefaultsOnly)
			float WeaponRange;

		/* Hit verification: threshold for dot product between view direction and hit direction */
		UPROPERTY(EditDefaultsOnly)
			float AllowedViewDotHitDir;

		/* Hit verification: scale for bounding box of hit actor */
		UPROPERTY(EditDefaultsOnly)
			float ClientSideHitLeeway;
	
	
	
};
