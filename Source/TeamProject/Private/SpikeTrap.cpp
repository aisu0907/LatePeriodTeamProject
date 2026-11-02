// Fill out your copyright notice in the Description page of Project Settings.


#include "SpikeTrap.h"
#include "Components/BoxComponent.h"
#include "AdventurerCharacter.h"

#include "Components/StaticMeshComponent.h"

ASpikeTrap::ASpikeTrap()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetBoxExtent(FVector(100.f, 100.f, 50.f));
	SetupTriggerVolume(TriggerBox);	//親のBeginPlayでOverla@が登録される

	ActivationDelay = 0.5f;//踏んでから0.5秒後にとげが出る
	CooldownTime = 3.f;

	SpikesMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpikesMeshMesh"));
	SpikesMesh->SetupAttachment(RootComponent);
	SpikesMesh->SetMobility(EComponentMobility::Movable);
	SpikesMesh->SetRelativeLocation(LoweredLocation);
	TargetLocation = LoweredLocation;
}

void ASpikeTrap::BeginPlay()
{
	Super::BeginPlay();

	//エディターで変えたlowedLocatoiomを反映して、隠れたオジュタイから始める
	TargetLocation = LoweredLocation;
	SpikesMesh->SetRelativeLocation(LoweredLocation);
}

void ASpikeTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FVector Current = SpikesMesh->GetRelativeLocation();
	const FVector Next = FMath::VInterpConstantTo(Current, TargetLocation, DeltaTime, MoveSpeed);
	SpikesMesh->SetRelativeLocation(Next);

	//目標についたらTICKを止める
	if (Next.Equals(TargetLocation, 0.1f))
	{
		SpikesMesh->SetRelativeLocation(TargetLocation);
		SetActorTickEnabled(false);
	}
}
void ASpikeTrap::MoveSpikesTo(const FVector& NewTarget)
{
	TargetLocation = NewTarget;
	SetActorTickEnabled(true);
}


void ASpikeTrap::ActivateTrap_Implementation(AActor* TriggeringActor)
{
	MoveSpikesTo(ReisedLocation);
	OnSpikesRaised();

	//予備動作宙に逃げた場合は当たらないように、発動時点で範囲内にいるPawnにダメージ
	TArray<AActor*>Overlapping;
	TriggerBox->GetOverlappingActors(Overlapping, APawn::StaticClass());
	for (AActor* Actor:Overlapping)
	{
		if (AAdventurerCharacter* Player=Cast<AAdventurerCharacter>(Actor))
		{
			Player->RespawnAtStart();
		}
	}
}

void ASpikeTrap::ResetTrap_Implementation()
{
	MoveSpikesTo(LoweredLocation);
	OnSpikesLowered();
}

