#ifndef FOW_ADVMAP_HPP
#define FOW_ADVMAP_HPP

#include "fow/Shared.hpp"
#include "fow/Renderer.hpp"

namespace fow {
    class AdvMap;
    class AdvMapTile;
    class AdvMapObject;

    using AdvMapPtr = SharedPtr<AdvMap>;

    class FOW_ENGINE_API AdvMap {
        TextureCubeMapPtr m_pSkyTexture;
        Vector<AdvMapTile> m_tiles;
        Vector<AdvMapObject> m_objects;
    public:

    };

    class FOW_ENGINE_API AdvMapTile {
        MaterialPtr m_pMaterial;
        AdvMapPtr m_pMap;
    public:
        AdvMapTile(const AdvMapPtr& map);


    };
    class FOW_ENGINE_API AdvMapObject {
        AdvMapPtr m_pMap;
    public:
        AdvMapObject(const AdvMapPtr& map);
    };
}

#endif