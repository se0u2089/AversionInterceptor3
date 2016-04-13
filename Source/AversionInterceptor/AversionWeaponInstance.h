// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AversionInterceptor.h"
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

	
	/************************************************************************/
	/* Damage Processing                                                    */
	/************************************************************************/

	AAversionWeaponInstance(const FObjectInitializer & PCIP);

	virtual void FireWeapon() override;

	void DealDamage(const FHitResult& Impact, const FVector& ShootDir);

	bool ShouldDealDamage(AActor* TestActor) const;

	void ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir);

	void ProcessInstantHitConfirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir);

	UFUNCTION(Reliable, Server, WithValidation)
		void ServerNotifyHit(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir);

	void ServerNotifyHit_Implementation(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir);

	bool ServerNotifyHit_Validate(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir);

	UFUNCTION(Reliable, Server, WithValidation)
		void ServerNotifyMiss(FVector_NetQuantizeNormal ShootDir);

	void ServerNotifyMiss_Implementation(FVector_NetQuantizeNormal ShootDir);

	bool ServerNotifyMiss_Validate(FVector_NetQuantizeNormal ShootDir);

	UPROPERTY(Transient, ReplicatedUsing = OnRep_HitLocation)
		FVector HitImpactNotify;

	UFUNCTION()
		void OnRep_HitLocation();

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
	

private:

	/* Material instance to manipulate emissive on the flashlight instance  */
	UMaterialInstanceDynamic* MatDynamic;

	UPROPERTY(EditDefaultsOnly, Category = "Material")
		FName EmissiveParamName;

	UPROPERTY(EditDefaultsOnly, Category = "Material")
		float MaxEmissiveIntensity;

	/* Last frame's emissive intensity for performance purposes */
	float LastEmissiveStrength;

public:

	UPROPERTY(EditDefaultsOnly)
		FName LightAttachPoint;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_IsActive)
		bool bIsActive;

	UPROPERTY(VisibleDefaultsOnly)
		UStaticMeshComponent* LightConeComp;

	UPROPERTY(VisibleDefaultsOnly)
		USpotLightComponent* SpotLightComp;

	//virtual void HandleFiring() override;
	virtual void BeginPlay() override;
	virtual void OnEquipFinished() override;

	virtual void OnUnEquip() override;

	virtual void OnEnterInventory(AAversionInterceptorCharacter* NewOwner);

	void UpdateLight(bool Enabled);

	UFUNCTION()
		void OnRep_IsActive();
};
