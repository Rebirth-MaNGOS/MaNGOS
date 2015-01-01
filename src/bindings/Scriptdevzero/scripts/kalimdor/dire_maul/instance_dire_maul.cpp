/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* ScriptData
SDName: Instance_Dire_Maul
SD%Complete:
SDComment:
SDCategory: Dire Maul
EndScriptData */

/*
    Encounter 0 = Pusillin
    Encounter 1 = Zevrim Thornhoof
    Encounter 2 = Ironbark The Redeemed
    Encounter 3 = Alzzin The WildShaper
    Encounter 4 = Isalien
    Encounter 5 = Stomper Kreeg
    Encounter 6 = Mol'dar
    Encounter 7 = Fengus
    Encounter 8 = Slip'kik
    Encounter 9 = Captain Kromcrush
    Encounter 10 = Cho'Rush The Observer
    Encounter 11 = Gordok
    Encounter 12 = Tribute Run
    Encounter 13 = Knot Thimblejack
    Encounter 14 = Tendris Warpwood
    Encounter 15 = Barrier
    Encounter 16 = Immolthar
    Encounter 17 = Prince Tortheldrin
*/

#include "precompiled.h"
#include "dire_maul.h"

// TODO: Fill proper entries if we use multiple GOs for Tribute Run
const uint32 aTribute[] = {179564, 179564, 300250, 300250, 300251, 300252, 300253};

static Loc Tendris[]=
{
    {-36.01f, 473.43f, -23.31f, 0},
    {60.99f, 470.43f, -23.31f, 0}
};

instance_dire_maul::instance_dire_maul(Map* pMap) : ScriptedInstance(pMap),
	// We won't count pylons OnObjectCreate() because they are not all created --> could be exploited
	m_uiPylonCount(5)
{
	Initialize();
};



void instance_dire_maul::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

    m_uiAlzzinsMinionGUID.clear();
    m_uiGordokGUID.clear();
    m_uiGordokTributeGUID.clear();
    m_uiProtectorGUID.clear();
    m_uiShardGUID.clear();
}

void instance_dire_maul::CallProtectors()
{
	if (Creature* pTendris = GetSingleCreatureFromStorage(NPC_TENDRIS_WARPWOOD))
    {
        float fX, fY, fZ;
        pTendris->GetPosition(fX, fY, fZ);
        for(GUIDList::iterator itr = m_uiProtectorGUID.begin(); itr != m_uiProtectorGUID.end(); ++itr)
        {
            Creature* pProtector = instance->GetCreature(*itr);
            if (pProtector && pProtector->isAlive())
            {
                uint8 i = urand(0,1);
                pTendris->GetMap()->CreatureRelocation(pProtector, Tendris[i].x, Tendris[i].y, Tendris[i].z, Tendris[i].o);
                pProtector->GetMotionMaster()->MoveChase(pTendris->getVictim());
                pProtector->AI()->AttackStart(pTendris->getVictim());
            }
        }
    }
}

