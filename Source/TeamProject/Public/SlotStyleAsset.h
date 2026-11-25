// SlotStyleAsset.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Styling/SlateBrush.h"
#include "SlotStyleAsset.generated.h"

// スロット枠の形(丸/四角など)をアセットで切り替える
UCLASS(BlueprintType)
class USlotStyleAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot")
    FSlateBrush FrameBrush;
};