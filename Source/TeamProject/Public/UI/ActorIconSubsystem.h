// ActorIconSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ActorIconSubsystem.generated.h"

class UTextureRenderTarget2D;
class USceneCaptureComponent2D;

UCLASS()
class UActorIconSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()
public:
    // 同じクラスならキャッシュを返す
    UTextureRenderTarget2D* GetIcon(TSubclassOf<AActor> ActorClass);

private:
    //カメラ生成用関数
    void EnsureCapture();

    //撮影済みアイコン
    UPROPERTY() TMap<TSubclassOf<AActor>, TObjectPtr<UTextureRenderTarget2D>> Cache;
    
    //撮影用コンポーネントを乗せるためのアクター
    UPROPERTY() TObjectPtr<AActor> CaptureHolder;

    //アクターを撮影して連だーあターゲットに描くカメラ
    UPROPERTY() TObjectPtr<USceneCaptureComponent2D> Capture;
};