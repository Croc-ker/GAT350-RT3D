#include "RotateComponent.h"
#include "Framework/Actor.h"


namespace nc
{
	CLASS_DEFINITION(RotateComponent)

	void RotateComponent::Update(float dt)
	{
		glm::quat rotation = EulerToQuaternion(m_euler * dt);
		m_owner->transform.rotation = m_owner->transform.rotation * rotation;
	}
	void RotateComponent::Read(const json_t& value)
	{
		READ_DATA(value, m_euler);
	}

}