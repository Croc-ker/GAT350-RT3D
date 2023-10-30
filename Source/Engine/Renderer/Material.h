#pragma once
#include "Framework/Resource/Resource.h"
#include <glm/glm/glm.hpp>
#include <vector>
#include <memory>

namespace nc
{
	class Texture;
	class Program;

	class Material : public Resource
	{
	public:
		virtual bool Create(std::string filename, ...) override;

		void Bind();

		res_t<Program> GetProgram() { return m_program; }
		void ProcessGui();

	public:
		glm::vec3 albedo{ 1 };
		glm::vec3 specular{ 1 };
		glm::vec3 emissive{ 0 };
		float shininess = 2;

		glm::vec2 tiling{ 1, 1 };
		glm::vec2 offset{ 0, 0 };

		res_t<Texture> albedoTexture;
		res_t<Texture> specularTexture;
		res_t<Texture> normalTexture;
		res_t<Texture> emissiveTexture;

	private:
		res_t<Program> m_program;
		std::vector<res_t<Texture>> m_textures;
	};
}