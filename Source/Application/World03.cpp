#include "World03.h"
#include "Framework/Framework.h"
#include "Renderer/Renderer.h"
#include "Input/InputSystem.h"
#include <glm/glm/gtc/type_ptr.hpp>

#define INTERLEAVE

namespace nc
{
    bool World03::Initialize()
    {
        m_program = GET_RESOURCE(Program, "shaders/unlit_texture.prog");
        m_program->Use();

        m_texture = GET_RESOURCE(Texture, "textures/llama.png");
        m_texture->Bind();
        m_texture->SetActive(GL_TEXTURE0);

#ifdef INTERLEAVE

        //vertex data
        float vertexData[] = {
            -0.8f, -0.8f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0, //bottom left
             0.8f, -0.8f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0,//bottom right
             0.8f,  0.8f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0,//top right
            -0.8f,  0.8f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0//top left
        };
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glBindVertexBuffer(0, vbo, 0, 8 * sizeof(GLfloat));

        //position
        glEnableVertexAttribArray(0);
        glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexAttribBinding(0, 0);

        //color
        glEnableVertexAttribArray(1);
        glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));
        glVertexAttribBinding(1, 0);

        //texcoord
        glEnableVertexAttribArray(2);
        glVertexAttribFormat(2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat));
        glVertexAttribBinding(2, 0);
#else
#endif
        m_transform.position.z = -10.0f;
        return true;
    }

    void World03::Shutdown()
    {
    }

    void World03::Update(float dt)
    {
        ENGINE.GetSystem<Gui>()->BeginFrame();
        ImGui::Begin("Transform");
        ImGui::DragFloat3("Position", &m_transform.position[0]);
        ImGui::DragFloat3("Rotation", &m_transform.rotation[0]);
        ImGui::DragFloat3("Scale", &m_transform.scale[0]);
        ImGui::End();
        
        
        //m_transform.rotation.x += 10 * dt;
        //m_transform.rotation.y += 10 * dt;
        //m_transform.rotation.z += 10 * dt;
        
        m_transform.position.x += ENGINE.GetSystem<InputSystem>()->GetKeyDown(SDL_SCANCODE_A) ? m_speed * -dt : 0;
        m_transform.position.x += ENGINE.GetSystem<InputSystem>()->GetKeyDown(SDL_SCANCODE_D) ? m_speed * +dt : 0;
        m_transform.position.z += ENGINE.GetSystem<InputSystem>()->GetKeyDown(SDL_SCANCODE_W) ? m_speed * -dt : 0;
        m_transform.position.z += ENGINE.GetSystem<InputSystem>()->GetKeyDown(SDL_SCANCODE_S) ? m_speed * +dt : 0;

        m_time += dt;

        m_program->SetUniform("offset", glm::vec2(m_time, 0));
        m_program->SetUniform("tiling", glm::vec2(2, 2));

        //model matrix
        //glm::mat4 position = glm::translate(glm::mat4{ 1 }, m_position);
        //glm::mat4 rotation = glm::rotate(glm::mat4{ 1 }, glm::radians(m_angle * 10), glm::vec3{0, 0, 1});
        //glm::mat4 model = position * rotation;
        m_program->SetUniform("model", m_transform.GetMatrix());


        //view matrix
        glm::mat4 view = glm::lookAt(glm::vec3{ 0, 2, 5 }, glm::vec3{ 0, 0, 3 }, glm::vec3{ 0, 1, 0 });
        m_program->SetUniform("view", view);
        //uniform = glGetUniformLocation(m_program->m_program, "view");
        //glUniformMaatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(view));

        //projection
        glm::mat4 projection = glm::perspective(glm::radians(90.0f), 800.0f / 600.0f, 0.01f, 100.0f);
        m_program->SetUniform("projection", projection);
        //uniform = glGetUniformLocation(m_program->m_program, "projection");
        //glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(projection));

        ENGINE.GetSystem<Gui>()->EndFrame();
    }

    void World03::Draw(Renderer& renderer)
    {
        // pre-render
        renderer.BeginFrame();

        // render
        glBindVertexArray(m_vao);
        glDrawArrays(GL_QUADS, 0, 4);

        ENGINE.GetSystem<Gui>()->Draw();

        // post-render
        renderer.EndFrame();
    }
}