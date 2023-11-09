#include "World05.h"
#include "Framework/Framework.h"
#include "Input/InputSystem.h"
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtx/color_space.hpp>

#define INTERLEAVE

namespace nc
{
    bool World05::Initialize()
    {
        m_scene = std::make_unique<Scene>();
        m_scene->Load("scenes/scene.json");
        m_scene->Initialize();

        {
            auto actor = CREATE_CLASS(Actor);
            actor->name = "camera1";
            actor->transform.position = glm::vec3{ 0, 0, 3 };
            actor->transform.rotation = glm::radians(glm::vec3{ 0, 180, 0 });

            auto cameraComponent = CREATE_CLASS(CameraComponent);
            cameraComponent->SetPerspective(70.0f, ENGINE.GetSystem<Renderer>()->GetWidth() / (float)ENGINE.GetSystem<Renderer>()->GetHeight(), 0.1f, 100.0f);
            actor->AddComponent(std::move(cameraComponent));

            auto cameraController = CREATE_CLASS(CameraController);
            cameraController->speed = 5;
            cameraController->sensitivity = 0.5f;
            cameraController->m_owner = actor.get();
            cameraController->Initialize();
            actor->AddComponent(std::move(cameraController));

            m_scene->Add(std::move(actor));

            for (int i = 0; i < 100; i++) {
                auto actor = CREATE_CLASS_BASE(Actor, "tree");
                actor->name = nc::StringUtils::CreateUnique("tree");
                actor->transform.position = glm::vec3{ randomf(-10, 10), 0, randomf(-10, 10) };
                actor->transform.scale = glm::vec3{ randomf(0.5f, 2.0f), randomf(0.5f, 2.0f) ,1 };
                actor->Initialize();
                m_scene->Add(std::move(actor));
            }
        }

        

        return true;
    }
    
    void World05::Shutdown()
    {
    }

    void World05::Update(float dt)
    {
        ENGINE.GetSystem<Gui>()->BeginFrame();

        m_scene->Update(dt);
        m_scene->ProcessGui();

        auto actor = m_scene->GetActorByName<Actor>("ogre");

        auto material = actor->GetComponent<ModelComponent>()->material;
        material->ProcessGui();
        material->Bind();

        material = GET_RESOURCE(Material, "materials/refraction.mtrl");
        if (material) {
            ImGui::Begin("Refraction");
            
            //neat effect
            //m_refraction = 1 + std::fabs(std::sin(m_time));

            ImGui::DragFloat("IOR", &m_refraction, 0.01f, 1, 3);
            auto program = material->GetProgram();
            program->Use();
            program->SetUniform("ior", m_refraction);
            
            ImGui::End();
        }
        
        ENGINE.GetSystem<Gui>()->EndFrame();
    }

    void World05::Draw(Renderer& renderer)
    {
        // pre-render
        renderer.BeginFrame();

       
        // render
        
        //will only draw a wire mesh
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        //will draw the full model
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        m_scene->Draw(renderer);

        ENGINE.GetSystem<Gui>()->Draw();


        // post-render
        renderer.EndFrame();
    }
}
