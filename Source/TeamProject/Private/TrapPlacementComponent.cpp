// Fill out your copyright notice in the Description page of Project Settings.


#include "TrapPlacementComponent.h"
#include "TrapPlacementZone.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values
UTrapPlacementComponent::UTrapPlacementComponent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryComponentTick.bCanEverTick = true;

}

TSubclassOf<AActor> UTrapPlacementComponent::GetTrapClassFor(const ATrapPlacementZone* Zone) const
{
	return (Zone->SurfaceType == EPlacementSurface::Floor) ? FloorTrapClass : WallTrapClass;
}


