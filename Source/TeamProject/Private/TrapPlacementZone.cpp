// Fill out your copyright notice in the Description page of Project Settings.


#include "TrapPlacementZone.h"
#include "Components/BoxComponent.h"

// Sets default values
ATrapPlacementZone::ATrapPlacementZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//オブジェクト生成
	ZoneBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneBox"));
	SetRootComponent(ZoneBox);
	ZoneBox->SetBoxExtent(FVector(500.f, 500.f, 5.f));

	ZoneBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ZoneBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	ZoneBox->SetCollisionResponseToChannel(ECC_Placement, ECR_Block);
}

// Called when the game starts or when spawned
void ATrapPlacementZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATrapPlacementZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//トラップをマスの中心に吸着
bool ATrapPlacementZone::TrySnap(const FVector& WorldPoint, FVector& OutLocation, FIntPoint& OutCell) const
{
<<<<<<< HEAD
	return 0;
=======
	//ワールド座標からローカル座標に変更
	const FTransform T = GetActorTransform();
	const FVector Local = T.InverseTransformPositionNoScale(WorldPoint);
>>>>>>> 48147ad95b31a5d4dd52c2c1cb39249aa96c4aa4

	OutCell = FIntPoint(
		FMath::FloorToInt(Local.X / CellSize),
		FMath::FloorToInt(Local.Y / CellSize));

	const FVector SnappedLocal(
		(OutCell.X + 0.5f) * CellSize,
		(OutCell.Y + 0.5f) * CellSize,
		Local.Z);

	//マス内判定
	const FVector Extent = ZoneBox->GetUnscaledBoxExtent();
	const float Half = CellSize * 0.5f;
	if (FMath::Abs(SnappedLocal.X) + Half > Extent.X ||
		FMath::Abs(SnappedLocal.Y) + Half > Extent.Y)
	{
		return false;
	}

	OutLocation = T.TransformPositionNoScale(SnappedLocal);
	return true;
}

//トラップが設置できるか
bool ATrapPlacementZone::IsCellFree(FIntPoint Cell) const
{
<<<<<<< HEAD
	return 0;
=======
	const TWeakObjectPtr<AActor>* Found = Occupied.Find(Cell);
	return !Found || !Found->IsValid();
>>>>>>> 48147ad95b31a5d4dd52c2c1cb39249aa96c4aa4
}

//トラップ設置処理
void ATrapPlacementZone::RegisterTrap(FIntPoint Cell, AActor* Trap)
{

}


void ATrapPlacementZone::ReleaseCell(FIntPoint Cell)
{

}
