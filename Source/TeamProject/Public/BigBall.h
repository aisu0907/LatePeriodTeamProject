// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TrapBase.h"
#include "BigBall.generated.h"

class USphereComponent;

UCLASS()
class TEAMPROJECT_API ABigBall : public ATrapBase
{
	GENERATED_BODY()
	
public:	
	ABigBall();
	virtual void Tick(float DeltaTime) override;

protected:
	

	virtual void BeginPlay() override;
	
	virtual void ActivateTrap_Implementation(AActor* TriggeringActor)override;

	void RespawnBall();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> TriggerSphere;

	//リスポーンの秒数
	UPROPERTY(EditAnywhere, Category = "Ball")
	float RespawnTime = 10.f;

	//速度上限
	UPROPERTY(EditAnywhere, Category = "Ball")
	float MaxSpeed = 1500.f;

	FVector InitialLocation;
	FRotator InitialRotation;
	FTimerHandle RespawnTimer;

	//消える判定を始める速度
	UPROPERTY(EditAnywhere, Category = "Ball")
	float ArmSpeed = 400.f;

	//下回ったっら消す
	UPROPERTY(EditAnywhere, Category = "Ball")
	float DestroySpeed = 100.f;

	bool bArmed = false;

};
