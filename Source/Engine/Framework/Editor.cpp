#include "Editor.h"
#include "Scene.h"
#include "Components/CameraComponent.h"

namespace nc
{
	void Editor::Update()
	{
		if (ImGui::IsKeyPressed(ImGuiKey_GraveAccent)) m_active = !m_active;
	}
	void Editor::ProcessGui(Scene* scene)
	{
		if (!m_active) return;
		//show resources
		ImGui::Begin("Resources");
		auto resources = GET_RESOURCES(Resource);
		for(auto resource: resources){
			if (ImGui::Selectable(resource->name.c_str(), resource.get() == m_selected))
			{
				m_selected = resource.get();
			}
		}
		//textures
		if (ImGui::CollapsingHeader("TExtures"))
		{

			auto resources = GET_RESOURCES(Texture);
			for (auto& resource : resources)
			{
				if (ImGui::Selectable(resource->name.c_str(), resource.get() == m_selected)) m_selected = resource.get();
			}
		}
		//models
		if (ImGui::CollapsingHeader("Models"))
		{
			auto resources = GET_RESOURCES(Model);
			for (auto& resource : resources)
			{
				if (ImGui::Selectable(resource->name.c_str(), resource.get() == m_selected)) m_selected = resource.get();
			}
		}
		//shaders
		if (ImGui::CollapsingHeader("Shaders"))
		{
			auto resources = GET_RESOURCES(Shader);
			for (auto& resource : resources)
			{
				if (ImGui::Selectable(resource->name.c_str(), resource.get() == m_selected)) m_selected = resource.get();
			}
		}
		//materials
		if (ImGui::CollapsingHeader("Materials"))
		{
			auto resources = GET_RESOURCES(Material);
			for (auto& resource : resources)
			{
				if (ImGui::Selectable(resource->name.c_str(), resource.get() == m_selected)) m_selected = resource.get();
			}
		}
		ImGui::End();

		ImGui::Begin("Scene");
		scene->ProcessGui();
		ImGui::Separator();

		//show actors
		ImGui::BeginChild("Actors");
		//actor controls
		if (ImGui::BeginPopupContextWindow()) {
			auto cameras = scene->GetComponents<CameraComponent>();
			auto camera = (!cameras.empty()) ? cameras[0] : nullptr;

			if (ImGui::MenuItem("Create Empty")) {
				auto actor = CREATE_CLASS(Actor);
				actor->name = nc::StringUtils::CreateUnique(actor->name);
				actor->Initialize();
				m_selected = actor.get();
				scene->Add(std::move(actor));
			}
			if (ImGui::MenuItem("Sphere")) {
				auto actor = CREATE_CLASS(Actor, "sphere");
				actor->name = nc::StringUtils::CreateUnique(actor->name);
				if (camera) {
					actor->transform.position = camera->m_owner->transform.position + camera->m_owner->transform.Forward() * 3.0f;
				}
				actor->Initialize();
				m_selected = actor.get();
				scene->Add(std::move(actor));
			}
			if (ImGui::MenuItem("Cube")) {
				auto actor = CREATE_CLASS(Actor, "cube");
				actor->name = nc::StringUtils::CreateUnique(actor->name);
				actor->Initialize();
				m_selected = actor.get();
				scene->Add(std::move(actor));
			}
			if (ImGui::MenuItem("Camera")) {
				auto actor = CREATE_CLASS(Actor, "camera");
				actor->name = nc::StringUtils::CreateUnique(actor->name);
				actor->Initialize();
				m_selected = actor.get();
				scene->Add(std::move(actor));
			}
			if (ImGui::MenuItem("Light")) {
				auto actor = CREATE_CLASS(Actor, "light");
				actor->name = nc::StringUtils::CreateUnique(actor->name);
				actor->Initialize();
				m_selected = actor.get();
				scene->Add(std::move(actor));
			}
			ImGui::EndPopup();
		}

		for (auto& actor : scene->m_actors)
		{
			if (ImGui::Selectable(actor->name.c_str(), actor.get() == m_selected))
			{
				m_selected = actor.get();
			}
		}
		ImGui::EndChild();
		ImGui::End();

		//show inspector
		ImGui::Begin("Inspector");
		if (m_selected)
		{
			m_selected->ProcessGui();
			// delete selected actor
			if (ImGui::IsKeyPressed(ImGuiKey_Delete))
			{
				auto actor = dynamic_cast<Actor*>(m_selected);
				if (actor)
				{
					scene->Remove(actor);
					m_selected = nullptr;
				}
			}
		}
		ImGui::End();
	}
}
