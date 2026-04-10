#ifndef FOW_RENDERQUEUE_HPP
#define FOW_RENDERQUEUE_HPP

#include <fow/Shared.hpp>

#include "Mesh.hpp"
#include "Model.hpp"
#include "Sprite.hpp"
#include "Skybox.hpp"

namespace fow {
    struct FOW_RENDER_API LightInfo {
        const Transform* transform;
        Vector3 color;
        float intensity;
        bool enabled;
    };
    using LightInfoPtr = Ref<LightInfo>;

    namespace RenderQueue {
        FOW_RENDER_API void Enqueue(const Ref<IDrawable3D>& drawable, const Transform& transform);
        FOW_RENDER_API void EnqueueInstanced(const Ref<IDrawable3DInstanced>& drawable, const Vector<Transform>& transforms);
        FOW_RENDER_API void SetSkybox(const SkyboxPtr& skybox);
        FOW_RENDER_API void SetEnvMap(const TextureCubeMapPtr& texture, const TextureCubeMapPtr& texture_blurred, float intensity);
        FOW_RENDER_API void SetSunlight(const Transform& transform, const Color& color, float intensity, bool is_enabled = true);
        FOW_RENDER_API void SetSunlight(const Transform& transform, const Vector3& color, float intensity, bool is_enabled = true);
        FOW_RENDER_API void SetSunlight(const Transform& transform, const Vector4& color, bool is_enabled = true);
        FOW_RENDER_API void SetSunlightColor(const Color& color, float intensity);
        FOW_RENDER_API void SetSunlightTransform(const Transform& transform);
        FOW_RENDER_API void SetSunlightEnabled(bool enabled);
        FOW_RENDER_API void RemoveSunlight();
        FOW_RENDER_API LightInfoPtr AddLight(const Transform& transform, const Vector4& color, bool is_enabled = true);
        FOW_RENDER_API LightInfoPtr AddLight(const Transform& transform, const Vector3& color, float intensity, bool is_enabled = true);
        FOW_RENDER_API LightInfoPtr AddLight(const Transform& transform, const Color& color, float intensity, bool is_enabled = true);
        FOW_RENDER_API void RemoveLight(const LightInfoPtr& light);
        FOW_RENDER_API void Render();
        FOW_RENDER_API void ApplyCurrentSceneParamsToMaterial(const MaterialPtr& mat);

        template<Drawable3DType T>
        inline void Enqueue(const Ref<T>& drawable, const Transform& transform) {
            Enqueue(CastRef<IDrawable3D>(drawable), transform);
        }
        template<Drawable3DType T>
        inline void EnqueueInstanced(const Ref<T>& drawable, const Vector<Transform>& transforms) {
            EnqueueInstanced(CastRef<IDrawable3DInstanced>(drawable), transforms);
        }
    }

    namespace RenderQueue2D {
        FOW_RENDER_API void Enqueue(const Sprite2DPtr& sprite, const Rectangle& rectangle);
        FOW_RENDER_API void Enqueue(const TextSpritePtr& sprite, const Rectangle& rectangle);
        FOW_RENDER_API void Render();
    }
}

#endif