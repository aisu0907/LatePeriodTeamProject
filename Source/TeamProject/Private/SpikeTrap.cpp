// Fill out your copyright notice in the Description page of Project Settings.


#include "SpikeTrap.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

ASpikeTrap::ASpikeTrap()
{
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetBoxExtent(FVector(100.f, 100.f, 50.f));
	SetupTriggerVolume(TriggerBox);	//親のBeginPlayでOverla@が登録される

	ActivationDelay = 0.5f;//踏んでから9.5秒後にとげが出る
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
		UGameplayStatics::ApplyDamage(Actor, Damage, nullptr, this, nullptr);
	}
}

void ASpikeTrap::ResetTrap_Implementation()
{
	OnSpikesLowered();
}

