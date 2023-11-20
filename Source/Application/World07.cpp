#include "World07.h"
#include "Framework/Framework.h"
#include "Input/InputSystem.h"
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtx/color_space.hpp>


#define INTERLEAVE

namespace nc
{
    bool World07::Initialize()
    {


        m_scene = std::make_unique<Scene>();
        m_scene->Load("scenes/scene_framebuffer.json");
        m_scene->Initialize();
        m_editor = std::make_unique<Editor>();

        //create depth texture
        auto depthTexture = std::make_shared<Texture>();
        depthTexture->CreateDepthTexture(1024, 1024);
        ADD_RESOURCE("depth_texture", depthTexture);

        //create framebuffer
        auto texture = std::make_shared<Texture>();
        texture->CreateTexture(1024, 1024);
        ADD_RESOURCE("fb_texture", texture);

        auto framebuffer = std::make_shared<Framebuffer>();
        framebuffer->CreateFramebuffer(texture);
        ADD_RESOURCE("fb", framebuffer);
        
        auto material = GET_RESOURCE(Material, "materials/postprocess.mtrl");
        if (material) {
            material->albedoTexture = texture;
        }

        return true;
    }

    void World07::Shutdown()
    {
    }

    void World07::Update(float dt)
    {
        ENGINE.GetSystem<Gui>()->BeginFrame();

        m_scene->Update(dt);
        m_scene->ProcessGui();

        m_editor->ProcessGui(m_scene.get());

        //set postprocess gui
        ImGui::Begin("Post-Process");
        ImGui::SliderFloat("Blend", &m_blend, 0, 1);
        bool effect = m_params & INVERT_MASK;
        if (ImGui::Checkbox("Invert", &effect))
        {
            if (effect) m_params |= INVERT_MASK;
            else m_params &= ~INVERT_MASK;
        }

        effect = m_params & GRAYSCALE_MASK;
        if (ImGui::Checkbox("Grayscale", &effect))
        {
            if (effect) m_params |= GRAYSCALE_MASK;
            else m_params &= ~GRAYSCALE_MASK;
        }

        effect = m_params & COLORTINT_MASK;
        if (ImGui::Checkbox("Color Tint", &effect))
        {
            if (effect)
            {
                m_params |= COLORTINT_MASK;
            }
            else m_params &= ~COLORTINT_MASK;
        }
        ImGui::ColorEdit3("Tint", glm::value_ptr(tint_color));

        effect = m_params & GRAIN_MASK;
        if (ImGui::Checkbox("Grain", &effect))
        {
            if (effect) m_params |= GRAIN_MASK;
            else m_params &= ~GRAIN_MASK;
        }

        effect = m_params & SCANLINE_MASK;
        if (ImGui::Checkbox("Scanline", &effect))
        {
            if (effect) m_params |= SCANLINE_MASK;
            else m_params &= ~SCANLINE_MASK;
        }

        effect = m_params & CUSTOM_MASK;
        if (ImGui::Checkbox("Custom", &effect))
        {
            if (effect) m_params |= CUSTOM_MASK;
            else m_params &= ~CUSTOM_MASK;
        }

        ImGui::End();

        //set posprocess shader
        auto program = GET_RESOURCE(Program, "shaders/postprocess.prog");
        if (program)
        {
            program->Use();
            program->SetUniform("blend", m_blend);
            program->SetUniform("params", m_params);
            program->SetUniform("tint", tint_color);
            program->SetUniform("time", ENGINE.GetTime().GetTime());
        }

        ENGINE.GetSystem<Gui>()->EndFrame();
    }

    void World07::Draw(Renderer& renderer)
    {
        // pass 1
        m_scene->GetActorByName("postprocess")->active = false;

        auto framebuffer = GET_RESOURCE(Framebuffer, "fb");
        renderer.SetViewport(framebuffer->GetSize().x, framebuffer->GetSize().y);
        framebuffer->Bind();

        renderer.BeginFrame({ 0, 0, 1});
        m_scene->Draw(renderer);

        framebuffer->Unbind();

        // pass 2
        m_scene->GetActorByName("postprocess")->active = true;

        renderer.ResetViewport();
        renderer.BeginFrame();
        m_scene->GetActorByName("postprocess")->Draw(renderer);
        //m_scene->Draw(renderer);

        // post-render
        ENGINE.GetSystem<Gui>()->Draw();
        renderer.EndFrame();
    }
}