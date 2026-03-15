#include "fow/Renderer/GL.hpp"
#include "fow/Renderer.hpp"
#include "fow/Renderer/ShaderLib.hpp"

#include <glm/gtx/transform.hpp>

namespace fow {
    namespace Renderer {
        static auto s_viewport = Rectangle { 0.0f, 0.0f, 1280.0f, 720.0f };
        static auto s_view_matrix = Matrix4 { 1.0f };
        static auto s_proj_matrix = Matrix4 { 1.0f };
        static Path s_base_path = Path::CurrentDir();
        static bool s_initialized = false;

        static Result<> InitializeShared(const Path& app_base_path, const int msaa, const Function<Result<>()>& loader) {
            if (s_initialized) {
                return Failure("Failed to initialize renderer: already initialized");
            }

            s_base_path = app_base_path;
            if (const auto result = loader(); !result.has_value()) {
                return result;
            }
            if (const auto result = ShaderLib::Load(s_base_path); !result.has_value()) {
                return result;
            }

            Debug::LogInfo(std::format("Initialized OpenGL v{}", reinterpret_cast<const char*>(glGetString(GL_VERSION))));

            if (msaa > 0) {
                glEnable(GL_MULTISAMPLE);
            } else {
                glDisable(GL_MULTISAMPLE);
            }

            glEnable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);

            s_initialized = true;

            return Success();
        }

        Result<> Initialize(const Path& app_base_path, const int msaa, void* (*loader)(const char*)) {
            return InitializeShared(app_base_path, msaa, [&loader]() -> Result<> {
#ifdef _WIN32
                if (const auto err = glewInit(); err != GLEW_OK) {
                    return Failure(std::format("Failed to initialize OpenGL: GL Error {}", reinterpret_cast<const char*>(glewGetErrorString(err))));
                }
#else
                if (loader != nullptr) {
                    if (!gladLoadGLLoader(loader)) {
                        return Failure("Failed to initialize OpenGL!");
                    }
                }
#endif
                return Success();
            });
        }

        Result<> InitializeForEditor(const Path& app_base_path, const int msaa) {
            return InitializeShared(app_base_path, msaa, []() -> Result<> {
#ifdef _WIN32
                if (const auto err = glewInit(); err != GLEW_OK) {
                    return Failure(std::format("Failed to initialize OpenGL: GL Error {}", reinterpret_cast<const char*>(glewGetErrorString(err))));
                }
#else
                if (!gladLoadGL()) {
                        return Failure("Failed to initialize OpenGL!");
                }
#endif
                return Success();
            });
        }

        void Terminate() {
            ShaderLib::Unload();
        }

        Path GetBasePath() {
            return s_base_path;
        }

        void EnableBlend(const bool enabled, BlendFactor src, BlendFactor dst) {
            if (enabled) {
                glEnable(GL_BLEND);
                glBlendFunc(static_cast<GLenum>(src), static_cast<GLenum>(dst));
            } else {
                glDisable(GL_BLEND);
            }
        }

        void UpdateCameraProjectionMatrix(const Matrix4& matrix) {
            s_proj_matrix = matrix;
        }
        void UpdateCameraProjectionOrtho(const float x, const float y, const float width, const float height, const float near, const float far) {
            s_proj_matrix = glm::ortho(x, x + width, y, y + height, near, far);
        }
        void UpdateCameraProjectionOrtho(const Rectangle& rect, const float near, const float far) {
            s_proj_matrix = glm::ortho(rect.x, rect.x + rect.width, rect.y, rect.y + rect.height, near, far);
        }
        void UpdateCameraProjectionOrtho(const float x, const float y, const float width, const float height) {
            s_proj_matrix = glm::ortho(x, x + width, y, y + height);
        }
        void UpdateCameraProjectionOrtho(const Rectangle& rect) {
            s_proj_matrix = glm::ortho(rect.x, rect.x + rect.width, rect.y, rect.y + rect.height);
        }
        void UpdateCameraProjectionPerspective(const float fov, const float width, const float height, const float near, const float far) {
            s_proj_matrix = glm::perspective(glm::radians(fov), width / height, near, far);
        }
        void UpdateCameraProjectionPerspective(const float fov, const Vector2& size, const float near, const float far) {
            s_proj_matrix = glm::perspective(glm::radians(fov), size.x / size.y, near, far);
        }

        void UpdateCameraViewMatrix(const Matrix4& matrix) {
            s_view_matrix = matrix;
        }

        void UpdateCameraPositionSimple(const Vector3& position) {
            s_view_matrix = glm::translate(s_view_matrix, position);
        }
        void UpdateCameraPosition(const Vector3& position, const Vector3& target, const Vector3& up) {
            s_view_matrix = glm::lookAt(position, target, up);
        }
        void UpdateCameraPosition(const Vector3& position, const Vector3& forward, const Vector3& up, const Quat& rotation) {
            s_view_matrix = glm::lookAt(position, position + glm::rotate(rotation, forward), up);
        }

        Matrix4 GetViewMatrix() {
            return s_view_matrix;
        }
        Matrix4 GetProjectionMatrix() {
            return s_proj_matrix;
        }
        void SetViewport(const Rectangle& rect) {
            s_viewport = rect;
        }
        void SetViewport(const float x, const float y, const float width, const float height) {
            s_viewport = Rectangle { x, y, width, height };
            glViewport(static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLint>(width), static_cast<GLint>(height));
        }
        Rectangle GetViewport() {
            return s_viewport;
        }
        void Clear(const Color& color) {
            glClearColor(color.r, color.g, color.b, color.a);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
    }
}
