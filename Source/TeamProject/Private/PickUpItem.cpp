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
#include"Camera/CameraComponent.h"

#include "TrapBase.h"

namespace
{
	//っ歯科医悪化の重複したときよう
	TMap<TWeakObjectPtr<UCameraComponent>, uint32>GVisionOwner;
	uint32 GVisionNextId = 1;
}

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

		//相手にかける効果のために、自分以外のプレイヤーを先に集める
		TArray<AAdventurerCharacter*>Opponent;
		{
			TArray<AActor*>All;
			UGameplayStatics::GetAllActorsOfClass(this, AAdventurerCharacter::StaticClass(), All);
			for (AActor* A : All)
			{
				AAdventurerCharacter* P = Cast < AAdventurerCharacter> (A);
				if (P && P != Ch && P->GetPlayerIndex() != Ch->GetPlayerIndex())
				{
					Opponent.Add(P);
				}
			}
		}

		//効果リストを全部適用
		float MaxDuration = 0.1f;
		for (const FItemEffectEntry& E : Effects)
		{
			if (E.Target == EItemEffectTarget::Self)
			{
				ApplyEntry(Ch, E);
				MaxDuration = FMath::Max(MaxDuration, E.Duration);
			}
			else
			{
				for (AAdventurerCharacter* Opp : Opponent)
				{
					ApplyEntry(Opp, E);
				}
			}
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
void APickUpItem::ApplyEntry(AAdventurerCharacter* Target, const FItemEffectEntry& E)
{
	if (!Target)return;

	UCharacterMovementComponent* CM = Target->GetCharacterMovement();
	if (!CM)return;

	const float Mult = (E.Type == EItemEffectType::Slow)
		? FMath::Clamp(E.Multiplier, 0.01f, 1.f)
		: FMath::Max(E.Multiplier, 0.01f);
	const EItemEffectType Type = E.Type;					//下から上に移動
	const float Duration = FMath::Max(E.Duration, 0.1f);

	FName GuardTag = NAME_None;

	switch (E.Type)
	{
	case EItemEffectType::MoveSpeed:      
	case EItemEffectType::Slow:          
		CM->MaxWalkSpeed *= Mult; break;
	case EItemEffectType::JumpPower:CM->JumpZVelocity *= Mult; break;
	case EItemEffectType::TrapGuard:GuardTag = ATrapBase::AddTrapGuard(Target); break;
	case EItemEffectType::Vision:
	{
		UCameraComponent* Cam = Target->FindComponentByClass<UCameraComponent>();
		if (!Cam) break;

		const float I = FMath::Clamp(E.Intensity, 0.f, 1.f);
		FPostProcessSettings& S = Cam->PostProcessSettings;

		S.bOverride_VignetteIntensity = true;
		S.VignetteIntensity = FMath::Lerp(0.4f, 2.0f, I);//周辺を暗く

		S.bOverride_ColorSaturation = true;
		S.ColorSaturation = FVector4(1.f, 1.f, 1.f, 1.f) * FMath::Lerp(1.f, 0.3f, I);

		S.bOverride_SceneFringeIntensity = true;
		S.SceneFringeIntensity = FMath::Lerp(0.f, 4.f, I);//色のにじみ

		Cam->PostProcessBlendWeight = 0.f;

		// このカメラに最後にかけた効果として記録(重なったら新しい方が優先)
		TWeakObjectPtr<UCameraComponent> WeakCam = Cam;
		const uint32 MyId = GVisionNextId++;
		GVisionOwner.Add(WeakCam, MyId);

		const float StartTime = Target->GetWorld()->GetTimeSeconds();
		const float FadeIn = FMath::Max(E.FadeIn, 0.f);
		const float FadeOut = FMath::Max(E.FadeOut, 0.f);
		TSharedPtr<FTimerHandle> FadeHandle = MakeShared<FTimerHandle>();

		// フェードイン → 維持 → フェードアウト
		FTimerDelegate FadeDelegate = FTimerDelegate::CreateWeakLambda(Target,
			[Target, WeakCam, MyId, StartTime, FadeIn, FadeOut, Duration, FadeHandle]()
			{
				UCameraComponent* C = WeakCam.Get();
				UWorld* W = Target->GetWorld();
				const uint32* Owner = GVisionOwner.Find(WeakCam);

				// カメラが無い/新しい効果に引き継がれた → この効果は何もせず終了
				if (!C || !W || !Owner || *Owner != MyId)
				{
					Target->GetWorldTimerManager().ClearTimer(*FadeHandle);
					return;
				}

				const float T = W->GetTimeSeconds() - StartTime;

				const float In = (FadeIn > 0.f) ? T / FadeIn : 1.f;                      // 0→1
				const float Out = (T <= Duration) ? 1.f
					: (FadeOut > 0.f) ? 1.f - (T - Duration) / FadeOut : 0.f;  // 1→0

				const float Weight = FMath::Clamp(FMath::Min(In, Out), 0.f, 1.f);
				C->PostProcessBlendWeight = FMath::SmoothStep(0.f, 1.f, Weight);

				if (T >= Duration + FadeOut)
				{
					C->PostProcessBlendWeight = 0.f;
					GVisionOwner.Remove(WeakCam);
					Target->GetWorldTimerManager().ClearTimer(*FadeHandle);
				}
			});

		Target->GetWorldTimerManager().SetTimer(*FadeHandle, FadeDelegate, 0.016f, true);
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

	FTimerDelegate Delegate = FTimerDelegate::CreateWeakLambda(Target,
		[Target, Type, Mult, LoopNC,GuardTag]()
			{
				if (UCharacterMovementComponent* C = Target->GetCharacterMovement())
				{
					switch (Type)
					{
					case EItemEffectType::MoveSpeed: 
					case EItemEffectType::Slow:         
						C->MaxWalkSpeed /= Mult; break;
					case EItemEffectType::JumpPower:C->JumpZVelocity /= Mult;	break;
					default: break;
					}
				}
		
				//時間切れになったガードを外す
				if (Type == EItemEffectType::TrapGuard)
				{
					Target->Tags.RemoveSingle(GuardTag);
				}

				if (UNiagaraComponent* NC = LoopNC.Get()) { NC->DestroyComponent(); }
					
		});
	FTimerHandle Handle;
	Target->GetWorldTimerManager().SetTimer(Handle, Delegate, Duration, false);

	
}


