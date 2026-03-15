#ifndef FOW_TRANSFORM_HPP
#define FOW_TRANSFORM_HPP

#include "fow/Shared/Api.hpp"
#include "fow/Shared/MathHelper.hpp"

namespace fow {
    class FOW_SHARED_API Transform {
        Vector3 m_position, m_scale;
        Quat m_rotation;
        Transform* m_pParent;
    public:
        FOW_CONSTEXPR Transform() :
            m_position(Vector3 { 0.0f }), m_scale(Vector3 { 1.0f }), m_rotation(Quat { 1.0f, 0.0f, 0.0f, 0.0f }), m_pParent(nullptr) { }
        FOW_CONSTEXPR Transform(const Vector3& position, const Vector3& scale, const Quat& rotation, Transform* parent = nullptr) :
            m_position(position), m_scale(scale), m_rotation(rotation), m_pParent(parent) { }
        FOW_CONSTEXPR Transform(const Transform& transform) = default;
        FOW_CONSTEXPR Transform(Transform&& transform) noexcept = default;
        FOW_CONSTEXPR Transform(const Transform& transform, Transform* parent) : Transform(transform) {
            m_pParent = parent;
        }
        FOW_CONSTEXPR Transform(Transform&& transform, Transform* parent) : Transform(std::forward<Transform>(transform)) {
            m_pParent = parent;
        }

        Transform& operator=(Transform&& transform) noexcept = default;
        Transform& operator=(const Transform& transform) = default;

        Transform with_parent(Transform* parent) const;

        [[nodiscard]] FOW_CONSTEXPR const Transform* get_parent() const { return m_pParent; }
        void set_parent(Transform* parent);
        [[nodiscard]] FOW_CONSTEXPR Vector3 get_position() const {
            return m_pParent != nullptr ? (m_pParent->get_position() + m_position) : m_position;
        }
        [[nodiscard]] FOW_CONSTEXPR Vector3 get_local_position() const {
            return m_position;
        }
        [[nodiscard]] FOW_CONSTEXPR Vector3 get_scale() const {
            return m_pParent != nullptr ? (m_pParent->get_scale() + m_scale) : m_scale;
        }
        [[nodiscard]] FOW_CONSTEXPR Vector3 get_local_scale() const {
            return m_scale;
        }
        [[nodiscard]] FOW_CONSTEXPR Quat get_rotation() const {
            return m_pParent != nullptr ? (m_pParent->get_rotation() + m_rotation) : m_rotation;
        }
        [[nodiscard]] FOW_CONSTEXPR Quat get_local_rotation() const {
            return m_rotation;
        }

        void set_local_position(const Vector3& position);
        void set_local_scale(const float scale) {
            set_local_scale(Vector3(scale));
        }
        void set_local_scale(const Vector3& scale);
        void set_local_rotation(const Vector3& euler_rotation);
        inline void set_local_rotation_deg(const Vector3& euler_rotation) {
            return set_local_rotation(glm::radians(euler_rotation));
        }
        void set_local_rotation(const Vector3& axis, float angle);
        inline void set_local_rotation_deg(const Vector3& axis, const float angle) {
            return set_local_rotation(axis, glm::radians(angle));
        }
        void set_local_rotation(const Quat& rotation);

        void set_position(const Vector3& position);
        inline void set_scale(const float scale) {
            set_scale(Vector3(scale));
        }
        void set_scale(const Vector3& scale);
        inline void set_rotation(const Vector3& euler_rotation) {
            set_rotation(Quat(euler_rotation));
        }
        inline void set_rotation_deg(const Vector3& euler_rotation) {
            return set_rotation(glm::radians(euler_rotation));
        }
        inline void set_rotation(const Vector3& axis, float angle) {
            set_rotation(glm::angleAxis(angle, axis));
        }
        inline void set_rotation_deg(const Vector3& axis, const float angle) {
            return set_rotation(axis, glm::radians(angle));
        }
        void set_rotation(const Quat& rotation);

        [[nodiscard]] FOW_CONSTEXPR Matrix4 matrix() const {
            auto matrix = Matrix4 { 1.0f };
            matrix = glm::translate(matrix, get_position());
            matrix *= glm::toMat4(get_rotation());
            matrix = glm::scale(matrix, get_scale());
            return matrix;
        }
    };
}

#endif