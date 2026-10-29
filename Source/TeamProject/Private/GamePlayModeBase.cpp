// Fill out your copyright notice in the Description page of Project Settings.

#include "GamePlayModeBase.h"
#include "AdventurerCharacter.h"
#include "Kismet/GameplayStatics.h"

AGamePlayModeBase::AGamePlayModeBase()
{
    
}

void AGamePlayModeBase::BeginPlay()
{
    Super::BeginPlay();

    // プレイヤー0は自動生成されるので、2人目を作る
    // 第2引数のbSpawnPlayerController = true でPawnも生成される
    UGameplayStatics::CreatePlayer(this, 1, true);
}

void AGamePlayModeBase::Result()
{

}

void AGamePlayModeBase::RoundWin(AAdventurerCharacter* Player)
{
    int32 Index = Player->GetPlayerIndex();

    PlayerWin[Index]++;

    for (int32 i = 0; i < 2; i++)
    {
        if (PlayerWin[i] >= VictoryConditions)
        {
            UE_LOG(LogTemp, Warning, TEXT("Hello"));
            Result();
        }
    }
}


