// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TrapBase.h"
#include "LandmineTrap.generated.h"

class USphereComponent;

UCLASS()
class TEAMPROJECT_API ALandmineTrap : public ATrapBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALandmineTrap();

protected:
	virtual void ActivateTrap_Implementation(AActor* TriggeringActor) override;

	//踏んだ判定用(小さめ)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> TriggerSphere;

	//爆発の範囲(半径)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap", meta = (ClampMin = "0.0"))
	float ExplosionRadius = 300.f;

	//爆発の演出用(エフェクト・サウンドなどをBPで繋ぐ)
	UFUNCTION(BlueprintImplementableEvent, Category = "Trap")
	void OnExploded();
public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

};
