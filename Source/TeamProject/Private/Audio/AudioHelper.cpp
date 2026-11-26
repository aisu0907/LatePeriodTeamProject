// Fill out your copyright notice in the Description page of Project Settings.
// AudioHelper.cpp

#include "Audio/AudioHelper.h"

namespace Audio
{
    void PlaySE2D(const UObject* WorldContext, FGameplayTag Tag)
    {
        if (UAudioSubsystem* Subsystem = UAudioSubsystem:: Get(WorldContext))
        {
            Subsystem->PlaySE2D(Tag);
        }
    }

    void PlaySEAtLocation(const UObject* WorldContext, FGameplayTag Tag, FVector Location)
    {
        if (UAudioSubsystem* Subsystem = UAudioSubsystem::Get(WorldContext))
        {
            Subsystem->PlaySEAtLocation(Tag, Location);
        }
    }

    void PlayBGM(const UObject* WorldContext, FGameplayTag Tag)
    {
        if (UAudioSubsystem* Subsystem = UAudioSubsystem::Get(WorldContext))
        {
            Subsystem->PlayBGM(Tag);
        }
    }

    void StopBGM(const UObject* WorldContext, float FadeOutTime)
    {
        if (UAudioSubsystem* Subsystem = UAudioSubsystem::Get(WorldContext))
        {
            Subsystem->StopBGM(FadeOutTime);
        }
    }

    void PlaySoundDirect(const UObject* WorldContext, USoundBase* Sound, FVector Location, float Volume)
    {
        if (UAudioSubsystem* Subsystem = UAudioSubsystem::Get(WorldContext))
        {
            Subsystem->PlaySoundDirect(Sound, Location, Volume);
        }
    }

    void PlaySoundDirect2D(const UObject* WorldContext, USoundBase* Sound, float Volume)
    {
        if (UAudioSubsystem* Subsystem = UAudioSubsystem::Get(WorldContext))
        {
            Subsystem->PlaySoundDirect2D(Sound, Volume);
        }
    }
}