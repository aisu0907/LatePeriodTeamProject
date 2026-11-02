// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GamePlayModeBase.generated.h"

class AAdventurerCharacter;
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

	void BeginPlay() override; 

	// Called every frame
	void Tick(float DeltaTime) override;

	void RoundWin(AAdventurerCharacter* Player);

	// Trap フェーズの残り秒数
	UFUNCTION(BlueprintPure, Category = "Phase")
	float GetTrapRemainingTime() const;
private:

	void Result();

	void RoundReset(EAdventurerState NewState);

	void TrapPhaseEnd();

	void SetPhase(EGamePhase NewPhase);

	EGamePhase GamePhase = EGamePhase::Trap;

	UPROPERTY( EditAnywhere, Category = "PlayerWin")
	int32 VictoryConditions = 3;

	//プレイヤーの勝利数
	UPROPERTY(VisibleAnywhere, Category = "PlayerWin")
	TArray<int32> PlayerWin;

	//トラップフェーズタイマー
	UPROPERTY()
	FTimerHandle TrapPhaseTimerHandle;
};
