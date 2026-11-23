// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AudioDataAsset.generated.h"

class USoundBase;

USTRUCT(BlueprintType)
struct FSoundEntry
{
    GENERATED_BODY()

    //音
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<USoundBase> Sound;

    //音量(最小値0.0, 最大値2.0)
    UPROPERTY(EditAnywhere, BlueprintReadOnly,
        meta = (ClampMin = "0.0", ClampMax = "2.0",
                   UIMin = "0.0",    UIMax = "2.0"))   
    float VolumeMultiplier = 1.f;
};

/**
 * 
 */
UCLASS()
class TEAMPROJECT_API UAudioDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

    // Tagがキー。同一アセット内でのタグ重複を構造的に防ぐ
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ForceInlineRow, Categories = "Audio"))
    TMap<FGameplayTag, FSoundEntry> Sounds;
};
