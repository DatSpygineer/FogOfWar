#include "GLView.hpp"

#include <QApplication>
#include <QDir>
#include <QMessageBox>

#include "../util/GameSettings.hpp"


namespace fow {
    void Scene::addModel(const ModelPtr& model, const Transform& transform) {
        m_models.emplace_back(model, transform);
    }
    void Scene::addLight(const Light& light) {
        m_lights.push_back(light);
    }

    void Scene::draw() {
        for (const auto& [model, transform] : m_models) {
            for (const auto& material : model->materials()) {
                size_t i = 0;
                for (const auto& [position, color, intensity] : m_lights) {
                    material->set_parameter(std::format("Lights[{}].Position", i), position);
                    material->set_parameter(std::format("Lights[{}].Color", i), Vector4 { color, intensity });
                    i++;
                }
            }
            model->draw(transform);
        }
    }

    bool GLView::s_bInitialized = false;

    GLView::GLView(QWidget* parent) : QOpenGLWidget(parent), m_scene() {
        m_scene.setBackgroundColor(Color { 0.25, 0.25, 0.25 });
    }

    void GLView::initializeGL() {
        if (!s_bInitialized) {
            if (const auto result = Renderer::InitializeForEditor(GetGameSettings().GamePath, 4); !result.has_value()) {
                QMessageBox::critical(this, "Error", QString::fromStdString(std::format("Error while initializing fow::Renderer: {}", result.error().message)));
            }

            updateView();
            s_bInitialized = true;
        }
    }
    void GLView::resizeGL(const int width, const int height) {
        Renderer::SetViewport(0, 0, width, height);
        Renderer::UpdateCameraProjectionPerspective(60.0f, Vector2 { width, height }, 0.1f, 1000.0f);
    }
    void GLView::paintGL() {
        if (!isEnabled()) {
            return;
        }

        if (m_bDrawWireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        Renderer::Clear(m_scene.getBackgroundColor());
    }

    void GLView::updateView() {
        Renderer::UpdateCameraProjectionPerspective(60.0f, Vector2 { width(), height() }, 0.1f, 1000.0f);
        Renderer::UpdateCameraPosition(Vector3 { 0.0, 1.0, -5.0 }, Vector3 { 0.0, 1.0, -5.0 } + Vector3 { 0.0, 0.0, 1.0 }, Vector3 { 0.0, 1.0, 0.0 });
    }

    void GLView::setDrawWireframe(const bool drawWireframe) {
        m_bDrawWireframe = drawWireframe;
        Debug::LogInfo(std::format("Set wireframe {}", m_bDrawWireframe));
    }
}
