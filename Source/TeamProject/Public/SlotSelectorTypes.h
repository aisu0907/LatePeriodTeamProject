// SlotSelectorTypes.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture.h"
#include "SlotSelectorTypes.generated.h"   // 必ず最後のinclude

// UIに渡す表示専用データ(1ブロック分)。トラップでもアイテムでも共通で使う
USTRUCT(BlueprintType)
struct FSlotView
{
    GENERATED_BODY()

    // Texture2D も RenderTarget も入れられるよう UTexture にしている
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UTexture> Icon = nullptr;

    //空チェック
    bool IsEmpty() const { return Icon == nullptr; }
};