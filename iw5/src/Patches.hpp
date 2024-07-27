#pragma once
#include "StdInclude.hpp"
#include "Addresses.hpp"
#include "../src/Utilities/Patches.hpp"

namespace IWXMVM::IW5::Patches
{
    using namespace IWXMVM::Patches;

    struct IW5Patches
    {
        ReturnPatch CL_Demo_HandleInput{GetGameAddresses().CL_Demo_HandleInput(), PatchApplySetting::Immediately};
        ReturnPatch CL_Demo_FreeCameraMove{GetGameAddresses().CL_Demo_FreeCameraMove(), PatchApplySetting::Immediately};
        ReturnPatch CL_Demo_FollowCameraMove{GetGameAddresses().CL_Demo_FollowCameraMove(),
                                             PatchApplySetting::Immediately};

        NopPatch<5> Con_TimeJumped{GetGameAddresses().Con_TimeJumped_Call(), PatchApplySetting::Deferred};

        ReturnPatch IN_Frame{GetGameAddresses().IN_Frame(), PatchApplySetting::Deferred};
    };

    inline IW5Patches& GetGamePatches()
    {
        static IW5Patches patches;
        return patches;
    }
}  // namespace IWXMVM::IW5::Patches
