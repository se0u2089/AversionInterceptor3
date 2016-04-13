// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#ifndef __AVERSIONINTERCEPTOR_H__
#define __AVERSIONINTERCEPTOR_H__


#include "Engine.h"


// This is NOT included by default in an empty project! It's required for replication and setting of the GetLifetimeReplicatedProps
#include "Net/UnrealNetwork.h"
#define SURFACE_DEFAULT				SurfaceType_Default
#define SURFACE_FLESH				SurfaceType1
#define SURFACE_ZOMBIEBODY			SurfaceType2
#define SURFACE_ZOMBIEHEAD			SurfaceType3
#define SURFACE_ZOMBIELIMB			SurfaceType4
#define COLLISION_PROJECTILE	ECC_GameTraceChannel2
#define COLLISION_WEAPON				ECC_GameTraceChannel1
#endif