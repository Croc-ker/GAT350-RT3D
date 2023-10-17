#pragma once
#include "Framework/World.h"
#include "Renderer/Renderer.h"
#include <vector>
#include <Renderer/VertexBuffer.h>
namespace nc
{
	class World03 : public World
	{
	public:
		bool Initialize() override;
		void Shutdown() override;
		void Update(float dt) override;
		void Draw(Renderer& renderer) override;

	private:
		float m_time;
		int m_speed = 10;

		Transform m_transform;

		//GLuint m_vao;
		res_t<Program> m_program;
		res_t<Texture> m_texture;
		res_t<Material> m_material;
		res_t<VertexBuffer> m_vertexBuffer;
	};
}