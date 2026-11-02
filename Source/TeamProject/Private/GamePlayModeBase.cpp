// Fill out your copyright notice in the Description page of Project Settings.

#include "GamePlayModeBase.h"
#include "AdventurerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "AdventurerCharacter.h"

using enum EGamePhase;

AGamePlayModeBase::AGamePlayModeBase()
{
    PlayerWin.Init(0, 2); // 要素0を2個
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

void AGamePlayModeBase::TrapPhaseEnd()
{

}

//ラウンド勝敗判定
void AGamePlayModeBase::RoundWin(AAdventurerCharacter* Player)
{
    //プレイヤーの番号を取得
    int32 Index = Player->GetPlayerIndex();

    //範囲外なら
    if (Index < 0)
        return;

    //プレイヤーの勝利数を上げる
    PlayerWin[Index]++;

    //プレイヤーの勝利数がクリア可能だったら
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

//クリア処理
void AGamePlayModeBase::Result()
{
    
}

//ラウンドリセット用
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
            //プレイヤーの情報リセット
            Adventurer->PlayerReset();
        }
    }

    GamePhase = Trap;
}
