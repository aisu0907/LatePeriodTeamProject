// Fill out your copyright notice in the Description page of Project Settings.


#include "TrapPlacementZone.h"
#include "Components/BoxComponent.h"

// Sets default values
ATrapPlacementZone::ATrapPlacementZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ZoneBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneBox"));
	SetRootComponent(ZoneBox);

}

// Called when the game starts or when spawned
void ATrapPlacementZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATrapPlacementZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ATrapPlacementZone::TrySnap(const FVector& WorldPoint, FVector& OutLocation, FIntPoint& OutCell) const
{

}

bool ATrapPlacementZone::IsCellFree(FIntPoint Cell) const
{

}

void ATrapPlacementZone::RegisterTrap(FIntPoint Cell, AActor* Trap)
{

}


void ATrapPlacementZone::ReleaseCell(FIntPoint Cell)
{

}
