#ifndef FOW_MODEL_HPP
#define FOW_MODEL_HPP

#include "fow/Shared.hpp"
#include "fow/Renderer/Material.hpp"
#include "fow/Renderer/Mesh.hpp"

namespace fow {
    class Model;
    using ModelPtr = Ref<Model>;

    class FOW_RENDER_API Model final : public IDrawable3D, IDrawable3DInstanced {
        Vector<MeshPtr> m_meshes;
        Vector<MaterialPtr> m_material_overrides;
    public:
        Model() = default;
        Model(const Model& other);
        Model(Model&&) noexcept = default;
        explicit Model(const Vector<MeshPtr>& meshes) : m_meshes(meshes) {
            m_material_overrides.reserve(m_meshes.size());
            std::ranges::fill(m_material_overrides, nullptr);
        }
        explicit Model(Vector<MeshPtr>&& meshes) : m_meshes(std::move(meshes)) {
            m_material_overrides.reserve(m_meshes.size());
            std::ranges::fill(m_material_overrides, nullptr);
        }

        Model& operator= (const Model&) = default;
        Model& operator= (Model&&) noexcept = default;

        [[nodiscard]] FOW_CONSTEXPR const Vector<MeshPtr>& meshes() const { return m_meshes; }

        void draw() const;
        void draw(const Transform& transform) const override;
        void draw(const Matrix4& model_matrix) const;
        void draw_instances(const Vector<Transform>& transforms) const override;

        [[nodiscard]] Vector<MaterialPtr> materials() const;
        void set_materials(const Vector<MaterialPtr>& materials) const;
        void set_material(const MaterialPtr& material, GLuint index) const;
        void set_all_materials(const MaterialPtr& material) const;

        [[nodiscard]] FOW_CONSTEXPR const Vector<MaterialPtr>& material_overrides() const { return m_material_overrides; }
        [[nodiscard]] FOW_CONSTEXPR Vector<MaterialPtr>& material_overrides() { return m_material_overrides; }
        void set_material_overrides(const Vector<MaterialPtr>& material_overrides);
        void set_material_override(const MaterialPtr& material, size_t index);

        static Result<ModelPtr> Load(const String& source_path, const Vector<uint8_t>& data, const Vector<MaterialPtr>& materials);
        static Result<ModelPtr> LoadAsset(const Path& path, AssetLoaderFlags::Type flags);

        friend class Animation;
    };
}

#endif