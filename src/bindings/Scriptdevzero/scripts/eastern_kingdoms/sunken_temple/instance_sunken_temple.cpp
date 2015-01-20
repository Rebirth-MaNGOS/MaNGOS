/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: instance_sunken_temple
SD%Complete: 95
SDComment:
SDCategory: Sunken Temple
EndScriptData */

/*
    Encounter 0 = Atalarion
    Encounter 1 = Defenders
    Encounter 2 = Jammal'an
    Encounter 3 = Shade Of Eranikus
    Encounter 4 = Malfurion
    Encounter 5 = Avatar of Hakkar
*/

#include "precompiled.h"
#include "sunken_temple.h"

static Loc Blood[]=
{
    {-474.64f,273.39f,-90.41f,3.123f},
    {-459.27f,273.36f,-90.42f,6.268f},
    {-467.34f,266.90f,-90.37f,4.737f},
    {-467.01f,279.98f,-90.47f,1.544f},
    {-472.22f,278.61f,-90.46f,2.319f},
    {-472.04f,268.93f,-90.39f,3.945f},
    {-462.23f,268.39f,-90.39f,5.523f},
    {-462.53f,277.48f,-90.45f,0.909f},
    {-467.56f,273.42f,-90.45f,6.227f}
};

static Loc Supp[]=
{
    {-453.18f,286.13f,-90.57f,0.819f},
    {-480.67f,285.84f,-90.59f,2.406f},
    {-485.12f,261.56f,-90.66f,3.969f},
    {-453.40f,260.39f,-90.50f,5.563f},

    {-421.14f,275.93f,-90.82f,3.142f},
    {-512.93f,275.95f,-90.82f,0.001f},
    {-450.91f,275.30f,-90.56f,3.150f},
    {-483.92f,273.49f,-90.65f,6.276f}
};

instance_sunken_temple::instance_sunken_temple(Map* pMap) : ScriptedInstance(pMap),
    m_bIsSerpentSummoning(false),

    m_uiSerpent(0),
    m_uiSerpentSummonTimer(2000),
    m_uiStatueOrder(0),
    m_uiWaveCount(0)
{
    Initialize();
}

void instance_sunken_temple::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

    m_lStatueGUID.clear();
    m_lTrollGUID.clear();
    m_lDragonkinGUID.clear();
    m_lEternalFlameGUID.clear();
}

