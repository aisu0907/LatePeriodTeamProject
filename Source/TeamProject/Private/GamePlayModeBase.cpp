// Fill out your copyright notice in the Description page of Project Settings.

#include "GamePlayModeBase.h"
#include "AdventurerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "AdventurerCharacter.h"

using enum EGamePhase;

AGamePlayModeBase::AGamePlayModeBase()
{
    
}

void AGamePlayModeBase::BeginPlay()
{
    Super::BeginPlay();

    if (UGameplayStatics::GetPlayerController(this, 1) == nullptr)
    {
        //プレイヤー2を作成
        UGameplayStatics::CreatePlayer(this, 1, true);
    }
}

void AGamePlayModeBase::Tick(float DeltaTime)
{
    if (GamePhase != Trap)
        return;
}

//ラウンド勝敗判定
void AGamePlayModeBase::RoundWin(AAdventurerCharacter* Player)
{
    int32 Index = Player->GetPlayerIndex();

    if (Index < 0)
        return;

    PlayerWin[Index]++;


    if (PlayerWin[Index] >= VictoryConditions)
    {
        UE_LOG(LogTemp, Warning, TEXT("Hello"));
        Result();
    }
    else
    {
        RoundReset();
    }
}

void AGamePlayModeBase::Result()
{
    
}

void AGamePlayModeBase::RoundReset()
{
    UE_LOG(LogTemp, Warning, TEXT("RoundReset"));

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (!PC)
        {
            continue;
        }

        if (AAdventurerCharacter* Adventurer = Cast<AAdventurerCharacter>(PC->GetPawn()))
        {
            UE_LOG(LogTemp, Warning, TEXT("RoundReset"));
            Adventurer->PlayerReset();
        }
    }

    GamePhase = Trap;
}
