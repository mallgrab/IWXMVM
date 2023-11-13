#pragma once
#include "UI/UIComponent.hpp"

namespace IWXMVM::UI
{
    class ControlBar : public UIComponent
    {
       public:
        void Render() final;
        void Release() final;

        bool DrawDemoProgressBar(int32_t* currentTick, uint32_t displayStartTick, uint32_t displayEndTick,
                                 uint32_t startTick, uint32_t endTick);

       private:
        void Initialize() final;
    };
}  // namespace IWXMVM::UI