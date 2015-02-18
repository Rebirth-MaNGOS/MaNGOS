/*
* Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
* Copyright (C) 2009-2011 MaNGOSZero <https://github.com/mangos-zero>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _MAP_BUILDER_H
#define _MAP_BUILDER_H

#include <vector>
#include <set>
#include <map>

#include "TerrainBuilder.h"
#include "IntermediateValues.h"

#include "IVMapManager.h"
#include "WorldModel.h"

#include "Recast.h"
#include "DetourNavMesh.h"

using namespace std;
using namespace VMAP;
// G3D namespace typedefs conflicts with ACE typedefs

namespace MMAP
{
struct ManualAreaEntry
{
    G3D::Array<float> vertices;
    int verticeCount;

    G3D::Array<int> triangleIndices;
    int indicesCount;

    ManualAreaEntry() {
        verticeCount = 0, indicesCount = 0;
    }
};

typedef map<uint32,set<uint32>*> TileList;
struct Tile
{
    Tile() : chf(NULL), solid(NULL), cset(NULL), pmesh(NULL), dmesh(NULL) {}
    ~Tile()
    {
        rcFreeCompactHeightfield(chf);
        rcFreeContourSet(cset);
        rcFreeHeightField(solid);
        rcFreePolyMesh(pmesh);
        rcFreePolyMeshDetail(dmesh);
    }
    rcCompactHeightfield* chf;
    rcHeightfield* solid;
    rcContourSet* cset;
    rcPolyMesh* pmesh;
    rcPolyMeshDetail* dmesh;
};

class MapBuilder
{
public:
    MapBuilder(float maxWalkableAngle   = 60.f,
               bool skipLiquid          = false,
               bool skipContinents      = false,
               bool skipJunkMaps        = true,
               bool skipBattlegrounds   = false,
               bool debugOutput         = false,
               bool bigBaseUnit         = false,
               bool manualOverwriteGeometry = false,
               const char* offMeshFilePath = NULL);

    ~MapBuilder();

    // builds all mmap tiles for the specified map id (ignores skip settings)
    void buildMap(uint32 mapID);
    
    // Only use imported data from obj-files to build the maps for mapID.
    void buildMapFromObjects(uint32 mapID);

    // builds an mmap tile for the specified map and its mesh
    void buildSingleTile(uint32 mapID, uint32 tileX, uint32 tileY);
    
    // Build a single tile using data from imported obj-files.
    void buildSingleTileFromObjects(uint32 mapID, uint32 tileX, uint32 tileY);
    
    void dumpSingleTile(uint32 mapID, uint32 tileX, uint32 tileY);

    // builds list of maps, then builds all of mmap tiles (based on the skip settings)
    void buildAllMaps();

    ManualAreaEntry LoadManualAreaEntries(uint32 mapID);
    ManualAreaEntry LoadObjFiles(uint32 mapID);
    

private:
    // detect maps and tiles
    void discoverTiles();
    set<uint32>* getTileList(uint32 mapID);

    void buildNavMesh(uint32 mapID, dtNavMesh* &navMesh);

    void buildTile(uint32 mapID, uint32 tileX, uint32 tileY, dtNavMesh* navMesh, ManualAreaEntry& manualAreas);
    
    void buildTileFromObjects(uint32 mapID, uint32 tileX, uint32 tileY, dtNavMesh* navMesh, ManualAreaEntry& manualAreas);

    ManualAreaEntry LoadSpecificManualAreaFile(uint32 mapID, uint32 extension);
    
    void writeSingleTileToDumpFile(MeshData& meshData);
    ManualAreaEntry ImportSpecificObjFile(uint32 mapID, uint32 extension);
    
    // move map building
    void buildMoveMapTile(uint32 mapID,
                          uint32 tileX,
                          uint32 tileY,
                          MeshData &meshData,
                          float bmin[3],
                          float bmax[3],
                          dtNavMesh* navMesh,
                          ManualAreaEntry& manualAreas);
    
    void buildMoveMapTileFromObjData(uint32 mapID,
                          uint32 tileX,
                          uint32 tileY,
                          float bmin[3],
                          float bmax[3],
                          dtNavMesh* navMesh,
                          ManualAreaEntry& manualAreas);

    void getTileBounds(uint32 tileX, uint32 tileY,
                       float* verts, int vertCount,
                       float* bmin, float* bmax);
    void getGridBounds(uint32 mapID, uint32 &minX, uint32 &minY, uint32 &maxX, uint32 &maxY);

    bool shouldSkipMap(uint32 mapID);
    bool isTransportMap(uint32 mapID);
    bool shouldSkipTile(uint32 mapID, uint32 tileX, uint32 tileY);

    TerrainBuilder* m_terrainBuilder;
    TileList m_tiles;

    bool m_debugOutput;

    const char* m_offMeshFilePath;
    bool m_skipContinents;
    bool m_skipJunkMaps;
    bool m_skipBattlegrounds;

    float m_maxWalkableAngle;
    bool m_bigBaseUnit;

    bool m_manualOverwriteGeometry;

    // build performance - not really used for now
    rcContext* m_rcContext;
};
}

#endif
