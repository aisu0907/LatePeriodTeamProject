// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BigBall.generated.h"

UCLASS()
class TEAMPROJECT_API ABigBall : public AActor
{
	GENERATED_BODY()
	
public:	
	ABigBall();
	virtual void Tick(float DeltaTime) override;

protected:
	

	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnBallHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse,
		const FHitResult& Hit);

	void RespawnPlayer(ACharacter* Player);

	void RespawnBall();

	//リスポーンの秒数
	UPROPERTY(EditAnywhere, Category = "Ball")
	float RespawnTime = 10.f;

	//速度上限
	UPROPERTY(EditAnywhere, Category = "Ball")
	float MaxSpeed = 1500.f;

	FTransform InitialTransform;
	FTimerHandle RespawnTimer;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	//消える判定を始める速度
	UPROPERTY(EditAnywhere, Category = "Ball")
	float ArmSpeed = 400.f;

	//下回ったっら消す
	UPROPERTY(EditAnywhere, Category = "Ball")
	float DestroySpeed = 100.f;

	bool bArmed = false;

};
