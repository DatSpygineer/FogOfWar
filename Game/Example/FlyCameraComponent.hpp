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

FOW_REGISTER_COMPONENT_WITH_DEPENDENCIES(FlyCameraComponent, "FlyCamera", /* Dependencies */ "Transform", "Camera");

#endif