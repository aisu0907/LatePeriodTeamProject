// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "AudioDataAsset.h"
#include "AudioSubsystem.generated.h"

/**
 * 
 */

class UAudioComponent;
class USoundBase;
struct FStreamableHandle;

UCLASS()
class TEAMPROJECT_API UAudioSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

    // ---- データ登録(ステージ開始/終了時に呼ぶ) ----
    UFUNCTION(BlueprintCallable, Category = "Audio")
    void RegisterAudioData(UAudioDataAsset* Data);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void UnregisterAudioData(UAudioDataAsset* Data);
	
    // ---- タグ経由(共通音) ----
    
    // レベル全体に鳴らす(共通音)
    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlaySE2D(FGameplayTag Tag);
    
    // アクターから鳴らす(共通音)
    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlaySEAtLocation(FGameplayTag Tag, FVector Location);

    // BGM再生用
    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayBGM(FGameplayTag Tag);

    // BGM停止用
    UFUNCTION(BlueprintCallable, Category = "Audio")
    void StopBGM(float FadeOutTime = 1.0f);

    // ---- 直接渡し(アクター固有音) ----
    
    // レベル全体に鳴らす(アクター固有音)
    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlaySoundDirect(USoundBase* Sound, FVector Location, float Volume = 1.f);

    // アクターから鳴らす(アクター固有音)
    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlaySoundDirect2D(USoundBase* Sound, float Volume = 1.f);

private:
    // ロード済みならSound、未ロードならnullptrを返す
    USoundBase* ResolveSound(const FGameplayTag& Tag, float& OutVolume) const;

    // マージ済みキャッシュ。再生時はここだけ見る
    TMap<FGameplayTag, FSoundEntry> SoundMap;

    // ハンドルを保持している間は音がメモリに残る
    TMap<TWeakObjectPtr<UAudioDataAsset>, TSharedPtr<FStreamableHandle>> LoadHandles;

    UPROPERTY()
    TObjectPtr<UAudioComponent> BGMComponent;

    FGameplayTag CurrentBGMTag;
};
