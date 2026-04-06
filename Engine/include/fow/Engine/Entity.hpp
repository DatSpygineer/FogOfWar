#ifndef FOW_ENGINE_ENTITY_HPP
#define FOW_ENGINE_ENTITY_HPP

#include <typeindex>
#include <fow/Shared.hpp>

#include "fow/Renderer/Model.hpp"
#include "fow/Renderer/RenderQueue.hpp"

#define FOW_COMPONENT_CLASS(__this_class, __base_class) \
    public: \
    using ThisClass = __this_class; \
    using BaseClass = __base_class; \
    explicit __this_class(::fow::Entity& entity) : __base_class(entity) { } \

#define FOW_REGISTER_COMPONENT(__component_type, __component_class_name) \
    const ::fow::ComponentRegistryObject FOW_UNIQUE(__ComponentRegistryObjectVar) = \
    ::fow::ComponentRegistryObject(typeid(__component_type), __component_class_name, \
    [](::fow::Entity& entity) { \
        return std::dynamic_pointer_cast<::fow::Component>(std::make_shared<__component_type>(entity));\
    }, { })

#define FOW_REGISTER_COMPONENT_WITH_DEPENDENCIES(__component_type, __component_class_name, ...) \
    const ::fow::ComponentRegistryObject FOW_UNIQUE(__ComponentRegistryObjectVar) = \
    ::fow::ComponentRegistryObject(typeid(__component_type), __component_class_name, \
    [](::fow::Entity& entity) { \
        return std::dynamic_pointer_cast<::fow::Component>(std::make_shared<__component_type>(entity));\
    }, { __VA_ARGS__ })

