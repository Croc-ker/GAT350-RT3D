#pragma once
#include "Framework/World.h"
#include "Core/Math/Vector2.h"
#include "Renderer/Renderer.h"
#include "Core/Math/Transform.h"
#include <vector>

#define POINT 0

namespace nc
{
	struct light_t {
		enum eType
		{
			Point,
			Directional,
			Spot
		};

		eType type;
		glm::vec3 position;
		glm::vec3 direction;
		glm::vec3 color;
		float intensity;
		float range;
		float innerAngle;
		float outerAngle;
	};

	class World04 : public World
	{
	public:
		bool Initialize() override;
		void Shutdown() override;
		void Update(float dt) override;
		void Draw(Renderer& renderer) override;

	private:
		float m_time = 0;
		float m_speed = 10.0f;


		//light_t m_light;
		light_t m_lights[3];
		int m_selected = 0;
		glm::vec3 m_aColor = glm::vec3(0.2f);
	

		res_t<VertexBuffer> m_vertexBuffer;
		res_t<Program> m_program;
		res_t<Texture> m_texture;

		Transform m_transform;
		res_t<Model> m_model;
		res_t<Material> m_material;

	};
}