void instance_dire_maul::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        // East
        case NPC_OLD_IRONBARK:
            if (m_auiEncounter[1] == DONE)
                pCreature->UpdateEntry(NPC_IRONBARK_THE_REDEEMED);
            break;
        case NPC_PIMGIB:
        case NPC_ALZZINS_THE_WILDSHAPER:
            break;
        case NPC_ALZZINS_MINION:
			m_uiAlzzinsMinionGUID.push_back(pCreature->GetObjectGuid());
            return;
        // North
        case NPC_GUARD_MOLDAR:
        case NPC_GUARD_FENGUS:
        case NPC_GUARD_SLIPKIK:
        case NPC_STOMPER_KREEG:
        case NPC_CAPTAIN_KROMCRUSH:
        case NPC_GORDOK_BRUTE:
        case NPC_GORDOK_MAULER:
        case NPC_GORDOK_MAGELORD:
        case NPC_GORDOK_CAPTAIN:
        case NPC_GORDOK_SPIRIT:
        case NPC_GORDOK_WARLOCK:
        case NPC_GORDOK_REAVER:
        case NPC_GORDOK_BUSHWACKER:
        case NPC_GORDOK_MASTIFF:
            m_uiGordokGUID.push_back(pCreature->GetObjectGuid());
            if (m_auiEncounter[12] == DONE)
                pCreature->setFaction(FACTION_FRIENDLY);
            return;
        case NPC_CHORUSH_THE_OBSERVER:
            if (m_auiEncounter[12] == DONE)
                pCreature->setFaction(FACTION_FRIENDLY);
            break;
        case NPC_KNOT_THIMBLEJACK:
            if (m_auiEncounter[13] == DONE)
            {
                pCreature->SetVisibility(VISIBILITY_OFF);
                pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }
            break;
        // West
        case NPC_TENDRIS_WARPWOOD:
            if (m_auiEncounter[14] == DONE)
                pCreature->SummonCreature(NPC_ANCIENT_EQUINE_SPIRIT, 9.18f, 491.05f, -23.29f, 4.89f, TEMPSUMMON_DEAD_DESPAWN, 0);
            break;
        case NPC_PROTECTOR:
            m_uiProtectorGUID.push_back(pCreature->GetObjectGuid());
            return;
        case NPC_IMMOLTHAR:
            if (m_auiEncounter[15] != DONE)
                pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE + UNIT_FLAG_NOT_SELECTABLE);
            break;
        case NPC_PRINCE_TORTHELDRIN:
            if (m_auiEncounter[16] != DONE)
                pCreature->setFaction(FACTION_FRIENDLY);
            break;
		default:
			return;
    }

	m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
}

