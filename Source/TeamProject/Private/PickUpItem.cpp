// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"

#include "AdventurerCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

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
	if (AAdventurerCharacter* Ch = Cast<AAdventurerCharacter>(GetAttachParentActor()))
	{
		// バフ開始
		UCharacterMovementComponent* CM = Ch->GetCharacterMovement();
		const float BaseSpeed = CM->MaxWalkSpeed;
		CM->MaxWalkSpeed = BaseSpeed * SpeedMultiplier;
		Ch->bBuffActive = true;

		const FVector Loc = Ch->GetActorLocation() + EffectOffset;
		const FRotator Rot = Ch->GetActorRotation() + EffectRotation;

		// 使った瞬間の単発エフェクト(Niagara)
		if (UseEffect)
		{
			UNiagaraComponent* UseNC = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(), UseEffect, Loc, Rot, EffectScale);
			if (UseNC)
			{
				FTimerHandle UseHandle;
				Ch->GetWorldTimerManager().SetTimer(UseHandle,
					FTimerDelegate::CreateWeakLambda(UseNC, [UseNC]()
						{
							UseNC->DestroyComponent();
						}),
					2.f, false);
			}
		}

		// 使った瞬間の単発エフェクト(Cascade)
		if (UseParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(), UseParticle, Loc, Rot, EffectScale, true);
		}

		// バフ中に出し続けるエフェクト(Niagara)
		TWeakObjectPtr<UNiagaraComponent> BuffNC;
		if (PickupEffect)
		{
			BuffNC = UNiagaraFunctionLibrary::SpawnSystemAttached(
				PickupEffect, Ch->GetRootComponent(), NAME_None,
				EffectOffset, EffectRotation, EffectScale,
				EAttachLocation::KeepRelativeOffset, true, ENCPoolMethod::None);
		}

		// バフ中に出し続けるエフェクト(Cascade)
		TWeakObjectPtr<UParticleSystemComponent> BuffPSC;
		if (BuffParticle)
		{
			BuffPSC = UGameplayStatics::SpawnEmitterAttached(
				BuffParticle, Ch->GetRootComponent(), NAME_None,
				EffectOffset, EffectRotation, EffectScale,
				EAttachLocation::KeepRelativeOffset, false);
		}

		// バフ終了時の処理
		FTimerHandle Handle;
		Ch->GetWorldTimerManager().SetTimer(Handle,
			FTimerDelegate::CreateWeakLambda(Ch, [Ch, BaseSpeed, BuffNC, BuffPSC]()
				{
					Ch->GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
					Ch->bBuffActive = false;
					if (UNiagaraComponent* NC = BuffNC.Get())
					{
						NC->DestroyComponent();
					}
					if (UParticleSystemComponent* PSC = BuffPSC.Get())
					{
						PSC->DestroyComponent();
					}
				}),
			FMath::Max(BuffDuration, 0.1f), false);
	}
	Destroy();
}

