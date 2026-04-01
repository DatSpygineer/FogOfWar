#include "Components.hpp"

using namespace fow;

void FlyCameraComponent::on_spawn() {
    FOW_ASSERT_COMPONENT_DEPENDENCY_FATAL(FlyCameraComponent, TransformComponent);
    FOW_ASSERT_COMPONENT_DEPENDENCY_FATAL(FlyCameraComponent, CameraComponent);

    m_pCamera = entity().get_component<CameraComponent>();
}
void FlyCameraComponent::on_update(const double dt) {
}

void TestSphereComponent::on_spawn() {
    FOW_ASSERT_COMPONENT_DEPENDENCY_FATAL(TestSphereComponent, TransformComponent);
    m_pTransform = entity().get_component<TransformComponent>();
}

void TestSphereComponent::on_update(const double dt) {
    const auto dir_mul = m_iDirection == 0 ? 1.0f : -1.0f;
    m_pTransform->transform().set_position(Lerp(Vector3(-2.0 * dir_mul, 0.0, -5.0), Vector3(2.0 * dir_mul, 0.0, -5.0), m_fProgress));

    if (m_fProgress >= 1.0f) {
        m_iDirection = 1 - m_iDirection;
        m_fProgress = 0.0f;
    } else {
        m_fProgress += static_cast<float>(dt) * 0.5f;
    }
}
