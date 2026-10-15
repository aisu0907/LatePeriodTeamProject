// Fill out your copyright notice in the Description page of Project Settings.


#include "TrapBase.h"

// Sets default values
ATrapBase::ATrapBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	//ÉÅÉbÉVÉÖçÏê¨

	TrapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrapMesh"));
	TrapMesh->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void ATrapBase::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerVolume)
	{

	}
}

//è’ìÀîªíËs
void ATrapBase::OnTriggerBeginOverlap(
	UPrimitiveComponent* OverlappedComp, 
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult)
{
	if (CanTrigger(OtherActor))
	{
		TriggerTrap(OtherActor);
	}
}

bool ATrapBase::CanTrigger(AActor* TriggeringActor) const
{
	return TrapState == ETrapState::Idle && Cast<APawn>(TriggeringActor) != nullptr;
}