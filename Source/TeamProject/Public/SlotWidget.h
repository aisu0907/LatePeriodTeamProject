// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlotSelectorTypes.h"
#include "SlotWidget.generated.h"

class UImage;
class UTextBlock;
class USlotStyleAsset;

UCLASS()
class TEAMPROJECT_API USlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	//スロットデータセット用
	void SetSlotData(const FSlotView& Data);
	//アイコンスタイルセット用
	void ApplyStyle(const USlotStyleAsset* Style);

protected:
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UImage> FrameImage;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UImage> IconImage;
};
