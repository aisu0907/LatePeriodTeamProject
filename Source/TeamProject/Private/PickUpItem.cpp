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

		// 使った瞬間の単発エフェクト
		if (UseEffect)
		{
			UNiagaraComponent* UseNC = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(), UseEffect,
				Ch->GetActorLocation() + EffectOffset,
				Ch->GetActorRotation() + EffectRotation,
				EffectScale);
			if (UseNC)
			{
				// 念のため2秒後に消す
				FTimerHandle UseHandle;
				Ch->GetWorldTimerManager().SetTimer(UseHandle,
					FTimerDelegate::CreateWeakLambda(UseNC, [UseNC]()
						{
							UseNC->DestroyComponent();
						}),
					2.f, false);
			}
		}

		// バフ中、キャラにくっつけてエフェクトを出し続ける
		TWeakObjectPtr<UNiagaraComponent> EffectComp;
		if (PickupEffect)
		{
			EffectComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
				PickupEffect, Ch->GetRootComponent(), NAME_None,
				EffectOffset, EffectRotation, EffectScale,
				EAttachLocation::KeepRelativeOffset, true, ENCPoolMethod::None);
		}

		// バフ終了時の処理
		FTimerHandle Handle;
		Ch->GetWorldTimerManager().SetTimer(Handle,
			FTimerDelegate::CreateWeakLambda(Ch, [Ch, BaseSpeed, EffectComp]()
				{
					Ch->GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
					Ch->bBuffActive = false;
					if (UNiagaraComponent* NC = EffectComp.Get())
					{
						NC->DestroyComponent();   // エフェクトを消す
					}
				}),
			BuffDuration, false);
	}
	Destroy();
}

