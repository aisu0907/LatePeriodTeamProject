#include "BigBall.h"
#include "Components/SphereComponent.h"        
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"  

ABigBall::ABigBall()
{
 	
	PrimaryActorTick.bCanEverTick = true;

	TrapMesh->SetSimulatePhysics(true);
	TrapMesh->SetCollisionProfileName(TEXT("PhysicsActor")); // 【修正】PhysicActor → PhysicsActor

	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	SetupTriggerVolume(TriggerSphere);        // これで基底のTriggerVolumeに登録される
	TriggerSphere->SetupAttachment(TrapMesh);
	TriggerSphere->InitSphereRadius(60.f);
}
void ABigBall::BeginPlay()
{
	Super::BeginPlay();
	InitialLocation = TrapMesh->GetComponentLocation();
	InitialRotation = TrapMesh->GetComponentRotation();

	GetWorldTimerManager().SetTimer(RespawnTimer, this, &ABigBall::RespawnBall, RespawnTime, false);
}


void ABigBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FVector Velocity = TrapMesh->GetPhysicsLinearVelocity();
	const float Speed = Velocity.Size();

	//速度上限
	if (Speed > MaxSpeed)
	{
		TrapMesh->SetPhysicsLinearVelocity(Velocity.GetSafeNormal() * MaxSpeed);
	}
	if (!bArmed)
	{
		if (Speed >= ArmSpeed) { bArmed = true; }
		return;
	}
	if (Speed <= DestroySpeed)
	{
		RespawnBall();
	}
}

void ABigBall::ActivateTrap_Implementation(AActor* TriggeringActor)
{
	if (!TriggeringActor) { return; }

	// シールドがあれば消費して終わり
	if (TryGuard(TriggeringActor)) { return; }

	SendToCheckpoint(TriggeringActor);
}

void ABigBall::RespawnBall()
{
	//速度をリセットしてから戻す
	TrapMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
	TrapMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	TrapMesh->SetWorldLocationAndRotation(InitialLocation, InitialRotation,
		false, nullptr, ETeleportType::TeleportPhysics);

	bArmed = false;//再スタートの時に生成直後の判定で消えないように

	//タイマーリセット
	GetWorldTimerManager().SetTimer(RespawnTimer, this, &ABigBall::RespawnBall,RespawnTime, false);
}