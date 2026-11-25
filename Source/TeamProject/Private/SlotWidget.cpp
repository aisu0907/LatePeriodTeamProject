// Fill out your copyright notice in the Description page of Project Settings.

#include "SlotWidget.h"
#include "SlotStyleAsset.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void USlotWidget::SetSlotData(const FSlotView& Data)
{
    if (Data.IsEmpty())
    {
        SetVisibility(ESlateVisibility::Hidden);   // ” ‚²‚ÆÁ‚·(êŠ‚ÍŽc‚é)
        return;
    }

    SetVisibility(ESlateVisibility::HitTestInvisible);
    IconImage->SetBrushResourceObject(Data.Icon);
}