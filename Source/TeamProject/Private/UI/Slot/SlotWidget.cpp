// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Slot/SlotWidget.h"
#include "UI/Slot/SlotStyleAsset.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void USlotWidget::SetSlotData(const FSlotView& Data)
{
    if (Data.IsEmpty())
    {
        SetVisibility(ESlateVisibility::Hidden);   // 箱ごと消す(場所は残る)
        return;
    }

    SetVisibility(ESlateVisibility::HitTestInvisible);
    IconImage->SetBrushResourceObject(Data.Icon); //表示するデータセット
}

void USlotWidget::ApplyStyle(const USlotStyleAsset* Style)
{
    if (!Style) return;
    FrameImage->SetBrush(Style->FrameBrush); //スタイルセット
}