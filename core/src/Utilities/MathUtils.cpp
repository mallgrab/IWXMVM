#include "StdInclude.hpp"
#include "MathUtils.hpp"

#include "UI/UIManager.hpp"

namespace IWXMVM::MathUtils
{
    glm::vec3 ForwardVectorFromAngles(glm::vec3 eulerAngles)
    {
        auto p = glm::radians(eulerAngles[0]);
        auto y = glm::radians(eulerAngles[1]);
        return glm::vec3(std::cos(y) * std::cos(p), std::sin(y) * std::cos(p), -std::sin(p));
    }

    glm::vec3 AnglesFromForwardVector(glm::vec3 forward)
    {
        auto yaw = std::atan2(forward.y, forward.x);
        auto pitch = std::atan2(-forward.z, std::sqrt(forward.x * forward.x + forward.y * forward.y));
        return glm::vec3(glm::degrees(pitch), glm::degrees(yaw), 0.0f);
    }

    constexpr int32_t PITCH = 0;
    constexpr int32_t YAW = 1;
    constexpr int32_t ROLL = 2;

    glm::mat3x3 AnglesToAxis(glm::vec3 angles)
    {
        float sy = sin(glm::radians(angles[YAW]));
        float cy = cos(glm::radians(angles[YAW]));

        float sp = sin(glm::radians(angles[PITCH]));
        float cp = cos(glm::radians(angles[PITCH]));

        float sr = sin(glm::radians(angles[ROLL]));
        float cr = cos(glm::radians(angles[ROLL]));

        glm::vec3 forward(cp * cy, cp * sy, -sp);
        glm::vec3 right(
            (-1 * sr * sp * cy + -1 * cr * -sy), 
            (-1 * sr * sp * sy + -1 * cr * cy), 
            -1 * sr * cp
        );
        glm::vec3 up(
            (cr * sp * cy + -sr * -sy), 
            (cr * sp * sy + -sr * cy),
            cr * cp
        );

        return glm::mat3x3(forward, glm::vector3::zero - right, up);
	}

    std::optional<ImVec2> WorldToScreenPoint(glm::vec3 point, Components::Camera& camera)
    {
        auto& gameView = UI::UIManager::Get().GetUIComponent(UI::Component::GameView);
        auto viewport = glm::vec4(gameView->GetPosition().x, gameView->GetPosition().y,
                                  gameView->GetPosition().x + gameView->GetSize().x,
                                  gameView->GetPosition().y + gameView->GetSize().y);

        auto lookat =
            glm::lookAtLH(camera.GetPosition(), camera.GetPosition() + camera.GetForwardVector(), glm::vector3::up);

        // this is quite the magic number, but it seems to be the scaling factor necessary to line this up with the
        // previous world to screen implementation
        const auto MAGIC_NUMBER = 0.65f;
        auto proj = glm::project(
            point, (lookat),
            glm::perspectiveFov(glm::radians(camera.GetFov() * 0.65f), viewport.z, viewport.w, 0.1f, 1000.0f),
            viewport);

        return std::make_optional(ImVec2(proj.x, proj.y));
    }

    // Copyright (c) by NUMERICAL RECIPES IN C: THE ART OF SCIENTIFIC COMPUTING (ISBN 0-521-43108-5)
    // Modified. Thank you to dtugend for finding this!
    float MathUtils::InterpolateCubicSpline(const std::vector<Types::Keyframe>& keyframes, uint32_t valueIndex, float tick)
    {
        const size_t n = keyframes.size();
        if (n < 2)
            throw std::exception("Not enough keyframes to interpolate");

        constexpr int32_t MAX_NODES = 256;
        if (keyframes.size() > MAX_NODES)
        {
            LOG_WARN("Exceeded maximum number of keyframes ({})", MAX_NODES);
            return keyframes.back().value.GetByIndex(valueIndex);
        }

        float ticks[MAX_NODES];
        float values[MAX_NODES];
        for (size_t i = 0; i < n; i++)
        {
            ticks[i] = static_cast<float>(keyframes[i].tick);
            values[i] = keyframes[i].value.GetByIndex(valueIndex);
        }

        float y2[MAX_NODES];  // second derivatives
        float u[MAX_NODES];

        y2[0] = -0.5f;
        u[0] = (3.0f / (ticks[1] - ticks[0])) * ((values[1] - values[0]) / (ticks[1] - ticks[0]));

        for (size_t i = 1; i <= n - 2; i++)
        {
            const auto prevTick = ticks[i - 1];
            const auto prevValue = values[i - 1];
            const auto currTick = ticks[i];
            const auto currValue = values[i];
            const auto nextTick = ticks[i + 1];
            const auto nextValue = values[i + 1];

            auto sig = (currTick - prevTick) / (nextTick - prevTick);
            auto p = sig * y2[i - 1] + 2.0f;
            y2[i] = (sig - 1.0f) / p;
            u[i] = (nextValue - currValue) / (nextTick - currTick) - (currValue - prevValue) / (currTick - prevTick);
            u[i] = (6.0f * u[i] / (nextTick - prevTick) - sig * u[i - 1]) / p;
        }

        auto qn = 0.5f;
        auto un = (3.0f / (ticks[n - 1] - ticks[n - 2])) *
                  (0.0f - (values[n - 1] - values[n - 2]) / (ticks[n - 1] - ticks[n - 2]));

        y2[n - 1] = (un - qn * u[n - 2]) / (qn * y2[n - 2] + 1.0f);

        for (int k = n - 2; k >= 0; k--)
            y2[k] = y2[k] * y2[k + 1] + u[k];

        int klo = 0;
        int khi = n - 1;
        while (khi - klo > 1)
        {
            int k = (khi + klo) >> 1;
            if (ticks[k] > tick)
                khi = k;
            else
                klo = k;
        }
        auto h = ticks[khi] - ticks[klo];
        auto a = (ticks[khi] - tick) / h;
        auto b = (tick - ticks[klo]) / h;
        return a * values[klo] + b * values[khi] +
               ((a * a * a - a) * y2[klo] + (b * b * b - b) * y2[khi]) * (h * h) / 6.0f;
    }

}  // namespace IWXMVM::MathUtils