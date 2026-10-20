// Fill out your copyright notice in the Description page of Project Settings.


#include "TrapBase.h"
#include "Components/ShapeComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"

//エリアの種類分け
UENUM(BlueprintType)
enum class ETrapType : uint8
{
	Wall,
	Floor,

};

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