#define FOW_ASSERT_COMPONENT_DEPENDENCY(__component, __required_component) \
    ::fow::Debug::Assert(entity().has_component<__required_component>(), "Entity with component \"" #__component "\" must have component \"" #__required_component "\"")

#define FOW_ASSERT_COMPONENT_DEPENDENCY_FATAL(__component, __required_component) \
    ::fow::Debug::AssertFatal(entity().has_component<__required_component>(), "Entity with component \"" #__component "\" must have component \"" #__required_component "\"")

namespace fow {
    using EntityId = uint64_t;

    class Entity;
    using EntityPtr = SharedPtr<Entity>;
    class Component;
    class Scene;
    using ScenePtr = SharedPtr<Scene>;

    template<typename T>
    concept ComponentType = std::is_base_of_v<Component, T>;

    template<ComponentType T>
    using ComponentPtr = SharedPtr<T>;

    class FOW_ENGINE_API Entity {
        Scene& m_rScene;
        EntityId m_uId;
        HashMap<std::type_index, ComponentPtr<Component>> m_components;
        bool m_bEnabled = true;
        bool m_bSpawned = false;

        Entity(Scene& scene, const EntityId id) : m_rScene(scene), m_uId(id) { }
    public:
        Entity(const Entity&) = default;
        Entity(Entity&&) noexcept = default;
        ~Entity() = default;

        [[nodiscard]] FOW_CONSTEXPR EntityId id() const { return m_uId; }
        [[nodiscard]] FOW_CONSTEXPR Scene& scene() { return m_rScene; }
        [[nodiscard]] FOW_CONSTEXPR const Scene& scene() const { return m_rScene; }

        template<ComponentType T>
        ComponentPtr<T> add_component(const HashMap<String, String>& parameters = { });
        ComponentPtr<Component> add_component(const String& class_name, const HashMap<String, String>& parameters = { });
        template<ComponentType T>
        ComponentPtr<T> get_component() const;
        template<ComponentType T>
        void remove_component();
        template<ComponentType T>
        FOW_CONSTEXPR bool has_component() const;
        bool has_component(const String& class_name) const;

        void enable();
        void disable();
        void destroy() const;

        [[nodiscard]] FOW_CONSTEXPR bool is_enabled() const { return m_bEnabled; }
        [[nodiscard]] FOW_CONSTEXPR bool is_spawned() const { return m_bSpawned; }

        friend class Scene;
    };

    class FOW_ENGINE_API Component {
        Entity& m_entity;
    public:
        explicit Component(Entity& entity) : m_entity(entity) { }

        virtual ~Component() = default;

        virtual void on_spawn() { }
        virtual void on_destroy() { }
        virtual void on_update(double dt) { }

        virtual void on_enable() { }
        virtual void on_disable() { }

        virtual void on_transform_changed() { }

        virtual void set_parameter(const String& name, const String& value) { }

        [[nodiscard]] FOW_CONSTEXPR Entity& entity() { return m_entity; }
        [[nodiscard]] FOW_CONSTEXPR const Entity& entity() const { return m_entity; }
    };

    using ComponentFactory = std::function<ComponentPtr<Component>(Entity&)>;

    class FOW_ENGINE_API ComponentRegistryObject {
        std::type_index m_type_index;
        String m_class_name;
        ComponentFactory m_factory;
        Vector<String> m_dependencies;

        inline static HashMap<String, ComponentRegistryObject> s_registry = { };

        static Result<ComponentRegistryObject> GetComponentRegistry(const String& class_name);
    public:
        ComponentRegistryObject(const std::type_index& type_index, const String& class_name, const ComponentFactory& factory, const Vector<String>& dependűencies);

        const std::type_index& type_index() const { return m_type_index; }
        const String& class_name() const { return m_class_name; }
        const ComponentFactory& factory() const { return m_factory; }
        const Vector<String>& dependencies() const { return m_dependencies; }

        static Result<ComponentRegistryObject> GetComponentRegistryObject(const String& class_name);
        static Result<ComponentRegistryObject> GetComponentRegistryObject(const std::type_index& type_index);

        friend class Entity;
    };

    class FOW_ENGINE_API Scene final {
        Vector<EntityPtr> m_Entities;
    public:
        explicit Scene(const size_t entity_capacity = 128) : m_Entities() { m_Entities.reserve(entity_capacity); }
        Scene(const Scene&) = delete;
        Scene(Scene&&) noexcept = default;
        ~Scene() = default;

        EntityPtr create_entity();
        void destroy_entity(const EntityPtr& entity);
        void destroy_entity(EntityId id);

        void dispatch_spawn(const EntityPtr& entity);
        void dispatch_spawn(EntityId id);

        void enable_entity(const EntityPtr& entity);
        void enable_entity(EntityId id);

        void disable_entity(const EntityPtr& entity);
        void disable_entity(EntityId id);

        void clear();

        void spawn();
        void update(double dt) const;
        void render(double dt) const;
        void destroy_all();

        static Result<ScenePtr> LoadAsset(const Path& path, AssetLoaderFlags::Type flags);
    };

    template<ComponentType T>
    FOW_CONSTEXPR bool Entity::has_component() const {
        return m_components.contains(typeid(T));
    }
    template<ComponentType T>
    ComponentPtr<T> Entity::add_component(const HashMap<String, String>& parameters) {
        std::type_index type_index = typeid(T);
        const String type_name = type_index.name();
        if (m_components.contains(type_index)) {
            return std::dynamic_pointer_cast<T>(m_components.at(type_index));
        }

        auto component = std::make_shared<T>(*this);

        const auto registry_object = ComponentRegistryObject::GetComponentRegistryObject(type_index);
        Debug::Assert(registry_object);
        if (registry_object.has_value()) {
            for (const auto& dependency : registry_object->dependencies()) {
                if (add_component(dependency, parameters) == nullptr) {
                    Debug::LogError(std::format("Failed to create dependency \"{}\" for component \"{}\"", dependency, type_name));
                }
            }
        }

        for (const auto& [name, value] : parameters) {
            component->set_parameter(name, value);
        }

        if (is_spawned()) {
            component->on_spawn();
        }

        m_components.emplace(type_index, component);
        return component;
    }

    template<ComponentType T>
    ComponentPtr<T> Entity::get_component() const {
        if (const std::type_index& type_index = typeid(T); m_components.contains(type_index)) {
            return std::dynamic_pointer_cast<T>(m_components.at(type_index));
        }
        return nullptr;
    }

    template<ComponentType T>
    void Entity::remove_component() {
        if (const std::type_index& type_index = typeid(T); m_components.contains(type_index)) {
            m_components.erase(type_index);
        }
    }
}

#endif
