#pragma once
#include "Component.h"
namespace nc
{
	class RotateComponent : public Component
	{
	public:
		CLASS_DECLARATION(RotateComponent)
		void Update(float dt);
	private:
		glm::vec3 m_euler{ 0 };
	};
}