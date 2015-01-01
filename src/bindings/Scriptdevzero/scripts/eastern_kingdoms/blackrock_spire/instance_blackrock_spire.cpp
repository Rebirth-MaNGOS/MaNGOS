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
SDName: Instance_Blackrock_Spire
SD%Complete: 90
SDComment:
SDCategory: Blackrock Spire
EndScriptData */

#include "precompiled.h"
#include "blackrock_spire.h"

instance_blackrock_spire::instance_blackrock_spire(Map* pMap) : ScriptedInstance(pMap),
    m_bEmberseerGrowing(false),
    m_bStartUpLoader(true),

    m_uiEmberseerGrowingTimer(0),
    m_uiRookeryEggsTimer(1000),
    m_uiStartUpLoaderTimer(5000)
{
    Initialize();
}

void instance_blackrock_spire::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

    for(uint8 i = 0; i < MAX_DRAGONSPINE_RUNE; i++)
        m_auiDragonspineRuneGUID[i].Clear();

    m_lDragonspineMobs.clear();
    m_lDarkIronRunes.clear();
    m_lIncarcerators.clear();
    m_lRookeryEggs.clear();
    m_lChromaticEliteGuards.clear();
    m_lTempList.clear();
}

void instance_blackrock_spire::OnPlayerEnter(Player* pPlayer)
{
    SetLavaState(pPlayer, true);
}

void instance_blackrock_spire::OnPlayerLeave(Player* pPlayer)
{
    SetLavaState(pPlayer, false);
}

void instance_blackrock_spire::OnCreatureCreate(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
		//Rare Spawns 
		case 9218:
		case 9718:
		case 10376:
		case 9596:
			SetRareBoss(pCreature,5);
			break;
        // Bosses    
        case NPC_PYROGUARD_EMBERSEER:
        case NPC_SCARSHIELD_INFILTRATOR:
            break;
        case NPC_LORD_VICTOR_NEFARIUS:
            if (m_auiEncounter[5] == DONE)
                pCreature->RemoveFromWorld();
            break;
        case NPC_JED_RUNEWATCHER:
            SetRareBoss(pCreature, 5);
            return;

        // Trash
        case NPC_BLACKHAND_INCARCERATOR:
            m_lIncarcerators.push_back(pCreature->GetObjectGuid());
            return;
        case NPC_BLACKHAND_VETERAN:
        case NPC_BLACKHAND_SUMMONER:
            m_lDragonspineMobs.push_back(pCreature->GetObjectGuid());
            return;
        case NPC_CHROMATIC_ELITE_GUARD:
            m_lChromaticEliteGuards.push_back(pCreature->GetObjectGuid());
            return;
        default:
            return;
    }

    m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
}

void instance_blackrock_spire::OnCreatureDeath(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_BLACKHAND_VETERAN:
        case NPC_BLACKHAND_SUMMONER:
        {
            if (GetData(TYPE_DRAGONSPINE_HALL) == DONE)
                break;

            // Sort mobs if they weren't sorted yet (players don't step on AT -> server crash after UBRS doors)
            if (GetData(TYPE_DRAGONSPINE_HALL) != IN_PROGRESS)
                SortDragonspineMobs();

            for (uint8 i = 0; i < MAX_DRAGONSPINE_RUNE; ++i)
            {
                if (m_alRuneMobsGUID[i].empty())                        // Make sure that all runes have proper state
                {
                    GameObject* pRune = instance->GetGameObject(m_auiDragonspineRuneGUID[i]);
                    if (pRune && pRune->GetGoState() != GO_STATE_READY)
                        pRune->SetGoState(GO_STATE_READY);

                    continue;
                }

                m_alRuneMobsGUID[i].remove(pCreature->GetObjectGuid());
                if (m_alRuneMobsGUID[i].empty())
                {
                    if (GameObject* pRune = instance->GetGameObject(m_auiDragonspineRuneGUID[i]))
                        pRune->SetGoState(GO_STATE_READY);
                }
            }

            bool bCanOpen = true;
            for (uint8 i = 0; i < MAX_DRAGONSPINE_RUNE; ++i)
            {
                GameObject* pRune = instance->GetGameObject(m_auiDragonspineRuneGUID[i]);

                if (pRune && pRune->GetGoState() == GO_STATE_ACTIVE)
                {
                    bCanOpen = false;
                    break;
                }
            }

            if (bCanOpen)
                SetData(TYPE_DRAGONSPINE_HALL, DONE);
            break;
        }
        case NPC_BLACKHAND_INCARCERATOR:
        {
            m_lTempList.remove(pCreature->GetObjectGuid());

            if (m_lTempList.empty())
            {
                if (Creature* pEmberseer = GetSingleCreatureFromStorage(NPC_PYROGUARD_EMBERSEER))
                    pEmberseer->CastSpell(pEmberseer, SPELL_EMBERSEER_GROWING, false);
                
                m_lTempList.clear();
                m_lTempList = m_lDarkIronRunes;
                m_bEmberseerGrowing = true;
                m_uiEmberseerGrowingTimer = 9000;
            }
            else
                debug_log("SD0: Instance UBRS: %lu Blackhand Incarcerators left to kill.", m_lTempList.size());
            break;
        }
        default:
            break;
    }
}

