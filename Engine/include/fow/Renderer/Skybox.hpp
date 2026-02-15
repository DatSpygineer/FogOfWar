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
        explicit Skybox(MaterialPtr&& material) noexcept;
        Skybox(const Skybox& skybox) = default;
        Skybox(Skybox&& skybox) noexcept : m_pMaterial(std::move(skybox.m_pMaterial)), m_uVbo(skybox.m_uVbo), m_uVao(skybox.m_uVao) { }
        ~Skybox();

        void draw() const;
    };
}

#endif