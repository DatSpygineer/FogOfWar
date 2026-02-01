#ifndef FOW_SKYBOX_HPP
#define FOW_SKYBOX_HPP

#include "fow/Shared.hpp"
#include "fow/Renderer/Material.hpp"

namespace fow {
    class Skybox;
    using SkyboxPtr = SharedPtr<Skybox>;

    class FOW_RENDER_API Skybox {
        MaterialPtr m_pMaterial;
        GLuint m_uVbo, m_uVao;
    public:
        Skybox() : m_pMaterial(nullptr), m_uVbo(0), m_uVao(0) { }
        explicit Skybox(const MaterialPtr& material);
        Skybox(const Skybox& skybox) : m_pMaterial(skybox.m_pMaterial), m_uVbo(skybox.m_uVbo), m_uVao(skybox.m_uVao) { }
        Skybox(Skybox&& skybox) noexcept : m_pMaterial(std::move(skybox.m_pMaterial)), m_uVbo(skybox.m_uVbo), m_uVao(skybox.m_uVao) { }
        ~Skybox();

        void draw() const;
    };
}

#endif