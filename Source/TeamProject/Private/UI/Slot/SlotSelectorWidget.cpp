// SlotSelectorWidget.cpp
#include "UI/Slot/SlotSelectorWidget.h"
#include "UI/Slot/SlotWidget.h"
#include "UI/Slot/SlotStyleAsset.h"

void USlotSelectorWidget::NativeConstruct()
{
    Super::NativeConstruct();

    //スタイル設定
    PrevSlot->ApplyStyle(SlotStyle);
    CurrentSlot->ApplyStyle(SlotStyle);
    NextSlot->ApplyStyle(SlotStyle);
}

void USlotSelectorWidget::SetDisplay(const FSlotView& Prev,
    const FSlotView& Current,
    const FSlotView& Next)
{
    //表示データ設定
    PrevSlot->SetSlotData(Prev);
    CurrentSlot->SetSlotData(Current);
    NextSlot->SetSlotData(Next);
}