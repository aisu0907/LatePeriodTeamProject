// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUpItem.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UNiagaraSystem;

UCLASS()
class TEAMPROJECT_API APickUpItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickUpItem();

	void OnPickedUp(ACharacter* NewOwner);
	void Use();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
	
	//ÉiÉCÉAÉKÉâ
	UPROPERTY(EditAnywhere, Category = "Effect")
	UNiagaraSystem* PickupEffect;

	UPROPERTY(EditAnywhere, Category = "Effect")
		FRotator EffectRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, Category = "Effect")
	FVector EffectScale = FVector(1.f);

	UPROPERTY(EditAnywhere, Category = "Effect")
	FVector EffectOffset = FVector::ZeroVector;

};
