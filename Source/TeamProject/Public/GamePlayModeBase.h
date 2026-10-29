// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GamePlayModeBase.generated.h"

class AAdventurerCharacter;

//ÉvÉåÉCÉÑÅ[Index
UENUM(BlueprintType)
enum class EPlayerIndex : uint8
{
	Player1,
	Player2,
};

UCLASS()
class TEAMPROJECT_API AGamePlayModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	AGamePlayModeBase();

	void RoundWin(AAdventurerCharacter* Player);

	void BeginPlay() override; 

private:

	void Result();

	UPROPERTY( EditAnywhere, Category = "PlayerWin")
	int32 VictoryConditions = 3;

	UPROPERTY(VisibleAnywhere, Category = "PlayerWin")
	int32 PlayerWin[2] = { 0 };
};
