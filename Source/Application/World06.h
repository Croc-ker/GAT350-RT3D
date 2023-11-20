#pragma once
#include "Framework/World.h"
#include "Renderer/Renderer.h"
#include "Core/Math/Transform.h"
#include <vector>

namespace nc
{
    class World06 : public World
    {
    public:
        uint32_t INVERT_MASK = (1<<0);
        uint32_t GRAYSCALE_MASK = (1 << 1);
        uint32_t COLORTINT_MASK = (1 << 2);
        uint32_t GRAIN_MASK = (1 << 3);
        uint32_t SCANLINE_MASK = (1 << 4);
        uint32_t CUSTOM_MASK = (1 << 5);

    public:
        bool Initialize() override;
        void Shutdown() override;
        void Update(float dt) override;
        void Draw(Renderer& renderer) override;

    private:
        float m_time;
        float m_speed = 5;
        float blend{ 1 };
        float grainIntensity{ 1.0f };
        float scanlineIntensity{ 1.0f };
        float customIntensity{ 1.0f };
        float m_refraction = 3;

        uint32_t params{ 0 };

        glm::vec3 m_ambientColor{ 0.2f }; 
        glm::vec3 tint_color{ 0.2f };

        bool m_guiInitialized = false;

        float shininess = 32.0f;  // Shininess property

    };
}
