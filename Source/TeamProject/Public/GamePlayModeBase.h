// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GamePlayModeBase.generated.h"

class AAdventurerCharacter;

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
	GameEnd,
};

UCLASS()
class TEAMPROJECT_API AGamePlayModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	AGamePlayModeBase();


	void BeginPlay() override; 

	// Called every frame
	void Tick(float DeltaTime) override;

	void RoundWin(AAdventurerCharacter* Player);
private:

	void Result();

	void RoundReset();

	void TrapPhaseEnd();

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
