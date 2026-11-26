// Fill out your copyright notice in the Description page of Project Settings.
//AudioHelper.h

#pragma once

#include "GameplayTagContainer.h"
#include "Audio/AudioTags.h"

class USoundBase;

namespace Audio
{
    // レベル全体に鳴らす(共通音)
    void PlaySE2D(const UObject* WorldContext, FGameplayTag Tag);
    // アクターから鳴らす(共通音)
    void PlaySEAtLocation(const UObject* WorldContext, FGameplayTag Tag, FVector Location);
    // BGM再生用
    void PlayBGM(const UObject* WorldContext, FGameplayTag Tag);
    // BGM停止用
    void StopBGM(const UObject* WorldContext, float FadeOutTime = 1.0f);
    // レベル全体に鳴らす(アクター固有音)
    void PlaySoundDirect(const UObject* WorldContext, USoundBase* Sound, FVector Location, float Volume = 1.f);
    // アクターから鳴らす(アクター固有音)
    void PlaySoundDirect2D(const UObject* WorldContext, USoundBase* Sound, float Volume = 1.f);
}
