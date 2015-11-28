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
SDName: Instance_Blackrock_Depths
SD%Complete: 80
SDComment: events: Ring of the Law, Darkkeeper Portrait, The Vault (In Hearth of the Mountain)
SDCategory: Blackrock Depths
EndScriptData */

#include "precompiled.h"
#include "blackrock_depths.h"

instance_blackrock_depths::instance_blackrock_depths(Map* pMap) : ScriptedInstance(pMap),
    m_bBannerSpawned(false),
    m_uiBarAleCount(0),
    m_uiBarrelsDestroyedCount(0),
    m_uiCoffersOpenedCount(0),
    m_uiDoomgripsBandDeadCount(0),
    m_uiRingOfTheLawSpecsTimer(1000)
{
    Initialize();
};

void instance_blackrock_depths::DoOpenToumbEntrance()
{
    // Open the entrace to the toumb if the boss fight is interrupted.
    GameObject* toumbEntrance = GetSingleGameObjectFromStorage(GO_TOMB_ENTER);
    if (toumbEntrance)
        toumbEntrance->SetGoState(GO_STATE_ACTIVE);
}


void instance_blackrock_depths::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

    m_uiRingSpectators.clear();
    m_uiBarPatrons.clear();
    m_uiLyceumCreatures.clear();
    m_uiBurningSpirit.clear();
    m_uiManufactoryCreatures.clear();
    m_uiFlamelashRunes.clear();

    m_bRelicCoffersRemoved = false;

	m_bBridgeEventDone = false;

    m_uiOpenCofferDoors = 0;
    m_goSecretSafe = 0;
}

void instance_blackrock_depths::OnPlayerEnter(Player* /*pPlayer*/)
{
}

void instance_blackrock_depths::OnPlayerLeave(Player* /*pPlayer*/)
{
}

void instance_blackrock_depths::OnCreatureCreate(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
    case NPC_PRINCESS:
        if (m_auiEncounter[10] == DONE)
            pCreature->setFaction(FACTION_FRIENDLY_FOR_ALL);
        break;
    case NPC_EMPEROR:
    case NPC_PHALANX:
    case NPC_HATEREL:
    case NPC_ANGERREL:
    case NPC_VILEREL:
    case NPC_GLOOMREL:
    case NPC_SEETHREL:
    case NPC_DOOMREL:
    case NPC_DOPEREL:
    case NPC_FLAMELASH:

    case NPC_KHARAN_MIGHTHAMMER:
    case NPC_COMMANDER_GORSHAK:
    case NPC_MARSHAL_WINSDOR:
    case NPC_DUGHAL_STORMWING:
    case NPC_CREST_KILLER:
    case NPC_TOBIAS_SEECHER:
    case NPC_SHILL_DINGER:
    case NPC_JAZ:
    case NPC_OGRABISI:
        break;

    case NPC_PLUGGER_SPAZZRING:
        if (m_auiEncounter[6] == DONE)
            pCreature->setFaction(FACTION_ENEMY);
        break;
    case NPC_PRIVATE_ROCKNOT:
    case NPC_MISTRESS_NAGMARA:
    case NPC_THELDREN_TRIGGER:
    case NPC_LORD_ARGELMARCH:
        break;

    case NPC_ANVILRAGE_SOLDIER:
    case NPC_ANVILRAGE_MEDIC:
    case NPC_ANVILRAGE_OFFICER:
    case NPC_SHADOWFORGE_PEASANT:
    case NPC_SHADOWFORGE_CITIZEN:
    case NPC_SHADOWFORGE_SANETOR:
    case NPC_ARENA_SPECTATOR:
        m_uiRingSpectators.push_back(pCreature->GetObjectGuid());
        return;

    case NPC_ANVILRAGE_RESERVIST:
    case NPC_SHADOWFORGE_FLAMEKEEPER:
        m_uiLyceumCreatures.push_back(pCreature->GetObjectGuid());
        return;

    case NPC_GRIM_PATRON:
    case NPC_GUZZLING_PATRON:
    case NPC_HAMMERED_PATRON:
        m_uiBarPatrons.push_back(pCreature->GetObjectGuid());
        if (m_auiEncounter[6] == DONE)
            pCreature->setFaction(FACTION_ENEMY);
        return;

    case NPC_BURNING_SPIRIT:
        m_uiBurningSpirit.push_back(pCreature->GetObjectGuid());
        return;
    case NPC_DOOMFORGE_ARCANESMITH:
    case NPC_WEAPON_TECHNICIAN:
    case NPC_RAGEREAVER_GOLEM:
        m_uiManufactoryCreatures.push_back(pCreature->GetObjectGuid());
        return;

        // Rare spawn
    case NPC_PANZOR:
        SetRareBoss(pCreature, 10);
        break;
    default:
        return;
    }

    m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
}

