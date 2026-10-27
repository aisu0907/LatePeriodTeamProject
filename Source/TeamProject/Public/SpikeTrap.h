// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TrapBase.h"
#include "SpikeTrap.generated.h"

class UBoxComponent;

/**
 * 
 */
UCLASS()
class TEAMPROJECT_API ASpikeTrap : public ATrapBase
{
	GENERATED_BODY()

public:
	ASpikeTrap();

protected:
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
	
};
