
#include "RotatingBridge.h"

ARotatingBridge::ARotatingBridge()
{
    // Tick‚ğ—LŒø‰»
    PrimaryActorTick.bCanEverTick = true;

    // ‰ñ“]‚Ì’†S‚ğì¬
    PivotRoot = CreateDefaultSubobject<USceneComponent>(TEXT("PivotRoot"));
    RootComponent = PivotRoot;

    // ã‘¤‚Ì‹´
    BridgeUpper = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BridgeUpper"));
    BridgeUpper->SetupAttachment(RootComponent);

    // ‰º‘¤‚Ì‹´
    BridgeLower = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BridgeLower"));
    BridgeLower->SetupAttachment(RootComponent);

    // ‹´‚ğ“®‚©‚¹‚é‚æ‚¤‚É‚·‚é
    BridgeUpper->SetMobility(EComponentMobility::Movable);
    BridgeLower->SetMobility(EComponentMobility::Movable);
}

void ARotatingBridge::BeginPlay()
{
    Super::BeginPlay();
}

void ARotatingBridge::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Z²‚ğ’†S‚É‰ñ“]
    FRotator Rotation(0.0f, RotationSpeed * DeltaTime, 0.0f);

    AddActorLocalRotation(Rotation);
}
