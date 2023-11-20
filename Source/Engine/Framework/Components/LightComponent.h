#pragma once
#include "Component.h"
#include "Renderer/Program.h"

namespace nc
{
	class LightComponent : public Component
	{
	public:
		CLASS_DECLARATION(LightComponent)

		bool Initialize() override;
		void Update(float dt) override;
		void SetProgram(const res_t<Program> program, const std::string& name);

		void ProcessGui() override;

		glm::mat4 GetShadowMatrix();

	public:
		enum eType
		{
			Point,
			Directional,
			Spot
		};

		eType type = eType::Point;
		glm::vec3 color{ 1 };
		float intensity = 1;
		float range = 20;
		float innerangle = 20;
		float outerangle = 30;

		bool castShadow = false;
		float shadowSize = 10;
		float shadowBias = 0.005f;
		bool celShade = false;
		int celShading = 4;
		float specCutoff = 4.0f;
		float Outline = 4.0f;

	};
}
