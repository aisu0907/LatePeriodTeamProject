// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TrapBase.h"
#include "SpikeTrap.generated.h"

class UBoxComponent;
class UstaticMeshComponent;

/**
 * 
 */
UCLASS()
class TEAMPROJECT_API ASpikeTrap : public ATrapBase
{
	GENERATED_BODY()

public:
	ASpikeTrap();
	virtual void Tick(float DeltaTIme)override;

protected:
	virtual void BeginPlay()override;
	virtual void ActivateTrap_Implementation(AActor* TriggeringActor) override;
	virtual void ResetTrap_Implementation() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> TriggerBox;

	// 与えるダメージ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap")
	float Damage = 20.f;

	// トゲが出る演出(Timelineなどで動かす用)
	UFUNCTION(BlueprintImplementableEvent, Category = "Trap")
	void OnSpikesRaised();

	// トゲが戻る演出
	UFUNCTION(BlueprintImplementableEvent, Category = "Trap")
	void OnSpikesLowered();
	
	//針のメッシュ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent>SpikesMesh;

	//針が隠れている位置
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike")
	FVector LoweredLocation = FVector(0.f, 0.f, -100.f);

	//針が出ている位置
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike")
	FVector ReisedLocation = FVector(0.f, 0.f, 0.f);
	
	// 針が動く速さ(cm/秒)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike")
	float MoveSpeed = 400.f;

private:
	void MoveSpikesTo(const FVector& NewTarget);
	FVector TargetLocation;
};
