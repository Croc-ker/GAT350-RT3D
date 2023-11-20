#include "World08.h"
#include "Framework/Framework.h"
#include "Input/InputSystem.h"
#include "imgui/imgui.h"
#include <glm/glm/gtc/type_ptr.hpp>
#include<glm/glm/gtx/color_space.hpp>

namespace Twili
{
	

	bool World08::Initialize()
	{
		m_scene = std::make_unique<Scene>();
		m_scene->Load("Scenes/scene_looney_tunes.json");
		m_scene->Initialize();

		auto texture = std::make_shared<Texture>();
		texture->CreateDepthTexture(1024, 1024);
		ADD_RESOURCE("depth_texture", texture);

		auto frameBuffer = std::make_shared<Framebuffer>();
		frameBuffer->CreateDepthBuffer(texture);
		ADD_RESOURCE("depth_buffer", frameBuffer);

		auto material = GET_RESOURCE(Material, "Materials/sprite.mtrl");
		if (material)
		{
			material->albedoTexture = texture;
		}

		auto materials = GET_RESOURCES(Material);
		for (auto material : materials)
		{
			material->depthTexture = texture;
		}

		return true;
	}

	void World08::Shutdown()
	{
	}

    void World08::Update(float dt) {
        m_time += dt;

        ENGINE.GetSystem<Gui>()->BeginFrame();

        m_scene->Update(dt);
        m_scene->ProcessGui();

		ENGINE.GetSystem<Gui>()->EndFrame();

	}

	void World08::Draw(Renderer& renderer)
	{
		//**PASS 1 ***
		
		auto framebuffer = GET_RESOURCE(Framebuffer,"depth_buffer");
		renderer.SetViewport(framebuffer->GetSize().x, framebuffer->GetSize().y);
		framebuffer->Bind();

		// Pre-render
		renderer.ClearDepth();
		auto program = GET_RESOURCE(Program, "shaders/shadow_depth.prog");
		program->Use();

		auto lights = m_scene->GetComponents<LightComponent>();
		for (auto light : lights) 
		{
			if (light->castShadow)
			{
				glm::mat4 shadowMatrix = light->GetShadowMatrix();
				program->SetUniform("shadowVP", shadowMatrix);
			}
		}

		auto models = m_scene->GetComponents<ModelComponent>();
		for (auto model : models)
		{
			if (model->castShadow)
			{
				//glCullFace(GL_FRONT);
				program->SetUniform("model", model->m_owner->transform.GetMatrix());
				model->model->Draw(); 
			}
			
		}
		m_scene->Draw(renderer);
		framebuffer->Unbind();

		//*** PASS 2 ***

		renderer.ResetViewport();
		renderer.BeginFrame();
		m_scene->Draw(renderer);



		ENGINE.GetSystem<Gui>()->Draw();
		renderer.EndFrame();
	}
}
