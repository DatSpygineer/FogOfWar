#include <utility>

#include "fow/Renderer/Skybox.hpp"
#include "fow/Renderer.hpp"

namespace fow {
    #define VERTEX_COUNT 108
    static constexpr float skyboxVertices[VERTEX_COUNT] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    Skybox::Skybox(const MaterialPtr& material) : m_pMaterial(material) {
        glGenVertexArrays(1, &m_uVao);
        glBindVertexArray(m_uVao);
        glGenBuffers(1, &m_uVbo);

        glBindBuffer(GL_ARRAY_BUFFER, m_uVbo);
        glBufferData(GL_ARRAY_BUFFER, VERTEX_COUNT * sizeof(float), skyboxVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    }
    Skybox::Skybox(MaterialPtr&& material) noexcept : m_pMaterial(std::move(material)) {
        glGenVertexArrays(1, &m_uVao);
        glBindVertexArray(m_uVao);
        glGenBuffers(1, &m_uVbo);

        glBindBuffer(GL_ARRAY_BUFFER, m_uVbo);
        glBufferData(GL_ARRAY_BUFFER, VERTEX_COUNT * sizeof(float), skyboxVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    }
    Skybox::~Skybox() {
        if (m_uVbo != 0) {
            glDeleteBuffers(1, &m_uVbo);
        }
        if (m_uVao != 0) {
            glDeleteVertexArrays(1, &m_uVao);
        }
    }

    void Skybox::draw() const {
        glDisable(GL_CULL_FACE);
        glDepthMask(GL_FALSE);
        glBindVertexArray(m_uVao);

        auto view = Renderer::GetViewMatrix();
        view[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // Clear translation

        Debug::Assert(m_pMaterial->apply());
        Debug::Assert(m_pMaterial->shader()->set_uniform("MATRIX_PROJECTION", Renderer::GetProjectionMatrix()), "Error while applying uniform \"MATRIX_PROJECTION\"");
        Debug::Assert(m_pMaterial->shader()->set_uniform("MATRIX_VIEW", view), "Error while applying uniform \"MATRIX_VIEW\"");

        glDrawArrays(GL_TRIANGLES, 0, VERTEX_COUNT);
        glDepthMask(GL_TRUE);
        glEnable(GL_CULL_FACE);
    }
}
