#include "fow/Renderer/Sprite.hpp"

namespace fow {
    static void setup_sprite(MeshPtr& mesh, const MaterialPtr& material) {
        if (mesh == nullptr) {
            const auto result = Mesh::CreateQuad(material);
            if (!result.has_value()) {
                Debug::LogError("Failed to create quad mesh for sprite");
                return;
            }
            mesh = result.value();
        }
    }

    void Sprite::set_material(const MaterialPtr& material) {
        m_pMaterial = material;
        if (m_pMesh != nullptr) {
            m_pMesh->material() = material;
        }
    }
    void Sprite::draw(const Transform& transform) {
        setup_sprite(m_pMesh, m_pMaterial);

        if (m_pMesh != nullptr) {
            Debug::AssertWarn(m_pMesh->material()->set_parameter_optional("BillboardMode", static_cast<GLuint>(m_eBillboardMode)));
            m_pMesh->draw(transform);
        }
    }

    void Sprite::draw_instances(const Vector<Transform>& transforms) {
        setup_sprite(m_pMesh, m_pMaterial);

        if (m_pMesh != nullptr) {
            Debug::AssertWarn(m_pMesh->material()->set_parameter_optional("BillboardMode", static_cast<GLuint>(m_eBillboardMode)));
            m_pMesh->draw_instances(transforms);
        }
    }

    Result<SpritePtr> Sprite::LoadAsset(const Path& path, AssetLoaderFlags::Type flags) {
        const auto xml = Assets::LoadAsXml(path, flags);
        if (!xml.has_value()) {
            return Failure(xml.error());
        }

        const auto root = xml->child("Sprite");
        if (!root) {
            return Failure(std::format("Failed to load Sprite \"{}\": Expected root node \"Sprite\"", path));
        }
        const auto mat_node = root.child("Material");
        if (!mat_node) {
            return Failure(std::format("Expected child node \"Material\" in root node \"Sprite\""));
        }

        MaterialPtr material;
        if (const auto src_attrib = mat_node.attribute("src"); src_attrib) {
            auto mat_result = Assets::Load<Material>(src_attrib.value(), flags);
            if (!mat_result.has_value()) {
                return Failure(std::format("Failed to load Sprite \"{}\": {}", path, mat_result.error().message));
            }
            material = std::move(mat_result.value().ptr());
        } else {
            auto mat_result = Material::ParseXml(std::format("{}:internal", path), mat_node, flags);
            if (!mat_result.has_value()) {
                return Failure(std::format("Failed to load Sprite \"{}\": {}", path, mat_result.error().message));
            }
            material = std::move(mat_result.value());
        }

        auto billboard_mode = BillboardMode::None;
        if (const auto billboard_node = root.child("Billboard"); billboard_node) {
            const auto billboard_str = String(billboard_node.child_value());
            if (billboard_str.equals_any({ "yaligned", "y_aligned", "cylindrical" }, StringCompareType::CaseInsensitive)) {
                billboard_mode = BillboardMode::BillboardCylindrical;
            } else if (billboard_str.equals_any({ "spherical" }, StringCompareType::CaseInsensitive)) {
                billboard_mode = BillboardMode::BillboardSpherical;
            }
        }

        return Success<SpritePtr>(std::make_shared<Sprite>(std::move(material), billboard_mode));
    }
}
