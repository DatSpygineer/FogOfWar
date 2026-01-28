#ifndef FOW_TRANSFORM_HPP
#define FOW_TRANSFORM_HPP

#include "fow/Shared/Api.hpp"
#include "fow/Shared/MathHelper.hpp"

namespace fow {
    class FOW_SHARED_API Transform {
        glm::vec3 m_position, m_scale;
        glm::quat m_rotation;
        Transform* m_pParent;
    public:
        constexpr Transform() :
            m_position(glm::vec3 { 0.0f }), m_scale(glm::vec3 { 1.0f }), m_rotation(glm::quat { 1.0f, 0.0f, 0.0f, 0.0f }), m_pParent(nullptr) { }
        constexpr Transform(const glm::vec3& position, const glm::vec3& scale, const glm::quat& rotation, Transform* parent = nullptr) :
            m_position(position), m_scale(scale), m_rotation(rotation), m_pParent(parent) { }
        constexpr Transform(const Transform& transform) = default;
        constexpr Transform(Transform&& transform) noexcept = default;
        constexpr Transform(const Transform& transform, Transform* parent) : Transform(transform) {
            m_pParent = parent;
        }
        constexpr Transform(Transform&& transform, Transform* parent) : Transform(std::forward<Transform>(transform)) {
            m_pParent = parent;
        }

        Transform with_parent(Transform* parent) const;

        [[nodiscard]] constexpr const Transform* get_parent() const { return m_pParent; }
        void set_parent(Transform* parent);
        [[nodiscard]] constexpr glm::vec3 get_position() const {
            return m_pParent != nullptr ? (m_pParent->get_position() + m_position) : m_position;
        }
        [[nodiscard]] constexpr glm::vec3 get_local_position() const {
            return m_position;
        }
        [[nodiscard]] constexpr glm::vec3 get_scale() const {
            return m_pParent != nullptr ? (m_pParent->get_scale() + m_scale) : m_scale;
        }
        [[nodiscard]] constexpr glm::vec3 get_local_scale() const {
            return m_scale;
        }
        [[nodiscard]] constexpr glm::quat get_rotation() const {
            return m_pParent != nullptr ? (m_pParent->get_rotation() + m_rotation) : m_rotation;
        }
        [[nodiscard]] constexpr glm::quat get_local_rotation() const {
            return m_rotation;
        }

        void set_local_position(const glm::vec3& position);
        void set_local_scale(const float scale) {
            set_local_scale(glm::vec3(scale));
        }
        void set_local_scale(const glm::vec3& scale);
        void set_local_rotation(const glm::vec3& euler_rotation);
        inline void set_local_rotation_deg(const glm::vec3& euler_rotation) {
            return set_local_rotation(glm::radians(euler_rotation));
        }
        void set_local_rotation(const glm::vec3& axis, float angle);
        inline void set_local_rotation_deg(const glm::vec3& axis, const float angle) {
            return set_local_rotation(axis, glm::radians(angle));
        }
        void set_local_rotation(const glm::quat& rotation);

        void set_position(const glm::vec3& position);
        inline void set_scale(const float scale) {
            set_scale(glm::vec3(scale));
        }
        void set_scale(const glm::vec3& scale);
        inline void set_rotation(const glm::vec3& euler_rotation) {
            set_rotation(glm::quat(euler_rotation));
        }
        inline void set_rotation_deg(const glm::vec3& euler_rotation) {
            return set_rotation(glm::radians(euler_rotation));
        }
        inline void set_rotation(const glm::vec3& axis, float angle) {
            set_rotation(glm::angleAxis(angle, axis));
        }
        inline void set_rotation_deg(const glm::vec3& axis, const float angle) {
            return set_rotation(axis, glm::radians(angle));
        }
        void set_rotation(const glm::quat& rotation);

        [[nodiscard]] constexpr glm::mat4 matrix() const {
            auto matrix = glm::mat4 { 1.0f };
            matrix = glm::translate(matrix, get_position());
            matrix *= glm::toMat4(get_rotation());
            matrix = glm::scale(matrix, get_scale());
            return matrix;
        }
    };
}

#endif