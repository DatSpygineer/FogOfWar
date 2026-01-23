#ifndef FOW_MODEL_HPP
#define FOW_MODEL_HPP

#include "fow/Shared.hpp"
#include "fow/Renderer/Material.hpp"
#include "fow/Renderer/Mesh.hpp"

namespace fow {
    class Model;
    using ModelPtr = SharedPtr<Model>;

    class FOW_RENDER_API Model final {
        Vector<MeshPtr> m_meshes;
    public:
        Model() = default;
        Model(const Model&) = default;
        Model(Model&&) noexcept = default;
        explicit Model(const Vector<MeshPtr>& meshes) : m_meshes(meshes) { }
        explicit Model(Vector<MeshPtr>&& meshes) : m_meshes(std::move(meshes)) { }

        Model& operator= (const Model&) = default;
        Model& operator= (Model&&) noexcept = default;

        [[nodiscard]] constexpr const Vector<MeshPtr>& meshes() const { return m_meshes; }

        void draw() const;
        void draw(const Transform& transform) const;

        Vector<MaterialPtr> materials() const;

        static Result<ModelPtr> Load(const String& source_path, const Vector<uint8_t>& data, const Vector<MaterialPtr>& materials);
        static Result<ModelPtr> LoadAsset(const Path& path, AssetLoaderFlags::Type flags);

        friend class Animation;
    };
}

#endif