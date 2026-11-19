// Fill out your copyright notice in the Description page of Project Settings.


#include "IceFloor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

namespace
{
	struct FIceState
	{
		int32 Count = 0;
		float OrigFriction = 0.f;
		float OrigBraking = 0.f;
		bool bOrigSeparate = false;
	};
	TMap<TWeakObjectPtr<UCharacterMovementComponent>, FIceState> GIceStates;
}

// Sets default values
AIceFloor::AIceFloor()
{
	UBoxComponent* Box = CreateDefaultSubobject<UBoxComponent>(TEXT("IceTrigger"));
	Box->SetBoxExtent(FVector(100.f, 100.f, 50.f));
	Box->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	SetupTriggerVolume(Box);

}

// Called when the game starts or when spawned
void AIceFloor::BeginPlay()
{
	Super::BeginPlay();
	
	if (TriggerVolume)
	{
		TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AIceFloor::OnIceBeginOverLap);
		TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &AIceFloor::OnIceEndOverlap);

		//開始時点で乗っているキャラ対応
		TArray<AActor*>Overlapping;
		TriggerVolume->GetOverlappingActors(Overlapping, ACharacter::StaticClass());
		for (AActor* Actor : Overlapping)
		{
			if (ACharacter* Ch = Cast<ACharacter>(Actor))
			{
				ApplyIce(Ch->GetCharacterMovement());
			}
		}
	}
}

void AIceFloor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//破棄時に氷が残らないように全部解除
	TArray < TWeakObjectPtr<UCharacterMovementComponent>>Copy = AffectedMoves.Array();
	{
		for (const TWeakObjectPtr<UCharacterMovementComponent>& move : Copy)
		{
			ReleaseIce(move.Get());
		}
	}
}

void AIceFloor::OnIceBeginOverLap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Ch = Cast<ACharacter>(OtherActor);
	//カプセルだけ見て、メッシュとうによる二重呼び出しを防ぐ
	if (!Ch || OtherComp != Ch->GetCapsuleComponent())return;

	ApplyIce(Ch->GetCharacterMovement());
}

void AIceFloor::OnIceEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACharacter* Ch = Cast<ACharacter>(OtherActor);
	if (!Ch || OtherComp != Ch->GetCapsuleComponent()) return;

	ReleaseIce(Ch->GetCharacterMovement());
}

void AIceFloor::ApplyIce(UCharacterMovementComponent* Move)
{
	if (!Move || AffectedMoves.Contains(Move)) return;
	AffectedMoves.Add(Move);

	FIceState& State = GIceStates.FindOrAdd(Move);
	if (State.Count == 0)
	{
		// 最初の氷のときだけ元の値を保存
		State.OrigFriction = Move->GroundFriction;
		State.OrigBraking = Move->BrakingDecelerationWalking;
		State.bOrigSeparate = Move->bUseSeparateBrakingFriction;
	}
	State.Count++;

	Move->GroundFriction = IceGroundFriction;
	Move->BrakingDecelerationWalking = IceBrakingDeceleration;
	Move->bUseSeparateBrakingFriction = true;
	Move->BrakingFriction = IceGroundFriction;
}

void AIceFloor::ReleaseIce(UCharacterMovementComponent* Move)
{
	if (!Move || AffectedMoves.Remove(Move) == 0) return;

	FIceState* State = GIceStates.Find(Move);
	if (!State) return;

	if (--State->Count <= 0)
	{
		// 最後の氷から降りたら元に戻す
		Move->GroundFriction = State->OrigFriction;
		Move->BrakingDecelerationWalking = State->OrigBraking;
		Move->bUseSeparateBrakingFriction = State->bOrigSeparate;
		GIceStates.Remove(Move);
	}
}