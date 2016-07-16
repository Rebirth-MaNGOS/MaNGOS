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
SDName: Instance_Stratholme
SD%Complete: 90
SDComment:
SDCategory: Stratholme
EndScriptData */

#include "precompiled.h"
#include "stratholme.h"

const uint32 NextPullTimer[13] = {20000, 40000, 80000, 35000, 80000, 90000, 60000, 50000, 70000, 90000, 90000, 80000, 80000};

static Locations Sentry[]=
{
    {4030.39f,-3407.06f,115.57f, 0, 0},
    {4036.55f,-3407.43f,115.51f, 0, 0},
    {4040.75f,-3404.32f,115.27f, 0, 0},
    {4025.98f,-3403.48f,115.49f, 0, 0},
    {4032.97f,-3396.12f,117.94f, 0, 0}
};

static Locations Square[]=
{
    {4032.85f,-3385.41f,119.74f, 0, 0},       // Ziggurat SummonPoint
    {4033.41f,-3406.87f,115.50f, 0, 0},       // Rammstein MovePoint
    {4036.52f,-3470.70f,121.75f, 0, 0}        // Abomination MovePoint
};

instance_stratholme::instance_stratholme(Map* pMap) : ScriptedInstance(pMap),
    m_bFiveUp(false),
    m_bTenUp(false),
    m_bAbomPull(false),
    m_bBaronRun(false),
    m_bBaronWarn(false),
    m_bZigguratDoor(false),
    m_bSummonAurius(false),

    m_uiAbCount(0),
    m_uiNextPull(0),
    m_uiAbomPullTimer(0),
    m_uiBaronRunTimer(0),
    m_uiBaronWarnTimer(0),
    m_uiZigguratDoorTimer(0)
{
    Initialize();
}

void instance_stratholme::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

    m_AbominationPull.clear();

    m_lAbominationGUID.clear();
    m_lAcolyteGUID.clear();
    m_lCrystalGUID.clear();
    m_lSentryGUID.clear();
}

void instance_stratholme::OnCreatureCreate(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
		//Rare Bosses
		case 10809:
		//case 10393:			// set his spawn in the pool instead
			SetRareBoss(pCreature,5);
			break;
        // Bosses
        case NPC_BARON_RIVENDARE:
        case NPC_MAGISTRATE_BARTHILAS:
        case NPC_JARIEN:
        case NPC_SOTHOS:
		case NPC_THE_UNFORGIVEN:
		// Npcs
        case NPC_AURIUS:
        case NPC_YSIDA_TRIGGER:
            m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        // Trash
        case NPC_BILE_SPEWER:
        case NPC_VENOM_BELCHER:
            m_lAbominationGUID.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_BLACK_GUARD_SENTRY:
            m_lSentryGUID.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_THUZADIN_ACOLYTE:
            m_lAcolyteGUID.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_ASHARI_CRYSTAL:			
            m_lCrystalGUID.push_back(pCreature->GetObjectGuid());
			pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);		// not working
            break;
        case NPC_CRIMSON_INITIATE:
        case NPC_CRIMSON_GALLANT:
        case NPC_CRIMSON_GUARDSMAN:
        case NPC_CRIMSON_CONJUROR:
        case NPC_CRIMSON_BATTLE_MAGE:
            // Store GUID of NPCs in the courtyard to spawn Timmy once they are all dead
            if (pCreature->IsWithinDist2d(aDefensePoints[TIMMY].m_fX, aDefensePoints[TIMMY].m_fY, 40.0f))
                m_suiCrimsonDefendersLowGuids[TIMMY].push_back(pCreature->GetObjectGuid());
            // Iterate also over all the defense points where those NPCs are possibly spawned
            for (uint8 i = BARRICADE; i <= FIRST_BARRICADES; i++)
            {
                // Do not store - again - GUIDs for Timmy spawn point, they were done previously with a different range
                if (i == TIMMY)
                    continue;
                // Store the GUID of the nearby NPCs for each defense point
                if (pCreature->IsWithinDist2d(aDefensePoints[i].m_fX, aDefensePoints[i].m_fY, 8.0f))
                {
                    m_suiCrimsonDefendersLowGuids[i].push_back(pCreature->GetObjectGuid());
                    break;
                }
            }
break;
    }
}

