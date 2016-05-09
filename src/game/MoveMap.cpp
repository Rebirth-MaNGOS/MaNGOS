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

#include <math.h>

#include "GridMap.h"
#include "Log.h"
#include "World.h"

#include "MoveMap.h"
#include "MoveMapSharedDefines.h"
#include "PathFinder.h"
#include "ObjectMgr.h"
#include "Creature.h"

namespace MMAP
{
    // ######################## MMapFactory ########################
    // our global singelton copy
    MMapManager *g_MMapManager = NULL;

    // stores list of mapids which do not use pathfinding
    std::set<uint32>* g_mmapDisabledIds = NULL;

    MMapManager* MMapFactory::createOrGetMMapManager()
    {
        if(g_MMapManager == NULL)
            g_MMapManager = new MMapManager();

        return g_MMapManager;
    }

    void MMapFactory::preventPathfindingOnMaps(const char* ignoreMapIds)
    {
        if(!g_mmapDisabledIds)
            g_mmapDisabledIds = new std::set<uint32>();

        uint32 strLenght = strlen(ignoreMapIds)+1;
        char* mapList = new char[strLenght];
        memcpy(mapList, ignoreMapIds, sizeof(char)*strLenght);

        char* idstr = strtok(mapList, ",");
        while (idstr)
        {
            g_mmapDisabledIds->insert(uint32(atoi(idstr)));
            idstr = strtok(NULL, ",");
        }

        delete[] mapList;
    }

    bool MMapFactory::IsPathfindingEnabled(uint32 mapId)
    {
        return sWorld.getConfig(CONFIG_BOOL_MMAP_ENABLED)
            && g_mmapDisabledIds->find(mapId) == g_mmapDisabledIds->end();
    }

    void MMapFactory::clear()
    {
        if(g_mmapDisabledIds)
        {
            delete g_mmapDisabledIds;
            g_mmapDisabledIds = NULL;
        }

        if(g_MMapManager)
        {
            delete g_MMapManager;
            g_MMapManager = NULL;
        }
    }

    // ######################## MMapManager ########################
    MMapManager::~MMapManager()
    {
        for (MMapDataSet::iterator i = loadedMMaps.begin(); i != loadedMMaps.end(); ++i)
            delete i->second;

        // by now we should not have maps loaded
        // if we had, tiles in MMapData->mmapLoadedTiles, their actual data is lost!
    }

    bool MMapManager::loadMapData(uint32 mapId)
    {
        // we already have this map loaded?
        if (loadedMMaps.find(mapId) != loadedMMaps.end())
            return true;

        // load and init dtNavMesh - read parameters from file
        uint32 pathLen = sWorld.GetDataPath().length() + strlen("mmaps/%03i.mmap")+1;
        char *fileName = new char[pathLen];
        snprintf(fileName, pathLen, (sWorld.GetDataPath()+"mmaps/%03i.mmap").c_str(), mapId);

        FILE* file = fopen(fileName, "rb");
        if (!file)
        {
            sLog.outDebug("MMAP:loadMapData: Error: Could not open mmap file '%s'", fileName);
            delete [] fileName;
            return false;
        }

        dtNavMeshParams params;
        fread(&params, sizeof(dtNavMeshParams), 1, file);
        fclose(file);

        dtNavMesh* mesh = dtAllocNavMesh();
        MANGOS_ASSERT(mesh);
        if (DT_SUCCESS != mesh->init(&params))
        {
            dtFreeNavMesh(mesh);
            sLog.outError("MMAP:loadMapData: Failed to initialize dtNavMesh for mmap %03u from file %s", mapId, fileName);
            delete [] fileName;
            return false;
        }

        delete [] fileName;

        sLog.outDetail("MMAP:loadMapData: Loaded %03i.mmap", mapId);

        // store inside our map list
        MMapData* mmap_data = new MMapData(mesh);
        mmap_data->mmapLoadedTiles.clear();

        loadedMMaps.insert(std::pair<uint32, MMapData*>(mapId, mmap_data));
        return true;
    }

