#include "World06.h"
#include "Framework/Framework.h"
#include "Input/InputSystem.h"
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtx/color_space.hpp>

#define INTERLEAVE

namespace nc
{
    bool World06::Initialize()
    {
        m_scene = std::make_unique<Scene>();
        m_scene->Load("scenes/scene_framebuffer.json");
        m_scene->Initialize();

        auto texture = std::make_shared<Texture>();
        texture->CreateTexture(512, 512);
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

    void World06::Shutdown()
    {
    }

    void World06::Update(float dt)
    {
        ENGINE.GetSystem<Gui>()->BeginFrame();

        m_scene->Update(dt);
        m_scene->ProcessGui();

        //set postprocess gui
        ImGui::Begin("Post-Process");
        ImGui::SliderFloat("Blend", &blend, 0, 1);
        bool effect = params & INVERT_MASK;
        if (ImGui::Checkbox("Invert", &effect))
        {
            if (effect) params |= INVERT_MASK;
            else params &= ~INVERT_MASK;
        }

        effect = params & GRAYSCALE_MASK;
        if (ImGui::Checkbox("Grayscale", &effect))
        {
            if (effect) params |= GRAYSCALE_MASK;
            else params &= ~GRAYSCALE_MASK;
        }

        effect = params & COLORTINT_MASK;
        if (ImGui::Checkbox("Color Tint", &effect))
        {
            if (effect)
            {
                params |= COLORTINT_MASK;
            }
            else params &= ~COLORTINT_MASK;
        }
        ImGui::ColorEdit3("Tint", glm::value_ptr(tint_color));

        effect = params & GRAIN_MASK;
        if (ImGui::Checkbox("Grain", &effect))
        {
            if (effect) params |= GRAIN_MASK;
            else params &= ~GRAIN_MASK;
        }

        effect = params & SCANLINE_MASK;
        if (ImGui::Checkbox("Scanline", &effect))
        {
            if (effect) params |= SCANLINE_MASK;
            else params &= ~SCANLINE_MASK;
        }

        effect = params & CUSTOM_MASK;
        if (ImGui::Checkbox("Custom", &effect))
        {
            if (effect) params |= CUSTOM_MASK;
            else params &= ~CUSTOM_MASK;
        }

        ImGui::End();

        //set posprocess shader
        auto program = GET_RESOURCE(Program, "shaders/postprocess.prog");
        if (program)
        {
            program->Use();
            program->SetUniform("blend", blend);
            program->SetUniform("params", params);
            program->SetUniform("tint", tint_color);
            program->SetUniform("time", ENGINE.GetTime().GetTime());
        }

        ENGINE.GetSystem<Gui>()->EndFrame();
    }

    void World06::Draw(Renderer& renderer)
    {
        // pass 1
        m_scene->GetActorByName("postprocess")->active = false;

        auto framebuffer = GET_RESOURCE(Framebuffer, "fb");
        renderer.SetViewport(framebuffer->GetSize().x, framebuffer->GetSize().y);
        framebuffer->Bind();

        renderer.BeginFrame({ 0, 0, 1 });
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