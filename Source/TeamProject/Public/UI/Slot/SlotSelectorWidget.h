// SlotSelectorWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlotSelectorTypes.h"
#include "SlotSelectorWidget.generated.h"

class USlotWidget;
class USlotStyleAsset;

UCLASS()
class USlotSelectorWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    //スロットデータセット用
    void SetDisplay(const FSlotView& Prev,
        const FSlotView& Current,
        const FSlotView& Next);

protected:
    virtual void NativeConstruct() override;

    //スロットスタイルを設定
    UPROPERTY(EditAnywhere, Category = "Style")
    TObjectPtr<USlotStyleAsset> SlotStyle;

    //前のトラップ
    UPROPERTY(meta = (BindWidget)) TObjectPtr<USlotWidget> PrevSlot;
    //選択中のトラップ
    UPROPERTY(meta = (BindWidget)) TObjectPtr<USlotWidget> CurrentSlot;
    //次のトラップ
    UPROPERTY(meta = (BindWidget)) TObjectPtr<USlotWidget> NextSlot;
};