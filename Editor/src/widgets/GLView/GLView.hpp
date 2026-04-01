#ifndef FOW_EDITOR_GLVIEW_HPP
#define FOW_EDITOR_GLVIEW_HPP

#include <fow/Renderer.hpp>
#include <QOpenGLWidget>

namespace fow {
    struct Light {
        Vector3 position;
        Vector3 color;
        float intensity;
    };
    struct ModelObject {
        ModelPtr model;
        Transform transform;
    };

    class Scene {
        Vector<ModelObject> m_models;
        Vector<Light> m_lights;
        Color m_backgroundColor;
    public:
        Scene() = default;

        void addModel(const ModelPtr& model, const Transform& transform);
        inline void addModel(const ModelPtr& model) {
            addModel(model, Transform { Vector3 { 0.0 }, Vector3 { 1.0 }, glm::quat { } });
        }
        void addLight(const Light& light);
        inline void addLight(const Vector3& pos, const Vector4& color) {
            addLight(pos, xyz(color), color.w);
        }
        inline void addLight(const Vector3& pos, const Vector3& color, float intensity) {
            addLight(Light { pos, color, intensity });
        }
        inline void setBackgroundColor(const Color& color) {
            m_backgroundColor = color;
        }

        void draw();
        [[nodiscard]] FOW_CONSTEXPR Color getBackgroundColor() const { return m_backgroundColor; }
    };

    class GLView : public QOpenGLWidget {
        bool m_bDrawWireframe = false;
        static bool s_bInitialized;
        Scene m_scene;
    public:
        explicit GLView(QWidget *parent);

        void initializeGL() override;
        void resizeGL(int width, int height) override;
        void paintGL() override;

        void updateView();

        void setDrawWireframe(bool drawWireframe);

        [[nodiscard]] FOW_CONSTEXPR bool drawWireFrame() const { return m_bDrawWireframe; }
    };
}

#endif