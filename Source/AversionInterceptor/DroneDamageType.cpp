// Fill out your copyright notice in the Description page of Project Settings.

#include "AversionInterceptor.h"
#include "DroneDamageType.h"


UDroneDamageType::UDroneDamageType(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/* We apply this modifier based on the physics material setup to the head of the enemy PhysAsset */
	HeadDmgModifier = 2.0f;
	LimbDmgModifier = 0.5f;

	bCanDieFrom = true;
}


bool UDroneDamageType::GetCanDieFrom()
{
	return bCanDieFrom;
}


float UDroneDamageType::GetHeadDamageModifier()
{
	return HeadDmgModifier;
}

float UDroneDamageType::GetLimbDamageModifier()
{
	return LimbDmgModifier;
}
