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

#include "Components/PostProcessComponent.h"

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

		//効果リストを全部適用
		float MaxDuration = 0.1f;
		for (const FItemEffectEntry& E : Effects)
		{
			ApplyEntry(Ch, E);
			MaxDuration = FMath::Max(MaxDuration, E.Duration);
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

		//バフ終了時の処理
		FTimerHandle Handle;
		Ch->GetWorldTimerManager().SetTimer(Handle,
			FTimerDelegate::CreateWeakLambda(Ch, [Ch, BuffNC, BuffPSC]()
				{
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
			MaxDuration, false);

		Ch->HeldItem = nullptr;
	}
	Destroy();
}

	//効果1件分の適応
	void APickUpItem::ApplyEntry(AAdventurerCharacter * Target, const FItemEffectEntry & E)
	{
		UCharacterMovementComponent* CM = Target->GetCharacterMovement();
		if (!CM)return;

		const float Mult = FMath::Max(E.Multiplier, 0.01f);
		TWeakObjectPtr<UPostProcessComponent>VisionPP;

		switch(E.Type)
		{
		case EItemEffectType::MoveSpeed:CM->MaxWalkSpeed *= Mult; break;
		case EItemEffectType::JumpPower:CM->JumpZVelocity *= Mult; break;
		case EItemEffectType::Vision:
		{
			UPostProcessComponent* PP = NewObject<UPostProcessComponent>(Target);
			PP->bUnbound = true;
			PP->BlendWeight = 1.f;
			PP->Priority = 10.f;

			const float I = FMath::Clamp(E.Intensity, 0.f, 1.f);
			FPostProcessSettings& S = PP->Settings;

			S.bOverride_VignetteIntensity = true;
			S.VignetteIntensity = FMath::Lerp(0.4f, 2.0f, I);//周辺を暗く

			S.bOverride_ColorSaturation = true;
			S.ColorSaturation = FVector4(1.f, 1.f, 1.f, 1.f) * FMath::Lerp(1.f, 0.3f, I);

			S.bOverride_SceneFringeIntensity = true;
			S.SceneFringeIntensity = FMath::Lerp(0.f, 4.f, I);//色のにじみ

			PP->RegisterComponent();
			VisionPP = PP;
			break;
		}
		}
		//この効果の間だけ出し続けるエフェクト
		TWeakObjectPtr<UNiagaraComponent> LoopNC;
		if (E.LoopEffect)
		{
			LoopNC = UNiagaraFunctionLibrary::SpawnSystemAttached(
				E.LoopEffect, Target->GetRootComponent(), NAME_None,
				EffectOffset, EffectRotation, EffectScale,
				EAttachLocation::KeepRelativeOffset, true, ENCPoolMethod::None);
		}

		//終了時に割り戻す(アイテム本体は消されるためキャラのタイマーを使う)
		FTimerHandle Handle;
	}