void instance_dire_maul::OnObjectCreate(GameObject* pGo)
{
    switch(pGo->GetEntry())
    {
        // East
        case GO_CONSERVATORY_DOOR:
            if (m_auiEncounter[2] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_CRUMBLE_WALL:
			pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND + GO_FLAG_NO_INTERACT);
            if (m_auiEncounter[3] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_CORRUPTED_CRYSTAL_VINE:
            break;
        case GO_FELVINE_SHARD:
            m_uiShardGUID.push_back(pGo->GetObjectGuid());
            return;
        // North
        case GO_GORDOK_TRIBUTE1:
		case GO_GORDOK_TRIBUTE2:
		case GO_GORDOK_TRIBUTE3:
		case GO_GORDOK_TRIBUTE4:
		case GO_GORDOK_TRIBUTE5:
            m_uiGordokTributeGUID.push_back(pGo->GetObjectGuid());
            return;
        case GO_KNOT_THIMBLEJACK_CACHE:
            break;
        // West
        case GO_PYLON_1:
        case GO_PYLON_2:
        case GO_PYLON_3:
        case GO_PYLON_4:
        case GO_PYLON_5:
            if (m_auiEncounter[15] == DONE)
                InteractWithGo(pGo->GetEntry(), false);
            return;
        case GO_FORCE_FIELD:
            if (m_auiEncounter[15] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
		case GO_BROKEN_TRAP:
        case GO_THE_PRINCES_CHEST:
            break;
		default:
			return;
    }

	m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_dire_maul::SetData(uint32 uiType, uint32 uiData)
{
    switch(uiType)
    {
        // East
        case TYPE_PUSILLIN:
            m_auiEncounter[0] = uiData;
            break;
        case TYPE_ZEVRIM_THORNHOOF:
            m_auiEncounter[1] = uiData;
            if (uiData == DONE)
				if (Creature* pIronbark = GetSingleCreatureFromStorage(NPC_OLD_IRONBARK))
                {
                    pIronbark->UpdateEntry(NPC_IRONBARK_THE_REDEEMED);
                    pIronbark->AIM_Initialize();
                    DoScriptText(SAY_REDEEMED, pIronbark);
                }
            break;
        case TYPE_IRONBARK_THE_REDEEMED:
            m_auiEncounter[2] = uiData;
            if (uiData == DONE)
                HandleGameObject(GO_CONSERVATORY_DOOR, true);
            break;
        case TYPE_ALZZIN_THE_WILDSHAPER:
            m_auiEncounter[3] = uiData;
            switch(uiData)
            {
                case NOT_STARTED:
                    HandleGameObject(GO_CRUMBLE_WALL, false);

                    for(GUIDList::iterator itr = m_uiAlzzinsMinionGUID.begin(); itr != m_uiAlzzinsMinionGUID.end(); ++itr)
                    {
                        if (Creature* pMinion = instance->GetCreature(*itr))
                            pMinion->Respawn();
                    }
                    break;
                case DONE:
                    HandleGameObject(GO_CORRUPTED_CRYSTAL_VINE, true);
                    for(GUIDList::iterator itr = m_uiShardGUID.begin(); itr != m_uiShardGUID.end(); ++itr)
                        DoRespawnGameObject(*itr, HOUR);
                    break;
                case SPECIAL:
                    HandleGameObject(GO_CRUMBLE_WALL, true);

                    if (Creature* pAlzzin = GetSingleCreatureFromStorage(NPC_ALZZINS_THE_WILDSHAPER))
                    {
                        for(GUIDList::iterator itr = m_uiAlzzinsMinionGUID.begin(); itr != m_uiAlzzinsMinionGUID.end(); ++itr)
                        {
                            if (Creature* pMinion = instance->GetCreature(*itr))
                                pMinion->AI()->AttackStart(pAlzzin->getVictim());
                        }
                    }
                    break;
            }
            break;
        case TYPE_ISALIEN:
            m_auiEncounter[4] = uiData;
            break;
        // North
        case TYPE_STOMPER_KREEG:
            m_auiEncounter[5] = uiData;
            break;
        case TYPE_GUARD_MOLDAR:
            m_auiEncounter[6] = uiData;
            break;
        case TYPE_GUARD_FENGUS:
            m_auiEncounter[7] = uiData;
            break;
        case TYPE_GUARD_SLIPKIK:
            m_auiEncounter[8] = uiData;
            break;
        case TYPE_CAPTAIN_KROMCRUSH:
            m_auiEncounter[9] = uiData;
            break;
        case TYPE_CHORUSH_THE_OBSERVER:
            m_auiEncounter[10] = uiData;
            break;
        case TYPE_KING_GORDOK:
            m_auiEncounter[11] = uiData;
            if (uiData == DONE)
            {
                Creature* pChorush = GetSingleCreatureFromStorage(NPC_CHORUSH_THE_OBSERVER);
                if (pChorush && pChorush->isAlive() && m_auiEncounter[10] != DONE)
                {
                    pChorush->setFaction(FACTION_FRIENDLY);
                    pChorush->InterruptNonMeleeSpells(false);
                    pChorush->AI()->ResetToHome();

                    if (Creature* pMizzle = pChorush->SummonCreature(NPC_MIZZLE_THE_CRAFTY, 839.83f, 549.14f, 34.26f, 4.32f, TEMPSUMMON_DEAD_DESPAWN, 10000))
                    {
                        pMizzle->setFaction(FACTION_FRIENDLY);
                        pMizzle->GetMotionMaster()->MovePoint(0, 818.92f, 481.95f, 37.32f);
                        CreatureCreatePos pos(pMizzle->GetMap(), 818.92f, 481.95f, 37.32f, 4.32f);
                        pMizzle->SetSummonPoint(pos);
                    }
                }
            }
            break;
        case TYPE_TRIBUTE_RUN:
            m_auiEncounter[12] = uiData;
            if (uiData == DONE)
            {
                // Set Gordok friendly (spell 22799 seems to be buggy)
                for(GUIDList::iterator itr = m_uiGordokGUID.begin(); itr != m_uiGordokGUID.end(); ++itr)
                {
                    if (Creature* pGordok = instance->GetCreature(*itr))
                    {
                        pGordok->setFaction(FACTION_FRIENDLY);

                        // Return Captain Krom'crush back
                        if (pGordok->GetEntry() == NPC_CAPTAIN_KROMCRUSH && pGordok->isAlive())
                        {
                            pGordok->SetVisibility(VISIBILITY_ON);
                            pGordok->GetMotionMaster()->MoveTargetedHome();
                        }
                    }
                }

                uint32 uiCount = 0;
                ObjectGuid uiTributeGUID = ObjectGuid();

                for(uint8 i = 5; i <= 10; ++i)
                    if (m_auiEncounter[i] != DONE)
                        ++uiCount;

                for(GUIDList::iterator itr = m_uiGordokTributeGUID.begin(); itr != m_uiGordokTributeGUID.end(); ++itr)
                {
                    if (GameObject* pTribute = instance->GetGameObject(*itr))
                        if (pTribute->GetEntry() == aTribute[uiCount])
                        {
							uiTributeGUID = pTribute->GetObjectGuid();
                            break;
                        }
                }

                if (uiTributeGUID)
                    DoRespawnGameObject(uiTributeGUID, HOUR);
            }
            break;
        case TYPE_KNOT_THIMBLEJACK:
            m_auiEncounter[13] = uiData;
            if (uiData == DONE)
                DoRespawnGameObject(GO_KNOT_THIMBLEJACK_CACHE, HOUR);
            break;
        // West
        case TYPE_TENDRIS_WARPWOOD:
            m_auiEncounter[14] = uiData;
            if (uiData == IN_PROGRESS)
                CallProtectors();
            if (uiData == DONE)
            {
                if (Creature* pTendris = GetSingleCreatureFromStorage(NPC_TENDRIS_WARPWOOD))
                    pTendris->SummonCreature(NPC_ANCIENT_EQUINE_SPIRIT, 9.18f, 491.05f, -23.29f, 4.89f, TEMPSUMMON_DEAD_DESPAWN, 0);
            }
            break;
        case TYPE_BARRIER:
            m_auiEncounter[15] = uiData;
            if (uiData == IN_PROGRESS)
            {
                --m_uiPylonCount;

                if (!m_uiPylonCount)
                    SetData(TYPE_BARRIER, DONE);
                else
                    debug_log("SD0: Instance Dire Maul: %u pylons left to activate.", m_uiPylonCount);
            }
            if (uiData == DONE)
            {
                HandleGameObject(GO_FORCE_FIELD, true);
                if (Creature* pImmolthar = GetSingleCreatureFromStorage(NPC_IMMOLTHAR))
                    pImmolthar->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE + UNIT_FLAG_NOT_SELECTABLE);
            }
            break;
        case TYPE_IMMOLTHAR:
            m_auiEncounter[16] = uiData;
            if (uiData == DONE)
            {
                if (Creature* pPrince = GetSingleCreatureFromStorage(NPC_PRINCE_TORTHELDRIN))
                    pPrince->setFaction(pPrince->GetCreatureInfo()->faction_A);
            }
            break;
        case TYPE_PRINCE_TORTHELDRIN:
            m_auiEncounter[17] = uiData;
            if (uiData == DONE)
                DoRespawnGameObject(GO_THE_PRINCES_CHEST, HOUR);
            break;
    }
        
    OUT_SAVE_INST_DATA;

    std::ostringstream saveStream;
    saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
	<< m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " "
	<< m_auiEncounter[6] << " " << m_auiEncounter[7] << " " << m_auiEncounter[8] << " "
	<< m_auiEncounter[9] << " " << m_auiEncounter[10] << " " << m_auiEncounter[11] << " "
	<< m_auiEncounter[12] << " " << m_auiEncounter[13] << " " << m_auiEncounter[14] << " "
	<< m_auiEncounter[15] << " " << m_auiEncounter[16] << " " << m_auiEncounter[17];

    strInstData = saveStream.str();

    SaveToDB();
    OUT_SAVE_INST_DATA_COMPLETE;
}

uint32 instance_dire_maul::GetData(uint32 uiType)
{
	if (uiType < MAX_ENCOUNTER)
		return m_auiEncounter[uiType];

	return 0;
}

void instance_dire_maul::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3]
        >> m_auiEncounter[4] >> m_auiEncounter[5] >> m_auiEncounter[6] >> m_auiEncounter[7]
        >> m_auiEncounter[8] >> m_auiEncounter[9] >> m_auiEncounter[10] >> m_auiEncounter[11]
        >> m_auiEncounter[12] >> m_auiEncounter[13] >> m_auiEncounter[14] >> m_auiEncounter[15]
        >> m_auiEncounter[16] >> m_auiEncounter[17];

    for(uint32 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    // Reset Pussilin's event if it hasn't been already done
    if (m_auiEncounter[0] < 5)
        m_auiEncounter[0] = NOT_STARTED;

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_dire_maul(Map* pMap)
{
    return new instance_dire_maul(pMap);
}

void AddSC_instance_dire_maul()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "instance_dire_maul";
    pNewscript->GetInstanceData = &GetInstanceData_instance_dire_maul;
    pNewscript->RegisterSelf();
}
