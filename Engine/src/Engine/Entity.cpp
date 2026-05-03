#include "fow/Engine/Entity.hpp"

#include "fow/Renderer/RenderQueue.hpp"

namespace fow {
    Result<ComponentRegistryObject> ComponentRegistryObject::GetComponentRegistry(const String& class_name) {
        if (const auto registry = s_registry; registry.contains(class_name)) {
            return Success<ComponentRegistryObject>(registry.at(class_name));
        }
        return Failure(std::format("Component registry does not contain component \"{}\"!", class_name));
    }

    Result<ComponentRegistryObject> ComponentRegistryObject::GetComponentRegistryObject(const String& class_name) {
        return GetComponentRegistry(class_name);
    }
    Result<ComponentRegistryObject> ComponentRegistryObject::GetComponentRegistryObject(const std::type_index& type_index) {
        if (!s_registry.empty()) {
            for (const auto& object : s_registry | std::views::values) {
                if (object.type_index() == type_index) {
                    return Success<ComponentRegistryObject>(object);
                }
            }
        }
        return Failure(std::format("Component registry does not contain component with type index \"{}\"!", type_index.name()));
    }

    Scene::Scene(const size_t entity_capacity, const UI::ThemePtr& ui_theme) : m_pFrame(nullptr) {
        UI::ThemePtr theme = ui_theme;
        if (theme == nullptr) {
            auto theme_result = Assets::Load<UI::Theme>("Default.theme.xml");
            if (!theme_result.has_value()) {
                Debug::LogError(std::format("Failed to load default UI theme: \"{}\"", theme_result.error().message));
            } else {
                theme = theme_result.value().ptr();
            }
        }

        if (theme != nullptr) {
            m_pFrame = CreateRef<UI::Frame>(theme);
        } else {
            Debug::LogFatal("Failed to create UI frame, theme is null!");
        }
        m_Entities.reserve(entity_capacity);
    }

    ComponentPtr<Component> Entity::add_component(const String& class_name, const HashMap<String, String>& parameters) {
        if (const auto reg = ComponentRegistryObject::GetComponentRegistry(class_name); reg.has_value()) {
            if (m_components.contains(reg->type_index())) {
                return m_components.at(reg->type_index());
            }

            auto component = reg->factory()(*this);

            for (const auto& dependency : reg->dependencies()) {
                Debug::Assert(add_component(dependency, parameters) != nullptr, std::format("Failed to create dependency \"{}\" for component \"{}\"", dependency, class_name));
            }

            for (const auto& [ name, value ] : parameters) {
                component->set_parameter(name, value);
            }

            if (is_spawned()) {
                component->on_spawn();
            }

            m_components.emplace(reg->type_index(), component);
            return component;
        }
        return nullptr;
    }

    bool Entity::has_component(const String& class_name) const {
        if (const auto reg = ComponentRegistryObject::GetComponentRegistry(class_name); reg.has_value()) {
            return m_components.contains(reg->type_index());
        }
        return false;
    }

    void Entity::enable() {
        if (m_bSpawned && !m_bEnabled) {
            m_bEnabled = true;
            for (const auto& component: m_components | std::views::values) {
                component->on_enable();
            }
        }
    }
    void Entity::disable() {
        if (m_bSpawned && m_bEnabled) {
            m_bEnabled = false;
            for (const auto& component: m_components | std::views::values) {
                component->on_disable();
            }
        }
    }

    void Entity::destroy() const {
        m_rScene.destroy_entity(m_uId);
    }

    ComponentRegistryObject::ComponentRegistryObject(const std::type_index& type_index, const String& class_name, const ComponentFactory& factory, const Vector<String>& dependencies)
        : m_type_index(type_index), m_class_name(class_name), m_factory(factory), m_dependencies(dependencies) {
        if (s_registry.contains(class_name)) {
            return;
        }

        s_registry.emplace(class_name, *this);
        Debug::LogDebug(std::format("Registered component type of \"{}\" (type: {})", class_name, type_index.name()));
    }

