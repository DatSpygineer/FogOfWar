#include "fow/Engine/AdvMap.hpp"
namespace fow {
    AdvMap::AdvMap(const int width, const int height) {
        m_objects.resize(width * height);
        m_tiles.resize(width * height);
    }

    Result<AdvMapPtr> AdvMap::LoadMap(const String& name) {
    }

    Result<AdvMapPtr> AdvMap::LoadFromMemory(const Vector<uint8_t>& data) {
    }
}