void instance_sunken_temple::OnObjectCreate(GameObject* pGo)
{
    switch(pGo->GetEntry())
    {
        case GO_ATALAI_STATUE_S:
        case GO_ATALAI_STATUE_N:
        case GO_ATALAI_STATUE_SW:
        case GO_ATALAI_STATUE_SE:
        case GO_ATALAI_STATUE_NW:
        case GO_ATALAI_STATUE_NE:
			m_lStatueGUID.push_back(pGo->GetObjectGuid());
            if (m_auiEncounter[0] >= DONE)
                InteractWithGo(pGo->GetEntry(), false);
            return;
        case GO_IDOL_OF_HAKKAR:
            switch(m_auiEncounter[0])
            {
                case NOT_STARTED:
                case IN_PROGRESS:
                    InteractWithGo(GO_IDOL_OF_HAKKAR, false);
                    if (m_auiEncounter[0] == IN_PROGRESS)
                        SetData(TYPE_ATALALARION, NOT_STARTED);
                    break;
                case DONE:
                case SPECIAL:
                    InteractWithGo(GO_IDOL_OF_HAKKAR, true);
                    if (m_auiEncounter[0] == SPECIAL)
                        SummonAtalalarion(pGo);
                    break;
            }
            break;
        case GO_FORCE_FIELD:
            if (m_auiEncounter[1] >= MAX_DEFENDER || m_auiEncounter[2] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_ETERNAL_FLAME_1:
        case GO_ETERNAL_FLAME_2:
        case GO_ETERNAL_FLAME_3:
        case GO_ETERNAL_FLAME_4:
            m_lEternalFlameGUID.push_back(pGo->GetObjectGuid());
            if (m_auiEncounter[5] >= FAIL)
            {
                pGo->SetGoState(GO_STATE_ACTIVE);
                InteractWithGo(pGo->GetEntry(), false);
            }
            return;
        case GO_SANCTUM_DOOR_1:
        case GO_SANCTUM_DOOR_2:
            if (m_auiEncounter[5] == SPECIAL)
                pGo->SetGoState(GO_STATE_READY);
            else
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_SANCTUM_OF_THE_FALLEN_GOD:
            if (m_auiEncounter[5] == IN_PROGRESS)
                SetData(TYPE_AVATAR_OF_HAKKAR, NOT_STARTED);
            if (m_auiEncounter[5] == SPECIAL)
                pGo->SummonCreature(NPC_AVATAR_OF_HAKKAR, pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), pGo->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 120000);
            break;
        default:
            return;
    }

	m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_sunken_temple::OnCreatureCreate(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_GASHER:
        case NPC_HUKKU:
        case NPC_LORO:
        case NPC_MIJAN:
        case NPC_ZOLO:
        case NPC_ZULLOR:
			pCreature->CastSpell(pCreature, GREEN_VISUAL, false);
          //  pCreature->SetStandState(UNIT_STAND_STATE_KNEEL);		//removed
            return;
        case NPC_ATALAI_DEATHWALKER:
        case NPC_ATALAI_HIGH_PRIEST:
        case NPC_ATALAI_PRIEST:
        case NPC_MUMMIFIED_ATALAI:
            m_lTrollGUID.push_back(pCreature->GetObjectGuid());
            pCreature->SetStandState(UNIT_STAND_STATE_KNEEL);
            return;
        case NPC_OGOM_THE_WRETCHED:
			m_lTrollGUID.push_back(pCreature->GetObjectGuid());
            pCreature->SetStandState(UNIT_STAND_STATE_KNEEL);
            break;
        case NPC_JAMMALAN_THE_PROPHET:
			pCreature->CastSpell(pCreature, GREEN_VISUAL, false);
       //     pCreature->SetStandState(UNIT_STAND_STATE_KNEEL);		//removed
            break;
        case NPC_DREAMSCYTHE:
        case NPC_WEAVER:
        case NPC_MORPHAZ:
        case NPC_HAZZAS:
        case NPC_NIGHTMARE_SCALEBANE:
        case NPC_NIGHTMARE_SUPPRESSOR:
        case NPC_NIGHTMARE_WANDERER:
        case NPC_NIGHTMARE_WHELP:
        case NPC_NIGHTMARE_WYRMKIN:
            m_lDragonkinGUID.push_back(pCreature->GetObjectGuid());
            return;
        case NPC_SHADE_OF_ERANIKUS:
            if (m_auiEncounter[2] == DONE)
            {
                pCreature->setFaction(FACTION_DRAGONKIN);
                pCreature->SetStandState(UNIT_STAND_STATE_STAND);
                pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }
            else
            {
                pCreature->setFaction(FACTION_FRIENDLY);
                pCreature->SetStandState(UNIT_STAND_STATE_SLEEP);
                pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }
            break;
        case NPC_SHADE_OF_HAKKAR:
            pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            break;
        default:
            return;
    }

	m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
}