void instance_stratholme::OnCreatureDeath(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_BLACK_GUARD_SENTRY:
        {
            m_lSentryGUID.remove(pCreature->GetObjectGuid());

            if (m_lSentryGUID.empty())
            {
                HandleGameObject(GO_ZIGGURAT_DOOR_4, true);
                HandleGameObject(GO_ZIGGURAT_DOOR_5, true);
            }
            else
                debug_log("SD0: Instance Stratholme: %lu Black Guard Sentries left to kill.", m_lSentryGUID.size());
            break;
        }
        case NPC_AURIUS:        
        {
            SetData(TYPE_AURIUS, FAIL);   
            break;
        }
        case NPC_THUZADIN_ACOLYTE:
        {
            for (uint8 i = 0; i < MAX_ZIGGURAT; ++i)
            {
                if (m_alZigguratAcolyteGUID[i].empty())
                    continue;                               // nothing to do anymore for this ziggurat

                m_alZigguratAcolyteGUID[i].remove(pCreature->GetObjectGuid());
                if (m_alZigguratAcolyteGUID[i].empty())
                {
                    // A random zone yell after one is cleared
                    int32 aAnnounceSay[MAX_ZIGGURAT] = {SAY_ANNOUNCE_ZIGGURAT_1, SAY_ANNOUNCE_ZIGGURAT_2, SAY_ANNOUNCE_ZIGGURAT_3};
					DoOrSimulateScriptTextForThisInstance(aAnnounceSay[i], NPC_THUZADIN_ACOLYTE);

                    // Kill Crystal
                    if (Creature* pCrystal = instance->GetCreature(m_auiCrystalSortedGUID[i]))
                        pCrystal->DealDamage(pCrystal, pCrystal->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

                    switch(i)
                    {
                        case 0:
                            SetData(TYPE_BARONESS_ANASTARI, SPECIAL);
                            break;
                        case 1:
                            SetData(TYPE_NERUBENKAN, SPECIAL);
                            break;
                        case 2:
                            SetData(TYPE_MALEKI_THE_PALLID, SPECIAL);
                            break;
                    }
                }                
            }
            break;
            
            // Scarlet Bastion defense and Timmy spawn support
            case NPC_CRIMSON_INITIATE:
            case NPC_CRIMSON_GALLANT:
            case NPC_CRIMSON_GUARDSMAN:
            case NPC_CRIMSON_CONJUROR:
            case NPC_CRIMSON_BATTLE_MAGE:
            for (uint8 i = BARRICADE; i <= FIRST_BARRICADES; i++)
            {
                if (m_suiCrimsonDefendersLowGuids[i].empty())
                    continue;

                m_suiCrimsonDefendersLowGuids[i].remove(pCreature->GetObjectGuid());
                // If all mobs from a defense group are dead then activate the related defense event
                if (m_suiCrimsonDefendersLowGuids[i].empty() && i != FIRST_BARRICADES)
                    DoScarletBastionDefense(i, pCreature);
            }
            break;
        case NPC_BALNAZZAR:
            DoScarletBastionDefense(CRIMSON_THRONE, pCreature);
            break;
        }
        default:
            break;
    }
}

