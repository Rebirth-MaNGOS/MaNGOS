/*
 * Copyright (C) 2006-2011 ScriptDev2 <http://www.scriptdev2.com/>
 * Copyright (C) 2010-2011 ScriptDev0 <http://github.com/mangos-zero/scriptdev0>
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

#ifndef DEF_BLACKWING_LAIR
#define DEF_BLACKWING_LAIR

#include "ThreatManager.h"

enum
{
    MAX_ENCOUNTER                       = 8,
    TOTAL_SAVE_DATA                     = MAX_ENCOUNTER + 4,

    TYPE_RAZORGORE                      = 0,
    TYPE_VAELASTRASZ                    = 1,
    TYPE_LASHLAYER                      = 2,
    TYPE_FIREMAW                        = 3,
    TYPE_EBONROC                        = 4,
    TYPE_FLAMEGOR                       = 5,
    TYPE_CHROMAGGUS                     = 6,
    TYPE_NEFARIAN                       = 7,
    CHROM_BREATH_1                      = 8,
    CHROM_BREATH_2                      = 9,
    DRAKONID_1                          = 10,
    DRAKONID_2                          = 11,

    NPC_RAZORGORE                       = 12435,
    NPC_GRETHOK                         = 12557,
    NPC_GRETHOK_GUARD                   = 14456,
    NPC_VAELASTRASZ                     = 13020,
    NPC_LASHLAYER                       = 12017,
    NPC_FIREMAW                         = 11983,
    NPC_EBONROC                         = 14601,
    NPC_FLAMEGOR                        = 11981,
    NPC_CHROMAGGUS                      = 14020,
    NPC_NEFARIAN                        = 11583,
    NPC_LORD_NEFARIAN                   = 10162,
    NPC_BLACKWING_TECHNICIAN            = 13996,                    // Flees at Vael intro event
    NPC_ORB_OF_DOMINATION_TRIGGER       = 14449,

    NPC_RED_WHELP                       = 14022,
    NPC_GREEN_WHELP                     = 14023,
    NPC_BLUE_WHELP                      = 14024,
    NPC_BRONZE_WHELP                    = 14025,
    NPC_DEATH_TALON_HATCHER             = 12468,
    NPC_BLACKWING_TASKMASTER            = 12458,

    NPC_DEATH_TALON_TRIGGER             = 800002,
    NPC_TECHNICIAN_PACK_TRIGGER         = 800003,
    NPC_TECHNICIAN_THREAT_HOLDER        = 800004,

    GO_DOOR_RAZORGORE_ENTER             = 176964,
    GO_DOOR_RAZORGORE_EXIT              = 176965,
    GO_DOOR_NEFARIAN                    = 176966,
    GO_DOOR_CHROMAGGUS_ENTER            = 179115,
    GO_DOOR_CHROMAGGUS_SIDE             = 179116,
    GO_DOOR_CHROMAGGUS_EXIT             = 179117,
    GO_DOOR_VAELASTRASZ                 = 179364,
    GO_DOOR_LASHLAYER                   = 179365,
    GO_BLACK_DRAGON_EGG                 = 177807,
    GO_SUPPRESSION_DEVICE               = 179784
};

struct creature_compare
{
    creature_compare(uint32 entry) : entry(entry) {};
    bool operator() (Creature* creature)
    {
        return creature->GetEntry() == entry;
    }

    uint32 entry;
};

// Coords used to spawn Nefarius at the throne
static const float aNefariusSpawnLoc[4] = {-7466.16f, -1040.80f, 412.053f, 4.644f};

class MANGOS_DLL_DECL instance_blackwing_lair : public ScriptedInstance
{
public:
    instance_blackwing_lair(Map* pMap);
    ~instance_blackwing_lair();

    void Initialize();
    bool IsEncounterInProgress() const;

    void OnCreatureCreate(Creature* pCreature);
    void OnObjectCreate(GameObject* pGo);

    void Update(uint32 uiDiff);

    void SetData(uint32 uiType, uint32 uiData);
    uint32 GetData(uint32 uiType);

    const char* Save() {
        return m_strInstData.c_str();
    }
    void Load(const char* chrIn);

    uint32 GetRazorgorePhase() {
        return m_uiCurrentPhase;
    }
    void SetRazorgorePhase(uint32 phase);
    void RemoveRazorgoreEgg(GameObject* egg);

    void ClearRazorgoreSpawns();

    void SuspendOrResumeSuppressionRooms(bool suspend);

    std::vector<Creature*>* GetGoblinGroup1()
    {
        return &goblin_group1;
    }

    std::vector<Creature*>* GetGoblinGroup2()
    {
        return &goblin_group2;
    }

    std::vector<Creature*> &GetDrakonoidsAndBoneConstructs()
    {
        return m_vecDrakonoidsAndBoneConstructs;
    }


    // Shared threat list for the Goblin Technicians after Lashlayer.
    ThreatManager* m_pSharedGoblinThreatList;

private:
    // For Razorgore
    uint32 m_uiDragonCount;
    uint32 m_uiCurrentPhase;
    uint32 m_uiSpawnTimer;
    uint32 m_uiUnusedEggCounter;
    uint32 m_uiRazorgoreRespawnTimer;
    uint32 m_uiLashlayerDoorTimer;
    bool m_bLashlayerDoorOpened;
    bool m_bHasRazorUsedAggro;						// At the end of phase one Razorgore will use his fireball volley once to get aggro.
    std::vector<Creature*> m_vecSummonList;
    std::vector<Creature*> m_vecGrethokGuardList;
    std::vector<GameObject*> m_vecEggList;
    std::vector<ObjectGuid> m_vecUsedEggList;

    // For Vaelastrasz.
    std::vector<Creature*> goblin_group1;
    std::vector<Creature*> goblin_group2;

    // For Nefarius.
    std::vector<Creature*> m_vecDrakonoidsAndBoneConstructs;

    // For the suppression rooms.
    std::vector<std::pair<std::pair<uint32, uint32>, GameObject*> > m_vecTrapList;		// The trap CD is saved in the first uint32 and the animation CD in the second uint32;
    std::vector<Creature*> m_vecWhelpList;


protected:
    std::string m_strInstData;
    uint32 m_auiEncounter[TOTAL_SAVE_DATA];

    GUIDList m_lTechnicianGuids;
};

#endif