    uint32 MMapManager::packTileID(int32 x, int32 y)
    {
        return uint32(x << 16 | y);
    }

    bool MMapManager::loadMap(uint32 mapId, int32 x, int32 y)
    {
        std::lock_guard<std::mutex> guard(m_MapMutex);

        // make sure the mmap is loaded and ready to load tiles
        if(!loadMapData(mapId))
            return false;

        // get this mmap data
        MMapData* mmap = loadedMMaps[mapId];
        MANGOS_ASSERT(mmap->navMesh);

        // check if we already have this tile loaded
        uint32 packedGridPos = packTileID(x, y);
        if (mmap->mmapLoadedTiles.find(packedGridPos) != mmap->mmapLoadedTiles.end())
        {
            sLog.outError("MMAP:loadMap: Asked to load already loaded navmesh tile. %03u%02i%02i.mmtile", mapId, x, y);
            return false;
        }

        // load this tile :: mmaps/MMMXXYY.mmtile
        uint32 pathLen = sWorld.GetDataPath().length() + strlen("mmaps/%03i%02i%02i.mmtile")+1;
        char *fileName = new char[pathLen];
        snprintf(fileName, pathLen, (sWorld.GetDataPath()+"mmaps/%03i%02i%02i.mmtile").c_str(), mapId, x, y);

        FILE *file = fopen(fileName, "rb");
        if (!file)
        {
            sLog.outDebug("MMAP:loadMap: Could not open mmtile file '%s'", fileName);
            delete [] fileName;
            return false;
        }
        delete [] fileName;

        // read header
        MmapTileHeader fileHeader;
        fread(&fileHeader, sizeof(MmapTileHeader), 1, file);

        if (fileHeader.mmapMagic != MMAP_MAGIC)
        {
            sLog.outError("MMAP:loadMap: Bad header in mmap %03u%02i%02i.mmtile", mapId, x, y);
            return false;
        }

        if (fileHeader.mmapVersion != MMAP_VERSION)
        {
            sLog.outError("MMAP:loadMap: %03u%02i%02i.mmtile was built with generator v%i, expected v%i",
                                                mapId, x, y, fileHeader.mmapVersion, MMAP_VERSION);
            return false;
        }

        unsigned char* data = (unsigned char*)dtAlloc(fileHeader.size, DT_ALLOC_PERM);
        MANGOS_ASSERT(data);

        size_t result = fread(data, fileHeader.size, 1, file);
        if(!result)
        {
            sLog.outError("MMAP:loadMap: Bad header or data in mmap %03u%02i%02i.mmtile", mapId, x, y);
            fclose(file);
            return false;
        }

        fclose(file);

        dtMeshHeader* header = (dtMeshHeader*)data;
        dtTileRef tileRef = 0;

        // memory allocated for data is now managed by detour, and will be deallocated when the tile is removed
        if(DT_SUCCESS == mmap->navMesh->addTile(data, fileHeader.size, DT_TILE_FREE_DATA, 0, &tileRef))
        {
            mmap->mmapLoadedTiles.insert(std::pair<uint32, dtTileRef>(packedGridPos, tileRef));
            ++loadedTiles;
            sLog.outDetail("MMAP:loadMap: Loaded mmtile %03i[%02i,%02i] into %03i[%02i,%02i]", mapId, x, y, mapId, header->x, header->y);
            return true;
        }
        else
        {
            sLog.outError("MMAP:loadMap: Could not load %03u%02i%02i.mmtile into navmesh", mapId, x, y);
            dtFree(data);
            return false;
        }

        return false;
    }