    EntityPtr Scene::create_entity() {
        EntityId id = 0;
        if (!m_Entities.empty()) {
            bool found = false;
            for (uint64_t i = 0; i < m_Entities.size(); ++i) {
                if (m_Entities.at(i) == nullptr) {
                    id = i;
                    found = true;
                    break;
                }
            }
            if (!found) {
                id = m_Entities.size();
            }
        }
        m_Entities.push_back(std::make_shared<Entity>(std::move(Entity { *this, id })));
        return m_Entities.back();
    }

    void Scene::destroy_entity(const EntityPtr& entity) {
        if (entity != nullptr) destroy_entity(entity->id());
    }

    void Scene::destroy_entity(const EntityId id) {
        if (id < m_Entities.size()) {
            auto& entity = m_Entities.at(id);
            for (const auto& component : entity->m_components | std::views::values) {
                component->on_destroy();
            }
            entity->m_bSpawned = false;
            entity = nullptr;
        }
    }

    void Scene::dispatch_spawn(const EntityPtr& entity) {
        if (entity != nullptr && !entity->is_spawned()) {
            for (const auto& component : entity->m_components | std::views::values) {
                component->on_spawn();
            }
            entity->m_bSpawned = true;
        }
    }

    void Scene::dispatch_spawn(const EntityId id) {
        if (id < m_Entities.size()) {
            dispatch_spawn(m_Entities.at(id));
        }
    }

    void Scene::enable_entity(const EntityPtr& entity) {
        if (entity != nullptr) {
            entity->enable();
        }
    }
    void Scene::enable_entity(const EntityId id) {
        if (id < m_Entities.size()) {
            enable_entity(m_Entities.at(id));
        }
    }

    void Scene::disable_entity(const EntityPtr& entity) {
        if (entity != nullptr) {
            entity->disable();
        }
    }
    void Scene::disable_entity(const EntityId id) {
        if (id < m_Entities.size()) {
            disable_entity(m_Entities.at(id));
        }
    }

    void Scene::clear() {
        destroy_all();
        m_Entities.clear();
    }

    void Scene::spawn() {
        for (auto& entity : m_Entities) {
            if (entity != nullptr) {
                dispatch_spawn(entity);
            }
        }
    }

    void Scene::update(const double dt) const {
        for (auto& entity : m_Entities) {
            if (entity != nullptr) {
                for (const auto& component : entity->m_components | std::views::values) {
                    component->on_update(dt);
                }
            }
        }
        if (m_pFrame != nullptr) {
            m_pFrame->update(dt);
        }
    }

    void Scene::render(const double dt) const {
        FOW_DISCARD(dt);
        RenderQueue::Render();
    }

    void Scene::destroy_all() {
        for (auto& entity : m_Entities) {
            if (entity != nullptr) {
                destroy_entity(entity);
            }
        }
    }

    Result<ScenePtr> Scene::LoadAsset(const Path& path, const AssetLoaderFlags::Type flags) {
        const auto xml = Assets::LoadAsXml(path, flags);
        if (!xml.has_value()) {
            return Failure(xml.error());
        }

        const auto root = xml->child("Scene");
        if (!root) {
            return Failure(std::format("Failed to load scene \"{}\": Expected root node \"Scene\" in XML document!", path));
        }

        Scene scene;

        if (const auto entities_node = root.child("Entities"); entities_node) {
            for (const auto entity_node : entities_node.children()) {
                auto ent = scene.create_entity();
                const auto enabled_attrib = entity_node.attribute("enabled");
                const auto components_node = entity_node.child("Components");

                for (const auto& component_node : components_node.children()) {
                    auto class_name_attrib = component_node.attribute("class_name");
                    if (!class_name_attrib) {
                        return Failure(std::format("Failed to load entity \"{}\": Expected attribute \"class_name\" in component node!", path));
                    }

                    auto component = ent->add_component(class_name_attrib.value());
                    if (component == nullptr) {
                        return Failure(std::format("Failed to load entity \"{}\": Failed to create component \"{}\"!", path, class_name_attrib.value()));
                    }

                    for (const auto& param_node : component_node.children()) {
                        component->set_parameter(param_node.name(), param_node.child_value());
                    }
                }

                if (enabled_attrib && StringToBool(enabled_attrib.value()).value_or(true)) {
                    ent->enable();
                }
                scene.dispatch_spawn(ent);
            }
        }
        return Success<ScenePtr>(std::make_shared<Scene>(std::move(scene)));
    }
}
