// Fill out your copyright notice in the Description page of Project Settings.


#include "TrapPlacementComponent.h"
#include "TrapPlacementZone.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "UI/ActorIconSubsystem.h"
#include "AdventurerCharacter.h"
#include "TrapBase.h"

// Sets default values
UTrapPlacementComponent::UTrapPlacementComponent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryComponentTick.bCanEverTick = true;

}

void UTrapPlacementComponent::BeginPlay()
{
    Super::BeginPlay();

    // 全トラップのアイコンを先に撮影しておく
    if (UWorld* World = GetWorld())
    {
        if (UActorIconSubsystem* Icons = World->GetSubsystem<UActorIconSubsystem>())
        {
            for (const TSubclassOf<ATrapBase>& TrapClass : TrapList)
            {
                TrapIcons.Add(Icons->GetIcon(TrapClass));
            }
        }
    }

    //親クラスのAdventurerを取得
    PlayerChara = Cast<AAdventurerCharacter>(GetOwner());

    //if (TrapList.Num() == 0)
    //{
    //    UClass* Loaded = LoadClass<ATrapBase>(
    //        nullptr, TEXT("/Game/Asset/BluePrint/BP_SpikeTrap.BP_SpikeTrap_C"));
    //    if (Loaded)
    //    {
    //        TrapList.Add(Loaded);
    //    }
    //    else
    //    {
    //        UE_LOG(LogTemp, Warning, TEXT("BP_SpikeTrap load failed"));
    //    }
    //}
}

//選択トラップ変更用
void UTrapPlacementComponent::ChangeSelectTrapIndex(int32 NewIndex)
{
    if (TrapList.IsValidIndex(SelectedIndex + NewIndex))
    {
        SelectedIndex += NewIndex;
    }
}


TSubclassOf<ATrapBase> UTrapPlacementComponent::GetSelectedTrapClass() const
{
    return TrapList.IsValidIndex(SelectedIndex) ? TrapList[SelectedIndex] : nullptr;
}

void UTrapPlacementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!PlayerChara || PlayerChara->GetAdventurerState() != EAdventurerState::TrapMode)
    {
        return;
    }

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

//選択してるマス判定用
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

    //DrawDebugLine(GetWorld(), CamLoc, End, FColor::Red, false, 0.f, 0, 2.f);

    //トレース・ECC_Placementに当たらなかった
    if (!GetWorld()->LineTraceSingleByChannel(Hit, CamLoc, End, ECC_Placement, Params))
    {
        //UE_LOG(LogTemp, Warning, TEXT("Trap: no hit"));
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
    const TSubclassOf<ATrapBase> SelectedClass = GetSelectedTrapClass();
    if (!SelectedClass) { return; }


    //スナップと空き確認
    FVector Snapped; FIntPoint Cell;
    if (Zone->TrySnap(Hit.ImpactPoint, Snapped, Cell)
        && Zone->IsCellFree(Cell) 
        && !IsCellBlocked(Zone, Snapped))
    {
        bHasValidTarget = true;
        
        //ターゲットの情報を取得
        TargetLocation = Snapped;
        TargetCell = Cell;
        TargetZone = Zone;
    }
}

//トラップ設置用
bool UTrapPlacementComponent::PlaceTrap()
{
    const TSubclassOf<ATrapBase> ClassToSpawn = GetSelectedTrapClass();
    if (!bHasValidTarget || !TargetZone.IsValid() || !ClassToSpawn) { return false; }

    if (IsCellBlocked(TargetZone.Get(), TargetLocation)) { return false; }

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    const FTransform SpawnTransform(TargetZone->GetActorQuat(), TargetLocation);

    ATrapBase* Trap = GetWorld()->SpawnActor<ATrapBase>(ClassToSpawn, SpawnTransform, Params);
    //トラップ設置チェック
    if (!Trap) { return false; }

    TargetZone->RegisterTrap(TargetCell, Trap);
    return true;
}

//アクター接触判定用
bool UTrapPlacementComponent::IsCellBlocked(const ATrapPlacementZone* Zone, const FVector& CellLocation) const
{
    const float Half = Zone->CellSize * 0.5f;
    const FVector Center = CellLocation + Zone->GetActorUpVector() * Half;

    //足元や板との接触だけで弾かないよう、少し小さくする
    const FCollisionShape Box = FCollisionShape::MakeBox(FVector(Half * 0.9f));

    FCollisionQueryParams QParams(SCENE_QUERY_STAT(TrapOverlap), false);

    //重なったら置けない
    return GetWorld()->OverlapAnyTestByChannel(
        Center, Zone->GetActorQuat(), ECC_Pawn, Box, QParams);
}