void instance_sunken_temple::SetData(uint32 uiType, uint32 uiData)
{
    switch(uiType)
    {
        case TYPE_ATALALARION:
            m_auiEncounter[0] = uiData;
            switch(uiData)
            {
                case NOT_STARTED:                           // Restart Atal'Alarion Event
                    if (!m_lStatueGUID.empty())
                        for(GUIDList::iterator itr = m_lStatueGUID.begin(); itr != m_lStatueGUID.end(); ++itr)
                        {
                            if (GameObject* pStatue = instance->GetGameObject(*itr))
                            {
                                if (pStatue->GetGoState() != GO_STATE_READY)
                                    pStatue->SetGoState(GO_STATE_READY);
                                InteractWithGo(*itr, true);
                            }
                        }

                    m_uiStatueOrder = 0;
                    break;
                case SPECIAL:                               // Summon Atal'Alarion, grant access to Idol
					if (GameObject* pIdol = GetSingleGameObjectFromStorage(GO_IDOL_OF_HAKKAR))
                    {
                        SummonAtalalarion(pIdol);
                        InteractWithGo(GO_IDOL_OF_HAKKAR, true);
                    }
                    break;
            }
            break;
        case TYPE_ATALAI_DEFENDERS:
            m_auiEncounter[1] = uiData;
            if (uiData >= MAX_DEFENDER)
            {
                HandleGameObject(GO_FORCE_FIELD, true);
                DoOrSimulateScriptTextForThisInstance(SAY_ZONE_JAMMAL, NPC_JAMMALAN_THE_PROPHET);
            }
            break;
        case TYPE_JAMMALAN_THE_PROPHET:
            m_auiEncounter[2] = uiData;
            if (uiData == IN_PROGRESS)
                CallForHelp(NPC_JAMMALAN_THE_PROPHET, m_lTrollGUID);
            if (uiData == DONE)
            {
                if (Creature* pEranikus = GetSingleCreatureFromStorage(NPC_SHADE_OF_ERANIKUS))
                {
                    pEranikus->SummonCreature(NPC_DREAMSCYTHE, -472.57f, 87.23f, -94.74f, 4.23f, TEMPSUMMON_DEAD_DESPAWN, 120000);
                    pEranikus->SummonCreature(NPC_WEAVER, -462.54f, 103.53f, -94.76f, 1.03f, TEMPSUMMON_DEAD_DESPAWN, 120000);

                    pEranikus->setFaction(FACTION_DRAGONKIN);
                    pEranikus->SetStandState(UNIT_STAND_STATE_STAND);
                    pEranikus->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                }
            }
            break;
        case TYPE_SHADE_OF_ERANIKUS:
            m_auiEncounter[3] = uiData;
            if (uiData == IN_PROGRESS)
                CallForHelp(NPC_SHADE_OF_ERANIKUS, m_lDragonkinGUID);
            break;
        case TYPE_MALFURION:
            m_auiEncounter[4] = uiData;
            break;
        case TYPE_AVATAR_OF_HAKKAR:
            m_auiEncounter[5] = uiData;
            switch(uiData)
            {
                case FAIL:                                  // Disable Summoning of Avatar of Hakkar
                case DONE:                                  // Avatar of Hakkar died
                {
                    HandleGameObject(GO_SANCTUM_DOOR_1, true);
                    HandleGameObject(GO_SANCTUM_DOOR_2, true);
                    
                    if (!m_lEternalFlameGUID.empty())
                        for(GUIDList::iterator itr = m_lEternalFlameGUID.begin(); itr != m_lEternalFlameGUID.end(); ++itr)
                            InteractWithGo(*itr, false);
                    
                    Map::PlayerList const& lPlayers = instance->GetPlayers();
                    if (!lPlayers.isEmpty())
                        for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                        {
                            if (Player* pPlayer = itr->getSource())
                                if (pPlayer->HasItemCount(ITEM_HAKKARI_BLOOD, 1))
                                    pPlayer->DestroyItemCount(ITEM_HAKKARI_BLOOD, 1, true);
                        }
                    break;
                }
                case IN_PROGRESS:                           // Summoning of Avatar of Hakkar in progress
                case SPECIAL:                               // Avatar of Hakkar summoned
                {
                    HandleGameObject(GO_SANCTUM_DOOR_1, false);
                    HandleGameObject(GO_SANCTUM_DOOR_2, false);
                    break;
                }
            }
            break;
    }

    OUT_SAVE_INST_DATA;

    std::ostringstream saveStream;
    saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
	<< m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5];

    strInstData = saveStream.str();

    SaveToDB();
    OUT_SAVE_INST_DATA_COMPLETE;
}

uint32 instance_sunken_temple::GetData(uint32 uiType)
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_sunken_temple::Update(uint32 uiDiff)
{
    if (m_bIsSerpentSummoning)
    {
        if (m_uiSerpentSummonTimer <= uiDiff)
        {
			if (GameObject* pSanctum = GetSingleGameObjectFromStorage(GO_SANCTUM_OF_THE_FALLEN_GOD))
                pSanctum->SummonCreature(NPC_HAKKARI_MINION, Blood[m_uiSerpent].x, Blood[m_uiSerpent].y, Blood[m_uiSerpent].z, Blood[m_uiSerpent].o, TEMPSUMMON_DEAD_DESPAWN, 0);
            ++m_uiSerpent;
            m_uiSerpentSummonTimer = urand(1500,2500);
            if (m_uiSerpent == 8)
                m_bIsSerpentSummoning = false;
        }
        else
            m_uiSerpentSummonTimer -= uiDiff;
    }
}

void instance_sunken_temple::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2]
        >> m_auiEncounter[3] >> m_auiEncounter[4] >> m_auiEncounter[5];

    OUT_LOAD_INST_DATA_COMPLETE;
}

void instance_sunken_temple::HandleAtalaiStatue(uint32 uiEntry)
{
    switch(uiEntry)
    {
        case GO_ATALAI_STATUE_S:
            if (!m_uiStatueOrder)
                m_uiStatueOrder |= ATALAI_STATUE_S;
            break;
        case GO_ATALAI_STATUE_N:
            if (m_uiStatueOrder & ATALAI_STATUE_S)
                m_uiStatueOrder |= ATALAI_STATUE_N;
            break;
        case GO_ATALAI_STATUE_SW:
            if (m_uiStatueOrder & ATALAI_STATUE_N)
                m_uiStatueOrder |= ATALAI_STATUE_SW;
            break;
        case GO_ATALAI_STATUE_SE:
            if (m_uiStatueOrder & ATALAI_STATUE_SW)
                m_uiStatueOrder |= ATALAI_STATUE_SE;
            break;
        case GO_ATALAI_STATUE_NW:
            if (m_uiStatueOrder & ATALAI_STATUE_SE)
                m_uiStatueOrder |= ATALAI_STATUE_NW;
            break;
        case GO_ATALAI_STATUE_NE:
            if (m_uiStatueOrder & ATALAI_STATUE_NW)
                m_uiStatueOrder |= ATALAI_STATUE_NE;
            break;
        default:
            return;
    }

    // complete event or set in progress
    if (m_uiStatueOrder == ATALAI_STATUE_ALL)
        SetData(TYPE_ATALALARION, SPECIAL);
    else if (GetData(TYPE_ATALALARION) != IN_PROGRESS)
        SetData(TYPE_ATALALARION, IN_PROGRESS);
}

