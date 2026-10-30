// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TrapPlacementComponent.generated.h"

class ATrapPlacementZone;
class AAdventurerCharacter;
class ATrapBase;

UCLASS()
class TEAMPROJECT_API UTrapPlacementComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	UTrapPlacementComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction) override;

	// 入力アクションから呼ぶ
	UFUNCTION(BlueprintCallable, Category = "Trap")
	bool PlaceTrap(TSubclassOf<ATrapBase> TrapBP);

	// 床ゾーンに置くトラップ
	UPROPERTY(EditAnywhere, Category = "Trap")
	TSubclassOf<AActor> FloorTrapClass;

	// 壁ゾーンに置くトラップ
	UPROPERTY(EditAnywhere, Category = "Trap")
	TSubclassOf<AActor> WallTrapClass;

	// 視線トレースの長さ(cm)
	UPROPERTY(EditAnywhere, Category = "Trap")
	float TraceDistance = 1500.f;

private:

	void UpdateTarget();

	TSubclassOf<AActor> GetTrapClassFor(const ATrapPlacementZone* Zone) const;

	bool bHasValidTarget = false;
	FVector TargetLocation = FVector::ZeroVector;
	FIntPoint TargetCell = FIntPoint::ZeroValue;
	TWeakObjectPtr<ATrapPlacementZone> TargetZone;

	AAdventurerCharacter* PlayerChara;
};