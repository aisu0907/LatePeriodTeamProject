// Fill out your copyright notice in the Description page of Project Settings.

#include "GamePlayModeBase.h"
#include "AdventurerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "AdventurerCharacter.h"

using enum EGamePhase;

//コンストラクタ
AGamePlayModeBase::AGamePlayModeBase()
{
    PrimaryActorTick.bCanEverTick = true;
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

    //初期フェーズ設定
    SetPhase(EGamePhase::Trap);
}

void AGamePlayModeBase::Tick(float DeltaTime)
{
    if (GamePhase != Trap)
        return;

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            1,                  // Key(同じ値なら上書き表示される)
            0.f,                // 表示時間(毎フレーム更新するので 0 でOK)
            FColor::Yellow,
            FString::Printf(TEXT("Trap残り: %.1f"), GetTrapRemainingTime()));
    }
}

//フェーズ設定用
void AGamePlayModeBase::SetPhase(EGamePhase NewPhase)
{
    //フェーズ変更時にタイマーをリセット
    GetWorldTimerManager().ClearTimer(TrapPhaseTimerHandle);

    //フェーズ更新
    GamePhase = NewPhase;

    //トラップフェーズの時
    if (NewPhase == EGamePhase::Trap)
    {
        //タイマー起動
        GetWorldTimerManager().SetTimer(
            TrapPhaseTimerHandle, this, &AGamePlayModeBase::TrapPhaseEnd,
            TrapPhaseTime, false);
    }
}

//Trapフェーズ終了時用
void AGamePlayModeBase::TrapPhaseEnd()
{
    SetPhase(EGamePhase::Play);
    RoundReset(EAdventurerState::PlayMode);
}

//ラウンド勝敗判定
void AGamePlayModeBase::RoundWin(AAdventurerCharacter* Player)
{

    if (!Player || GamePhase != EGamePhase::Play)
        return;

    //プレイヤーの番号を取得
    int32 Index = Player->GetPlayerIndex();

    //範囲外なら
    if (!PlayerWin.IsValidIndex(Index))
        return;

    //プレイヤーの勝利数を上げる
    ++PlayerWin[Index];

    //プレイヤーの勝利数がクリア可能だったら
    if (PlayerWin[Index] >= VictoryConditions)
    {
        UE_LOG(LogTemp, Warning, TEXT("Hello"));
        Result();
    }
    else
    {
        RoundReset(EAdventurerState::TrapMode);
        SetPhase(EGamePhase::Trap);
    }
}

//クリア処理
void AGamePlayModeBase::Result()
{
    
}

//ラウンドリセット用
void AGamePlayModeBase::RoundReset(EAdventurerState NewState)
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
            Adventurer->PlayerReset(NewState);
        }
    }
}

float AGamePlayModeBase::GetTrapRemainingTime() const
{
    FTimerManager& TM = GetWorldTimerManager();

    if (!TM.IsTimerActive(TrapPhaseTimerHandle))
    {
        return 0.f;
    }
    return TM.GetTimerRemaining(TrapPhaseTimerHandle);
}
