// Fill out your copyright notice in the Description page of Project Settings.


#include "LandmineTrap.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AdventurerCharacter.h"

// Sets default values
ALandmineTrap::ALandmineTrap()
{
	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	TriggerSphere->SetSphereRadius(60.f);
	SetupTriggerVolume(TriggerSphere);	//親のBeginPlayでOverlapが登録される

	ActivationDelay = 0.4f;	//踏んでから0.4秒後に爆発
	CooldownTime = 1.f;
	bOneShot = true;		//一度爆発したら、もう発動しない
}


void ALandmineTrap::ActivateTrap_Implementation(AActor* TriggeringActor)
{
	//演出(BP側)と、地雷本体を消す
	OnExploded();
	TrapMesh->SetVisibility(false);

	//爆発範囲内のプレイヤーを初期位置へ戻す
	TArray<AActor*> Players;
	UGameplayStatics::GetAllActorsOfClass(this, AAdventurerCharacter::StaticClass(), Players);

	for (AActor* Actor : Players)
	{
		AAdventurerCharacter* Player = Cast<AAdventurerCharacter>(Actor);
		if (!Player)
			continue;

		//範囲外なら無視
		if (FVector::Dist(Player->GetActorLocation(), GetActorLocation()) > ExplosionRadius)
			continue;

		Player->RespawnAtStart();
	}
}