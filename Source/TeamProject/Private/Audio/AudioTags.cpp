// Private/Audio/AudioTags.cpp
#include "Audio/AudioTags.h"

namespace AudioTags
{
#pragma region BGM

#pragma region BGM_Title
#pragma endregion

#pragma region BGM_Stage
    UE_DEFINE_GAMEPLAY_TAG(BGM_Stage1,    "Audio.BGM.Stage1")
    UE_DEFINE_GAMEPLAY_TAG(BGM_TrapPhase, "Audio.BGM.TrapPhase")
#pragma endregion

#pragma region BGM_Result
#pragma endregion

#pragma endregion // BGM

#pragma region SE

#pragma region SE_Trap
#pragma endregion

#pragma region SE_Player
#pragma endregion

#pragma region SE_Item
#pragma endregion

#pragma region SE_UI
    UE_DEFINE_GAMEPLAY_TAG(SE_UI_Click, "Audio.SE.UI.Click")
#pragma endregion

#pragma endregion // SE
}