// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GamePlayModeBase.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJECT_API AGamePlayModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	AGamePlayModeBase();

	void BeginPlay();
};
