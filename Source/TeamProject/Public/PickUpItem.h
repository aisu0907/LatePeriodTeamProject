// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUpItem.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UNiagaraSystem;

//カスケードも読み込めるように
class UParticleSystem;

//デバフ
class ACharacter;
class AAdventurerChracter;

UENUM(BlueprintType)
enum  class EItemEffectType:uint8
{
	MoveSpeed	UMETA(DisplayName = "MoveSpeed"),
	JumpPower	UMETA(DisplayName = "JumpPower"),
	Vision		UMETA(DisplayName = "Vision"),
};
USTRUCT(BlueprintType)
struct  FItemEffectEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemEffectType Type = EItemEffectType::MoveSpeed;

	// 1未満=デバフ、1より大きい=バフ(視界悪化では使わない)
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (ClampMin = "0.01", EditCondition = "Type != EItemEffectType::Vision", EditConditionHides))
	float Multiplier = 0.5f;

	// 視界悪化の強さ(0=ほぼ変化なし、1=最大)
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (ClampMin = "0.0", ClampMax = "1.0", EditCondition = "Type == EItemEffectType::Vision", EditConditionHides))
	float Intensity = 0.7f;

	// 効果時間(秒)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.1"))
	float Duration = 5.f;

	// この効果の間だけ出し続けるエフェクト(任意)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* LoopEffect = nullptr;
};

UCLASS()
class TEAMPROJECT_API APickUpItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickUpItem();

	void OnPickedUp(ACharacter* NewOwner);
	void Use();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
	
	// バフ中に出し続けるエフェクト
	UPROPERTY(EditAnywhere, Category = "Effect", meta = (DisplayName = "Buff"))
	UNiagaraSystem* PickupEffect = nullptr;

	// 使った瞬間に1回だけ出すエフェクト
	UPROPERTY(EditAnywhere, Category = "Effect", meta = (DisplayName = "Use"))
	UNiagaraSystem* UseEffect = nullptr;

	//カスケード版
	UPROPERTY(EditAnywhere, Category = "Effect", meta = (DisplayName = "USECascade"))
	UParticleSystem* UseParticle = nullptr;

	// (使用中)
	UPROPERTY(EditAnywhere, Category = "Effect", meta = (DisplayName = "BUFFCascade"))
	UParticleSystem* BuffParticle = nullptr;

	UPROPERTY(EditAnywhere, Category = "Effect")
		FRotator EffectRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, Category = "Effect")
	FVector EffectScale = FVector(1.f);

	UPROPERTY(EditAnywhere, Category = "Effect")
	FVector EffectOffset = FVector::ZeroVector;

	//効果リスト
	UPROPERTY(EditAnywhere, Category = "Effects")
	TArray<FItemEffectEntry>Effects;

	void ApplyEntry(AAdventurerChracter* Target, const FItemEffectEntry& Entry);
};
