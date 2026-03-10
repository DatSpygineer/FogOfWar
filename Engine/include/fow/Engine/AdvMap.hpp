#ifndef FOW_ENGINE_ADVMAP_HPP
#define FOW_ENGINE_ADVMAP_HPP

#include "fow/Shared.hpp"

namespace fow {
    class AdvMap;
    using AdvMapPtr = SharedPtr<AdvMap>;
    class AdvMapObject;
    using AdvMapObjectPtr = SharedPtr<AdvMapObject>;
    class AdvMapTile;
    using AdvMapTilePtr = SharedPtr<AdvMapTile>;

    class FOW_ENGINE_API AdvMap {
        Vector<AdvMapObjectPtr> m_objects;
        Vector<AdvMapTilePtr> m_tiles;
    public:
        AdvMap(int width, int height);

        [[nodiscard]] const Vector<AdvMapObjectPtr>& objects() const { return m_objects; }
        [[nodiscard]] Vector<AdvMapObjectPtr>& objects() { return m_objects; }
        [[nodiscard]] const Vector<AdvMapTilePtr>& tiles() const { return m_tiles; }
        [[nodiscard]] Vector<AdvMapTilePtr>& tiles() { return m_tiles; }

        static Result<AdvMapPtr> LoadMap(const String& name);

        bool destroy_object_at(const Vector2i& position);
    };

    class FOW_ENGINE_API AdvMapTile {
        AdvMapPtr m_pMap;
    public:
        explicit AdvMapTile(const AdvMapPtr& map) : m_pMap(map) { }
        explicit AdvMapTile(AdvMapPtr&& map)      : m_pMap(std::move(map)) { }
    };

    class FOW_ENGINE_API AdvMapObject {
        Vector3i m_Position;
        uint16_t m_uAngle;
        uint8_t m_uOwnerPlayerId;
        AdvMapPtr m_pMap;
    public:
        AdvMapObject(const AdvMapPtr& map, const Vector3i& position, const uint16_t angle, const uint8_t ownerPlayerId = 0) :
            m_Position(position), m_uAngle(angle), m_uOwnerPlayerId(ownerPlayerId), m_pMap(map) { }
        AdvMapObject(AdvMapPtr&& map, const Vector3i& position, const uint16_t angle, const uint8_t ownerPlayerId = 0) :
            m_Position(position), m_uAngle(angle), m_uOwnerPlayerId(ownerPlayerId), m_pMap(std::move(map)) { }
        virtual ~AdvMapObject() = default;

        virtual void set_param(const String& name, const String& value) { }
        virtual void set_owner(const uint8_t playerId) { m_uOwnerPlayerId = playerId; }
        [[nodiscard]] FOW_CONSTEXPR uint8_t get_owner() const { return m_uOwnerPlayerId; }
        [[nodiscard]] FOW_CONSTEXPR const Vector3i& get_position() const { return m_Position; }
        [[nodiscard]] FOW_CONSTEXPR uint16_t get_angle() const { return m_uAngle; }

        void destroy();
    };
}

#endif