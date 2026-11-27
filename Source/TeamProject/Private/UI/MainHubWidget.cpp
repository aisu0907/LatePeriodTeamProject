// MainHUDWidget.cpp
#include "UI/MainHubWidget.h"
#include "UI/Slot/SlotSelectorWidget.h"

void UMainHUDWidget::HandleTrapDisplayChanged(const FSlotView& Prev, const FSlotView& Current, const FSlotView& Next)
{
    TrapSelector->SetDisplay(Prev, Current, Next);
}

void UMainHUDWidget::HandleItemDisplayChanged(const FSlotView& Prev, const FSlotView& Current, const FSlotView& Next)
{
    if (ItemSelector)
    {
        ItemSelector->SetDisplay(Prev, Current, Next);
    }
}