void instance_blackrock_depths::OnObjectCreate(GameObject* pGo)
{
    switch(pGo->GetEntry())
    {
        // Cells
    case GO_CELL_DOOR__DUGHAL:
    case GO_CELL_DOOR__WINDSOR:
    case GO_CELL_DOOR__GORSHAK:
    case GO_CELL_DOOR__KHARAN:
    case GO_CELL_DOOR__TOBIAS:
    case GO_CELL_DOOR__CREST:
    case GO_CELL_DOOR__JAZ__OGRABISI:
    case GO_CELL_DOOR__SHILL:
    case GO_SUPPLY_ROOM_DOOR:
        break;

        // Ring of the Law
    case GO_ARENA1:
    case GO_ARENA2:
        break;
    case GO_ARENA3:
        if (m_auiEncounter[0] == DONE)
            pGo->SetGoState(GO_STATE_ACTIVE);
        break;
    case GO_ARENA4:
    case GO_ARENA_SPOILS:
    case GO_TEMP_BRD_ARENA:
        break;
    case GO_BANNER_OF_PROVOCATION:
        m_bBannerSpawned = true;
        return;

    case GO_BAR_THUNDERBREW_LAGER:
        if (m_auiEncounter[5] == DONE)
        {
            pGo->SetGoState(GO_STATE_ACTIVE);
            pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
        }
        return;

        // Other
    case GO_SHADOW_LOCK:
        //case GO_SHADOW_MECHANISM:
    case GO_GIANT_DOOR_FAKE_COLLISION:
    case GO_SHADOW_DUMMY:
        if (m_auiEncounter[13] == DONE)
            pGo->SetGoState(GO_STATE_ACTIVE);
        return;
    case GO_SHADOW_GIANT_DOOR:
        pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
        pGo->SetGoState(GO_STATE_ACTIVE);
        return;

        // The Grim Guzzler (Bar)
    case GO_BAR_KEG_SHOT:
    case GO_BAR_KEG_TRAP:
        break;
    case GO_BAR_DOOR:
        if (m_auiEncounter[2] == DONE || m_auiEncounter[3] == DONE || m_auiEncounter[4] == DONE || m_auiEncounter[5] == DONE || m_auiEncounter[6] == DONE)
            pGo->SetGoState(GO_STATE_ACTIVE);
        break;

        // The Black Vault: Dark Keeper Nameplate + Dark Coffer + Relic Coffer Door
    case GO_DARK_KEEPER_PORTRAIT:
    case GO_DARK_KEEPER_NAMEPLATE1:
    case GO_DARK_KEEPER_NAMEPLATE2:
    case GO_DARK_KEEPER_NAMEPLATE3:
    case GO_DARK_KEEPER_NAMEPLATE4:
    case GO_DARK_KEEPER_NAMEPLATE5:
    case GO_DARK_KEEPER_NAMEPLATE6:
        break;

    case GO_RELIC_COFFER_DOOR_01:
    case GO_RELIC_COFFER_DOOR_02:
    case GO_RELIC_COFFER_DOOR_03:
    case GO_RELIC_COFFER_DOOR_04:
    case GO_RELIC_COFFER_DOOR_05:
    case GO_RELIC_COFFER_DOOR_06:
    case GO_RELIC_COFFER_DOOR_07:
    case GO_RELIC_COFFER_DOOR_08:
    case GO_RELIC_COFFER_DOOR_09:
    case GO_RELIC_COFFER_DOOR_10:
    case GO_RELIC_COFFER_DOOR_11:
    case GO_RELIC_COFFER_DOOR_12:
        if (m_auiEncounter[1] == DONE)
            pGo->SetGoState(GO_STATE_ACTIVE);
        return;
    case GO_SECRET_DOOR:
        if (m_auiEncounter[1] == DONE)
            pGo->SetGoState(GO_STATE_ACTIVE);
        break;

        // Ambassador Flamelash
    case GO_DARK_IRON_DWARF_RUNE_A01:
    case GO_DARK_IRON_DWARF_RUNE_B01:
    case GO_DARK_IRON_DWARF_RUNE_C01:
    case GO_DARK_IRON_DWARF_RUNE_D01:
    case GO_DARK_IRON_DWARF_RUNE_E01:
    case GO_DARK_IRON_DWARF_RUNE_F01:
    case GO_DARK_IRON_DWARF_RUNE_G01:
        m_uiFlamelashRunes.push_back(pGo->GetObjectGuid());
        return;

        // The Seven
    case GO_TOMB_EXIT:
        if (m_auiEncounter[7] == DONE)
            pGo->SetGoState(GO_STATE_ACTIVE);
        break;
    case GO_TOMB_ENTER:
        pGo->SetGoState(GO_STATE_ACTIVE);
        break;
    case GO_SPECTRAL_CHALICE:
        break;
    case GO_CHEST_SEVEN:
        pGo->SetRespawnTime(10 * DAY);
        pGo->SetLootState(LootState::GO_JUST_DEACTIVATED);
        break;

        // The Lyceum + The Iron Hall (Magmus)
    case GO_LYCEUM:
        break;
    case GO_SHADOWFORGE_BRAZIER_N:
    case GO_SHADOWFORGE_BRAZIER_S:
        if (m_auiEncounter[8] == DONE || m_auiEncounter[9] == DONE || m_auiEncounter[10] == DONE)
        {
            pGo->SetGoState(GO_STATE_ACTIVE);
            pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
        }
        return;
    case GO_GOLEM_ROOM_N:
    case GO_GOLEM_ROOM_S:
        if (m_auiEncounter[8] == DONE || m_auiEncounter[9] == DONE || m_auiEncounter[10] == DONE)
            pGo->SetGoState(GO_STATE_ACTIVE);
        break;
    case GO_THRONE_ROOM:
        if (m_auiEncounter[9] == DONE || m_auiEncounter[10] == DONE)
            pGo->SetGoState(GO_STATE_ACTIVE);
        break;
    case GO_RELIC_COFFER:
        m_uiRelicCoffers.push_back(pGo);
        return;
    case GO_SECRET_SAFE:
	m_goSecretSafe = pGo;
	return;
    default:
        return;
    }

    m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_blackrock_depths::DoUseFlamelashRunes()
{
    if (!m_uiFlamelashRunes.empty())
    {
        for(GUIDList::iterator itr = m_uiFlamelashRunes.begin(); itr != m_uiFlamelashRunes.end(); ++itr)
            if (instance->GetGameObject(*itr))
                DoUseDoorOrButton(*itr);
    }
    else
        debug_log("SD0: Blackrock Depths: Flamelash's runes change state, but list of runes is empty!");
}

void instance_blackrock_depths::DoAggroManufactory()
{
    debug_log("SD0: Blackrock Depths: The Manufactory aggroed.");
    Creature* pLordArgelmarch = GetSingleCreatureFromStorage(NPC_LORD_ARGELMARCH);

    if (m_uiManufactoryCreatures.empty() || !pLordArgelmarch)
        return;

    if (!pLordArgelmarch->getVictim())
        return;

    Unit* pArgTarget = pLordArgelmarch->getVictim();

    for (GUIDList::iterator itr = m_uiManufactoryCreatures.begin(); itr != m_uiManufactoryCreatures.end(); ++itr)
    {
        if (Creature* pTarget = instance->GetCreature(*itr))
        {
            if (pTarget->isAlive() && pTarget->GetDistance(pLordArgelmarch) < 105.0f)
            {
                pTarget->GetMotionMaster()->MoveChase(pArgTarget);
                pTarget->AI()->AttackStart(pArgTarget);
            }
        }
    }
}

void instance_blackrock_depths::DoOpenBarDoor()
{
    HandleGameObject(GO_BAR_DOOR, true);
}

void instance_blackrock_depths::SetData(uint32 uiType, uint32 uiData)
{
    debug_log("SD2: Instance Blackrock Depths: SetData update (Type: %u Data %u)", uiType, uiData);

    switch(uiType)
    {
    case TYPE_RING_OF_LAW:
        m_auiEncounter[0] = uiData;
        if (uiData == DONE)
        {
            if (m_bBannerSpawned)
                DoRespawnGameObject(GO_ARENA_SPOILS, 2*HOUR);
            m_uiRingOfTheLawSpecsTimer = 100;
        }
        break;
    case TYPE_VAULT:
        m_auiEncounter[1] = uiData;
        switch(uiData)
        {
        case IN_PROGRESS:
            if (m_uiCoffersOpenedCount < 12)
                ++m_uiCoffersOpenedCount;
            else if (++m_uiDoomgripsBandDeadCount == 5)
            {
                HandleGameObject(GO_SECRET_DOOR, true);
                m_auiEncounter[1] = uiData = DONE;
            }
            break;
        case DONE:
            HandleGameObject(GO_SECRET_DOOR, true);
            break;
        }
        break;
    case TYPE_BAR:
        if (uiData == SPECIAL)
            ++m_uiBarAleCount;
        else
            m_auiEncounter[2] = uiData;
        break;
    case TYPE_BAR2:
        m_auiEncounter[3] = uiData;
        if (uiData == DONE)
            DoOpenBarDoor();
        break;
    case TYPE_BAR3:     // Mistress Nagmara
        m_auiEncounter[4] = uiData;
        if (uiData == DONE)
            DoOpenBarDoor();
        break;
    case TYPE_BAR4:
        m_auiEncounter[5] = uiData;
        if (uiData == IN_PROGRESS)
            ++m_uiBarrelsDestroyedCount;
        break;
    case TYPE_BAR5:
        m_auiEncounter[6] = uiData;
        if (uiData == DONE && !m_uiBarPatrons.empty())
            for(GUIDList::iterator i = m_uiBarPatrons.begin(); i != m_uiBarPatrons.end(); ++i)
            {
                if (Creature* target = instance->GetCreature(*i))
                    if (target->isAlive() && !target->isDead())
                        target->setFaction(FACTION_ENEMY);
            }
        break;
    case TYPE_TOMB_OF_SEVEN:
        m_auiEncounter[7] = uiData;
        switch(uiData)
        {
        case IN_PROGRESS:
            DoUseDoorOrButton(GO_TOMB_ENTER);
            break;
        case FAIL:
            DoOpenToumbEntrance();
            break;
        case DONE:
        {
            GameObject* pChest = GetSingleGameObjectFromStorage(GO_CHEST_SEVEN);
            if (pChest)
            {
                pChest->SetLootState(LootState::GO_READY);
                pChest->Respawn();
                pChest->Refresh();
            }

            DoUseDoorOrButton(GO_TOMB_EXIT);
            DoUseDoorOrButton(GO_TOMB_ENTER);
        }
        break;
        }
        break;
    case TYPE_LYCEUM:
        m_auiEncounter[8] = uiData;
        if (uiData == DONE)
        {
            HandleGameObject(GO_GOLEM_ROOM_N, true);
            HandleGameObject(GO_GOLEM_ROOM_S, true);

            if (!m_uiLyceumCreatures.empty())
            {
                for(GUIDList::iterator itr = m_uiLyceumCreatures.begin(); itr != m_uiLyceumCreatures.end(); ++itr)
                {
                    if (Creature* pTarget = instance->GetCreature(*itr))
                    {
                        pTarget->SetRespawnDelay(7200);
						if (!pTarget->isAlive())
						{
							pTarget->SetRespawnTime(7200);
							pTarget->SaveRespawnTime();
						}
					}
				}
			}
		}
        break;
    case TYPE_IRON_HALL:
        m_auiEncounter[9] = uiData;
        switch(uiData)
        {
        case IN_PROGRESS:
            HandleGameObject(GO_GOLEM_ROOM_N, false);
            HandleGameObject(GO_GOLEM_ROOM_S, false);
            break;
        case FAIL:
            HandleGameObject(GO_GOLEM_ROOM_N, true);
            HandleGameObject(GO_GOLEM_ROOM_S, true);
            break;
        case DONE:
            HandleGameObject(GO_GOLEM_ROOM_N, true);
            HandleGameObject(GO_GOLEM_ROOM_S, true);
            HandleGameObject(GO_THRONE_ROOM, true);
            break;
        }
        break;
    case TYPE_IMPERIAL_SEAT:
        m_auiEncounter[10] = uiData;
        switch(uiData)
        {
        case IN_PROGRESS:
            HandleGameObject(GO_THRONE_ROOM, false);
            break;
        case FAIL:
        case DONE:
            HandleGameObject(GO_THRONE_ROOM, true);
            break;
        }
        break;
    case TYPE_CHAMBER_OF_ENCHANTMENT:
        m_auiEncounter[11] = uiData;

        DoUseFlamelashRunes();

        // Despawn summoned Burning Spirits on event DONE
        if (uiData == DONE)
        {
            for(GUIDList::iterator itr = m_uiBurningSpirit.begin(); itr != m_uiBurningSpirit.end(); ++itr)
            {
                if (Creature* pBurningSpirit = instance->GetCreature(*itr))
                {
                    pBurningSpirit->SetDeathState(JUST_DIED);
                    pBurningSpirit->RemoveCorpse();
                }
            }
            m_uiBurningSpirit.clear();
        }
        break;
    case TYPE_MANUFACTORY:
        m_auiEncounter[12] = uiData;
        if (uiData == IN_PROGRESS)
            DoAggroManufactory();
        break;
    case TYPE_GIANT_DOOR:
        m_auiEncounter[13] = uiData;
        break;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " " << m_auiEncounter[6] << " " << m_auiEncounter[7] << " " << m_auiEncounter[8] << " " << m_auiEncounter[9] << " " << m_auiEncounter[10] << " " << m_auiEncounter[11] << " " << m_auiEncounter[12] << " " << m_auiEncounter[13];

        strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_blackrock_depths::GetData(uint32 uiType)
{
    switch(uiType)
    {
    case TYPE_RING_OF_LAW:
        return m_auiEncounter[0] == IN_PROGRESS && m_bBannerSpawned ? (uint32) SPECIAL : m_auiEncounter[0];
    case TYPE_VAULT:
        return m_auiEncounter[1] == IN_PROGRESS && m_uiCoffersOpenedCount == 12 ? (uint32) SPECIAL : m_auiEncounter[1];
    case TYPE_BAR:
        if (m_auiEncounter[2] == IN_PROGRESS && m_uiBarAleCount == 3)
            return SPECIAL;
        else
            return m_auiEncounter[2];
    case TYPE_BAR2:
        return m_auiEncounter[3];
    case TYPE_BAR3:
        return m_auiEncounter[4];
    case TYPE_BAR4:
        return ((m_auiEncounter[5] == IN_PROGRESS && m_uiBarrelsDestroyedCount == 3) ? (uint32) SPECIAL : m_auiEncounter[5]);
    case TYPE_BAR5:
        return m_auiEncounter[6];
    case TYPE_TOMB_OF_SEVEN:
        return m_auiEncounter[7];
    case TYPE_LYCEUM:
        return m_auiEncounter[8];
    case TYPE_IRON_HALL:
        return m_auiEncounter[9];
    case TYPE_IMPERIAL_SEAT:
        return m_auiEncounter[10];
    case TYPE_CHAMBER_OF_ENCHANTMENT:
        return m_auiEncounter[11];
    case TYPE_MANUFACTORY:
        return m_auiEncounter[12];
    case TYPE_GIANT_DOOR:
        return m_auiEncounter[13];
    }
    return 0;
}

void instance_blackrock_depths::Load(const char* in)
{
    if (!in)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(in);

    std::istringstream loadStream(in);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3] >> m_auiEncounter[4] >> m_auiEncounter[5] >> m_auiEncounter[6] >> m_auiEncounter[7] >> m_auiEncounter[8] >> m_auiEncounter[9] >> m_auiEncounter[10] >> m_auiEncounter[11] >> m_auiEncounter[12] >> m_auiEncounter[13];

    for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;

    // hack for vault
    if (m_auiEncounter[1] >= 100)
        m_auiEncounter[1] = NOT_STARTED;

    OUT_LOAD_INST_DATA_COMPLETE;
}

void instance_blackrock_depths::Update(uint32 uiDiff)
{
    // Spectators will change faction when event in Ring of the Law is done
    if (GetData(TYPE_RING_OF_LAW) == DONE)
    {
        if (m_uiRingOfTheLawSpecsTimer)
        {
            if (m_uiRingOfTheLawSpecsTimer <= uiDiff)
            {
                m_uiRingOfTheLawSpecsTimer = 0;
                debug_log("SD0: Blackrock Depths: Ring of the Law spectators faction change.");

                GameObject* TempGoArena = GetSingleGameObjectFromStorage(GO_TEMP_BRD_ARENA);
                Creature* pTheldrenTrigger = GetSingleCreatureFromStorage(NPC_THELDREN_TRIGGER);

                if (!m_uiRingSpectators.empty() && (TempGoArena || pTheldrenTrigger))
                {
                    for(GUIDList::iterator itr = m_uiRingSpectators.begin(); itr != m_uiRingSpectators.end(); ++itr)
                    {
                        if (Unit* pSpectator = instance->GetCreature(*itr))
                            if ((TempGoArena && pSpectator->IsWithinDistInMap(TempGoArena, 70.0f)) || (pTheldrenTrigger && pSpectator->IsWithinDistInMap(pTheldrenTrigger, 70.0f)))
                                pSpectator->setFaction(FACTION_NEUTRAL_TO_ALL);
                    }
                }
            }
            else
                m_uiRingOfTheLawSpecsTimer -= uiDiff;
        }
    }

    if(!m_bRelicCoffersRemoved) // Make sure noone of the coffers are in the world before the doors are opened.
    {
        m_bRelicCoffersRemoved = true;

        std::for_each(m_uiRelicCoffers.begin(), m_uiRelicCoffers.end(), [&](GameObject* current_object)
        {

            if (!current_object)
                return;

            if (current_object->IsInWorld())
                current_object->RemoveFromWorld();
        });

        if (m_goSecretSafe && m_goSecretSafe->IsInWorld())
            m_goSecretSafe->RemoveFromWorld();
    }
    
    // If 12 coffer doors are opened we spawn the secret safe.
    if (m_goSecretSafe && m_uiOpenCofferDoors >= 12)
	m_goSecretSafe->AddToWorld();

    if (GetData(TYPE_GIANT_DOOR) == DONE)
        HandleGameObject(GO_SHADOW_GIANT_DOOR, false);
}

InstanceData* GetInstanceData_instance_blackrock_depths(Map* pMap)
{
    return new instance_blackrock_depths(pMap);
}

void AddSC_instance_blackrock_depths()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "instance_blackrock_depths";
    pNewScript->GetInstanceData = &GetInstanceData_instance_blackrock_depths;
    pNewScript->RegisterSelf();
}
