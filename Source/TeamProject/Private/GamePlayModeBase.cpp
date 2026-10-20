// Fill out your copyright notice in the Description page of Project Settings.

#include "GamePlayModeBase.h"
#include "Kismet/GameplayStatics.h"

AGamePlayModeBase::AGamePlayModeBase()
{
    DefaultPawnClass = AMyCharacter::StaticClass();
    PlayerControllerClass = AMyPlayerController::StaticClass();
    GameStateClass = AMyGameState::StaticClass();
    PlayerStateClass = AMyPlayerState::StaticClass();
    HUDClass = AMyHUD::StaticClass();
}

AGamePlayModeBase::AGamePlayModeBase()
{
    Super::BeginPlay();

    // プレイヤー0は自動生成されるので、2人目を作る
    // 第2引数のbSpawnPlayerController = true でPawnも生成される
    UGameplayStatics::CreatePlayer(this, 1, true);
}