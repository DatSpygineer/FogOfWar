#include "ShaderParamInfo.hpp"

#include "ResourceHelper.hpp"

namespace fow {
    static const char* attribute_value_or(const pugi::xml_attribute& attribute, const char* _default) {
        return attribute ? attribute.value() : _default;
    }

    Result<ShaderParams> ShaderParams::Load(const String& name) {
        const auto result = LoadEditorShaderParamsXml(name);
        if (result.has_value()) {
            return FromXml(name, result.value().child("ShaderParams"));
        }
        return Failure(std::format("Failed to load shader parameters \"{}\": {}", name, result.error().message));
    }
    Result<ShaderParams> ShaderParams::FromXml(const String& name, const pugi::xml_node& node) {
        ShaderParams params;
        size_t i = 0;
        for (const auto& param : node.child("Param")) {
            ShaderParamInfo info;
            const auto name_node = param.attribute("name");
            if (!name_node) {
                return Failure(std::format("Failed to load Shader info \"{}\": Parameter #{} must have attribute 'name'", name, i));
            }
            info.name = name_node.value();
            const auto type_node = param.attribute("type");
            if (!type_node) {
                return Failure(std::format("Failed to load Shader info \"{}\": Parameter \"{}\" must have attribute 'type'", name, name_node.value()));
            }

            const auto type = Match(String::CreateLowercase(type_node.value()), HashMap<String, ShaderParamType> {
                { "bool",     ShaderParamType::Bool         },
                { "int",      ShaderParamType::Int          },
                { "uint",     ShaderParamType::UInt         },
                { "float",    ShaderParamType::Float        },
                { "ivec2",    ShaderParamType::IntVector2   },
                { "ivec3",    ShaderParamType::IntVector3   },
                { "ivec4",    ShaderParamType::IntVector4   },
                { "ivector2", ShaderParamType::IntVector2   },
                { "ivector3", ShaderParamType::IntVector3   },
                { "ivector4", ShaderParamType::IntVector4   },
                { "uvec2",    ShaderParamType::UIntVector2  },
                { "uvec3",    ShaderParamType::UIntVector3  },
                { "uvec4",    ShaderParamType::UIntVector4  },
                { "uvector2", ShaderParamType::UIntVector2  },
                { "uvector3", ShaderParamType::UIntVector3  },
                { "uvector4", ShaderParamType::UIntVector4  },
                { "vec2",     ShaderParamType::FloatVector2 },
                { "vec3",     ShaderParamType::FloatVector3 },
                { "vec4",     ShaderParamType::FloatVector4 },
                { "vector2",  ShaderParamType::FloatVector2 },
                { "vector3",  ShaderParamType::FloatVector3 },
                { "vector4",  ShaderParamType::FloatVector4 },
                { "color",    ShaderParamType::Color        },
                { "mat4",     ShaderParamType::Matrix4      },
                { "texture",  ShaderParamType::Texture      },
            });

            if (!type.has_value()) {
                return Failure(std::format("Failed to load Shader info \"{}\": Shader parameter \"{}\" has invalid type \"{}\"", name, name_node.value(), type_node.value()));
            }

            info.type = type.value();
            info.category = attribute_value_or(param.attribute("category"), "");
            info.defaultValue = attribute_value_or(param.attribute("default"), "");
            params.emplace(info.name, info);
            i++;
        }
        return Success(std::move(params));
    }
}
