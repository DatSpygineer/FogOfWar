#ifndef FOW_RENDERER_HPP
#define FOW_RENDERER_HPP

#include "fow/Renderer/Texture.hpp"
#include "fow/Renderer/Shader.hpp"
#include "fow/Renderer/Material.hpp"
#include "fow/Renderer/Mesh.hpp"
#include "fow/Renderer/Model.hpp"

namespace fow {
    enum class BlendFactor : GLenum {
        SrcColor         = GL_SRC_COLOR,
        OneMinusSrcColor = GL_ONE_MINUS_SRC_COLOR,
        SrcAlpha         = GL_SRC_ALPHA,
        OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
        DstColor         = GL_DST_COLOR,
        OneMinusDstColor = GL_ONE_MINUS_DST_COLOR,
        SrcAlphaSaturate = GL_SRC_ALPHA_SATURATE
    };

    namespace Renderer {
        FOW_RENDER_API Result<> Initialize(const Path& app_base_path);
        FOW_RENDER_API void Terminate();
        FOW_RENDER_API Path GetBasePath();
        FOW_RENDER_API void EnableBlend(bool enabled, BlendFactor src = BlendFactor::SrcAlpha, BlendFactor dst = BlendFactor::OneMinusSrcAlpha);
        FOW_RENDER_API void UpdateCameraProjectionMatrix(const glm::mat4& matrix);
        FOW_RENDER_API void UpdateCameraProjectionOrtho(float x, float y, float width, float height, float near, float far);
        FOW_RENDER_API void UpdateCameraProjectionOrtho(const Rectangle& rect, float near, float far);
        FOW_RENDER_API void UpdateCameraProjectionOrtho(float x, float y, float width, float height);
        FOW_RENDER_API void UpdateCameraProjectionOrtho(const Rectangle& rect);
        FOW_RENDER_API void UpdateCameraProjectionPerspective(float fov, float width, float height, float near, float far);
        FOW_RENDER_API void UpdateCameraProjectionPerspective(float fov, const glm::vec2& size, float near, float far);
        FOW_RENDER_API void UpdateCameraViewMatrix(const glm::mat4& matrix);
        FOW_RENDER_API void UpdateCameraPositionSimple(const glm::vec3& position);
        FOW_RENDER_API void UpdateCameraPosition(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up);
        FOW_RENDER_API void UpdateCameraPosition(const glm::vec3& position, const glm::vec3& forward, const glm::vec3& up, const glm::quat& rotation);
        FOW_RENDER_API glm::mat4 GetViewMatrix();
        FOW_RENDER_API glm::mat4 GetProjectionMatrix();
        FOW_RENDER_API void SetViewport(const Rectangle& rect);
        FOW_RENDER_API void SetViewport(float x, float y, float width, float height);
        FOW_RENDER_API Rectangle GetViewport();
    }
}

#endif