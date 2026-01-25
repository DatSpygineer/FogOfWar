#include "fow/Renderer/Model.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace fow {
    void Model::draw() const {
        for (const auto& mesh : m_meshes) {
            mesh->draw();
        }
    }
    void Model::draw(const Transform& transform) const {
        for (const auto& mesh : m_meshes) {
            mesh->draw(transform);
        }
    }

    Vector<MaterialPtr> Model::materials() const {
        Vector<MaterialPtr> materials;
        for (const auto& mesh : m_meshes) {
            materials.push_back(mesh->material());
        }
        return materials;
    }

    static Result<> ProcessModelNodes(const String& source_path, const aiScene* scene, const aiNode* node, Vector<MeshPtr>& meshes, const Vector<MaterialPtr>& materials) {
        if (!scene->HasMeshes()) {
            return Failure("No mesh data found!");
        }
        for (size_t mesh_i = 0; mesh_i < node->mNumMeshes; ++mesh_i) {
            const auto* mesh = scene->mMeshes[node->mMeshes[mesh_i]];

            MaterialPtr material = nullptr;
            Vector<Vertex> vertices;
            Vector<GLuint> indices;
            for (size_t face_i = 0; face_i < mesh->mNumFaces; ++face_i) {
                const auto& face = mesh->mFaces[face_i];
                for (size_t idx_i = 0; idx_i < face.mNumIndices; ++idx_i) {
                    const auto idx  = face.mIndices[idx_i];
                    const auto pos  = mesh->mVertices[idx];
                    const auto norm = mesh->mNormals[idx];
                    aiVector3D uv;
                    if (mesh->HasTextureCoords(0)) {
                        uv = mesh->mTextureCoords[0][idx];
                    } else {
                        uv = aiVector3D(0.0f, 0.0f, 0.0f);
                    }
                    indices.emplace_back(vertices.size());
                    vertices.emplace_back(glm::vec3 { pos.x, pos.y, pos.z }, glm::vec3 { norm.x, norm.y, norm.z }, glm::vec2 { uv.x, 1.0f - uv.y });
                }
                if (mesh->mMaterialIndex < materials.size()) {
                    material = materials.at(mesh->mMaterialIndex);
                } else {
                    Debug::LogError(std::format("Failed to set material {} for model \"{}\": Material at index is not defined!", mesh->mMaterialIndex, source_path));
                }
            }

            const auto mesh_result = Mesh::Create(material, vertices, indices);
            if (!mesh_result.has_value()) {
                return Failure(std::format("Failed to load mesh data: {}", mesh_result.error().message));
            }
            meshes.emplace_back(std::move(mesh_result.value()));
        }

        if (node->mNumChildren > 0) {
            for (size_t i = 0; i < node->mNumChildren; ++i) {
                if (const auto result = ProcessModelNodes(source_path, scene, node->mChildren[i], meshes, materials); !result.has_value()) {
                    return result;
                }
            }
        }
        return Success();
    }

    Result<ModelPtr> Model::Load(const String& source_path, const Vector<uint8_t>& data, const Vector<MaterialPtr>& materials) {
        Assimp::Importer importer;
        const auto scene = importer.ReadFileFromMemory(data.data(), data.size(), aiProcessPreset_TargetRealtime_Quality);
        if (scene == nullptr) {
            return Failure<ModelPtr>(std::format("Failed to load model \"{}\": {}", source_path, importer.GetErrorString()));
        }

        Vector<MeshPtr> meshes;
        if (const auto proc_result = ProcessModelNodes(source_path, scene, scene->mRootNode, meshes, materials); !proc_result.has_value()) {
            return Failure<ModelPtr>(std::format("Failed to load model \"{}\": {}", source_path, proc_result.error().message));
        }
        return Success<ModelPtr>(std::move(std::make_shared<Model>(meshes)));
    }

    Result<ModelPtr> Model::LoadAsset(const Path& path, const AssetLoaderFlags::Type flags) {
        if (path.extension().equals(".xml", StringCompareType::CaseInsensitive)) {
            const auto doc = Assets::LoadAsXml(path, flags);
            if (!doc.has_value()) {
                return Failure<ModelPtr>(doc.error());
            }
            const auto root = doc->child("Model");
            if (!root) {
                return Failure<ModelPtr>(std::format("Failed to load model \"{}\": Expected root node \"Model\" in XML document!", path));
            }

            const auto src_node = root.child("Source");
            if (!src_node) {
                return Failure<ModelPtr>(std::format("Failed to load model \"{}\": Expected node \"Source\" in root node \"Model\"!", path));
            }
            const auto data = Assets::LoadAsBytes(src_node.child_value(), flags);
            if (!data.has_value()) {
                return Failure<ModelPtr>(std::format("Failed to load model \"{}\": Could not read model data \"{}\"", path, src_node.child_value()));
            }

            Vector<MaterialPtr> materials;

            if (const auto materials_node = root.child("Materials"); materials_node) {
                int i = 0;
                for (const auto& material_node : materials_node.children()) {
                    if (const auto attrib = material_node.attribute("src"); attrib) {
                        if (const auto mat = Assets::Load<Material>(attrib.value(), flags); mat.has_value()) {
                            materials.push_back(mat.value().ptr());
                        } else {
                            return Failure<ModelPtr>(std::format("Failed to load material \"{}\" for model \"{}\":\n{}", attrib.value(), path, mat.error().message));
                        }
                    } else {
                        if (const auto mat = Material::ParseXml(std::format("{}:{}", path, i), material_node, flags); mat.has_value()) {
                            materials.push_back(mat.value());
                        } else {
                            return Failure<ModelPtr>(std::format("Failed to load material {} for model \"{}\":\n{}", i, path, mat.error().message));
                        }
                    }
                    ++i;
                }
            }
            return Load(path.as_string(), data.value(), materials);
        }
        return Failure<ModelPtr>(std::format("Failed to load model \"{}\": Expected asset extension '.xml'", path));
    }
}
