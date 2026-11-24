// Fill out your copyright notice in the Description page of Project Settings.


#include "TrapBase.h"
#include "Components/ShapeComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"

#include "GamePlayModeBase.h" //ゲームモード

//---------------------------------------------------

#include "Misc/Guid.h"//シールドのために追加
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

namespace
{
	const TCHAR* GuardPrefix = TEXT("TrapGuard_");
}

//---------------------------------------------------

// Sets default values
ATrapBase::ATrapBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	//メッシュ作成
	TrapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrapMesh"));
	TrapMesh->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void ATrapBase::BeginPlay()
{
	Super::BeginPlay();

	GamePlayMode = GetWorld()->GetAuthGameMode<AGamePlayModeBase>();

	//コリジョンが設定されていたら
	if (TriggerVolume)
	{
		//TriggerVolumeをOverlapに登録
		TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ATrapBase::OnTriggerBeginOverlap);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: TriggerVolume が設定されていません"), *GetName());
	}
}

//コリジョンセット用
void ATrapBase::SetupTriggerVolume(UShapeComponent* Shape)
{
	if (!Shape)
		return;

	TriggerVolume = Shape;
	TriggerVolume->SetupAttachment(Root);
	TriggerVolume->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
	TriggerVolume->SetGenerateOverlapEvents(true);
}


//衝突判定
void ATrapBase::OnTriggerBeginOverlap(
	UPrimitiveComponent* OverlappedComp, 
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult)
{
	//衝突したオブジェクトを判定
	if (CanTrigger(OtherActor))
	{
		//トラップの予備動作を起動
		TriggerTrap(OtherActor);
	}
}

//Pawn判定
bool ATrapBase::CanTrigger(AActor* TriggeringActor) const
{
	if (!GamePlayMode || GamePlayMode->GetGamePhase() != EGamePhase::Play)
		return false;

	//待機状態でオブジェクトがPawnクラスの場合true : じゃない場合 false
	return TrapState == ETrapState::Idle && Cast<APawn>(TriggeringActor) != nullptr;
}

//トラップ予備動作発動
void ATrapBase::TriggerTrap(AActor* TriggeringActor)
{

	//トラップの状態を発動状態に
	TrapState = ETrapState::Triggered;

	TWeakObjectPtr<AActor> WeakActor = TriggeringActor;

	//予備動作がある場合
	if (ActivationDelay > 0.f)
	{
		FTimerDelegate Delegate = FTimerDelegate::CreateWeakLambda(this, [this, WeakActor]()
			{
				//トラップ発動
				ExecuteActivation(WeakActor);
			});
		GetWorldTimerManager().SetTimer(ActivationTimer, Delegate, ActivationDelay, false);
	}
	else
	{
		//トラップ発動
		ExecuteActivation(WeakActor);
	}
}

//トラップ発動
void ATrapBase::ExecuteActivation(TWeakObjectPtr<AActor> WeakActor)
{
	// 遅延中に相手が消えていても、トラップ自体は発動させる
	// (相手が必要な処理は派生側で nullptr チェックすること)
	ActivateTrap(WeakActor.Get());

	//トラップの状態を待機状態に
	TrapState = ETrapState::Cooldown;
	GetWorldTimerManager().SetTimer(CooldownTimer, this, &ATrapBase::FinishCooldown, CooldownTime, false);
}

void ATrapBase::FinishCooldown()
{
	ResetTrap();
	TrapState = bOneShot ? ETrapState::Disabled : ETrapState::Idle;
}

void ATrapBase::ActivateTrap_Implementation(AActor* TriggeringActor)
{
}

void ATrapBase::ResetTrap_Implementation()
{
}

void ATrapBase::SendToCheckpoint(AActor* Victim)
{
}

void ATrapBase::SetTrapEnabled(bool bEnabled)
{
	// 進行中のタイマーを止めてから状態を切り替える
	GetWorldTimerManager().ClearTimer(ActivationTimer);
	GetWorldTimerManager().ClearTimer(CooldownTimer);

	TrapState = bEnabled ? ETrapState::Idle : ETrapState::Disabled;
}

//ガード効果----------------------------------------

FName ATrapBase::AddTrapGuard(AActor* Actor)
{
	if (!Actor) { return NAME_None; }

	//重ね掛けの区別が可能なように、個別の固有の名前にする
	const FName Tag(*FString::Printf(TEXT("%s%s"), GuardPrefix, *FGuid::NewGuid().ToString()));
	Actor->Tags.Add(Tag);
	return Tag;
}

bool ATrapBase::ConsumeTrapGuard(AActor* Actor)
{
	if (!Actor) { return false; }

	for (int32 i = 0; i < Actor->Tags.Num(); ++i)
	{
		if (Actor->Tags[i].ToString().StartsWith(GuardPrefix))
		{
			Actor->Tags.RemoveAt(i);//一個消費
			return true;
		}
	}
	return false;
}

//シールド
bool ATrapBase::TryGuard(AActor* Victim)
{
	if (!ConsumeTrapGuard(Victim)) { return false; }

	if (GuardEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation
		(GetWorld(), GuardEffect, Victim->GetActorLocation());
	}
	OnGuarded(Victim);
	return true;
}