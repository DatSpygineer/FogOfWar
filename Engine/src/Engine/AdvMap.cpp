#include "fow/Engine/AdvMap.hpp"

#include "fow/Engine.hpp"

namespace fow {
    AdvMap::AdvMap(const int width, const int height) {
        m_objects.resize(width * height);
        m_tiles.resize(width * height);
    }

    Result<AdvMapPtr> AdvMap::LoadMap(const String& name) {
        const auto mapsFolder = Engine::GetGameBasePath() / "maps" / (name + ".fmap");
        if (!mapsFolder.exists()) {
            return Failure(std::format("Map \"{}\" doesn't exists", name));
        }


    }
}
