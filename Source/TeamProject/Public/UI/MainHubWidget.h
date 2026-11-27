// MainHUDWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Slot/SlotSelectorTypes.h"
#include "MainHubWidget.generated.h"

class USlotSelectorWidget;

UCLASS()
class UMainHUDWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    //トラップ表示更新
    void HandleTrapDisplayChanged(const FSlotView& Prev, const FSlotView& Current, const FSlotView& Next);
    //アイテム表示更新
    void HandleItemDisplayChanged(const FSlotView& Prev, const FSlotView& Current, const FSlotView& Next);

protected:
    UPROPERTY(meta = (BindWidget)) TObjectPtr<USlotSelectorWidget> TrapSelector;

    // アイテム用を使わない間は UMG に置かなくても動く
    UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<USlotSelectorWidget> ItemSelector;
};