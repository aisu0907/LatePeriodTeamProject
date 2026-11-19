// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TrapBase.h"
#include "IceFloor.generated.h"

class UCharacterMovementComponent;

UCLASS()
class TEAMPROJECT_API AIceFloor : public ATrapBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIceFloor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;

	//Šî’ê‚Ì“¥‚ñ‚¾‚ç”­“®‚ð–³Œø
	virtual bool CanTrigger(AActor* TriggeringActor)const override { return false; }

	UFUNCTION()
	void OnIceBeginOverLap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnIceEndOverlap(UPrimitiveComponent* overlappedComp, AActor* otherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void ApplyIce(UCharacterMovementComponent* Move);
	void ReleaseIce(UCharacterMovementComponent* Move);

protected:
	//•X‚Ì–€ŽC
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ice", meta = (ClapMin="0.0"))
	float IceGroundFriction = 0.3f;

	//•X‚ÌŒ¸‘¬
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ice", meta = (ClapMin = "0.0"))
	float IceBrakingDeceleration = 50.f;

private:
	//ƒLƒƒƒ‰‚É’á–€ŽC‚ð“K—p
	TSet<TWeakObjectPtr<UCharacterMovementComponent>>AffectedMoves;

};
