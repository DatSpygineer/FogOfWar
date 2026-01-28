#include "fow/Shared/Transform.hpp"

namespace fow {
    Transform Transform::with_parent(Transform* parent) const {
        Transform result = { *this };
        Transform* root = &result;
        while (root->m_pParent != nullptr) {
            root = root->m_pParent;
        }
        root->m_pParent = parent;
        return result;
    }

    void Transform::set_parent(Transform* parent) {
        m_pParent = parent;
    }
    void Transform::set_local_position(const glm::vec3& position) {
        m_position = position;
    }
    void Transform::set_local_scale(const glm::vec3& scale) {
        m_scale = scale;
    }
    void Transform::set_local_rotation(const glm::vec3& euler_rotation) {
        m_rotation = glm::quat(euler_rotation);
    }
    void Transform::set_local_rotation(const glm::vec3& axis, const float angle) {
        m_rotation = glm::angleAxis(angle, axis);
    }
    void Transform::set_local_rotation(const glm::quat& rotation) {
        m_rotation = rotation;
    }

    void Transform::set_position(const glm::vec3& position) {
        m_position = position;
        if (m_pParent != nullptr) {
            m_position -= m_pParent->get_position();
        }
    }
    void Transform::set_scale(const glm::vec3& scale) {
        m_scale = scale;
        if (m_pParent != nullptr) {
            m_scale -= m_pParent->get_scale();
        }
    }
    void Transform::set_rotation(const glm::quat& rotation) {
        m_rotation = rotation;
        if (m_pParent != nullptr) {
            m_rotation -= rotation;
        }
    }
}
