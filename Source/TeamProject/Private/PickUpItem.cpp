// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

#include "AdventurerCharacter.h"

// Sets default values
APickUpItem::APickUpItem()
{
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetSphereRadius(60.f);
	Sphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RootComponent = Sphere;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);


 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APickUpItem::BeginPlay()
{
	Super::BeginPlay();
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &APickUpItem::OnOverlapBegin);
}

void APickUpItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	AAdventurerCharacter* Ch = Cast<AAdventurerCharacter>(OtherActor);
	if (Ch && !Ch->HeldItem)
	{
		Ch->HeldItem = this;
		OnPickedUp(Ch);
	}
}


void APickUpItem::OnPickedUp(ACharacter* NewOwner)
{
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttachToComponent(NewOwner->GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		TEXT("hand_r"));
}

void APickUpItem::Use()
{
	if (PickupEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), PickupEffect, GetActorLocation() + EffectOffset,
			GetActorRotation() + EffectRotation, EffectScale);
	}
	Destroy();
}