void instance_sunken_temple::SummonAtalalarion(GameObject* pGo)
{
    float fX, fY, fZ;
    pGo->GetPosition(fX, fY, fZ);
    pGo->SummonCreature(NPC_ATALALARION, fX+5, fY+5, fZ, 0, TEMPSUMMON_DEAD_DESPAWN, 120000);
}

void instance_sunken_temple::CallForHelp(uint32 uiCalllerEntry, GUIDList lCallings)
{
    if (!uiCalllerEntry || lCallings.empty())
        return;

	if (Creature* pCaller = GetSingleCreatureFromStorage(uiCalllerEntry))
    {
        for(GUIDList::iterator itr = lCallings.begin(); itr != lCallings.end(); ++itr)
        {
            Creature* pCalling = instance->GetCreature(*itr);
            if (pCalling && pCalling->isAlive())
            {
                if (pCaller->GetEntry() == NPC_SHADE_OF_ERANIKUS)
                    pCaller->GetMap()->CreatureRelocation(pCalling, -661.89f, 60.38f, -90.83f, 4.75f);
                pCalling->GetMotionMaster()->MoveChase(pCaller->getVictim());
                pCalling->AI()->AttackStart(pCaller->getVictim());
            }
        }
    }
}

void instance_sunken_temple::HandleAvatarEventWave()
{
    ++m_uiWaveCount;

    GameObject* pSanctum = GetSingleGameObjectFromStorage(GO_SANCTUM_OF_THE_FALLEN_GOD);
    if (!pSanctum)
        return;

    float fX, fY, fZ, fO;
    pSanctum->GetPosition(fX, fY, fZ);
    fO = pSanctum->GetOrientation();

    switch(m_uiWaveCount)
    {
        case 1:
        case 3:
        case 5:
        case 7:
        {
            m_uiSerpent = 0;
            m_bIsSerpentSummoning = true;
            pSanctum->SummonCreature(NPC_HAKKARI_BLOODKEEPER, fX+2, fY+2, fZ, fO, TEMPSUMMON_DEAD_DESPAWN, 15000);
            if (GetData(TYPE_AVATAR_OF_HAKKAR) != IN_PROGRESS)
                SetData(TYPE_AVATAR_OF_HAKKAR, IN_PROGRESS);
            break;
        }
        case 2:
        case 4:
        case 6:
        {
            for(uint8 i = 0; i < 4; ++i)
                pSanctum->SummonCreature(NPC_HAKKARI_MINION, Supp[i].x, Supp[i].y, Supp[i].z, Supp[i].o, TEMPSUMMON_DEAD_DESPAWN, 0);
            uint32 j = urand(4,5);
            if (Creature* pSuppressor = pSanctum->SummonCreature(NPC_NIGHTMARE_SUPPRESSOR, Supp[j].x, Supp[j].y, Supp[j].z, Supp[j].o, TEMPSUMMON_DEAD_DESPAWN, 30000))
                pSuppressor->GetMotionMaster()->MovePoint(0, Supp[j+2].x, Supp[j+2].y, Supp[j+2].z);
            if (GetData(TYPE_AVATAR_OF_HAKKAR) != IN_PROGRESS)
                SetData(TYPE_AVATAR_OF_HAKKAR, IN_PROGRESS);
            break;
        }
        case 8:
        {
            if (Creature* pShade = GetSingleCreatureFromStorage(NPC_SHADE_OF_HAKKAR))
                pShade->RemoveFromWorld();

            pSanctum->SummonCreature(NPC_AVATAR_OF_HAKKAR, fX, fY, fZ, fO, TEMPSUMMON_DEAD_DESPAWN, 120000);
            SetData(TYPE_AVATAR_OF_HAKKAR, SPECIAL);
            break;
        }
    }
}

InstanceData* GetInstanceData_instance_sunken_temple(Map* pMap)
{
    return new instance_sunken_temple(pMap);
}

void AddSC_instance_sunken_temple()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_sunken_temple";
    pNewScript->GetInstanceData = &GetInstanceData_instance_sunken_temple;
    pNewScript->RegisterSelf();
}
