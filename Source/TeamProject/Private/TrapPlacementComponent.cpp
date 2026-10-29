// Fill out your copyright notice in the Description page of Project Settings.


#include "TrapPlacementComponent.h"
#include "TrapPlacementZone.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "AdventurerCharacter.h"

// Sets default values
UTrapPlacementComponent::UTrapPlacementComponent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryComponentTick.bCanEverTick = true;

}

//トラップの設置タイプを設定
TSubclassOf<AActor> UTrapPlacementComponent::GetTrapClassFor(const ATrapPlacementZone* Zone) const
{
	return (Zone->SurfaceType == EPlacementSurface::Floor) ? FloorTrapClass : WallTrapClass;
}

void UTrapPlacementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (PlayerChara->GetAdvennturerState() != EAdvennturerState::TrapMode)
        return;

    //見ているオブジェクト確認
    UpdateTarget();

    // 仮プレビュー(本番ではゴーストメッシュに置き換える)
    if (bHasValidTarget && TargetZone.IsValid())
    {
        const float Half = TargetZone->CellSize * 0.5f;
        const FVector Normal = TargetZone->GetActorUpVector();

        //プレビューを表示
        DrawDebugBox(GetWorld(), TargetLocation + Normal * Half,
            FVector(Half), TargetZone->GetActorQuat(), FColor::Green, false, 0.f);
    }
}

void UTrapPlacementComponent::UpdateTarget()
{
    bHasValidTarget = false;

    const APawn* Pawn = Cast<APawn>(GetOwner()); //プレイヤーを取得
    const APlayerController* Controller = Pawn ? Cast<APlayerController>(Pawn->GetController()) : nullptr; //コントローラーを取得

    //コントローラーを取得できない場合
    if (!Controller) { return; }

    //カメラの位置と向きを取得
    FVector CamLoc; FRotator CamRot;
    Controller->GetPlayerViewPoint(CamLoc, CamRot);

    //トレース設定
    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(TrapPlacement), false, GetOwner());
    const FVector End = CamLoc + CamRot.Vector() * TraceDistance;

    //トレース・ECC_Placementに当たらなかった
    if (!GetWorld()->LineTraceSingleByChannel(Hit, CamLoc, End, ECC_Placement, Params))
    {
        return;
    }

    ATrapPlacementZone* Zone = Cast<ATrapPlacementZone>(Hit.GetActor());
    
    //当たったオブジェクトがZoneじゃなかったら
    if (!Zone) { return; }

    // 板の表面(ローカルZ+側)から当たったときだけ有効(裏面は弾く)
    if (FVector::DotProduct(Hit.ImpactNormal, Zone->GetActorUpVector()) < 0.5f)
    {
        return;
    }

    // この面に置けるトラップが未設定ならプレビューも出さない
    if (!GetTrapClassFor(Zone)) { return; }

    //スナップと空き確認
    FVector Snapped; FIntPoint Cell;
    if (Zone->TrySnap(Hit.ImpactPoint, Snapped, Cell) && Zone->IsCellFree(Cell))
    {
        bHasValidTarget = true;
        TargetLocation = Snapped;
        TargetCell = Cell;
        TargetZone = Zone;
    }
}

void UTrapPlacementComponent::PlaceTrap()
{

}
