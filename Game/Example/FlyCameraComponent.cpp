#include "FlyCameraComponent.hpp"

using namespace fow;

void FlyCameraComponent::on_spawn() {
    FOW_ASSERT_COMPONENT_DEPENDENCY_FATAL(FlyCameraComponent, TransformComponent);
    FOW_ASSERT_COMPONENT_DEPENDENCY_FATAL(FlyCameraComponent, CameraComponent);

    m_pCamera = entity().get_component<CameraComponent>();
}
void FlyCameraComponent::on_update(const double dt) {
    
}
