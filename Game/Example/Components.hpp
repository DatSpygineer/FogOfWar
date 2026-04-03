#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "fow/Engine.hpp"

class FlyCameraComponent : public fow::Component {
    fow::ComponentPtr<fow::CameraComponent> m_pCamera = nullptr;
    fow::ComponentPtr<fow::TransformComponent> m_pTransform = nullptr;
    bool m_bFreeLook = false;
    float m_fPitch = 0;
    float m_fYaw = 0;
public:
    FOW_COMPONENT_CLASS(FlyCameraComponent, fow::Component)

    void on_spawn() override;
    void on_update(double dt) override;
};

class TestSphereComponent : public fow::Component {
    fow::ComponentPtr<fow::TransformComponent> m_pTransform = nullptr;
    float m_fRotation = 0;
public:
    FOW_COMPONENT_CLASS(TestSphereComponent, fow::Component)

    void on_spawn() override;
    void on_update(double dt) override;
};

FOW_REGISTER_COMPONENT_WITH_DEPENDENCIES(FlyCameraComponent, "FlyCamera",   /* Dependencies */ "Transform", "Camera");
FOW_REGISTER_COMPONENT_WITH_DEPENDENCIES(TestSphereComponent, "TestSphere", /* Dependencies */ "Transform");

#endif