    bool MMapManager::unloadMap(uint32 mapId, int32 x, int32 y)
    {
        std::lock_guard<std::mutex> guard(m_MapMutex);

        // check if we have this map loaded
        if (loadedMMaps.find(mapId) == loadedMMaps.end())
        {
            // file may not exist, therefore not loaded
            sLog.outDebug("MMAP:unloadMap: Asked to unload not loaded navmesh map. %03u%02i%02i.mmtile", mapId, x, y);
            return false;
        }

        MMapData* mmap = loadedMMaps[mapId];

        // check if we have this tile loaded
        uint32 packedGridPos = packTileID(x, y);
        if (mmap->mmapLoadedTiles.find(packedGridPos) == mmap->mmapLoadedTiles.end())
        {
            // file may not exist, therefore not loaded
            sLog.outDebug("MMAP:unloadMap: Asked to unload not loaded navmesh tile. %03u%02i%02i.mmtile", mapId, x, y);
            return false;
        }

        dtTileRef tileRef = mmap->mmapLoadedTiles[packedGridPos];

        // unload, and mark as non loaded
        if(DT_SUCCESS != mmap->navMesh->removeTile(tileRef, NULL, NULL))
        {
            // this is technically a memory leak
            // if the grid is later reloaded, dtNavMesh::addTile will return error but no extra memory is used
            // we cannot recover from this error - assert out
            sLog.outError("MMAP:unloadMap: Could not unload %03u%02i%02i.mmtile from navmesh", mapId, x, y);
            MANGOS_ASSERT(false);
        }
        else
        {
            mmap->mmapLoadedTiles.erase(packedGridPos);
            --loadedTiles;
            sLog.outDetail("MMAP:unloadMap: Unloaded mmtile %03i[%02i,%02i] from %03i", mapId, x, y, mapId);
            return true;
        }

        return false;
    }

    bool MMapManager::unloadMap(uint32 mapId)
    {
        std::lock_guard<std::mutex> guard(m_MapMutex);

        if (loadedMMaps.find(mapId) == loadedMMaps.end())
        {
            // file may not exist, therefore not loaded
            sLog.outDebug("MMAP:unloadMap: Asked to unload not loaded navmesh map %03u", mapId);
            return false;
        }

        // unload all tiles from given map
        MMapData* mmap = loadedMMaps[mapId];
        for (MMapTileSet::iterator i = mmap->mmapLoadedTiles.begin(); i != mmap->mmapLoadedTiles.end(); ++i)
        {
            uint32 x = (i->first >> 16);
            uint32 y = (i->first & 0x0000FFFF);
            if(DT_SUCCESS != mmap->navMesh->removeTile(i->second, NULL, NULL))
                sLog.outError("MMAP:unloadMap: Could not unload %03u%02i%02i.mmtile from navmesh", mapId, x, y);
            else
            {
                --loadedTiles;
                sLog.outDetail("MMAP:unloadMap: Unloaded mmtile %03i[%02i,%02i] from %03i", mapId, x, y, mapId);
            }
        }

        delete mmap;
        loadedMMaps.erase(mapId);
        sLog.outDetail("MMAP:unloadMap: Unloaded %03i.mmap", mapId);

        return true;
    }

    bool MMapManager::unloadMapInstance(uint32 mapId, uint32 instanceId)
    {
        std::lock_guard<std::mutex> guard(m_MapMutex);

        // check if we have this map loaded
        if (loadedMMaps.find(mapId) == loadedMMaps.end())
        {
            // file may not exist, therefore not loaded
            sLog.outDebug("MMAP:unloadMapInstance: Asked to unload not loaded navmesh map %03u", mapId);
            return false;
        }

        MMapData* mmap = loadedMMaps[mapId];
        if (mmap->navMeshQueries.find(instanceId) == mmap->navMeshQueries.end())
        {
            sLog.outDebug("MMAP:unloadMapInstance: Asked to unload not loaded dtNavMeshQuery mapId %03u instanceId %u", mapId, instanceId);
            return false;
        }

        dtNavMeshQuery* query = mmap->navMeshQueries[instanceId];

        dtFreeNavMeshQuery(query);
        mmap->navMeshQueries.erase(instanceId);
        sLog.outDetail("MMAP:unloadMapInstance: Unloaded mapId %03u instanceId %u", mapId, instanceId);

        return true;
    }

