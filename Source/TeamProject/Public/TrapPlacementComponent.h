// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TrapPlacementComponent.generated.h"

class ATrapPlacementZone;
class ATrapBase;
class AAdventurerCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEAMPROJECT_API UTrapPlacementComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	UTrapPlacementComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction) override;

	// 設置出来るトラップを入れる用
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap")
	TArray  <TSubclassOf<ATrapBase>> TrapList;

	// 選択してるトラップの番号
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Trap")
	int32 SelectedIndex = 0;

	// 選択中のインデックスを変更
	UFUNCTION(BlueprintCallable, Category = "Trap")
	void SelectTrapIndex(int32 NewIndex);

	// 入力アクションから呼ぶ
	UFUNCTION(BlueprintCallable, Category = "Trap")
	bool PlaceTrap();

	// 視線トレースの長さ(cm)
	UPROPERTY(EditAnywhere, Category = "Trap")
	float TraceDistance = 1500.f;

protected:

	virtual void BeginPlay() override;

private:

	void UpdateTarget();

	UPROPERTY()
	TObjectPtr<AAdventurerCharacter> PlayerChara;

	// 選択したトラップ取り出し用
	TSubclassOf<ATrapBase> GetSelectedTrapClass() const;




	bool bHasValidTarget = false;

	FVector TargetLocation = FVector::ZeroVector;

	FIntPoint TargetCell = FIntPoint::ZeroValue;

	TWeakObjectPtr<ATrapPlacementZone> TargetZone;
	
};