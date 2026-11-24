
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RotatingBridge.generated.h"

UCLASS()
class TEAMPROJECT_API ARotatingBridge : public AActor
{
    GENERATED_BODY()

public:
    ARotatingBridge();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // 回転の中心
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USceneComponent* PivotRoot;

    // 上側の橋
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* BridgeUpper;

    // 下側の橋
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* BridgeLower;

    // 回転速度（度/秒）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
    float RotationSpeed = 30.0f;
};
