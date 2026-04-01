#ifndef FLY_CAMERA_COMPONENTS_HPP
#define FLY_CAMERA_COMPONENTS_HPP

#include "fow/Engine.hpp"

class FlyCameraComponent : public fow::Component {
    fow::ComponentPtr<fow::CameraComponent> m_pCamera = nullptr;
public:
    FOW_COMPONENT_CLASS(FlyCameraComponent, fow::Component)

    void on_spawn() override;
    void on_update(double dt) override;
};

class TestSphereComponent : public fow::Component {
    fow::ComponentPtr<fow::TransformComponent> m_pTransform = nullptr;
    int m_iDirection = 0;
    float m_fProgress = 0.0f;
public:
    FOW_COMPONENT_CLASS(TestSphereComponent, fow::Component)

    void on_spawn() override;
    void on_update(double dt) override;
};

FOW_REGISTER_COMPONENT_WITH_DEPENDENCIES(FlyCameraComponent, "FlyCamera",   /* Dependencies */ "Transform", "Camera");
FOW_REGISTER_COMPONENT_WITH_DEPENDENCIES(TestSphereComponent, "TestSphere", /* Dependencies */ "Transform");

#endif