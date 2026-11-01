// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrapPlacementZone.h" 
#include "TrapBase.generated.h"

class UShapeComponent;
class UStaticMeshComponent;

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EPlaceableSurface : uint8
{
	None = 0      UMETA(Hidden),
	Floor = 1 << 0,
	Wall = 1 << 1
};

//トラップの状態管理用
UENUM(BlueprintType)
enum class ETrapState : uint8
{
	Idle, 
	Triggered, 
	Cooldown, 
	Disabled
};

UCLASS(Abstract) //エディター上での設置不可
class TEAMPROJECT_API ATrapBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrapBase();

	//外部からのトラップON/OFF切り替え
	UFUNCTION(BlueprintCallable, Category = "Trap")
	void SetTrapEnabled(bool bEnabled);

	//外部からのトラップの状態参照
	UFUNCTION(BlueprintPure, Category = "Trap")
	ETrapState GetTrapState() const { return TrapState; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trap",
		meta = (Bitmask, BitmaskEnum = "/Script/TeamProject.EPlaceableSurface"))
	int32 PlaceableSurfaces = static_cast<int32>(EPlaceableSurface::Floor); 

	//トラップの設置の種類判定用
	bool CanPlaceOn(EPlacementSurface Surface) const
	{
		//立っていビットの数でトラップの種類を判定する
		const int32 Bit = (Surface == EPlacementSurface::Floor)
			? static_cast<int32>(EPlaceableSurface::Floor)
			: static_cast<int32>(EPlaceableSurface::Wall);
		return (PlaceableSurfaces & Bit) != 0;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//大まかな条件分岐
	virtual bool CanTrigger(AActor* TriggeringActor) const;

	//トラップで相手に起こす処理
	UFUNCTION(BlueprintNativeEvent, Category = "Trap")
	void ActivateTrap(AActor* TriggeringActor);
	virtual void ActivateTrap_Implementation(AActor* TriggeringActor);

	//トラップの状態をリセットする用
	UFUNCTION(BlueprintNativeEvent, Category = "Trap")
	void ResetTrap();
	virtual void ResetTrap_Implementation();

	//プレイヤーチェックポイント転送用
	void SendToCheckpoint(AActor* Victim);

	//オブジェクトコリジョン設定
	void SetupTriggerVolume(UShapeComponent* Shape);

	//衝突判定
	UFUNCTION()
	void OnTriggerBeginOverlap(
		UPrimitiveComponent* OverlappedComp, 
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex,
		bool bFromSweep, 
		const FHitResult& SweepResult);

	//トラップの動き
	void TriggerTrap(AActor* TriggeringActor);
	//
	void ExecuteActivation(TWeakObjectPtr<AActor> WeakActor);
	void FinishCooldown();
protected:	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> Root;

	//メッシュ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> TrapMesh;

	//コリジョン
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UShapeComponent> TriggerVolume;

	//予兆演出用
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trap", meta = (ClampMin = "0.0"))
	float ActivationDelay = 0.f;

	//トラップのクールタイム
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trap", meta = (ClampMin = "0.0"))
	float CooldownTime = 3.f;

	//一度きりのトラップ用 trueでもう発動しない
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trap")
	bool bOneShot = false;

	//トラップの状態保存用
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Trap")
	ETrapState TrapState = ETrapState::Idle;

	//タイマー管理用
	FTimerHandle ActivationTimer;
	FTimerHandle CooldownTimer;
};