    dtNavMesh const* MMapManager::GetNavMesh(uint32 mapId)
    {
        std::lock_guard<std::mutex> guard(m_MapMutex);

        if (loadedMMaps.find(mapId) == loadedMMaps.end())
            return NULL;

        return loadedMMaps[mapId]->navMesh;
    }

    dtNavMeshQuery const* MMapManager::GetNavMeshQuery(uint32 mapId, uint32 instanceId)
    {
        std::lock_guard<std::mutex> guard(m_MapMutex);

        if (loadedMMaps.find(mapId) == loadedMMaps.end())
            return NULL;

        MMapData* mmap = loadedMMaps[mapId];
        if (mmap->navMeshQueries.find(instanceId) == mmap->navMeshQueries.end())
        {
             // allocate mesh query
            dtNavMeshQuery* query = dtAllocNavMeshQuery();
            MANGOS_ASSERT(query);
            if(DT_SUCCESS != query->init(mmap->navMesh, 1024))
            {
                dtFreeNavMeshQuery(query);
                sLog.outError("MMAP:GetNavMeshQuery: Failed to initialize dtNavMeshQuery for mapId %03u instanceId %u", mapId, instanceId);
                return NULL;
            }

            sLog.outDetail("MMAP:GetNavMeshQuery: created dtNavMeshQuery for mapId %03u instanceId %u", mapId, instanceId);
            mmap->navMeshQueries.insert(std::pair<uint32, dtNavMeshQuery*>(instanceId, query));
        }

        return mmap->navMeshQueries[instanceId];
    }

    bool MMapManager::DrawRay(Unit *queryUnit, dtPolyRef unitPoly, float startX, float startY, float startZ, float &endX, float &endY, float &endZ)
    {
        bool canSwim = true;
        bool canWalk = true;
        Creature *creatureUnit = dynamic_cast<Creature*>(queryUnit);

        if (creatureUnit != NULL)
        {
            canSwim = creatureUnit->CanSwim();
            canWalk = creatureUnit->CanWalk();
        }

        MaNGOS::NormalizeMapCoord(endX);
        MaNGOS::NormalizeMapCoord(endY);

        bool is_water = queryUnit->GetTerrain()->IsInWater(endX,endY,endZ);

        if ((is_water && !canSwim) || (!is_water && !canWalk))
        {
            endX = startX;
            endY = startY;
            endZ = startZ;
            return false;
        }

        if (is_water && queryUnit->IsWithinLOS(endX,endY,endZ))
            return true;

        uint32 mapId = queryUnit->GetMapId();
        const dtNavMeshQuery *query = GetNavMeshQuery(mapId, queryUnit->GetInstanceId());

        if(!query)
            return false;

        dtQueryFilter filter;

        uint32 flags = 0;
        if (canWalk)
            flags |= NAV_GROUND;
            
        if (canSwim)
            flags |= NAV_MAGMA|NAV_SLIME|NAV_WATER;

        filter.setIncludeFlags(flags);
        
		const int maxPath = 64;
        float rayLength, normal[3];
        int pathLength;
        dtPolyRef polyPath[maxPath];

		// Check if the current path will cross a barrier.
		bool had_intersection = false;
		std::vector<PathingBarrier> const& barriers = sObjectMgr.GetPathingBarrier(mapId);
		for (PathingBarrier current_barrier : barriers)
		{
			const GameObjectData* connected_door_data = sObjectMgr.GetGOData(current_barrier.guid);
			if (connected_door_data)
			{
				GameObject* connected_door = queryUnit->GetMap()->GetGameObject(ObjectGuid(HIGHGUID_GAMEOBJECT, connected_door_data->id, current_barrier.guid));

				if (connected_door && connected_door->GetGoState() == GO_STATE_ACTIVE)
					continue;
			}

			double x_in[5] = { current_barrier.x0, current_barrier.x1, current_barrier.x2, startX, endX };
			double y_in[5] = { current_barrier.y0, current_barrier.y1, current_barrier.y2, startY, endY };
			double z_in[5] = { current_barrier.z0, current_barrier.z1, current_barrier.z2, startZ, endZ };

			had_intersection = IsLineIntersecting2DPoly(x_in, y_in, z_in, &endX, &endY, &endZ );
			if (had_intersection)
				break;
		}

		// Calculate new coordinates a little before the barrier if an intersection was found.
		if (had_intersection)
		{
			float length = sqrt(pow(endX - startX, 2) + pow(endY - startY, 2) + pow(endZ - startZ, 2));
			endX = -3 * (endX - startX) / length + endX;
			endY = -3 * (endY - startY) / length + endY;
			endZ = -3 * (endZ - startZ) / length + endZ;
		}


		float fromPoint[3] = { startY, startZ, startX };
        float toPoint[3] = { endY, endZ, endX };

        if (query->raycast(unitPoly,fromPoint,toPoint,&filter, &rayLength, normal, polyPath, &pathLength, maxPath) != DT_SUCCESS)
        {
            endX = startX;
            endY = startY;
            endZ = startZ;
            return false;
        }

        if (rayLength <= 1)
        {
            endX = startX + (endX - startX) * (rayLength-0.001f);
            endY = startY + (endY - startY) * (rayLength-0.001f);
            endZ = startZ + (endZ - startZ) * (rayLength-0.001f);
        }

        if (pathLength)
		{
            float contactPosition[3] = {endY, endZ, endX};
			float h = 0;

            for (int polygonIndex = pathLength-1; polygonIndex >= 0; polygonIndex--)
            {
			    if (query->getPolyHeight(polyPath[polygonIndex], contactPosition, &h) == DT_SUCCESS)
                {
			        endZ = h;
                    break;
                }
            }
		}

        return true;
    }

