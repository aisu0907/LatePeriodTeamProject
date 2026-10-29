// Fill out your copyright notice in the Description page of Project Settings.


#include "SpikeTrap.h"
#include "Components/BoxComponent.h"
#include "AdventurerCharacter.h"

ASpikeTrap::ASpikeTrap()
{
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetBoxExtent(FVector(100.f, 100.f, 50.f));
	SetupTriggerVolume(TriggerBox);	//親のBeginPlayでOverla@が登録される

	ActivationDelay = 0.5f;//踏んでから0.5秒後にとげが出る
	CooldownTime = 3.f;
}

void ASpikeTrap::ActivateTrap_Implementation(AActor* TriggeringActor)
{
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
	OnSpikesLowered();
}

