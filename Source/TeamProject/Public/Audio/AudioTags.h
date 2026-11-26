#pragma once
#include "NativeGameplayTags.h"

namespace AudioTags
{
#pragma region BGM

#pragma region BGM_Title
#pragma endregion

#pragma region BGM_Stage
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(BGM_Stage1);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(BGM_TrapPhase);
#pragma endregion

#pragma region BGM_Result
#pragma endregion

#pragma endregion BGM

#pragma region SE

#pragma region SE_Trap
#pragma endregion

#pragma region SE_Player
#pragma endregion

#pragma region SE_Item
#pragma endregion

#pragma region SE_UI
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(SE_UI_Click);
#pragma endregion

#pragma endregion SE
}