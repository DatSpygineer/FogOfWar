#include "fow/Renderer.hpp"

#include <glm/gtx/transform.hpp>

namespace fow {
    namespace Renderer {
        static auto s_viewport = Rectangle { 0.0f, 0.0f, 1280.0f, 720.0f };
        static auto s_view_matrix = glm::mat4 { 1.0f };
        static auto s_proj_matrix = glm::mat4 { 1.0f };
        static Path s_base_path = Path::CurrentDir();
        static bool s_initialized = false;
        static FT_Library s_ft_lib = nullptr;

        Result<> Initialize(const Path& app_base_path) {
            if (s_initialized) {
                return Failure("Failed to initialize renderer: already initialized");
            }

            if (s_ft_lib == nullptr) {
                if (FT_Init_FreeType(&s_ft_lib) != FT_Err_Ok) {
                    return Failure(std::format("Could not initialize FreeType library!"));
                }
            }
            s_base_path = app_base_path;
            s_initialized = true;

            return Success();
        }
        void Terminate() {
            if (s_ft_lib != nullptr) {
                FT_Done_FreeType(s_ft_lib);
                s_ft_lib = nullptr;
            }
        }

        FT_Library GetFreetypeLibrary() {
            return s_ft_lib;
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

        void UpdateCameraProjectionMatrix(const glm::mat4& matrix) {
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
        void UpdateCameraProjectionPerspective(const float fov, const glm::vec2& size, const float near, const float far) {
            s_proj_matrix = glm::perspective(glm::radians(fov), size.x / size.y, near, far);
        }

        void UpdateCameraViewMatrix(const glm::mat4& matrix) {
            s_view_matrix = matrix;
        }

        void UpdateCameraPositionSimple(const glm::vec3& position) {
            s_view_matrix = glm::translate(s_view_matrix, position);
        }
        void UpdateCameraPosition(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up) {
            s_view_matrix = glm::lookAt(position, target, up);
        }
        void UpdateCameraPosition(const glm::vec3& position, const glm::vec3& forward, const glm::vec3& up, const glm::quat& rotation) {
            s_view_matrix = glm::lookAt(position, position + glm::rotate(rotation, forward), up);
        }

        glm::mat4 GetViewMatrix() {
            return s_view_matrix;
        }
        glm::mat4 GetProjectionMatrix() {
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
    }
}