    bool MMapManager::GetNearestValidPosition(Unit *queryUnit, float searchX, float searchY, float searchZ, float &outX, float &outY, float &outZ, dtPolyRef *activePoly)
    {
        bool canSwim = true;
        bool canWalk = true;
        Creature *creatureUnit = dynamic_cast<Creature*>(queryUnit);

        if (creatureUnit != NULL)
        {
            canSwim = creatureUnit->CanSwim();
            canWalk = creatureUnit->CanWalk();
        }

        MaNGOS::NormalizeMapCoord(outX);
        MaNGOS::NormalizeMapCoord(outY);

        bool is_water = queryUnit->GetTerrain()->IsInWater(outX,outY,outZ);

        if ((is_water && !canSwim) || (!is_water && !canWalk))
        {
            return false;
        }

        if (is_water)
        {
            *activePoly = INVALID_POLYREF;
            return true;
        }

        uint32 mapId = queryUnit->GetMapId();
        const dtNavMeshQuery *query = GetNavMeshQuery(mapId, queryUnit->GetInstanceId());
        if (!query)
            return false;

        dtQueryFilter filter;

        uint32 flags = 0;
        if (canWalk)
            flags |= NAV_GROUND;
            
        if (canSwim)
            flags |= NAV_MAGMA|NAV_SLIME|NAV_WATER;

        filter.setIncludeFlags(flags);

        float startPoint[3] = { outY, outZ+(searchZ/4.0f), outX };
        float extents[3] = {searchY/2.0f, searchZ/2.0f, searchX/2.0f};
        float outStartPoint[3] = {0,0,0};
        if (query->findNearestPoly(startPoint, extents, &filter,activePoly,outStartPoint) != DT_SUCCESS || (*activePoly) == INVALID_POLYREF)
        {
            return false;
        }

        outX = outStartPoint[2];
        outY = outStartPoint[0];
        outZ = outStartPoint[1];
        return true;
    }
}
