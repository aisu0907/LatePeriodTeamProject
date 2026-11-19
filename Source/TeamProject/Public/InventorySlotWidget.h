// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotWidget.generated.h"

class UImage;
class UTextBlock;
class USlotStyleAsset;

/**
 * 
 */
UCLASS()
class TEAMPROJECT_API UInventorySlotWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    // 中身(アイコンと個数)を反映
    //void SetSlotData(const FHotbarSlotView& Data);

    // 枠の形を反映(最初に1回)
    void ApplyStyle(const USlotStyleAsset* Style);

protected:
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UImage> FrameImage;
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UImage> IconImage;
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> QuantityText;
};
