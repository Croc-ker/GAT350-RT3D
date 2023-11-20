#include "CameraComponent.h"
#include "Framework/Actor.h"
#include "Framework/Engine.h"
#include "Renderer/Program.h"
#include <glm/glm/gtc/matrix_transform.hpp>

namespace nc {
    CLASS_DEFINITION(CameraComponent)

        bool CameraComponent::Initialize() {
        // Get aspect from window if not set
        if (aspect == 0) {
            // Set aspect with renderer width / renderer height (make sure it is a floating point division)
            aspect = static_cast<float>(ENGINE.GetSystem<Renderer>()->GetWidth()) / static_cast<float>(ENGINE.GetSystem<Renderer>()->GetHeight());
        }
        return true;
    }

    void CameraComponent::Update(float dt) {
        // Set view matrix with glm::lookAt function, use owner position
        view = glm::lookAt(m_owner->transform.position, m_owner->transform.position + m_owner->transform.Forward(), m_owner->transform.Up());
        if (projectionType == perspective)
        {
           projection = glm::perspective(glm::radians(fov), aspect, near, far);
        }
        else
        {
            projection = glm::ortho(-size *aspect*  0.5f, size * aspect * 0.5f, -size * 0.5f, size * 0.5f, near, far);
        }
       


    }

    void CameraComponent::SetPerspective(float fov, float aspect, float near, float far) {
        this->fov = fov;
        this->aspect = aspect;
        this->near = near;
        this->far = far;

        // Set projection matrix with glm::perspective function (fov is in degrees, convert to radians)
        projection = glm::perspective(glm::radians(fov), aspect, near, far);
    }

    void CameraComponent::SetLookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up) {
        view = glm::lookAt(eye, center, up);
    }

    void CameraComponent::SetProgram(res_t<Program> program) {
        // Set program uniform for "view" with view matrix
        program->SetUniform("view", view);
        // Set program uniform for "projection" with projection matrix
        program->SetUniform("projection", projection);
    }

    void CameraComponent::ProcessGui() 
    {
        const char* types[] = { "perspective", "orthographic" };
        ImGui::Combo("Projection", (int*)(&projectionType), types, 2);

        // Use ImGui::DragFloat to set fov, aspect, near, and far values (use speed of 0.1f)
        ImGui::DragFloat("FOV", &fov, 0.1f);
        ImGui::DragFloat("Aspect Ratio", &aspect, 0.1f);
        ImGui::DragFloat("Near Clip", &near, 0.1f);
        ImGui::DragFloat("Far Clip", &far, 0.1f);
    }

    void CameraComponent::Read(const json_t& value) {
        // READ_DATA of fov, aspect, near, and far values
        READ_DATA(value, fov);
        READ_DATA(value, aspect);
        READ_DATA(value, near);
        READ_DATA(value, far);

        std::string projectionTypeName;
        READ_NAME_DATA(value, "projectionType", projectionTypeName);
        if (nc::StringUtils::IsEqualIgnoreCase("orthographic", projectionTypeName)) projectionType = orthographic;
        READ_DATA(value, size);
    }
}
