// ActorIconSubsystem.cpp

#include "UI/ActorIconSubsystem.h"
#include "Components/MeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"

void UActorIconSubsystem::EnsureCapture()
{
    //キャッシュにあったら
    if (Capture) return;

    //アクター生成
    UWorld* W = GetWorld();
    CaptureHolder = W->SpawnActor<AActor>();

    //アクター設定
    USceneComponent* Root = NewObject<USceneComponent>(CaptureHolder);
    CaptureHolder->SetRootComponent(Root);
    Root->RegisterComponent();
    CaptureHolder->SetActorLocation(FVector(0, 0, 100000));   // ゲーム空間から遠ざける

    //カメラ生成
    Capture = NewObject<USceneCaptureComponent2D>(CaptureHolder);
    Capture->SetupAttachment(Root);
    Capture->RegisterComponent();
    //カメラ設定
    Capture->bCaptureEveryFrame = false;
    Capture->bCaptureOnMovement = false;
    Capture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
    Capture->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;
}

UTextureRenderTarget2D* UActorIconSubsystem::GetIcon(TSubclassOf<AActor> ActorClass)
{
    //アクターがなかった・アクターがすでに撮られていた
    if (!ActorClass) return nullptr;
    if (auto* Found = Cache.Find(ActorClass)) return *Found;

    //カメラ生成
    EnsureCapture();

    FActorSpawnParameters P;
    P.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    
    //カメラの近くに撮影用アクターを生成
    AActor* Preview = GetWorld()->SpawnActor<AActor>(
        ActorClass, CaptureHolder->GetActorLocation() + FVector(300, 0, 0), FRotator::ZeroRotator, P);
    if (!Preview) return nullptr;
    Preview->SetActorEnableCollision(false);

    //アクターの画角を調整
    // メッシュだけを対象にバウンズを作る
    FBox Box(ForceInit);
    TArray<UMeshComponent*> Meshes;
    Preview->GetComponents<UMeshComponent>(Meshes);
    for (const UMeshComponent* Mesh : Meshes)
    {
        if (Mesh->IsVisible())
        {
            Box += Mesh->Bounds.GetBox();
        }
    }
    FVector Origin, Extent;
    if (Box.IsValid)
    {
        Box.GetCenterAndExtents(Origin, Extent);
    }
    else
    {
        // メッシュが1つも無いアクターは従来の方法にフォールバック
        Preview->GetActorBounds(false, Origin, Extent);
    }
    const float Radius = Extent.Size();    const float Dist = Radius * 2.5f;

    const FVector CamPos = Origin + FVector(-Dist, 0, Radius * 0.5f);
    Capture->SetWorldLocationAndRotation(CamPos, (Origin - CamPos).Rotation());

    //写真を撮影
    UTextureRenderTarget2D* RT = NewObject<UTextureRenderTarget2D>(this);
    RT->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA16f;
    RT->InitAutoFormat(256, 256);
    Capture->TextureTarget = RT;
    Capture->ShowOnlyActors.Reset();
    Capture->ShowOnlyActors.Add(Preview);
    Capture->CaptureScene();

    Preview->Destroy();
    Cache.Add(ActorClass, RT);
    return RT;
}