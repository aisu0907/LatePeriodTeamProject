// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrapPlacementZone.generated.h"

class UBoxComponent;

#define ECC_Placement ECC_GameTraceChannel1

//エリアの種類分け
UENUM(BlueprintType)
enum class EPlacementSurface : uint8
{
	Floor,
	Wall
};

UCLASS(Blueprintable)
class TEAMPROJECT_API ATrapPlacementZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrapPlacementZone();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> ZoneBox;

	// 床用か壁用か（置けるトラップの種類を決める）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone")
	EPlacementSurface SurfaceType = EPlacementSurface::Floor;

	// 1マスの大きさ(cm)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid")
	float CellSize = 100.f;

	// ワールド座標をマス中心に吸着。ゾーン外なら false
	bool TrySnap(const FVector& WorldPoint, FVector& OutLocation, FIntPoint& OutCell) const;

	// マスが空いているか（設置済みトラップが破壊されていれば空きとして扱う）
	bool IsCellFree(FIntPoint Cell) const;

	// 設置したトラップをマスに登録 / 解除
	void RegisterTrap(FIntPoint Cell, AActor* Trap);
	void ReleaseCell(FIntPoint Cell);

	// BP側で演出を足すためのフック(本体はBPで実装。C++には書かない)
	UFUNCTION(BlueprintImplementableEvent, Category = "Zone")
	void OnCellOccupied(FIntPoint Cell, bool bOccupied);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	TMap<FIntPoint, TWeakObjectPtr<AActor>> Occupied;
	
};
