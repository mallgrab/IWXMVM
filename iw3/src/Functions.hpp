#pragma once
#include "Structures.hpp"

namespace IWXMVM::IW3::Functions
{
    Structures::dvar_s* FindDvar(const std::string_view name);

    std::string GetFilePath(const std::string_view demoName);

    void Cbuf_AddText(std::string command);

    uint16_t SL_GetStringOfSize(const char* string, int entityType, int stringLength);

    bool CG_DObjGetWorldBoneMatrix(Structures::centity_s* entity /*@<eax>*/, int boneIndex /*@<ecx>*/,
                                        float* matrix /*@<esi>*/, Structures::DObj_s* dobj, float* origin);

    void Dvar_SetStringByName(const char* dvarName, const char* value);
}  // namespace IWXMVM::IW3::Functions
