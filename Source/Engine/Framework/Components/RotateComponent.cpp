#include "RotateComponent.h"
#include "Framework/Actor.h"

namespace nc {

	CLASS_DEFINITION(RotateComponent)


	void RotateComponent::ProcessGui()
	{
		ImGui::DragFloat3("Rotate", glm::value_ptr(euler));
	}

	void RotateComponent::Update(float dt)
	{
		glm::quat rotation = EulerToQuaternion(euler * dt);
		m_owner->transform.rotation = m_owner->transform.rotation * rotation;
	}

	void RotateComponent::Read(const json_t& value) 
	{
		READ_DATA(value, euler);
	}
}