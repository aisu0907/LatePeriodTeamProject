#include "BigBall.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

ABigBall::ABigBall()
{
 	
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetNotifyRigidBodyCollision(true);//Hitイベントを発生させる
	Mesh->SetCollisionProfileName(TEXT("PhysicActor"));

}
void ABigBall::BeginPlay()
{
	Super::BeginPlay();
	Mesh->OnComponentHit.AddDynamic(this, &ABigBall::OnBallHit);

	InitialTransform = GetActorTransform();
	GetWorldTimerManager().SetTimer(RespawnTimer, this, &ABigBall::RespawnBall, RespawnTime, false);
}


void ABigBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FVector Velocity = Mesh->GetPhysicsLinearVelocity();
	const float Speed = Velocity.Size();

	//速度上限
	if (Speed > MaxSpeed)
	{
		Mesh->SetPhysicsLinearVelocity(Velocity.GetSafeNormal() * MaxSpeed);
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

void ABigBall::OnBallHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse,
	const FHitResult& Hit)
{
	if (ACharacter* Player = Cast<ACharacter>(OtherActor))
	{
		RespawnPlayer(Player);
	}
}

void ABigBall::RespawnPlayer(ACharacter* Player)
{
	AActor* Start = UGameplayStatics::GetActorOfClass(this, APlayerStart::StaticClass());
	if (!Start) { return; }

	Player->GetCharacterMovement()->StopMovementImmediately();
	Player->TeleportTo(Start->GetActorLocation(), Start->GetActorRotation());
}

void ABigBall::RespawnBall()
{
	//速度をリセットしてから戻す
	Mesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
	Mesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	SetActorTransform(InitialTransform, false, nullptr, ETeleportType::TeleportPhysics);

	bArmed = false;//再スタートの時に生成直後の判定で消えないように

	//タイマーリセット
	GetWorldTimerManager().SetTimer(RespawnTimer, this, &ABigBall::RespawnBall,RespawnTime, false);
}