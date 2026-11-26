// Fill out your copyright notice in the Description page of Project Settings.
//AudioSubsystem.cpp

#include "Audio/AudioSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "Engine/GameInstance.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

void UAudioSubsystem::RegisterAudioData(UAudioDataAsset* Data)
{
    if (!Data || LoadHandles.Contains(Data)) return;

    TArray<FSoftObjectPath> Paths;
    for (const TPair<FGameplayTag, FSoundEntry>& Pair : Data->Sounds)
    {
        if (SoundMap.Contains(Pair.Key))
        {
            UE_LOG(LogTemp, Warning, TEXT("Duplicate audio tag: %s"), *Pair.Key.ToString());
        }
        SoundMap.Add(Pair.Key, Pair.Value);
        Paths.Add(Pair.Value.Sound.ToSoftObjectPath());
    }

    // 非同期プリロード
    LoadHandles.Add(Data, UAssetManager::GetStreamableManager().RequestAsyncLoad(Paths));
}

void UAudioSubsystem::UnregisterAudioData(UAudioDataAsset* Data)
{
    if (!Data) return;
    for (const TPair<FGameplayTag, FSoundEntry>& Pair : Data->Sounds)
    {
        SoundMap.Remove(Pair.Key);
    }
    LoadHandles.Remove(Data);   // ハンドル解放でアンロード対象に
}

USoundBase* UAudioSubsystem::ResolveSound(const FGameplayTag& Tag, float& OutVolume) const
{
    //指定タグのデータがあったら
    if (const FSoundEntry* Entry = SoundMap.Find(Tag))
    {
        OutVolume = Entry->VolumeMultiplier; // 音量を取得
        return Entry->Sound.Get();   // ロード前ならnullptr
    }
    UE_LOG(LogTemp, Warning, TEXT("Sound not found: %s"), *Tag.ToString());
    return nullptr;
}

void UAudioSubsystem::PlaySE2D(FGameplayTag Tag)
{
    float Vol = 1.f;

    //サウンドデータを取得できたら
    if (USoundBase* S = ResolveSound(Tag, Vol))
    {
        //音を鳴らす
        UGameplayStatics::PlaySound2D(GetWorld(), S, Vol);
    }
}

void UAudioSubsystem::PlaySEAtLocation(FGameplayTag Tag, FVector Location)
{
    float Vol = 1.f;

    //サウンドデータを取得できたら
    if (USoundBase* S = ResolveSound(Tag, Vol))
    {
        //音を鳴らす
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), S, Location, Vol);
    }
}

void UAudioSubsystem::PlayBGM(FGameplayTag Tag)
{
    //再生してる曲と同じ場合
    if (Tag == CurrentBGMTag && BGMComponent && BGMComponent->IsPlaying())
    {
        return;
    }

    float Vol = 1.f;

    //サウンドデータを取得
    USoundBase* S = ResolveSound(Tag, Vol);
    //取得出来ていなかったら
    if (!S) return;

    //再生中の曲をゆっくり消す
    if (BGMComponent) { BGMComponent->FadeOut(0.5f, 0.f); }
    BGMComponent = UGameplayStatics::SpawnSound2D(GetWorld(), S, Vol);
    //再生予定の曲をゆっくりつける
    if (BGMComponent) { BGMComponent->FadeIn(0.5f, 1.f); }

    //再生中のBGMを保存
    CurrentBGMTag = Tag;
}

void UAudioSubsystem::StopBGM(float FadeOutTime)
{
    //即停止
    if (BGMComponent) { BGMComponent->FadeOut(FadeOutTime, 0.f); }
}

void UAudioSubsystem::PlaySoundDirect(USoundBase* Sound, FVector Location, float Volume)
{
    //音があった場合
    if (!Sound) return;
    //音を鳴らす
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, Location, Volume);
}

void UAudioSubsystem::PlaySoundDirect2D(USoundBase* Sound, float Volume)
{
    //音があった場合
    if (!Sound) return;
    //音を鳴らす
    UGameplayStatics::PlaySound2D(GetWorld(), Sound, Volume);
}

UAudioSubsystem* UAudioSubsystem::Get(const UObject* WorldContext)
{
    //レベルが取得出来ない場合
    if (!WorldContext) return nullptr;

    // 専用サーバーでは音を鳴らさない
    if (IsRunningDedicatedServer()) return nullptr;

    const UGameInstance* GI = UGameplayStatics::GetGameInstance(WorldContext);
    return GI ? GI->GetSubsystem<UAudioSubsystem>() : nullptr;
}