void instance_blackrock_spire::OnObjectCreate(GameObject* pGo)
{
    switch(pGo->GetEntry())
    {
        case GO_DRAGONSPINE_HALL_RUNE_1:
            m_auiDragonspineRuneGUID[0] = pGo->GetObjectGuid();
            if (m_auiEncounter[2] == DONE)
                pGo->SetGoState(GO_STATE_READY);
            break;
        case GO_DRAGONSPINE_HALL_RUNE_2:
            m_auiDragonspineRuneGUID[1] = pGo->GetObjectGuid();
            if (m_auiEncounter[2] == DONE)
                pGo->SetGoState(GO_STATE_READY);
            return;
        case GO_DRAGONSPINE_HALL_RUNE_3:
            m_auiDragonspineRuneGUID[2] = pGo->GetObjectGuid();
            if (m_auiEncounter[2] == DONE)
                pGo->SetGoState(GO_STATE_READY);
            return;
        case GO_DRAGONSPINE_HALL_RUNE_4:
            m_auiDragonspineRuneGUID[3] = pGo->GetObjectGuid();
            if (m_auiEncounter[2] == DONE)
                pGo->SetGoState(GO_STATE_READY);
            return;
        case GO_DRAGONSPINE_HALL_RUNE_5:
            m_auiDragonspineRuneGUID[4] = pGo->GetObjectGuid();
            if (m_auiEncounter[2] == DONE)
                pGo->SetGoState(GO_STATE_READY);
            return;
        case GO_DRAGONSPINE_HALL_RUNE_6:
            m_auiDragonspineRuneGUID[5] = pGo->GetObjectGuid();
            if (m_auiEncounter[2] == DONE)
                pGo->SetGoState(GO_STATE_READY);
            return;
        case GO_DRAGONSPINE_HALL_RUNE_7:
            m_auiDragonspineRuneGUID[6] = pGo->GetObjectGuid();
            if (m_auiEncounter[2] == DONE)
                pGo->SetGoState(GO_STATE_READY);
            return;
        case GO_DRAGONSPINE_HALL_DOOR:
            if (m_auiEncounter[2] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_EMBERSEER_DOOR_IN:
        case GO_EMBERSEER_DOOR_OUT:
            if (m_auiEncounter[3] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_DARK_IRON_RUNE_1:
        case GO_DARK_IRON_RUNE_2:
        case GO_DARK_IRON_RUNE_3:
        case GO_DARK_IRON_RUNE_4:
        case GO_DARK_IRON_RUNE_5:
        case GO_DARK_IRON_RUNE_6:
        case GO_DARK_IRON_RUNE_7:
			m_lDarkIronRunes.push_back(pGo->GetObjectGuid());
            if (m_auiEncounter[3] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            return;
        case GO_ROOKERY_EGG:
            m_lRookeryEggs.push_back(pGo->GetObjectGuid());
            return;
        case GO_STADIUM_DOOR:
        case GO_STADIUM_PORTCULLIS:
            break;
        case GO_THE_FURNACE_DOOR:
            if (m_auiEncounter[5] == DONE || m_auiEncounter[7] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_DRAKKISATH_GATE01:
        case GO_DRAKKISATH_GATE02:
            if (m_auiEncounter[7] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
    }

    m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_blackrock_spire::SetData(uint32 uiType, uint32 uiData)
{
    switch(uiType)
    {
        // LBRS
        case TYPE_MOR_GRAYHOOF:
            m_auiEncounter[0] = uiData;
            break;
        case TYPE_UROK_DOOMHOWL:
            m_auiEncounter[1] = uiData;
            break;
        // UBRS
        case TYPE_DRAGONSPINE_HALL:
            if (uiData == DONE)
                HandleGameObject(GO_DRAGONSPINE_HALL_DOOR, true);
            m_auiEncounter[2] = uiData;
            break;
        case TYPE_PYROGUARD_EMBERSEER:
            switch(uiData)
            {
                case NOT_STARTED:
                    for(GUIDList::iterator itr = m_lIncarcerators.begin(); itr != m_lIncarcerators.end(); ++itr)
                        if (Creature* pIncarcerator = instance->GetCreature(*itr))
                            pIncarcerator->Respawn();

                    for(GUIDList::iterator itr = m_lDarkIronRunes.begin(); itr != m_lDarkIronRunes.end(); ++itr)
                        HandleGameObject(*itr, false);

                    m_lTempList.clear();
                    HandleGameObject(GO_EMBERSEER_DOOR_IN, true);
                    break;
                case IN_PROGRESS:
                    if (m_auiEncounter[3] != IN_PROGRESS)
                    {
                        m_lTempList = m_lIncarcerators;
                        for(GUIDList::iterator itr = m_lIncarcerators.begin(); itr != m_lIncarcerators.end(); ++itr)
                        {
                            if (Creature* pIncarcerator = instance->GetCreature(*itr))
                            {
                                pIncarcerator->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE + UNIT_FLAG_NOT_SELECTABLE);
                                pIncarcerator->InterruptNonMeleeSpells(false);
                                pIncarcerator->SetInCombatWithZone();
                            }
                        }

                        HandleGameObject(GO_EMBERSEER_DOOR_IN, false);
                        HandleGameObject(GO_EMBERSEER_DOOR_OUT, false);
                    }
                    break;
                case DONE:
                    for(GUIDList::iterator itr = m_lDarkIronRunes.begin(); itr != m_lDarkIronRunes.end(); ++itr)
                        HandleGameObject(*itr, false);

                    HandleGameObject(GO_EMBERSEER_DOOR_IN, true);
                    HandleGameObject(GO_EMBERSEER_DOOR_OUT, true);
                    break;
            }
            m_auiEncounter[3] = uiData;
            break;
        case TYPE_ROOKERY:
            m_auiEncounter[4] = uiData;
            break;
        case TYPE_REND_BLACKHAND:
            switch(uiData)
            {
                case NOT_STARTED:
                    HandleGameObject(GO_STADIUM_DOOR, true);
                    break;
                case IN_PROGRESS:
                    HandleGameObject(GO_STADIUM_DOOR, false);
                    if (Creature* pNefarius = GetSingleCreatureFromStorage(NPC_LORD_VICTOR_NEFARIUS))
                    {
                        pNefarius->GetMotionMaster()->MovePoint(0, 147.87f, -444.59f, 121.97f);

                        if (Creature* pRend = pNefarius->SummonCreature(NPC_WARCHIEF_REND_BLACKHAND, 167.32f, -456.94f, 121.97f, 2.2f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                        {
                            pRend->setFaction(FACTION_FRIENDLY_FOR_ALL);
                            pRend->GetMotionMaster()->MovePoint(0, 154.86f, -444.63f, 121.97f);
                            CreatureCreatePos pos(pRend->GetMap(), 154.86f, -444.63f, 121.97f, 1.58f);
                            pRend->SetSummonPoint(pos);
                        }
                    }
                    break;
                case DONE:
                    HandleGameObject(GO_STADIUM_DOOR, true);
                    HandleGameObject(GO_THE_FURNACE_DOOR, true);

                    if (Creature* pNefarius = GetSingleCreatureFromStorage(NPC_LORD_VICTOR_NEFARIUS))
                    {
                        DoScriptText(SAY_NEFARIAN_REND_END, pNefarius);
                        pNefarius->RemoveFromWorld();
                    }
                    break;
            }
            m_auiEncounter[5] = uiData;
            break;
        case TYPE_LORD_VALTHALAK:
            m_auiEncounter[6] = uiData;
            break;
        case TYPE_DRAKKISATH:
            switch(uiData)
            {
                case FAIL:
                    if (!m_lChromaticEliteGuards.empty())
                    {
                        for(GUIDList::iterator itr = m_lChromaticEliteGuards.begin(); itr != m_lChromaticEliteGuards.end(); ++itr)
                        {
                            Creature* pGuard = instance->GetCreature(*itr);
                            if (pGuard && !pGuard->isAlive() && pGuard->isDead())
                            {
                                pGuard->RemoveCorpse();
                                pGuard->Respawn();
                            }
                        }
                    }
                    break;
                case DONE:
                    HandleGameObject(GO_DRAKKISATH_GATE01, true);
                    HandleGameObject(GO_DRAKKISATH_GATE02, true);
                    break;
            }
            m_auiEncounter[7] = uiData;
            break;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " " << m_auiEncounter[6] << " " << m_auiEncounter[7];

        strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_blackrock_spire::GetData(uint32 uiType)
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_blackrock_spire::Update(uint32 uiDiff)
{
    if (m_bStartUpLoader)
    {
        if (m_uiStartUpLoaderTimer <= uiDiff)
        {
            if (GetData(TYPE_PYROGUARD_EMBERSEER) != DONE)
                SetData(TYPE_PYROGUARD_EMBERSEER, NOT_STARTED);

            if (GetData(TYPE_REND_BLACKHAND) == DONE)
                if (Creature* pNefarius = GetSingleCreatureFromStorage(NPC_LORD_VICTOR_NEFARIUS))
                    pNefarius->RemoveFromWorld();

            m_bStartUpLoader = false;
        }
        else
            m_uiStartUpLoaderTimer -= uiDiff;
    }

    if (m_bEmberseerGrowing)
    {
        if (m_uiEmberseerGrowingTimer <= uiDiff)
        {
            if (!m_lTempList.empty())
            {
                HandleGameObject(m_lTempList.back(), true);
                m_lTempList.pop_back();
                m_uiEmberseerGrowingTimer = 9000;
            }
            else
                m_bEmberseerGrowing = false;
        }
        else
            m_uiEmberseerGrowingTimer -= uiDiff;
    }

    // The Rookery Eggs
    if (m_uiRookeryEggsTimer <= uiDiff)
    {
        Map::PlayerList const &lPlayers = instance->GetPlayers();

        if (!m_lRookeryEggs.empty() && !lPlayers.isEmpty())
        {
            for(GUIDList::iterator i = m_lRookeryEggs.begin(); i != m_lRookeryEggs.end(); ++i)
            {
                if (GameObject* pEgg = instance->GetGameObject(*i))
                {
                    for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                    {
                        Player* pPlayer = (itr)->getSource();
                        if (pPlayer && pPlayer->IsWithinDist(pEgg, 3.0f))
                        {
                            float fX, fY, fZ;
                            pEgg->GetPosition(fX, fY, fZ);
                            pPlayer->CastSpell(pPlayer, 15750, false);    // for visual effect
                            pEgg->RemoveFromWorld();
                            pEgg->SetRespawnTime(43200);
                            for(uint32 i = 0; i < 3; ++i)
                            {
                              if (Creature* Whelp = pEgg->SummonCreature(NPC_ROOKERY_WHELP, fX, fY, fZ, 0, TEMPSUMMON_DEAD_DESPAWN, 10000))
                                    Whelp->AI()->AttackStart(pPlayer);
                            }
                        }
                    }
                }
            }
        }
        m_uiRookeryEggsTimer = 500;
    }
    else
        m_uiRookeryEggsTimer -= uiDiff;

    DoLavaDamage(uiDiff);
}

void instance_blackrock_spire::Load(const char* in)
{
    if (!in)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(in);

    std::istringstream loadStream(in);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3] >> m_auiEncounter[4] >> m_auiEncounter[5] >> m_auiEncounter[6] >> m_auiEncounter[7];

    for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;

    OUT_LOAD_INST_DATA_COMPLETE;
}

void instance_blackrock_spire::SortDragonspineMobs()
{
    // we do this for detecting at least on sorting check
    if (GetData(TYPE_DRAGONSPINE_HALL) == NOT_STARTED)
        SetData(TYPE_DRAGONSPINE_HALL, IN_PROGRESS);

    // If all mobs were sorted or event is done
    if (m_lDragonspineMobs.empty() || GetData(TYPE_DRAGONSPINE_HALL) == DONE)
        return;

    // Sort Mobs in Dragonspine Hall
    for(GUIDList::iterator itr = m_lDragonspineMobs.begin(); itr != m_lDragonspineMobs.end();)
    {
        bool bAlreadyIterated = false;

        Creature* pMob = instance->GetCreature(*itr);
        if (pMob && pMob->isAlive())
        {
            for(uint8 i = 0; i < MAX_DRAGONSPINE_RUNE; ++i)
            {
                if (GameObject* pRune = instance->GetGameObject(m_auiDragonspineRuneGUID[i]))
                {
                    if (pMob->IsWithinDistInMap(pRune, 20.0f, false) && pMob->IsWithinLOSInMap(pRune))
                    {
                        m_alRuneMobsGUID[i].push_back(*itr);
                        itr = m_lDragonspineMobs.erase(itr);
                        bAlreadyIterated = true;

                        // Check proper state of rune if alive mob is nearby
                        if (pRune->GetGoState() != GO_STATE_ACTIVE)
                            pRune->SetGoState(GO_STATE_ACTIVE);
                        break;
                    }
                }
            }
        }

        if (itr != m_lDragonspineMobs.end() && !bAlreadyIterated)
            ++itr;
    }
}

InstanceData* GetInstanceData_instance_blackrock_spire(Map* pMap)
{
    return new instance_blackrock_spire(pMap);
}

void AddSC_instance_blackrock_spire()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "instance_blackrock_spire";
    pNewscript->GetInstanceData = &GetInstanceData_instance_blackrock_spire;
    pNewscript->RegisterSelf();
}
