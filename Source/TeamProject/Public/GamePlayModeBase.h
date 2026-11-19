// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GamePlayModeBase.generated.h"

class AAdventurerCharacter;
class ATrapPlacementZone;
enum class EAdventurerState : uint8;

//プレイヤーIndex
UENUM(BlueprintType)
enum class EPlayerIndex : uint8
{
	Player1,
	Player2,
};

//フェーズ管理用
UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	Trap,
	Play,
	Result,
};

UCLASS()
class TEAMPROJECT_API AGamePlayModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	AGamePlayModeBase();

	UPROPERTY(EditDefaultsOnly, Category = "Phase")
	float TrapPhaseTime = 30.f;

	//GamePhase取得用
	EGamePhase GetGamePhase() const { return GamePhase; }

	void BeginPlay() override; 

	// Called every frame
	void Tick(float DeltaTime) override;

	void RoundWin(AAdventurerCharacter* Player);

	// Trap フェーズの残り秒数
	UFUNCTION(BlueprintPure, Category = "Phase")
	float GetTrapRemainingTime() const;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

private:

	void Result();

	//ラウンドリセット用
	void RoundReset(EAdventurerState NewState);

	//トラップフェーズ終了用
	void TrapPhaseEnd();

	//フェーズセット用
	void SetPhase(EGamePhase NewPhase);

	//Zoneの表示切り替え用
	void SetZonesVisible(bool bVisible);

	EGamePhase GamePhase = EGamePhase::Trap;

	//必要勝利数
	UPROPERTY( EditAnywhere, Category = "PlayerWin")
	int32 VictoryConditions = 3;

	//プレイヤーの勝利数
	UPROPERTY(VisibleAnywhere, Category = "PlayerWin")
	TArray<int32> PlayerWin;

	//トラップフェーズタイマー
	UPROPERTY()
	FTimerHandle TrapPhaseTimerHandle;

	UPROPERTY()
	TArray<TObjectPtr<ATrapPlacementZone>> Zones;

	//ログインした順に0,1
	int32 NextPlayerIndex = 0;
};
