#include "Components.hpp"

using namespace fow;

#define CAMERA_MOVE_SPEED_DEFAULT 10.0f
#define CAMERA_LOOK_SPEED_DEFAULT 0.5f

static CVarPtr pl_cam_speed = CVar::Create("pl_cam_speed", CAMERA_MOVE_SPEED_DEFAULT, CVarFlags::Default);
static CVarPtr pl_mouse_speed = CVar::Create("pl_mouse_speed", CAMERA_LOOK_SPEED_DEFAULT, CVarFlags::Default);

void FlyCameraComponent::on_spawn() {
    FOW_ASSERT_COMPONENT_DEPENDENCY_FATAL(FlyCameraComponent, TransformComponent);
    FOW_ASSERT_COMPONENT_DEPENDENCY_FATAL(FlyCameraComponent, CameraComponent);

    m_pTransform = entity().get_component<TransformComponent>();
    m_pCamera = entity().get_component<CameraComponent>();
}
void FlyCameraComponent::on_update(const double dt) {
    if (Input::KeyIsPressed(KeyCode::Escape)) {
        m_bFreeLook = !m_bFreeLook;
        Input::SetCursorMode(m_bFreeLook ? Input::CursorMode::Locked : Input::CursorMode::Unlocked);
    }

    if (m_bFreeLook) {
        const auto look_speed = pl_mouse_speed->as_float().value_or(CAMERA_LOOK_SPEED_DEFAULT);
        const auto mouse_xy = Input::MouseMovement();
        if (mouse_xy.x != 0.0f) {
            m_fYaw -= mouse_xy.x * look_speed;
        }
        if (mouse_xy.y != 0.0f) {
            m_fPitch = Clamp(m_fPitch - (mouse_xy.y * look_speed), -89.0f, 89.0f);
        }

        m_pTransform->set_rotation_deg(Vector3Constants::Right * m_fPitch + Vector3Constants::Up * m_fYaw);
    }

    const auto speed = pl_cam_speed->as_float().value_or(CAMERA_MOVE_SPEED_DEFAULT);
    const auto move_xz = Input::GetAxis2D("move_right", "move_left", "move_forward", "move_backward") * static_cast<float>(dt) * speed;
    auto movement = Vector3(0.0);
    if (move_xz.length() != 0.0f) {
        movement += (m_pTransform->get_right() * move_xz.x + m_pTransform->get_forward() * move_xz.y);
    }
    const auto move_y = Input::GetAxis("move_up", "move_down") * static_cast<float>(dt) * speed;
    if (move_y != 0.0f) {
        movement += Vector3Constants::Up * move_y;
    }
    m_pTransform->set_position(m_pTransform->get_position() + movement);
}

void TestSphereComponent::on_spawn() {
    FOW_ASSERT_COMPONENT_DEPENDENCY_FATAL(TestSphereComponent, TransformComponent);
    m_pTransform = entity().get_component<TransformComponent>();
}

void TestSphereComponent::on_update(const double dt) {
    m_pTransform->set_rotation(Vector3Constants::Up * m_fRotation);
    m_fRotation += static_cast<float>(dt);
}