void instance_stratholme::OnObjectCreate(GameObject* pGo)
{
    switch(pGo->GetEntry())
    {
        case GO_SERVICE_ENTRANCE:
        case GO_GAUNTLET_INNER_GATE:
		case GO_HEIRLOOMS:
            break;
        case GO_ZIGGURAT_DOOR_1:
            m_auiZigguratDoorGUID[0] = pGo->GetObjectGuid();
            if (m_auiEncounter[2] >= DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            return;
        case GO_ZIGGURAT_DOOR_2:
            m_auiZigguratDoorGUID[1] = pGo->GetObjectGuid();
            if (m_auiEncounter[3] >= DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            return;
        case GO_ZIGGURAT_DOOR_3:
            m_auiZigguratDoorGUID[2] = pGo->GetObjectGuid();
            if (m_auiEncounter[4] >= DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            return;
        case GO_ZIGGURAT_DOOR_4:
        case GO_ZIGGURAT_DOOR_5:
            if (m_auiEncounter[5] == DONE || m_auiEncounter[6] >= FAIL)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_PORT_GAUNTLET:
        case GO_PORT_SLAUGTHER:
            if (m_auiEncounter[2] == SPECIAL && m_auiEncounter[3] == SPECIAL && m_auiEncounter[4] == SPECIAL)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
		case GO_YSIDAS_CAGE:
			if (m_auiEncounter[TYPE_BARON_RUN] >= DONE)
			{
				pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
				pGo->SetGoState(GO_STATE_ACTIVE);
				pGo->SetLootState(GO_ACTIVATED);
				pGo->SetGoAnimProgress(0);
				
				pGo->SummonCreature(NPC_YSIDA, 4044.2017f, -3339.0044f, 115.06f, 4.3366f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 1800000, true);
			}
			break;
		default:
			return;
    }

	m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_stratholme::SetData(uint32 uiType, uint32 uiData)
{
    switch(uiType)
    {
        case TYPE_BARON_RUN:
            switch(uiData)
            {
                case IN_PROGRESS:
                    if (m_auiEncounter[0] == IN_PROGRESS || m_auiEncounter[0] == FAIL)
                        break;

                    PlayersRun(ACTION_RUN_START);
					DoOrSimulateScriptTextForThisInstance(SAY_RUN_START, NPC_BARON_RIVENDARE);
                    m_uiBaronRunTimer = 45*MINUTE*IN_MILLISECONDS;
                    m_bBaronRun = true;
                    m_auiEncounter[0] = uiData;
                    debug_log("SD0: Instance Stratholme: Baron run in progress.");
                    break;
                case FAIL:
					DoOrSimulateScriptTextForThisInstance(SAY_RUN_TIME_UP, NPC_BARON_RIVENDARE);
                    m_auiEncounter[0] = uiData;
                    break;
                case DONE:
					EntryGuidMap::iterator find = m_mGoEntryGuidStore.find(GO_YSIDAS_CAGE);

					if (find != m_mGoEntryGuidStore.end())
					{
						GameObject *cage = instance->GetGameObject(find->second);
						if (cage != NULL)
							cage->Use(instance->GetCreature(m_mNpcEntryGuidStore[NPC_YSIDA_TRIGGER]));
					}

                    PlayersRun(ACTION_RUN_COMPLETE);
                    m_bBaronRun = false;
                    m_auiEncounter[0] = uiData;
                    break;
            }
            break;
        case TYPE_MAGISTRATE:
            if (uiData == IN_PROGRESS && m_auiEncounter[1] != IN_PROGRESS)
            {
               // m_uiBaronWarnTimer = 3*IN_MILLISECONDS;
                m_bBaronWarn = true;
            }
            m_auiEncounter[1] = uiData;
            break;
        case TYPE_BARONESS_ANASTARI:
            m_auiEncounter[2] = uiData;
            if (uiData == DONE)
            {
                DoSortZiggurats();
                HandleGameObject(m_auiZigguratDoorGUID[0], true);
            }
            if (uiData == SPECIAL)
                OpenSlaugtherSquare();
            break;
        case TYPE_NERUBENKAN:
            m_auiEncounter[3] = uiData;
            if (uiData == DONE)
            {
                DoSortZiggurats();
                HandleGameObject(m_auiZigguratDoorGUID[1], true);
            }
            if (uiData == SPECIAL)
                OpenSlaugtherSquare();
            break;
        case TYPE_MALEKI_THE_PALLID:
            m_auiEncounter[4] = uiData;
            if (uiData == DONE)
            {
                DoSortZiggurats();
                HandleGameObject(m_auiZigguratDoorGUID[2], true);
            }
            if (uiData == SPECIAL)
                OpenSlaugtherSquare();
            break;
        case TYPE_SLAUGHTER_SQUARE:
            switch(uiData)
            {
                case NOT_STARTED:
                    m_bAbomPull = false;
                    m_uiNextPull = 0;
                    HandleGameObject(GO_PORT_GAUNTLET, true);

                    for(GUIDList::iterator itr = m_lAbominationGUID.begin(); itr != m_lAbominationGUID.end(); ++itr)
                    {
                        if (Creature* pAbom = instance->GetCreature(*itr))
                            pAbom->Respawn();
                    }

                    m_auiEncounter[5] = uiData;
                    break;
                case IN_PROGRESS:
                    if (m_auiEncounter[5] != IN_PROGRESS)
                    {
                        FillAbomPullList();
                        HandleGameObject(GO_PORT_GAUNTLET, false);
                        m_uiAbomPullTimer = NextPullTimer[m_uiNextPull];
                        m_bAbomPull = true;
                        ++m_uiNextPull;
                    }

                    m_uiAbCount = m_lAbominationGUID.size();
                    for(GUIDList::iterator itr = m_lAbominationGUID.begin(); itr != m_lAbominationGUID.end(); ++itr)
                    {
                        if (Creature* pAbom = instance->GetCreature(*itr))
                            if (!pAbom->isAlive())
                                --m_uiAbCount;
                    }

                    if (!m_uiAbCount)
                    {
						if (Creature* pBaron = GetSingleCreatureFromStorage(NPC_BARON_RIVENDARE))
                        {
                            DoScriptText(SAY_SQUARE_CLEARED_OUT, pBaron);

                            HandleGameObject(GO_ZIGGURAT_DOOR_5, true);
                            m_uiZigguratDoorTimer = 5*IN_MILLISECONDS;
                            m_bZigguratDoor = true;

                            if (Creature* pRammstein = pBaron->SummonCreature(NPC_RAMSTEIN, Square[0].x, Square[0].y, Square[0].z, 4.71f, TEMPSUMMON_DEAD_DESPAWN, 30000))
                            {
                                pRammstein->GetMotionMaster()->MovePoint(0, Square[1].x, Square[1].y, Square[1].z);
                                CreatureCreatePos pos(pRammstein->GetMap(), Square[1].x, Square[1].y, Square[1].z, 4.71f);
                                pRammstein->SetSummonPoint(pos);
                            }
                        }
                        else
                            debug_log("SD0: Instance Stratholme: Ramstein NOT spawned.");
                    }
                    else
                        debug_log("SD0: Instance Stratholme: %u Abomnation left to kill.", m_uiAbCount);

                    m_auiEncounter[5] = uiData;
                    break;
                case DONE:
					DoOrSimulateScriptTextForThisInstance(SAY_RAMMSTEIN_DEATH, NPC_BARON_RIVENDARE);
					if (Creature* pBaron = GetSingleCreatureFromStorage(NPC_BARON_RIVENDARE))
                    {
                        for(uint8 i = 0; i < sizeof(Sentry)/sizeof(Locations); ++i)
                        {
                            if (Creature* pGuard = pBaron->SummonCreature(NPC_BLACK_GUARD_SENTRY, Square[0].x, Square[0].y, Square[0].z, 4.71f, TEMPSUMMON_DEAD_DESPAWN, 30000))
                            {
                                pGuard->GetMotionMaster()->MovePoint(0, Sentry[i].x, Sentry[i].y, Sentry[i].z);
                                CreatureCreatePos pos(pGuard->GetMap(), Sentry[i].x, Sentry[i].y, Sentry[i].z, 4.71f);
                                pGuard->SetSummonPoint(pos);
                            }
                        }

                        HandleGameObject(GO_ZIGGURAT_DOOR_4, true);
                        m_uiZigguratDoorTimer = 5*IN_MILLISECONDS;
                        m_bZigguratDoor = true;
                        debug_log("SD0: Instance Stratholme: Black Guard Sentries spawned.");
                    }
                    m_auiEncounter[5] = uiData;
                    break;
            }
            break;
        case TYPE_BARON_RIVENDARE:
            switch(uiData)
            {
                case IN_PROGRESS:
                    PlayersRun(ACTION_RUN_ENCOUNTER);
                    m_uiZigguratDoorTimer = 100;
                    m_bZigguratDoor = true;
                    m_auiEncounter[6] = uiData;
                    break;
                case FAIL:
                case DONE:
                    if (uiData == DONE)
                    {
                        if (GetData(TYPE_BARON_RUN) == IN_PROGRESS)
                                SetData(TYPE_BARON_RUN, DONE);   
                        if (GetData(TYPE_AURIUS) == SPECIAL)                        
                            SetData(TYPE_AURIUS, DONE);  
                    }

                    HandleGameObject(GO_ZIGGURAT_DOOR_4, true);
                    HandleGameObject(GO_ZIGGURAT_DOOR_5, true);
                    HandleGameObject(GO_PORT_GAUNTLET, true);
                    m_auiEncounter[6] = uiData;
                    break;
            }
            break;
        case TYPE_JARIEN:
            m_auiEncounter[7] = uiData;
            if (uiData == DONE && GetData(TYPE_SOTHOS) == DONE)
                DoRespawnGameObject(GO_HEIRLOOMS, HOUR);
            break;
        case TYPE_SOTHOS:
            m_auiEncounter[8] = uiData;
            if (uiData == DONE && GetData(TYPE_JARIEN) == DONE)
                DoRespawnGameObject(GO_HEIRLOOMS, HOUR);
            break;
        case TYPE_POSTBOX:
            m_auiEncounter[9] = uiData;
            break;
        case TYPE_TIMMY_THE_CRUEL:
            m_auiEncounter[10] = uiData;
            break;
        case TYPE_THE_UNFORGIVEN:
            m_auiEncounter[11] = uiData;
            break;
        case TYPE_AURIUS:
            m_auiEncounter[12] = uiData;
            // Baron killed and Aurius is alive: give him his NPC Flags back
            // So players can complete the quest
            // Fake his death
            if (uiData == DONE)
            {
                if (Creature* pAurius = GetSingleCreatureFromStorage(NPC_AURIUS))
                {
                    if(pAurius && pAurius->isAlive())
                    {
                        CreatureCreatePos pos(pAurius->GetMap(), pAurius->GetPositionX(), pAurius->GetPositionY(), pAurius->GetPositionZ(), pAurius->GetOrientation());
                        pAurius->SetSummonPoint(pos);
                        pAurius->MonsterSay("Argh!", LANG_UNIVERSAL, nullptr);                        
                        pAurius->StopMoving();
                        pAurius->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                        pAurius->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        pAurius->InterruptNonMeleeSpells(true);
                        pAurius->SetHealth(1);
                        pAurius->GetMotionMaster()->MovementExpired();
                        pAurius->GetMotionMaster()->MoveIdle();
                        pAurius->RemoveAllAurasOnDeath();
                        pAurius->SetStandState(UNIT_STAND_STATE_DEAD);
                    }
                }
                break;
            }
            if (uiData == FAIL)
            {
                // Baron encounter failed and Aurius is spawned: kill him
                if (Creature* pAurius = GetSingleCreatureFromStorage(NPC_AURIUS))
                {
                    if (pAurius->isAlive())
                    {
                        pAurius->MonsterSay("Argh!", LANG_UNIVERSAL, nullptr);
                        pAurius->DealDamage(pAurius, pAurius->GetHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, nullptr, false);
                    }
                }
                break;
           }
            break;
    }
   
    OUT_SAVE_INST_DATA;

    std::ostringstream saveStream;
    saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
	<< m_auiEncounter[3]<< " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " "
	<< m_auiEncounter[6] << " " << m_auiEncounter[7] << " " << m_auiEncounter[8] << " "
	<< m_auiEncounter[9] << " " << m_auiEncounter[10] << " " << m_auiEncounter[11] << " "
	<< m_auiEncounter[12];

    strInstData = saveStream.str();

    SaveToDB();
    OUT_SAVE_INST_DATA_COMPLETE;

}

void instance_stratholme::Load(const char* chrIn)
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
        >> m_auiEncounter[12];

    for(uint32 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
        {
            if (i == 0)
                m_auiEncounter[i] = FAIL;         // Prevent exploiting Baron Run, TODO: Implement loading Baron run with timer?
            else
                m_auiEncounter[i] = NOT_STARTED;
        }
    }

    // Special Treatment for the Ziggurat-Bosses, as otherwise the event couldn't reload
    if (m_auiEncounter[2] == DONE)
        m_auiEncounter[2] = SPECIAL;
    if (m_auiEncounter[3] == DONE)
        m_auiEncounter[3] = SPECIAL;
    if (m_auiEncounter[4] == DONE)
        m_auiEncounter[4] = SPECIAL;

    OUT_LOAD_INST_DATA_COMPLETE;
}

uint32 instance_stratholme::GetData(uint32 uiType)
{
	if (uiType < MAX_ENCOUNTER)
		return m_auiEncounter[uiType];

	return 0;
}

void instance_stratholme::DoSpawnScarletGuards(uint8 uiStep, Player* pSummoner)
{
    if (!pSummoner)
        return;

    uint32 uiNPCEntry[2];
    uint8  uiIndex;

    switch (uiStep)
    {
        case HALL_OF_LIGHTS:
            uiNPCEntry[0] = NPC_CRIMSON_GALLANT;
            uiNPCEntry[1] = NPC_CRIMSON_GALLANT;
            uiIndex = 0;
            break;
        case INNER_BASTION_2:
            uiNPCEntry[0] = NPC_CRIMSON_MONK;
            uiNPCEntry[1] = NPC_CRIMSON_SORCERER;
            uiIndex = 3;
            break;
        default:
            return;    // avoid indexing the following tables with a wrong index. Should never happen.
    }

    // Spawn the two guards and move them to where they will guard (each side of a door)
    for (uint8 i = 0; i < 2; i++)
    {
        if (Creature* pTemp = pSummoner->SummonCreature(uiNPCEntry[i], aScarletGuards[uiIndex].m_fX, aScarletGuards[uiIndex].m_fY, aScarletGuards[uiIndex].m_fZ, aScarletGuards[uiIndex].m_fO, TEMPSUMMON_DEAD_DESPAWN, 0))
        {
            pTemp->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
            pTemp->GetMotionMaster()->MovePoint(0, aScarletGuards[uiIndex + i + 1].m_fX, aScarletGuards[uiIndex + i + 1].m_fY, aScarletGuards[uiIndex + i + 1].m_fZ);
            
            CreatureCreatePos pos(pTemp->GetMap(), aScarletGuards[uiIndex + i + 1].m_fX, aScarletGuards[uiIndex + i + 1].m_fY, aScarletGuards[uiIndex + i + 1].m_fZ, 5.35f);
            pTemp->SetSummonPoint(pos);
        }
    }

    return;
}

void instance_stratholme::DoSpawnScourgeInvaders(uint8 uiStep, Player* pSummoner)
{
    if (!pSummoner)
        return;

    // Define the group of 5 Scourge invaders
    std::vector<uint32> uiMobList;                  // Vector holding the 5 creatures entries for each Scourge invaders group
    uiMobList.push_back(NPC_SKELETAL_GUARDIAN);     // 4 static NPC entries
    uiMobList.push_back(NPC_SKELETAL_GUARDIAN);
    uiMobList.push_back(NPC_SKELETAL_BERSERKER);
    uiMobList.push_back(NPC_SKELETAL_BERSERKER);

    uint32 uiMobEntry;                              // will hold the last random creature entry
    uint8  uiIndex;

    // Pick the fifth NPC in the group and randomize the five possible spawns
    switch (urand(0, 1))
    {
        case 0: uiMobEntry = NPC_SKELETAL_GUARDIAN;     break;
        case 1: uiMobEntry = NPC_SKELETAL_BERSERKER;    break;
    }

    uiMobList.push_back(uiMobEntry);
    std::random_shuffle(uiMobList.begin(), uiMobList.end());

    // Define the correct index for the spawn/move coords table
    switch (uiStep)
    {
        case ENTRANCE:          uiIndex = 1; break;
        case INNER_BASTION_1:   uiIndex = 3; break;
        case CRIMSON_THRONE:    uiIndex = 5; break;
        default: return;    // avoid indexing the following table with a wrong index. Should never happen.
    }

    // Summon the five invaders and make them run into the room
    for (uint8 i = 0; i < 5; i++)
    {
        float fTargetPosX, fTargetPosY, fTargetPosZ;

        if (Creature* pTemp = pSummoner->SummonCreature(uiMobList[i], aScourgeInvaders[uiIndex].m_fX, aScourgeInvaders[uiIndex].m_fY, aScourgeInvaders[uiIndex].m_fZ, aScourgeInvaders[uiIndex].m_fO, TEMPSUMMON_DEAD_DESPAWN, 0))
        {
            pTemp->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
            pTemp->GetRandomPoint(aScourgeInvaders[uiIndex + 1].m_fX, aScourgeInvaders[uiIndex + 1].m_fY, aScourgeInvaders[uiIndex + 1].m_fZ, 3.0f, fTargetPosX, fTargetPosY, fTargetPosZ);
            pTemp->GetMotionMaster()->MovePoint(0, fTargetPosX, fTargetPosY, fTargetPosZ);
            
            CreatureCreatePos pos(pTemp->GetMap(), aScourgeInvaders[uiIndex + 1].m_fX, aScourgeInvaders[uiIndex + 1].m_fY, aScourgeInvaders[uiIndex + 1].m_fZ, aScourgeInvaders[uiIndex].m_fO);
            pTemp->SetSummonPoint(pos);                      
        }
    }

    return;
}

void instance_stratholme::DoMoveBackDefenders(uint8 uiStep, Creature* pCreature)
{
    uint8 uiIndex;
    uint8 uiTreshold = 0;
    uint8 uiFoundGuards = 0;

    switch (uiStep)
    {
        case BARRICADE:
            uiIndex = FIRST_BARRICADES;
            break;
        case STAIRS:
            uiIndex = BARRICADE;
            uiTreshold = 3;
            break;
        default:
            return;     // avoid indexing the following table with a wrong index. Should never happen.
    }

    // Check that there are still defenders to move to the stairs/last barricade
    if (m_suiCrimsonDefendersLowGuids[uiIndex].empty())
        return;
    if (pCreature)
        DoScriptText(ScarletEventYells[uiStep], pCreature);

    for (GUIDList::const_iterator itr = m_suiCrimsonDefendersLowGuids[uiIndex].begin(); itr != m_suiCrimsonDefendersLowGuids[uiIndex].end(); ++itr)
    {
        Creature* pGuard = instance->GetCreature(*itr);
        if (pGuard && pGuard->isAlive() && !pGuard->isInCombat())
        {
            pGuard->GetMotionMaster()->MoveIdle();
            pGuard->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
            pGuard->GetMotionMaster()->MovePoint(0, aScarletLastStand[uiTreshold + uiFoundGuards].m_fX, aScarletLastStand[uiTreshold + uiFoundGuards].m_fY, aScarletLastStand[uiTreshold + uiFoundGuards].m_fZ);
            uiFoundGuards++;
        }

        if (uiFoundGuards == 3)
            return;
    }

    return;
}

void instance_stratholme::DoScarletBastionDefense(uint8 uiStep, Creature* pCreature)
{
    if (!pCreature)
        return;

    switch (uiStep)
    {
        case BARRICADE:
        case STAIRS:
            DoMoveBackDefenders(uiStep, pCreature);
            return;
        case TIMMY:
            if(Creature* pTimmy = pCreature->SummonCreature(NPC_TIMMY_THE_CRUEL, aScourgeInvaders[0].m_fX, aScourgeInvaders[0].m_fY, 
                aScourgeInvaders[0].m_fZ, aScourgeInvaders[0].m_fO, TEMPSUMMON_DEAD_DESPAWN, 0))
            {
                pTimmy->GetMotionMaster()->MovePoint(1, 3669.12f, -3186.15f, 126.19f);
                DoScriptText(SAY_SPAWN_TIMMY, pTimmy);
            }
            SetData(TYPE_TIMMY_THE_CRUEL, IN_PROGRESS);
            return;
        // Scarlet guards spawned
        case HALL_OF_LIGHTS:
        case INNER_BASTION_2:
            DoScriptText(ScarletEventYells[uiStep], pCreature);
            if (Player* pPlayer = GetPlayerInMap())
                DoSpawnScarletGuards(uiStep, pPlayer);
            return;
        // Scourge invading
        case ENTRANCE:
        case INNER_BASTION_1:
            DoScriptText(ScarletEventYells[uiStep], pCreature);
        case CRIMSON_THRONE:
            if (Player* pPlayer = GetPlayerInMap())
                DoSpawnScourgeInvaders(uiStep, pPlayer);
            return;
    }
    return;
}

void instance_stratholme::Update(uint32 uiDiff)
{
    if (m_bAbomPull)
    {
        if (m_uiAbomPullTimer <= uiDiff)
        {
            if (!m_AbominationPull.empty())
            {
                if (Creature* pAbom = instance->GetCreature(m_AbominationPull.back().guid))
                {
                    if (pAbom->isAlive())
                    {
                        pAbom->GetMotionMaster()->MovePoint(0, Square[2].x, Square[2].y, Square[2].z);
                        m_uiAbomPullTimer = NextPullTimer[m_uiNextPull];
                    }
                    else
                        m_uiAbomPullTimer = IN_MILLISECONDS;
                }
                ++m_uiNextPull;
                m_AbominationPull.pop_back();
            }
            else
                m_bAbomPull = false;
        }
        else
            m_uiAbomPullTimer -= uiDiff;
    }

    if (m_bBaronWarn)
    {
        if (m_uiBaronWarnTimer <= uiDiff)
        {
			GameObject* pGauntlet = GetSingleGameObjectFromStorage(GO_SERVICE_ENTRANCE);
			Creature* pMagistrate = GetSingleCreatureFromStorage(NPC_MAGISTRATE_BARTHILAS);
            if (pGauntlet && pMagistrate && pMagistrate->isAlive() && !pMagistrate->isInCombat())
            {
				DoScriptText(SAY_WARN_BARON, pMagistrate);
				/*float fX, fY, fZ;
                pGauntlet->GetPosition(fX, fY, fZ);
                pMagistrate->GetMotionMaster()->MovePoint(0, fX, fY, fZ);*/
				pMagistrate->RemoveSplineFlag(SPLINEFLAG_WALKMODE);		//to make boss run.
				pMagistrate->GetMotionMaster()->MoveWaypoint();
			

            }
            m_bBaronWarn = false;
        }
        else
            m_uiBaronWarnTimer -= uiDiff;
    }
    
    if (m_bBaronRun)
    {
        if (!m_bTenUp && m_uiBaronRunTimer <= 10*MINUTE*IN_MILLISECONDS)
        {
			DoOrSimulateScriptTextForThisInstance(SAY_RUN_10_UP, NPC_BARON_RIVENDARE);
            m_bTenUp = true;
        }

        if (!m_bFiveUp && m_uiBaronRunTimer <= 5*MINUTE*IN_MILLISECONDS)
        {
            DoOrSimulateScriptTextForThisInstance(SAY_RUN_5_UP, NPC_BARON_RIVENDARE);
            m_bFiveUp = true;
        }

        if (m_uiBaronRunTimer <= uiDiff)
        {
			m_uiBaronRunTimer = 0;

			//Buzzer doesn't count if the play is still in progress, the players could still score- also
			//don't fail them the moment the baron dies, that isn't cool
			if (GetData(TYPE_BARON_RIVENDARE) != IN_PROGRESS && GetData(TYPE_BARON_RIVENDARE) != DONE)
			{
				if (GetData(TYPE_BARON_RUN) != DONE)
					SetData(TYPE_BARON_RUN, FAIL);

				m_bBaronRun = false;
				debug_log("SD0: Instance Stratholme: Baron run event reached end. Event has state %u.", GetData(TYPE_BARON_RUN));
			}
        }
        else
            m_uiBaronRunTimer -= uiDiff;
    }

    if (m_bZigguratDoor)
    {
        if (m_uiZigguratDoorTimer <= uiDiff)
        {
            HandleGameObject(GO_ZIGGURAT_DOOR_4, false);
            HandleGameObject(GO_ZIGGURAT_DOOR_5, false);
            m_bZigguratDoor = false;
        }
        else
		{
			m_uiZigguratDoorTimer -= uiDiff;
		}
	}
}

void instance_stratholme::DoSortZiggurats()
{
    if (m_lAcolyteGUID.empty())
        return;

    // Sort Acolytes
    for(GUIDList::iterator itr = m_lAcolyteGUID.begin(); itr != m_lAcolyteGUID.end(); )
    {
        bool bAlreadyIterated = false;

        if (Creature* pAcolyte = instance->GetCreature(*itr))
        {
            for(uint8 i = 0; i < MAX_ZIGGURAT; ++i)
            {
                if (GameObject* pZigguratDoor = instance->GetGameObject(m_auiZigguratDoorGUID[i]))
                {
                    if (pAcolyte->isAlive() && pAcolyte->IsWithinDistInMap(pZigguratDoor, 50.0f, false))
                    {
                        m_alZigguratAcolyteGUID[i].push_back(*itr);
                        itr = m_lAcolyteGUID.erase(itr);
                        bAlreadyIterated = true;
                        break;
                    }
                }
            }
        }

        if (itr != m_lAcolyteGUID.end() && !bAlreadyIterated)
            ++itr;
    }

    // Sort Crystal
    for (GUIDList::iterator itr = m_lCrystalGUID.begin(); itr != m_lCrystalGUID.end(); )
    {
        bool bAlreadyIterated = false;

        if (Creature* pCrystal = instance->GetCreature(*itr))
        {
            for (uint8 i = 0; i < MAX_ZIGGURAT; ++i)
            {
                if (GameObject* pZigguratDoor = instance->GetGameObject(m_auiZigguratDoorGUID[i]))
                {
                    if (pCrystal->IsWithinDistInMap(pZigguratDoor, 50.0f, false))
                    {
                        m_auiCrystalSortedGUID[i] = (*itr);
                        itr = m_lCrystalGUID.erase(itr);
                        bAlreadyIterated = true;
                        break;
                    }
                }
            }
        }

        if (itr != m_lCrystalGUID.end() && !bAlreadyIterated)
            ++itr;
    }
}

void instance_stratholme::FillAbomPullList()
{
	if (GameObject* pGate = GetSingleGameObjectFromStorage(GO_PORT_SLAUGTHER))
    {
        m_AbominationPull.clear();
        for(GUIDList::iterator itr = m_lAbominationGUID.begin(); itr != m_lAbominationGUID.end(); ++itr)
        {
            Creature* pAbom = instance->GetCreature(*itr);

			if (pAbom && pAbom->isAlive())
				m_AbominationPull.push_back(MobInfo(pAbom->GetObjectGuid(), pGate->GetDistance2d(pAbom)));
        }
        m_AbominationPull.sort([=](MobInfo first, MobInfo second) -> bool { return first.dist < second.dist; });
    }
    else
        debug_log("SD0: Instance Stratholme: Filling distances and sorting abominations failed.");
}

void instance_stratholme::OpenSlaugtherSquare()
{
    if (m_auiEncounter[2] == SPECIAL && m_auiEncounter[3] == SPECIAL && m_auiEncounter[4] == SPECIAL)
    {
		DoOrSimulateScriptTextForThisInstance(SAY_ASHARI_FALLEN, NPC_BARON_RIVENDARE);
        HandleGameObject(GO_PORT_GAUNTLET, true);
        HandleGameObject(GO_PORT_SLAUGTHER, true);
    }
    else
        debug_log("SD0: Instance Stratholme: Cannot open slaugther square yet.");
}

void instance_stratholme::PlayersRun(uint32 uiAction)
{
    Map::PlayerList const& lPlayers = instance->GetPlayers();

    if (lPlayers.isEmpty())
        return;

    for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
    {
        if (Player* pPlayer = itr->getSource())
        {
            switch(uiAction)
            {
                case ACTION_RUN_START:
                    if (pPlayer->HasAura(SPELL_BARON_ULTIMATUM, EFFECT_INDEX_0))
                        pPlayer->RemoveAurasDueToSpell(SPELL_BARON_ULTIMATUM);
                    pPlayer->CastSpell(pPlayer, SPELL_BARON_ULTIMATUM, true);
                    break;
                case ACTION_RUN_ENCOUNTER:
                    if (pPlayer->HasAura(SPELL_BARON_ULTIMATUM))
                        pPlayer->RemoveAurasDueToSpell(SPELL_BARON_ULTIMATUM);
                    //pPlayer->NearTeleportTo(4032.77f, -3350.6f, 115.06f, 0);			// Not sure why it was there, non-blizz
                    break;
                case ACTION_RUN_COMPLETE:
                    if (pPlayer->GetQuestStatus(QUEST_DEAD_MAN_PLEA) == QUEST_STATUS_INCOMPLETE)
                        pPlayer->AreaExploredOrEventHappens(QUEST_DEAD_MAN_PLEA);

					pPlayer->CastSpell(pPlayer,SPELL_BARON_RUN_REP, true);
                    break;
            }
        }
    }
}

InstanceData* GetInstanceData_instance_stratholme(Map* pMap)
{
    return new instance_stratholme(pMap);
}

void AddSC_instance_stratholme()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "instance_stratholme";
    pNewscript->GetInstanceData = &GetInstanceData_instance_stratholme;
    pNewscript->RegisterSelf();
}
