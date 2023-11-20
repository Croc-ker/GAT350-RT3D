#include "LightComponent.h"
#include "Framework/Actor.h"
#include "Core/Json.h"

namespace nc
{
	CLASS_DEFINITION(LightComponent)

	bool LightComponent::Initialize()
	{
		return true;
	}

	void LightComponent::Update(float dt)
	{
	}

	void LightComponent::SetProgram(const res_t<Program> program, const std::string& name)
	{
		program->SetUniform(name + ".type", type);
		program->SetUniform(name + ".position", m_owner->transform.position);
		program->SetUniform(name + ".direction", m_owner->transform.Forward());
		program->SetUniform(name + ".color", color);
		program->SetUniform(name + ".intensity", intensity);
		program->SetUniform(name + ".range", range);
		program->SetUniform(name + ".innerAngle", glm::radians(innerangle));
		program->SetUniform(name + ".outerAngle", glm::radians(outerangle));

		if (castShadow)
		{
			glm::mat4 bias = glm::mat4(
				glm::vec4(0.5f, 0.0f, 0.0f, 0.0f),
				glm::vec4(0.0f, 0.5f, 0.0f, 0.0f),
				glm::vec4(0.0f, 0.0f, 0.5f, 0.0f),
				glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

			program->SetUniform("shadowVP", bias * GetShadowMatrix());
			program->SetUniform("shadowBias", shadowBias);
			program->SetUniform("shadingLevels", celShading);
		}

		if (celShade)
		{
			program->SetUniform("shadingLevels", celShading);
			program->SetUniform("celSpecularCutoff", specCutoff);
			program->SetUniform("celOutline", Outline);
		}
	}

	void LightComponent::ProcessGui()
	{
		const char* types[] = { "Point", "Directional", "Spot" };
		ImGui::Combo("Type", (int*)(&type), types, 3);

		if (type == Spot)
		{
			ImGui::DragFloat("Inner Angle", &innerangle, 1, 0, outerangle);
			ImGui::DragFloat("Outer Angle", &outerangle, 1, innerangle, 90);
		}

		ImGui::ColorEdit3("Color", glm::value_ptr(color));
		ImGui::DragFloat("Intensity", &intensity, 0.1f, 0, 10);
		if (type != Directional) ImGui::DragFloat("Range", &range, 0.1f, 0.1f, 50);

		ImGui::Checkbox("Cast Shadow", &castShadow);
		if (castShadow)
		{
			ImGui::DragFloat("Shadow Size", &shadowSize, 0.1f, 1, 60);
			ImGui::DragFloat("Shadow Dias", &shadowBias, 0.001f, 0, 0.5f);
		}
		ImGui::Checkbox("Cel Shader", &celShade);
		if (celShade)
		{
			ImGui::SliderInt("Shading Threshold", &celShading, 0, 1);
			ImGui::SliderFloat("Specular Cutoff", &specCutoff, 0.0f, 1.0f);
			ImGui::SliderFloat("outline", &Outline, 0.0f, 1.0f);
		}
	}

	glm::mat4 LightComponent::GetShadowMatrix()
	{
		glm::mat4 projection = glm::ortho(-shadowSize * 0.5f, shadowSize * 0.5f, -shadowSize * 0.5f, shadowSize * 0.5f, 0.1f, 50.0f);
		glm::mat4 view = glm::lookAt(m_owner->transform.position, m_owner->transform.position + m_owner->transform.Forward(), glm::vec3{ 0,1,0 });


		return projection * view;
	}

	void LightComponent::Read(const nc::json_t& value)
	{
		std::string lightTypeName;
		READ_NAME_DATA(value, "lightType", lightTypeName);
		if (nc::StringUtils::IsEqualIgnoreCase(lightTypeName, "point")) type = LightComponent::eType::Point;
		if (nc::StringUtils::IsEqualIgnoreCase(lightTypeName, "directional")) type = LightComponent::eType::Directional;
		if (nc::StringUtils::IsEqualIgnoreCase(lightTypeName, "spot")) type = LightComponent::eType::Spot;

		READ_DATA(value, color);
		READ_DATA(value, intensity);
		READ_DATA(value, range);
		READ_DATA(value, innerangle);
		READ_DATA(value, outerangle);
		READ_DATA(value, castShadow);


	}
}