#ifndef FOW_RENDER_SHARED_HPP
#define FOW_RENDER_SHARED_HPP

#include <fow/Shared.hpp>

namespace fow {
    struct FOW_RENDER_API IDrawable2D {
        virtual ~IDrawable2D() = default;

        FOW_ABSTRACT(void draw_2d(const Rectangle& rect) const);
    };
    struct FOW_RENDER_API IDrawable3D {
        virtual ~IDrawable3D() = default;

        FOW_ABSTRACT(void draw(const Transform& transform) const);
    };
    struct FOW_RENDER_API IDrawable3DInstanced {
        virtual ~IDrawable3DInstanced() = default;

        FOW_ABSTRACT(void draw_instances(const Vector<Transform>& transforms) const);
    };

    template<typename T>
    concept Drawable2DType = std::is_base_of_v<IDrawable2D, T>;
    template<typename T>
    concept Drawable3DType = std::is_base_of_v<IDrawable3D, T>;
    template<typename T>
    concept Drawable3DInstancedType = std::is_base_of_v<IDrawable3DInstanced, T>;
}

#endif