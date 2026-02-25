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
        static Result<AdvMapPtr> LoadFromMemory(const Vector<uint8_t>& data);
    };
}

#endif