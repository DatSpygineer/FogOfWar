#include "fow/Renderer/GL.hpp"
#include "fow/Renderer/Material.hpp"

namespace fow {
    Result<> Material::set_parameter(const String& name, const MaterialParameterValue& value) {
        ShaderPtr shader = m_pShader == nullptr ? Shader::PlaceHolder() : m_pShader;
        const auto info = shader->get_uniform_info(name);
        if (!info.has_value()) {
            return Failure(std::format("Shader has no parameter \"{}\"", name));
        }
        switch (info->type) {
            case ShaderUniformType::Bool: {
                if (value.index() == static_cast<int>(MaterialParameterType::Bool)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = value;
                    } else {
                        m_mParams.emplace(name, value);
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::UInt)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = std::get<1>(value) != 0;
                    } else {
                        m_mParams.emplace(name, std::get<1>(value) != 0);
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::Int)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = std::get<2>(value) != 0;
                    } else {
                        m_mParams.emplace(name, std::get<2>(value) != 0);
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::Float)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = std::get<3>(value) != 0.0f;
                    } else {
                        m_mParams.emplace(name, std::get<3>(value) != 0.0f);
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::Double)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = std::get<4>(value) != 0.0;
                    } else {
                        m_mParams.emplace(name, std::get<4>(value) != 0.0);
                    }
                    return Success();
                }
                return Failure(std::format("Cannot set parameter \"{}\", cannot convert {} type to boolean!",
                    name,
                    rfl::enum_to_string(static_cast<MaterialParameterType>(value.index())))
                );
            }
            case ShaderUniformType::Int: {
                if (value.index() == static_cast<int>(MaterialParameterType::Bool)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = std::get<0>(value) ? 1 : 0;
                    } else {
                        m_mParams.emplace(name, std::get<0>(value) ? 1 : 0);
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::UInt)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = static_cast<GLint>(std::get<1>(value));
                    } else {
                        m_mParams.emplace(name, static_cast<GLint>(std::get<1>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::Int)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = value;
                    } else {
                        m_mParams.emplace(name, value);
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::Float)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = static_cast<GLint>(std::get<3>(value));
                    } else {
                        m_mParams.emplace(name, static_cast<GLint>(std::get<3>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::Double)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = static_cast<GLint>(std::get<4>(value));
                    } else {
                        m_mParams.emplace(name, static_cast<GLint>(std::get<4>(value)));
                    }
                    return Success();
                }
                return Failure(std::format("Cannot set parameter \"{}\", cannot convert {} type to integer!",
                    name,
                    rfl::enum_to_string(static_cast<MaterialParameterType>(value.index())))
                );
            }
            case ShaderUniformType::UInt: {
                if (value.index() == static_cast<int>(MaterialParameterType::Bool)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = std::get<0>(value) ? 1u : 0u;
                    } else {
                        m_mParams.emplace(name, std::get<0>(value) ? 1u : 0u);
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::UInt)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = std::get<1>(value);
                    } else {
                        m_mParams.emplace(name, std::get<1>(value));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::Int)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = static_cast<GLuint>(std::get<2>(value));
                    } else {
                        m_mParams.emplace(name, static_cast<GLuint>(std::get<2>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::Float)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = static_cast<GLuint>(std::get<3>(value));
                    } else {
                        m_mParams.emplace(name, static_cast<GLuint>(std::get<3>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::Double)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = static_cast<GLuint>(std::get<4>(value));
                    } else {
                        m_mParams.emplace(name, static_cast<GLuint>(std::get<4>(value)));
                    }
                    return Success();
                }
                return Failure(std::format("Cannot set parameter \"{}\", cannot convert {} type to unsigned integer!",
                    name,
                    rfl::enum_to_string(static_cast<MaterialParameterType>(value.index())))
                );
            }
            case ShaderUniformType::Float: {
                if (value.index() == static_cast<int>(MaterialParameterType::Bool)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = std::get<0>(value) ? 1.0f : 0.0f;
                    } else {
                        m_mParams.emplace(name, std::get<0>(value) ? 1.0f : 0.0f);
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::UInt)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = static_cast<GLfloat>(std::get<1>(value));
                    } else {
                        m_mParams.emplace(name, static_cast<GLfloat>(std::get<1>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::Int)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = static_cast<GLfloat>(std::get<2>(value));
                    } else {
                        m_mParams.emplace(name, static_cast<GLfloat>(std::get<2>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::Float)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = value;
                    } else {
                        m_mParams.emplace(name, value);
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::Double)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = static_cast<GLfloat>(std::get<4>(value));
                    } else {
                        m_mParams.emplace(name, static_cast<GLfloat>(std::get<4>(value)));
                    }
                    return Success();
                }
                return Failure(std::format("Cannot set parameter \"{}\", cannot convert {} type to float!",
                    name,
                    rfl::enum_to_string(static_cast<MaterialParameterType>(value.index())))
                );
            }
            case ShaderUniformType::Double: {
                if (value.index() == static_cast<int>(MaterialParameterType::Bool)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = std::get<0>(value) ? 1.0 : 0.0;
                    } else {
                        m_mParams.emplace(name, std::get<0>(value) ? 1.0 : 0.0);
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::UInt)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = static_cast<GLdouble>(std::get<1>(value));
                    } else {
                        m_mParams.emplace(name, static_cast<GLdouble>(std::get<1>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::Int)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = static_cast<GLdouble>(std::get<2>(value));
                    } else {
                        m_mParams.emplace(name, static_cast<GLdouble>(std::get<2>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::Float)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = static_cast<GLdouble>(std::get<3>(value));
                    } else {
                        m_mParams.emplace(name, static_cast<GLdouble>(std::get<3>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::Double)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = value;
                    } else {
                        m_mParams.emplace(name, value);
                    }
                    return Success();
                }
                return Failure(std::format("Cannot set parameter \"{}\", cannot convert {} type to double!",
                    name,
                    rfl::enum_to_string(static_cast<MaterialParameterType>(value.index())))
                );
            }
            case ShaderUniformType::BoolVector2: {
                if (value.index() == static_cast<int>(MaterialParameterType::BoolVector2)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = value;
                    } else {
                        m_mParams.emplace(name, value);
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::UIntVector2)) {
                    if (m_mParams.contains(name)) {
                        const auto uvec2 = std::get<8>(value);
                        m_mParams.at(name) = glm::bvec2(uvec2.x != 0, uvec2.y != 0);
                    } else {
                        const auto uvec2 = std::get<8>(value);
                        m_mParams.emplace(name, glm::bvec2(uvec2.x != 0, uvec2.y != 0));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::IntVector2)) {
                    if (m_mParams.contains(name)) {
                        const auto ivec2 = std::get<11>(value);
                        m_mParams.at(name) = glm::bvec2(ivec2.x != 0, ivec2.y != 0);
                    } else {
                        const auto ivec2 = std::get<11>(value);
                        m_mParams.emplace(name, glm::bvec2(ivec2.x != 0, ivec2.y != 0));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::FloatVector2)) {
                    if (m_mParams.contains(name)) {
                        const auto fvec2 = std::get<14>(value);
                        m_mParams.at(name) = glm::bvec2(fvec2.x != 0, fvec2.y != 0);
                    } else {
                        const auto fvec2 = std::get<14>(value);
                        m_mParams.emplace(name, glm::bvec2(fvec2.x != 0, fvec2.y != 0));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::DoubleVector2)) {
                    if (m_mParams.contains(name)) {
                        const auto dvec2 = std::get<17>(value);
                        m_mParams.at(name) = glm::bvec2(dvec2.x != 0, dvec2.y != 0);
                    } else {
                        const auto dvec2 = std::get<17>(value);
                        m_mParams.emplace(name, glm::bvec2(dvec2.x != 0, dvec2.y != 0));
                    }
                    return Success();
                }
                return Failure(std::format("Cannot set parameter \"{}\", cannot convert {} type to bvec2!",
                    name,
                    rfl::enum_to_string(static_cast<MaterialParameterType>(value.index())))
                );
            }
            case ShaderUniformType::BoolVector3: {
                if (value.index() == static_cast<int>(MaterialParameterType::BoolVector3)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = value;
                    } else {
                        m_mParams.emplace(name, value);
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::UIntVector3)) {
                    if (m_mParams.contains(name)) {
                        const auto uvec3 = std::get<9>(value);
                        m_mParams.at(name) = glm::bvec3(uvec3.x != 0, uvec3.y != 0, uvec3.z != 0);
                    } else {
                        const auto uvec3 = std::get<9>(value);
                        m_mParams.emplace(name, glm::bvec3(uvec3.x != 0, uvec3.y != 0, uvec3.z != 0));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::IntVector3)) {
                    if (m_mParams.contains(name)) {
                        const auto ivec3 = std::get<12>(value);
                        m_mParams.at(name) = glm::bvec3(ivec3.x != 0, ivec3.y != 0, ivec3.z != 0);
                    } else {
                        const auto ivec3 = std::get<12>(value);
                        m_mParams.emplace(name, glm::bvec3(ivec3.x != 0, ivec3.y != 0, ivec3.z != 0));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::FloatVector3)) {
                    if (m_mParams.contains(name)) {
                        const auto fvec3 = std::get<15>(value);
                        m_mParams.at(name) = glm::bvec3(fvec3.x != 0, fvec3.y != 0, fvec3.z != 0);
                    } else {
                        const auto fvec3 = std::get<15>(value);
                        m_mParams.emplace(name, glm::bvec3(fvec3.x != 0, fvec3.y != 0, fvec3.z != 0));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::DoubleVector3)) {
                    if (m_mParams.contains(name)) {
                        const auto dvec3 = std::get<18>(value);
                        m_mParams.at(name) = glm::bvec3(dvec3.x != 0, dvec3.y != 0, dvec3.z != 0);
                    } else {
                        const auto dvec3 = std::get<18>(value);
                        m_mParams.emplace(name, glm::bvec3(dvec3.x != 0, dvec3.y != 0, dvec3.z != 0));
                    }
                    return Success();
                }
                return Failure(std::format("Cannot set parameter \"{}\", cannot convert {} type to bvec3!",
                    name,
                    rfl::enum_to_string(static_cast<MaterialParameterType>(value.index())))
                );
            }
            case ShaderUniformType::BoolVector4: {
                if (value.index() == static_cast<int>(MaterialParameterType::BoolVector4)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = value;
                    } else {
                        m_mParams.emplace(name, value);
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::UIntVector4)) {
                    if (m_mParams.contains(name)) {
                        const auto uvec4 = std::get<10>(value);
                        m_mParams.at(name) = glm::bvec4(uvec4.x != 0, uvec4.y != 0, uvec4.z != 0, uvec4.w != 0);
                    } else {
                        const auto uvec4 = std::get<10>(value);
                        m_mParams.emplace(name, glm::bvec4(uvec4.x != 0, uvec4.y != 0, uvec4.z != 0, uvec4.w != 0));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::IntVector4)) {
                    if (m_mParams.contains(name)) {
                        const auto ivec4 = std::get<13>(value);
                        m_mParams.at(name) = glm::bvec4(ivec4.x != 0, ivec4.y != 0, ivec4.z != 0, ivec4.w != 0);
                    } else {
                        const auto ivec4 = std::get<13>(value);
                        m_mParams.emplace(name, glm::bvec4(ivec4.x != 0, ivec4.y != 0, ivec4.z != 0, ivec4.w != 0));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::FloatVector4)) {
                    if (m_mParams.contains(name)) {
                        const auto fvec4 = std::get<16>(value);
                        m_mParams.at(name) = glm::bvec4(fvec4.x != 0, fvec4.y != 0, fvec4.z != 0, fvec4.w != 0);
                    } else {
                        const auto fvec4 = std::get<16>(value);
                        m_mParams.emplace(name, glm::bvec4(fvec4.x != 0, fvec4.y != 0, fvec4.z != 0, fvec4.w != 0));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::DoubleVector4)) {
                    if (m_mParams.contains(name)) {
                        const auto dvec4 = std::get<19>(value);
                        m_mParams.at(name) = glm::bvec4(dvec4.x != 0, dvec4.y != 0, dvec4.z != 0, dvec4.w != 0);
                    } else {
                        const auto dvec4 = std::get<19>(value);
                        m_mParams.emplace(name, glm::bvec4(dvec4.x != 0, dvec4.y != 0, dvec4.z != 0, dvec4.w != 0));
                    }
                    return Success();
                }
                return Failure(std::format("Cannot set parameter \"{}\", cannot convert {} type to bvec4!",
                    name,
                    rfl::enum_to_string(static_cast<MaterialParameterType>(value.index())))
                );
            }
            case ShaderUniformType::IntVector2: {
                if (value.index() == static_cast<int>(MaterialParameterType::BoolVector2)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::ivec2(std::get<5>(value));
                    } else {
                        m_mParams.emplace(name, glm::ivec2(std::get<5>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::UIntVector2)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::ivec2(std::get<8>(value));
                    } else {
                        m_mParams.emplace(name, glm::ivec2(std::get<8>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::IntVector2)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::ivec2(std::get<11>(value));
                    } else {
                        m_mParams.emplace(name, glm::ivec2(std::get<11>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::FloatVector2)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::ivec2(std::get<14>(value));
                    } else {
                        m_mParams.emplace(name, glm::ivec2(std::get<14>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::DoubleVector2)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::ivec2(std::get<17>(value));
                    } else {
                        m_mParams.emplace(name, glm::ivec2(std::get<17>(value)));
                    }
                    return Success();
                }
                return Failure(std::format("Cannot set parameter \"{}\", cannot convert {} type to ivec2!",
                    name,
                    rfl::enum_to_string(static_cast<MaterialParameterType>(value.index())))
                );
            }
            case ShaderUniformType::IntVector3: {
                if (value.index() == static_cast<int>(MaterialParameterType::BoolVector3)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::ivec3(std::get<6>(value));
                    } else {
                        m_mParams.emplace(name, glm::ivec3(std::get<6>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::UIntVector3)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::ivec3(std::get<9>(value));
                    } else {
                        m_mParams.emplace(name, glm::ivec3(std::get<9>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::IntVector3)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::ivec3(std::get<12>(value));
                    } else {
                        m_mParams.emplace(name, glm::ivec3(std::get<12>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::FloatVector3)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::ivec3(std::get<15>(value));
                    } else {
                        m_mParams.emplace(name, glm::ivec3(std::get<15>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::DoubleVector3)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::ivec3(std::get<18>(value));
                    } else {
                        m_mParams.emplace(name, glm::ivec3(std::get<18>(value)));
                    }
                    return Success();
                }
                return Failure(std::format("Cannot set parameter \"{}\", cannot convert {} type to ivec3!",
                    name,
                    rfl::enum_to_string(static_cast<MaterialParameterType>(value.index())))
                );
            }
            case ShaderUniformType::IntVector4: {
                if (value.index() == static_cast<int>(MaterialParameterType::BoolVector4)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::ivec4(std::get<7>(value));
                    } else {
                        m_mParams.emplace(name, glm::ivec4(std::get<7>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::UIntVector4)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::ivec4(std::get<10>(value));
                    } else {
                        m_mParams.emplace(name, glm::ivec4(std::get<10>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::IntVector4)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::ivec4(std::get<13>(value));
                    } else {
                        m_mParams.emplace(name, glm::ivec4(std::get<13>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::FloatVector4)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::ivec4(std::get<16>(value));
                    } else {
                        m_mParams.emplace(name, glm::ivec4(std::get<16>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::DoubleVector4)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::ivec4(std::get<19>(value));
                    } else {
                        m_mParams.emplace(name, glm::ivec4(std::get<19>(value)));
                    }
                    return Success();
                }
                return Failure(std::format("Cannot set parameter \"{}\", cannot convert {} type to ivec4!",
                    name,
                    rfl::enum_to_string(static_cast<MaterialParameterType>(value.index())))
                );
            }
            case ShaderUniformType::UIntVector2: {
                if (value.index() == static_cast<int>(MaterialParameterType::BoolVector2)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::uvec2(std::get<5>(value));
                    } else {
                        m_mParams.emplace(name, glm::uvec2(std::get<5>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::UIntVector2)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::uvec2(std::get<8>(value));
                    } else {
                        m_mParams.emplace(name, glm::uvec2(std::get<8>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::IntVector2)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::uvec2(std::get<11>(value));
                    } else {
                        m_mParams.emplace(name, glm::uvec2(std::get<11>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::FloatVector2)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::uvec2(std::get<14>(value));
                    } else {
                        m_mParams.emplace(name, glm::uvec2(std::get<14>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::DoubleVector2)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::uvec2(std::get<17>(value));
                    } else {
                        m_mParams.emplace(name, glm::uvec2(std::get<17>(value)));
                    }
                    return Success();
                }
                return Failure(std::format("Cannot set parameter \"{}\", cannot convert {} type to uvec2!",
                    name,
                    rfl::enum_to_string(static_cast<MaterialParameterType>(value.index())))
                );
            }
            case ShaderUniformType::UIntVector3: {
                if (value.index() == static_cast<int>(MaterialParameterType::BoolVector3)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::uvec3(std::get<6>(value));
                    } else {
                        m_mParams.emplace(name, glm::uvec3(std::get<6>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::UIntVector3)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::uvec3(std::get<9>(value));
                    } else {
                        m_mParams.emplace(name, glm::uvec3(std::get<9>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::IntVector3)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::uvec3(std::get<12>(value));
                    } else {
                        m_mParams.emplace(name, glm::uvec3(std::get<12>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::FloatVector3)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::uvec3(std::get<15>(value));
                    } else {
                        m_mParams.emplace(name, glm::uvec3(std::get<15>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::DoubleVector3)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::uvec3(std::get<18>(value));
                    } else {
                        m_mParams.emplace(name, glm::uvec3(std::get<18>(value)));
                    }
                    return Success();
                }
                return Failure(std::format("Cannot set parameter \"{}\", cannot convert {} type to uvec3!",
                    name,
                    rfl::enum_to_string(static_cast<MaterialParameterType>(value.index())))
                );
            }
            case ShaderUniformType::UIntVector4: {
                if (value.index() == static_cast<int>(MaterialParameterType::BoolVector4)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::uvec4(std::get<7>(value));
                    } else {
                        m_mParams.emplace(name, glm::uvec4(std::get<7>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::UIntVector4)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::uvec4(std::get<10>(value));
                    } else {
                        m_mParams.emplace(name, glm::uvec4(std::get<10>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::IntVector4)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::uvec4(std::get<13>(value));
                    } else {
                        m_mParams.emplace(name, glm::uvec4(std::get<13>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::FloatVector4)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::uvec4(std::get<16>(value));
                    } else {
                        m_mParams.emplace(name, glm::uvec4(std::get<16>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::DoubleVector4)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::uvec4(std::get<19>(value));
                    } else {
                        m_mParams.emplace(name, glm::uvec4(std::get<19>(value)));
                    }
                    return Success();
                }
                return Failure(std::format("Cannot set parameter \"{}\", cannot convert {} type to uvec4!",
                    name,
                    rfl::enum_to_string(static_cast<MaterialParameterType>(value.index())))
                );
            }
            case ShaderUniformType::FloatVector2: {
                if (value.index() == static_cast<int>(MaterialParameterType::BoolVector2)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::vec2(std::get<5>(value));
                    } else {
                        m_mParams.emplace(name, glm::vec2(std::get<5>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::UIntVector2)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::vec2(std::get<8>(value));
                    } else {
                        m_mParams.emplace(name, glm::vec2(std::get<8>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::IntVector2)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::vec2(std::get<11>(value));
                    } else {
                        m_mParams.emplace(name, glm::vec2(std::get<11>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::FloatVector2)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::vec2(std::get<14>(value));
                    } else {
                        m_mParams.emplace(name, glm::vec2(std::get<14>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::DoubleVector2)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::vec2(std::get<17>(value));
                    } else {
                        m_mParams.emplace(name, glm::vec2(std::get<17>(value)));
                    }
                    return Success();
                }
                return Failure(std::format("Cannot set parameter \"{}\", cannot convert {} type to vec2!",
                    name,
                    rfl::enum_to_string(static_cast<MaterialParameterType>(value.index())))
                );
            }
            case ShaderUniformType::FloatVector3: {
                if (value.index() == static_cast<int>(MaterialParameterType::BoolVector3)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::vec3(std::get<6>(value));
                    } else {
                        m_mParams.emplace(name, glm::vec3(std::get<6>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::UIntVector3)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::vec3(std::get<9>(value));
                    } else {
                        m_mParams.emplace(name, glm::vec3(std::get<9>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::IntVector3)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::vec3(std::get<12>(value));
                    } else {
                        m_mParams.emplace(name, glm::vec3(std::get<12>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::FloatVector3)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::vec3(std::get<15>(value));
                    } else {
                        m_mParams.emplace(name, glm::vec3(std::get<15>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::DoubleVector3)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::vec3(std::get<18>(value));
                    } else {
                        m_mParams.emplace(name, glm::vec3(std::get<18>(value)));
                    }
                    return Success();
                }
                return Failure(std::format("Cannot set parameter \"{}\", cannot convert {} type to vec3!",
                    name,
                    rfl::enum_to_string(static_cast<MaterialParameterType>(value.index())))
                );
            }
            case ShaderUniformType::FloatVector4: {
                if (value.index() == static_cast<int>(MaterialParameterType::BoolVector4)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::vec4(std::get<7>(value));
                    } else {
                        m_mParams.emplace(name, glm::vec4(std::get<7>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::UIntVector4)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::vec4(std::get<10>(value));
                    } else {
                        m_mParams.emplace(name, glm::vec4(std::get<10>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::IntVector4)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::vec4(std::get<13>(value));
                    } else {
                        m_mParams.emplace(name, glm::vec4(std::get<13>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::FloatVector4)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::vec4(std::get<16>(value));
                    } else {
                        m_mParams.emplace(name, glm::vec4(std::get<16>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::DoubleVector4)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::vec4(std::get<19>(value));
                    } else {
                        m_mParams.emplace(name, glm::vec4(std::get<19>(value)));
                    }
                    return Success();
                }
                return Failure(std::format("Cannot set parameter \"{}\", cannot convert {} type to vec4!",
                    name,
                    rfl::enum_to_string(static_cast<MaterialParameterType>(value.index())))
                );
            }
            case ShaderUniformType::DoubleVector2: {
                if (value.index() == static_cast<int>(MaterialParameterType::BoolVector2)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::dvec2(std::get<5>(value));
                    } else {
                        m_mParams.emplace(name, glm::dvec2(std::get<5>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::UIntVector2)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::dvec2(std::get<8>(value));
                    } else {
                        m_mParams.emplace(name, glm::dvec2(std::get<8>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::IntVector2)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::dvec2(std::get<11>(value));
                    } else {
                        m_mParams.emplace(name, glm::dvec2(std::get<11>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::FloatVector2)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::dvec2(std::get<14>(value));
                    } else {
                        m_mParams.emplace(name, glm::dvec2(std::get<14>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::DoubleVector2)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::dvec2(std::get<17>(value));
                    } else {
                        m_mParams.emplace(name, glm::dvec2(std::get<17>(value)));
                    }
                    return Success();
                }
                return Failure(std::format("Cannot set parameter \"{}\", cannot convert {} type to dvec2!",
                    name,
                    rfl::enum_to_string(static_cast<MaterialParameterType>(value.index())))
                );
            }
            case ShaderUniformType::DoubleVector3: {
                if (value.index() == static_cast<int>(MaterialParameterType::BoolVector3)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::dvec3(std::get<6>(value));
                    } else {
                        m_mParams.emplace(name, glm::dvec3(std::get<6>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::UIntVector3)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::dvec3(std::get<9>(value));
                    } else {
                        m_mParams.emplace(name, glm::dvec3(std::get<9>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::IntVector3)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::dvec3(std::get<12>(value));
                    } else {
                        m_mParams.emplace(name, glm::dvec3(std::get<12>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::FloatVector3)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::dvec3(std::get<15>(value));
                    } else {
                        m_mParams.emplace(name, glm::dvec3(std::get<15>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::DoubleVector3)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::dvec3(std::get<18>(value));
                    } else {
                        m_mParams.emplace(name, glm::dvec3(std::get<18>(value)));
                    }
                    return Success();
                }
                return Failure(std::format("Cannot set parameter \"{}\", cannot convert {} type to dvec3!",
                    name,
                    rfl::enum_to_string(static_cast<MaterialParameterType>(value.index())))
                );
            }
            case ShaderUniformType::DoubleVector4: {
                if (value.index() == static_cast<int>(MaterialParameterType::BoolVector4)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::dvec4(std::get<7>(value));
                    } else {
                        m_mParams.emplace(name, glm::dvec4(std::get<7>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::UIntVector4)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::dvec4(std::get<10>(value));
                    } else {
                        m_mParams.emplace(name, glm::dvec4(std::get<10>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::IntVector4)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::dvec4(std::get<13>(value));
                    } else {
                        m_mParams.emplace(name, glm::dvec4(std::get<13>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::FloatVector4)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::dvec4(std::get<16>(value));
                    } else {
                        m_mParams.emplace(name, glm::dvec4(std::get<16>(value)));
                    }
                    return Success();
                }
                if (value.index() == static_cast<int>(MaterialParameterType::DoubleVector4)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = glm::dvec4(std::get<19>(value));
                    } else {
                        m_mParams.emplace(name, glm::dvec4(std::get<19>(value)));
                    }
                    return Success();
                }
                return Failure(std::format("Cannot set parameter \"{}\", cannot convert {} type to dvec4!",
                    name,
                    rfl::enum_to_string(static_cast<MaterialParameterType>(value.index())))
                );
            }
            case ShaderUniformType::Matrix4: {
                 if (value.index() == static_cast<int>(MaterialParameterType::Matrix4)) {
                     if (m_mParams.contains(name)) {
                         m_mParams.at(name) = value;
                     } else {
                         m_mParams.emplace(name, value);
                     }
                    return Success();
                 }
                return Failure(std::format("Cannot set parameter \"{}\", cannot convert {} type to mat4!",
                    name,
                    rfl::enum_to_string(static_cast<MaterialParameterType>(value.index())))
                );
            }
            case ShaderUniformType::Sampler1D:
            case ShaderUniformType::Sampler1DArray:
            case ShaderUniformType::Sampler2D:
            case ShaderUniformType::Sampler2DArray:
            case ShaderUniformType::Sampler3D:
            case ShaderUniformType::SamplerCube:
            case ShaderUniformType::SamplerCubeArray: {
                if (value.index() == static_cast<int>(MaterialParameterType::Texture)) {
                    if (m_mParams.contains(name)) {
                        m_mParams.at(name) = value;
                    } else {
                        m_mParams.emplace(name, value);
                    }
                    return Success();
                }
                return Failure(std::format("Cannot set parameter \"{}\", cannot convert {} type to {}!",
                    name,
                    rfl::enum_to_string(static_cast<MaterialParameterType>(value.index())),
                    info->type
                ));
            } break;
        }
        return Failure(std::format("Cannot set parameter \"{}\", unsupported parameter type {}!",
            name,
            rfl::enum_to_string(static_cast<MaterialParameterType>(value.index()))
        ));
    }
    Result<> Material::get_parameter(const String& name, MaterialParameterValue& value) const {
        if (const auto info = m_pShader->get_uniform_info(name); !info.has_value()) {
            return Failure(std::format("Shader has no parameter \"{}\"", name));
        }
        if (m_mParams.contains(name)) {
            value = m_mParams.at(name);
            return Success();
        }
        return Failure(std::format("Material has no parameter defined \"{}\"", name));
    }

    void Material::set_opaque(const bool value) {
        m_options.opaque = value;
    }
    void Material::set_backface_culling(const bool value) {
        m_options.backface_culling = value;
    }
    void Material::set_depth_test(const bool value) {
        m_options.depth_test = value;
    }

    Result<> Material::apply() const {
        ShaderPtr shader = m_pShader;
        if (shader == nullptr) {
            shader = Shader::PlaceHolder();
        }

        if (get_backface_culling()) {
            glEnable(GL_CULL_FACE);
        } else {
            glDisable(GL_CULL_FACE);
        }
        if (!get_opaque()) {
            glEnable(GL_BLEND);
        } else {
            glDisable(GL_BLEND);
        }
        if (get_depth_test()) {
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }

        Vector<String> used_uniforms = { };
        shader->use();
        GLuint texture_unit = 0;
        for (const auto& [ name, value ] : m_mParams) {
            switch (static_cast<MaterialParameterType>(value.index())) {
                case MaterialParameterType::Bool: {
                    shader->set_uniform(name, std::get<0>(value));
                } break;
                case MaterialParameterType::UInt: {
                    shader->set_uniform(name, std::get<1>(value));
                } break;
                case MaterialParameterType::Int: {
                    shader->set_uniform(name, std::get<2>(value));
                } break;
                case MaterialParameterType::Float: {
                    shader->set_uniform(name, std::get<3>(value));
                } break;
                case MaterialParameterType::Double: {
                    shader->set_uniform(name, std::get<4>(value));
                } break;
                case MaterialParameterType::BoolVector2: {
                    shader->set_uniform(name, std::get<5>(value));
                } break;
                case MaterialParameterType::BoolVector3: {
                    shader->set_uniform(name, std::get<6>(value));
                } break;
                case MaterialParameterType::BoolVector4: {
                    shader->set_uniform(name, std::get<7>(value));
                } break;
                case MaterialParameterType::UIntVector2: {
                    shader->set_uniform(name, std::get<8>(value));
                } break;
                case MaterialParameterType::UIntVector3: {
                    shader->set_uniform(name, std::get<9>(value));
                } break;
                case MaterialParameterType::UIntVector4: {
                    shader->set_uniform(name, std::get<10>(value));
                } break;
                case MaterialParameterType::IntVector2: {
                    shader->set_uniform(name, std::get<11>(value));
                } break;
                case MaterialParameterType::IntVector3: {
                    shader->set_uniform(name, std::get<12>(value));
                } break;
                case MaterialParameterType::IntVector4: {
                    shader->set_uniform(name, std::get<13>(value));
                } break;
                case MaterialParameterType::FloatVector2: {
                    shader->set_uniform(name, std::get<14>(value));
                } break;
                case MaterialParameterType::FloatVector3: {
                    shader->set_uniform(name, std::get<15>(value));
                } break;
                case MaterialParameterType::FloatVector4: {
                    shader->set_uniform(name, std::get<16>(value));
                } break;
                case MaterialParameterType::DoubleVector2: {
                    shader->set_uniform(name, std::get<17>(value));
                } break;
                case MaterialParameterType::DoubleVector3: {
                    shader->set_uniform(name, std::get<18>(value));
                } break;
                case MaterialParameterType::DoubleVector4: {
                    shader->set_uniform(name, std::get<19>(value));
                } break;
                case MaterialParameterType::Matrix4: {
                    shader->set_uniform(name, std::get<20>(value));
                } break;
                case MaterialParameterType::Texture: {
                    if (texture_unit > 31) {
                        return Failure(std::format("Texture limit reached! Material cannot have more then 32 texture parameters!"));
                    }
                    auto texture = std::get<21>(value);
                    if (!texture->is_valid()) {
                        texture = Texture2D::PlaceHolder();
                    }
                    texture->bind(texture_unit);
                    Debug::Assert(shader->set_uniform(name, static_cast<GLint>(texture_unit)), "Failed to set texture!");
                    ++texture_unit;
                } break;
            }
            used_uniforms.emplace_back(name);
        }

        for (size_t i = 0; i < shader->get_uniform_count(); ++i) {
            if (const auto info = shader->get_uniform_info(i); info.has_value()) {
                if (std::ranges::find(used_uniforms, info->name) == used_uniforms.end()) {
                    if (info->type == ShaderUniformType::Sampler1D ||
                        info->type == ShaderUniformType::Sampler2D ||
                        info->type == ShaderUniformType::Sampler3D ||
                        info->type == ShaderUniformType::SamplerCube ||
                        info->type == ShaderUniformType::Sampler1DArray ||
                        info->type == ShaderUniformType::Sampler2DArray ||
                        info->type == ShaderUniformType::SamplerCubeArray
                    ) {
                        const auto placeholder = Texture::PlaceHolder();
                        placeholder->bind(31);
                        shader->set_uniform(info->name, 31);
                    }
                }
            }
        }

        return Success();
    }

    Result<MaterialPtr> Material::ParseXml(const String& source_asset_path, const String& xml_src, const AssetLoaderFlags::Type flags) {
        pugi::xml_document doc;
        if (const auto result = doc.load_string(xml_src.as_cstr()); result.status != pugi::status_ok) {
            return Failure(std::format("Failed to load material \"{}\": Error while parsing XML document: {}", source_asset_path, result.description()));
        }
        return ParseXml(source_asset_path, doc, flags);
    }
    Result<MaterialPtr> Material::ParseXml(const String& source, const pugi::xml_document& doc, const AssetLoaderFlags::Type flags) {
        if (const auto root = doc.child("Material")) {
            return ParseXml(source, root, flags);
        }
        return Failure(std::format("Failed to load material \"{}\": Expected root node 'Material' in XML document!", source));
    }

    Result<MaterialPtr> Material::ParseXml(const String& source, const pugi::xml_node& root, const AssetLoaderFlags::Type flags) {
        const auto shader_attrib = root.attribute("shader");
        if (!shader_attrib) {
            return Failure(std::format("Failed to load material \"{}\": Expected attribute 'shader' in root node 'Material'!", source));
        }

        const auto shader_sources_result = ShaderLib::GetSourcesForShader(shader_attrib.value());
        if (!shader_sources_result.has_value()) {
            return Failure(std::format("Failed to load material \"{}\": Could not find shader \"{}\"\n{}", source, shader_attrib.value(), shader_sources_result.error().message));
        }

        const auto vert_result = ShaderLib::GetSource(shader_sources_result.value().vertex);
        if (!vert_result.has_value()) {
            return Failure(std::format("Failed to load material \"{}\": Could not find shader source for vertex shader \"{}\": {}", source, shader_sources_result.value().vertex, vert_result.error().message));
        }
        const auto frag_result = ShaderLib::GetSource(shader_sources_result.value().fragment);
        if (!frag_result.has_value()) {
            return Failure(std::format("Failed to load material \"{}\": Could not find shader source for fragment shader \"{}\": {}", source, shader_sources_result.value().fragment, frag_result.error().message));
        }

        const auto shader_result = Shader::Compile(shader_attrib.value(), vert_result.value(), frag_result.value());
        if (!shader_result.has_value()) {
            return Failure(std::format("Failed to load material \"{}\": Failed to load shader \"{}\":\n{}", source, shader_attrib.value(), shader_result.error().message));
        }

        auto shader = std::move(shader_result.value());
        auto params = HashMap<String, MaterialParameterValue>();

        auto opaque_node           = root.child("Opaque");
        auto backface_culling_node = root.child("BackfaceCulling");
        auto depth_test_node       = root.child("DepthTest");

        if (auto params_node = root.child("Parameters")) {
            for (const auto& child : params_node.children()) {
                if (const char* param_name = child.name(); param_name != nullptr) {
                    if (const auto uniform_info = shader->get_uniform_info(child.name()); uniform_info.has_value()) {
                        switch (uniform_info->type) {
                            case ShaderUniformType::Bool: {
                                if (const auto result = StringToBool(child.child_value()); result.has_value()) {
                                    params.emplace(child.name(), result.value());
                                } else {
                                    Debug::LogError(std::format("Ignoring parameter \"{}\" in material \"{}\": Failed to parse data: \"{}\"!", child.name(), source, uniform_info->type, result.error().message));
                                }
                            } break;
                            case ShaderUniformType::BoolVector2: {
                                if (const auto result = StringToBVec2(child.child_value()); result.has_value()) {
                                    params.emplace(child.name(), result.value());
                                } else {
                                    Debug::LogError(std::format("Ignoring parameter \"{}\" in material \"{}\": Failed to parse data: \"{}\"!", child.name(), source, uniform_info->type, result.error().message));
                                }
                            } break;
                            case ShaderUniformType::BoolVector3: {
                                if (const auto result = StringToBVec3(child.child_value()); result.has_value()) {
                                    params.emplace(child.name(), result.value());
                                } else {
                                    Debug::LogError(std::format("Ignoring parameter \"{}\" in material \"{}\": Failed to parse data: \"{}\"!", child.name(), source, uniform_info->type, result.error().message));
                                }
                            } break;
                            case ShaderUniformType::Int: {
                                if (const auto result = StringToInt<GLint>(child.child_value()); result.has_value()) {
                                    params.emplace(child.name(), result.value());
                                } else {
                                    Debug::LogError(std::format("Ignoring parameter \"{}\" in material \"{}\": Failed to parse data: \"{}\"!", child.name(), source, uniform_info->type, result.error().message));
                                }
                            } break;
                            case ShaderUniformType::IntVector2: {
                                if (const auto result = StringToIVec2(child.child_value()); result.has_value()) {
                                    params.emplace(child.name(), result.value());
                                } else {
                                    Debug::LogError(std::format("Ignoring parameter \"{}\" in material \"{}\": Failed to parse data: \"{}\"!", child.name(), source, uniform_info->type, result.error().message));
                                }
                            } break;
                            case ShaderUniformType::IntVector3: {
                                if (const auto result = StringToIVec3(child.child_value()); result.has_value()) {
                                    params.emplace(child.name(), result.value());
                                } else {
                                    Debug::LogError(std::format("Ignoring parameter \"{}\" in material \"{}\": Failed to parse data: \"{}\"!", child.name(), source, uniform_info->type, result.error().message));
                                }
                            } break;
                            case ShaderUniformType::IntVector4: {
                                if (const auto result = StringToIVec4(child.child_value()); result.has_value()) {
                                    params.emplace(child.name(), result.value());
                                } else {
                                    Debug::LogError(std::format("Ignoring parameter \"{}\" in material \"{}\": Failed to parse data: \"{}\"!", child.name(), source, uniform_info->type, result.error().message));
                                }
                            } break;
                            case ShaderUniformType::UInt: {
                                if (const auto result = StringToInt<GLuint>(child.child_value()); result.has_value()) {
                                    params.emplace(child.name(), result.value());
                                } else {
                                    Debug::LogError(std::format("Ignoring parameter \"{}\" in material \"{}\": Failed to parse data: \"{}\"!", child.name(), source, uniform_info->type, result.error().message));
                                }
                            } break;
                            case ShaderUniformType::UIntVector2: {
                                if (const auto result = StringToUVec2(child.child_value()); result.has_value()) {
                                    params.emplace(child.name(), result.value());
                                } else {
                                    Debug::LogError(std::format("Ignoring parameter \"{}\" in material \"{}\": Failed to parse data: \"{}\"!", child.name(), source, uniform_info->type, result.error().message));
                                }
                            } break;
                            case ShaderUniformType::UIntVector3: {
                                if (const auto result = StringToUVec3(child.child_value()); result.has_value()) {
                                    params.emplace(child.name(), result.value());
                                } else {
                                    Debug::LogError(std::format("Ignoring parameter \"{}\" in material \"{}\": Failed to parse data: \"{}\"!", child.name(), source, uniform_info->type, result.error().message));
                                }
                            } break;
                            case ShaderUniformType::UIntVector4: {
                                if (const auto result = StringToUVec4(child.child_value()); result.has_value()) {
                                    params.emplace(child.name(), result.value());
                                } else {
                                    Debug::LogError(std::format("Ignoring parameter \"{}\" in material \"{}\": Failed to parse data: \"{}\"!", child.name(), source, uniform_info->type, result.error().message));
                                }
                            } break;
                            case ShaderUniformType::Float: {
                                if (const auto result = StringToFloat<GLfloat>(child.child_value()); result.has_value()) {
                                    params.emplace(child.name(), result.value());
                                } else {
                                    Debug::LogError(std::format("Ignoring parameter \"{}\" in material \"{}\": Failed to parse data: \"{}\"!", child.name(), source, uniform_info->type, result.error().message));
                                }
                            } break;
                            case ShaderUniformType::FloatVector2: {
                                if (const auto result = StringToVec2(child.child_value()); result.has_value()) {
                                    params.emplace(child.name(), result.value());
                                } else {
                                    Debug::LogError(std::format("Ignoring parameter \"{}\" in material \"{}\": Failed to parse data: \"{}\"!", child.name(), source, uniform_info->type, result.error().message));
                                }
                            } break;
                            case ShaderUniformType::FloatVector3: {
                                if (const auto result = StringToVec3(child.child_value()); result.has_value()) {
                                    params.emplace(child.name(), result.value());
                                } else {
                                    Debug::LogError(std::format("Ignoring parameter \"{}\" in material \"{}\": Failed to parse data: \"{}\"!", child.name(), source, uniform_info->type, result.error().message));
                                }
                            } break;
                            case ShaderUniformType::FloatVector4: {
                                if (const auto value = String(child.child_value()).clone_trimmed(); value.starts_with('#')) {
                                    if (const auto result = StringToInt<uint32_t>(value); result.has_value()) {
                                        const uint32_t rgba = result.value();
                                        params.emplace(child.name(), glm::vec4 {
                                                           ((rgba >> 24) & 0xFF) / 255.0f,
                                                           ((rgba >> 16) & 0xFF) / 255.0f,
                                                           ((rgba >> 8 ) & 0xFF) / 255.0f,
                                                           (rgba & 0xFF)         / 255.0f
                                                       });
                                    } else {
                                        Debug::LogError(std::format("Ignoring parameter \"{}\" in material \"{}\": Failed to parse data: \"{}\"!", child.name(), source, uniform_info->type, result.error().message));
                                    }
                                } else {
                                    if (const auto result = StringToVec4(value); result.has_value()) {
                                        params.emplace(child.name(), result.value());
                                    } else {
                                        Debug::LogError(std::format("Ignoring parameter \"{}\" in material \"{}\": Failed to parse data: \"{}\"!", child.name(), source, uniform_info->type, result.error().message));
                                    }
                                }
                            } break;
                            case ShaderUniformType::Double: {
                                if (const auto result = StringToFloat<GLdouble>(child.child_value()); result.has_value()) {
                                    params.emplace(child.name(), result.value());
                                } else {
                                    Debug::LogError(std::format("Ignoring parameter \"{}\" in material \"{}\": Failed to parse data: \"{}\"!", child.name(), source, uniform_info->type, result.error().message));
                                }
                            } break;
                            case ShaderUniformType::DoubleVector2: {
                                if (const auto result = StringToDVec2(child.child_value()); result.has_value()) {
                                    params.emplace(child.name(), result.value());
                                } else {
                                    Debug::LogError(std::format("Ignoring parameter \"{}\" in material \"{}\": Failed to parse data: \"{}\"!", child.name(), source, uniform_info->type, result.error().message));
                                }
                            } break;
                            case ShaderUniformType::DoubleVector3: {
                                if (const auto result = StringToDVec3(child.child_value()); result.has_value()) {
                                    params.emplace(child.name(), result.value());
                                } else {
                                    Debug::LogError(std::format("Ignoring parameter \"{}\" in material \"{}\": Failed to parse data: \"{}\"!", child.name(), source, uniform_info->type, result.error().message));
                                }
                            } break;
                            case ShaderUniformType::DoubleVector4: {
                                if (const auto result = StringToDVec4(child.child_value()); result.has_value()) {
                                    params.emplace(child.name(), result.value());
                                } else {
                                    Debug::LogError(std::format("Ignoring parameter \"{}\" in material \"{}\": Failed to parse data: \"{}\"!", child.name(), source, uniform_info->type, result.error().message));
                                }
                            } break;
                            case ShaderUniformType::Matrix4: {
                                if (const auto result = StringToMat4(child.child_value()); result.has_value()) {
                                    params.emplace(child.name(), result.value());
                                } else {
                                    Debug::LogError(std::format("Ignoring parameter \"{}\" in material \"{}\": Failed to parse data: \"{}\"!", child.name(), source, uniform_info->type, result.error().message));
                                }
                            } break;
                            case ShaderUniformType::Sampler2D: {
                                const char* texture_path = child.child_value();
                                if (strcmp(texture_path, "$DEFAULT_WHITE") == 0) {
                                    params.emplace(child.name(), Texture::DefaultWhite());
                                } else if (strcmp(texture_path, "$DEFAULT_BLACK") == 0) {
                                    params.emplace(child.name(), Texture::DefaultBlack());
                                } else if (strcmp(texture_path, "$DEFAULT_NORMAL") == 0) {
                                    params.emplace(child.name(), Texture::DefaultNormal());
                                } else if (strcmp(texture_path, "$PLACEHOLDER") == 0) {
                                    params.emplace(child.name(), Texture::PlaceHolder());
                                } else {
                                    const auto texture_result = Assets::Load<Texture2D>(texture_path, flags);
                                    if (texture_result.has_value()) {
                                        params.emplace(child.name(), texture_result.value().ptr());
                                    } else {
                                        params.emplace(child.name(), Texture::PlaceHolder());
                                        Debug::LogError(std::format("Failed to load texture \"{}\" for parameter \"{}\":\n", child.child_value(), source, texture_result.error().message));
                                    }
                                }
                            } break;
                            case ShaderUniformType::Sampler2DArray: {
                                const char* texture_path = child.child_value();
                                if (strcmp(texture_path, "$DEFAULT_WHITE") == 0) {
                                    params.emplace(child.name(), Texture::DefaultWhite());
                                } else if (strcmp(texture_path, "$DEFAULT_BLACK") == 0) {
                                    params.emplace(child.name(), Texture::DefaultBlack());
                                } else if (strcmp(texture_path, "$DEFAULT_NORMAL") == 0) {
                                    params.emplace(child.name(), Texture::DefaultNormal());
                                } else if (strcmp(texture_path, "$PLACEHOLDER") == 0) {
                                    params.emplace(child.name(), Texture::PlaceHolder());
                                } else {
                                    const auto texture_result = Assets::Load<Texture2DArray>(texture_path, flags);
                                    if (texture_result.has_value()) {
                                        params.emplace(child.name(), texture_result.value().ptr());
                                    } else {
                                        params.emplace(child.name(), Texture::PlaceHolder());
                                        Debug::LogError(std::format("Failed to load texture \"{}\" for parameter \"{}\":\n{}", child.child_value(), source, texture_result.error().message));
                                    }
                                }
                            } break;
                            case ShaderUniformType::SamplerCube: {
                                const char* texture_path = child.child_value();
                                if (strcmp(texture_path, "$DEFAULT_WHITE") == 0) {
                                    params.emplace(child.name(), Texture::DefaultWhite());
                                } else if (strcmp(texture_path, "$DEFAULT_BLACK") == 0) {
                                    params.emplace(child.name(), Texture::DefaultBlack());
                                } else if (strcmp(texture_path, "$DEFAULT_NORMAL") == 0) {
                                    params.emplace(child.name(), Texture::DefaultNormal());
                                } else if (strcmp(texture_path, "$PLACEHOLDER") == 0) {
                                    params.emplace(child.name(), Texture::PlaceHolder());
                                } else {
                                    const auto texture_result = Assets::Load<TextureCubeMap>(texture_path, flags);
                                    if (texture_result.has_value()) {
                                        params.emplace(child.name(), texture_result.value().ptr());
                                    } else {
                                        params.emplace(child.name(), Texture::PlaceHolder());
                                        Debug::LogError(std::format("Failed to load texture \"{}\" for parameter \"{}\":\n{}", child.child_value(), source, texture_result.error().message));
                                    }
                                }
                            } break;
                            default: {
                                Debug::LogError(std::format("Ignoring parameter \"{}\" in material \"{}\": Uniform type \"{}\" is not supported!", child.name(), source, uniform_info->type));
                            } break;
                        }
                    } else {
                        Debug::LogWarning(std::format("Ignoring parameter \"{}\" in material \"{}\": Shader \"{}\" has no such uniform!", child.name(), source, shader_attrib.value()));
                    }
                } else {
                    Debug::LogError(std::format("Material XML document \"{}\" is potentially corrupted!", source));

                }
            }
        }
        auto mat = std::make_shared<Material>(shader, params);
        if (opaque_node) {
            const auto value = StringToBool(opaque_node.child_value());
            if (!value.has_value()) {
                return Failure(std::format("Failed to load material \"{}\": Expected boolean value for node \"Opaque\"", source));
            }
            mat->set_opaque(value.value());
        }
        if (backface_culling_node) {
            const auto value = StringToBool(backface_culling_node.child_value());
            if (!value.has_value()) {
                return Failure(std::format("Failed to load material \"{}\": Expected boolean value for node \"BackfaceCulling\"", source));
            }
            mat->set_backface_culling(value.value());
        }
        if (depth_test_node) {
            const auto value = StringToBool(depth_test_node.child_value());
            if (!value.has_value()) {
                return Failure(std::format("Failed to load material \"{}\": Expected boolean value for node \"DepthTest\"", source));
            }
            mat->set_depth_test(value.value());
        }
        return Success<MaterialPtr>(std::move(mat));
    }

    Result<MaterialPtr> Material::LoadAsset(const Path& path, const AssetLoaderFlags::Type flags) {
        if (!path.extension().equals(".xml", StringCompareType::CaseInsensitive)) {
            return Failure(std::format("Failed to load material \"{}\": Expected asset extension '.xml'", path));
        }

        const auto doc = Assets::LoadAsXml(path, flags);
        if (!doc.has_value()) {
            return Failure(std::format("Failed to load material \"{}\": {}", path, doc.error().message));
        }
        return ParseXml(path.as_string(), doc.value(), flags);
    }

    const Material Material::Null = { };
}
