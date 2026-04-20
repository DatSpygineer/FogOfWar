#include "fow/Renderer/GL.hpp"
#include "fow/Renderer.hpp"
#include "fow/Renderer/ShaderLib.hpp"

namespace fow {
    namespace Renderer {
        static auto s_viewport = Rectangle { 0.0f, 0.0f, 1280.0f, 720.0f };
        static auto s_view_matrix = Matrix4 { 1.0f };
        static auto s_proj_matrix = Matrix4 { 1.0f };
        static auto s_proj_matrix_2d = Matrix4 { 1.0f };
        static Path s_base_path = Path::CurrentDir();
        static bool s_initialized = false;
        static auto s_camera_position = Vector3 { 0.0f, 0.0f, 0.0f };
        static auto s_camera_target   = Vector3 { 0.0f, 0.0f, 0.0f };
        static auto s_camera_up       = Vector3 { 0.0f, 1.0f, 0.0f };
        static auto s_camera_forward  = Vector3 { 0.0f, 0.0f, 1.0f };
        static TTF_TextEngine* s_pTextEngine = nullptr;
        static FT_Library s_pFontLibrary = nullptr;
        static FontPtr s_pDefaultFont = nullptr;

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

            s_pTextEngine = TTF_CreateSurfaceTextEngine();
            if (s_pTextEngine == nullptr) {
                ShaderLib::Unload();
                return Failure(std::format("Failed to initialize TextEngine: {}", SDL_GetError()));
            }

            if (const auto error = FT_Init_FreeType(&s_pFontLibrary); error != FT_Err_Ok) {
                return Failure(std::format("Failed to initialize FreeType: {}", FT_Error_String(error)));
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
            if (s_pTextEngine != nullptr) {
                TTF_DestroySurfaceTextEngine(s_pTextEngine);
                s_pTextEngine = nullptr;
            }
            if (s_pFontLibrary != nullptr) {
                FT_Done_FreeType(s_pFontLibrary);
                s_pFontLibrary = nullptr;
            }
            Debug::FreeDebugMesh();
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
            s_camera_position = s_camera_target = position;
            s_view_matrix = glm::translate(s_view_matrix, position);
        }
        void UpdateCameraPosition(const Vector3& position, const Vector3& target, const Vector3& up) {
            s_camera_position = position;
            s_camera_target = target;
            s_camera_up = up;
            s_camera_forward = glm::normalize(target - position);
            s_view_matrix = glm::lookAt(position, target, up);
        }
        void UpdateCameraPosition(const Vector3& position, const Vector3& forward, const Vector3& up, const Quat& rotation) {
            s_camera_position = position;
            s_camera_forward = forward;
            s_camera_up = up;
            s_camera_target = position + glm::rotate(rotation, forward);
            s_view_matrix = glm::lookAt(position, position + glm::rotate(rotation, forward), up);
        }

        Matrix4 GetViewMatrix() {
            return s_view_matrix;
        }
        Matrix4 GetProjectionMatrix() {
            return s_proj_matrix;
        }
        Matrix4 GetProjectionMatrix2D() {
            return s_proj_matrix_2d;
        }
        void SetViewport(const Rectangle& rect) {
            s_viewport = rect;
            s_proj_matrix_2d = glm::ortho(s_viewport.x, s_viewport.x + s_viewport.width, s_viewport.y + s_viewport.height, s_viewport.y);
            glViewport(static_cast<GLint>(s_viewport.x), static_cast<GLint>(s_viewport.y), static_cast<GLint>(s_viewport.width), static_cast<GLint>(s_viewport.height));
        }
        void SetViewport(const float x, const float y, const float width, const float height) {
            SetViewport(Rectangle { x, y, width, height });
        }
        Rectangle GetViewport() {
            return s_viewport;
        }
        void Clear(const Color& color) {
            glClearColor(color.r, color.g, color.b, color.a);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        void SetDefaultFont(const Path& font_path, float size) {
            if (s_pDefaultFont != nullptr) {
                Debug::Assert(s_pDefaultFont->change_font(font_path, size));
            } else {
                s_pDefaultFont = CreateRef<Font>(font_path, size);
                Debug::Assert(s_pDefaultFont == nullptr || s_pDefaultFont->is_valid(), std::format("Failed to load font \"{}\"!", font_path));
            }
        }
        FontPtr GetDefaultFont() {
            return s_pDefaultFont;
        }

        Vector3 GetCameraPosition() {
            return s_camera_position;
        }
        Vector3 GetCameraTarget() {
            return s_camera_target;
        }
        Vector3 GetCameraForward() {
            return s_camera_forward;
        }
        Vector3 GetCameraUp() {
            return s_camera_up;
        }

        FT_Library FontLibrary() {
            return s_pFontLibrary;
        }

        TTF_TextEngine* TextEngine() {
            return s_pTextEngine;
        }
    }
}
