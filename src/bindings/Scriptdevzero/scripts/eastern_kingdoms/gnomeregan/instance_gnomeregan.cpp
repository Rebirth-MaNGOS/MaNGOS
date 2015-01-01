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
SDName: Instance_Gnomeregan
SD%Complete: 100
SDComment:
SDCategory: Gnomeregan
EndScriptData */

#include "precompiled.h"
#include "gnomeregan.h"

instance_gnomeregan::instance_gnomeregan(Map* pMap) : ScriptedInstance(pMap),
	m_uiSummonWalkingBombTimer(0),
	m_uiActivateNewGnomeFaceTimer(0)
{
	Initialize();
};

void instance_gnomeregan::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

    for(uint8 i = 0; i < 6; ++i)
		m_uiGnomeFaceGUID[i].Clear();

    lGnomeFace1BombsGUID.clear();
    lGnomeFace2BombsGUID.clear();
    lGnomeFace3BombsGUID.clear();
    lGnomeFace4BombsGUID.clear();
    lGnomeFace5BombsGUID.clear();
    lGnomeFace6BombsGUID.clear();
}

void instance_gnomeregan::OnCreatureCreate(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_MEKGINEER_THERMAPLUG:
        case NPC_KERNOBEE:
            m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
    }
}

void instance_gnomeregan::OnObjectCreate(GameObject* pGo)
{
    switch(pGo->GetEntry())
    {
        case GO_CAVE_IN_1:
        case GO_CAVE_IN_2:
		case GO_THE_FINAL_CHAMBER:
            m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
            break;
        case GO_WEEGLIS_BARREL:
            lExplosiveBarrels.push_back(pGo->GetObjectGuid());
            break;
        case GO_GNOME_FACE_01:
            m_uiGnomeFaceGUID[0] = pGo->GetObjectGuid();
            break;
        case GO_GNOME_FACE_02:
            m_uiGnomeFaceGUID[1] = pGo->GetObjectGuid();
            break;
        case GO_GNOME_FACE_03:
            m_uiGnomeFaceGUID[2] = pGo->GetObjectGuid();
            break;
        case GO_GNOME_FACE_04:
            m_uiGnomeFaceGUID[3] = pGo->GetObjectGuid();
            break;
        case GO_GNOME_FACE_05:
            m_uiGnomeFaceGUID[4] = pGo->GetObjectGuid();
            break;
        case GO_GNOME_FACE_06:
            m_uiGnomeFaceGUID[5] = pGo->GetObjectGuid();
            break;
    }
}
    
void instance_gnomeregan::SetData(uint32 uiType, uint32 uiData)
{
    switch(uiType)
    {
        case TYPE_EMI:
            m_auiEncounter[0] = uiData;
            if (uiData == SPECIAL)
	    {
                if (!lExplosiveBarrels.empty())
                {
                    for(GUIDList::iterator itr = lExplosiveBarrels.begin(); itr != lExplosiveBarrels.end(); ++itr)
                    {
                        if (GameObject* GoBarrel = instance->GetGameObject(*itr))
                        {
                            if (GoBarrel->GetGoState() != GO_STATE_ACTIVE_ALTERNATIVE)
                            {
                                GoBarrel->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);

                                // Kill Players which are too close to the explosive barrel
                                Map::PlayerList const& lPlayers = instance->GetPlayers();
                                if (!lPlayers.isEmpty())
                                {
                                    for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                                    {
                                        if (Player* pPlayer = itr->getSource())
                                        {
                                            if (pPlayer->isAlive() && pPlayer->IsWithinDist(GoBarrel, 10.0f))
                                            {
                                                pPlayer->SetHealth(0);
                                                pPlayer->SetDeathState(JUST_DIED);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        //itr = lExplosiveBarrels.erase(itr);
                    }
                }
                else
                    error_log("SD0: Gnomeregan: List of Emi's explosive barrels is empty!");
	    }
	    break;
        case TYPE_THERMAPLUG:
            m_auiEncounter[1] = uiData;
            if (uiData == IN_PROGRESS)
            {
                m_uiSummonWalkingBombTimer = 9000;
                m_uiActivateNewGnomeFaceTimer = 30000;

                HandleGameObject(GO_THE_FINAL_CHAMBER, false);
            }

            if (uiData == NOT_STARTED || uiData == DONE)
            {
                m_uiSummonWalkingBombTimer = 0;
                m_uiActivateNewGnomeFaceTimer = 0;

                HandleGameObject(GO_THE_FINAL_CHAMBER, true);

                // Close Gnome Face gameobjects
                for(uint8 i = 0; i < 6; ++i)
                    ActivateGnomeFace(i, false);

                // Despawn summoned Walking Bombs when event is done?
                if (uiData == NOT_STARTED)
                {
                    // Despawn summoned Walking Bomb creatures
                    ResetWalkingBombs(lGnomeFace1BombsGUID);
                    ResetWalkingBombs(lGnomeFace2BombsGUID);
                    ResetWalkingBombs(lGnomeFace3BombsGUID);
                    ResetWalkingBombs(lGnomeFace4BombsGUID);
                    ResetWalkingBombs(lGnomeFace5BombsGUID);
                    ResetWalkingBombs(lGnomeFace6BombsGUID);
                }
            }
            break;
                
        // Bombs: 0 == 1th Gnome Face, 1 == 2th Gnome Face, ... 5 == 6th Gnome Face
        case TYPE_GNOME_FACE_01:
            ActivateGnomeFace(0, false);
            ResetWalkingBombs(lGnomeFace1BombsGUID);
            break;
        case TYPE_GNOME_FACE_02:
            ActivateGnomeFace(1, false);
            ResetWalkingBombs(lGnomeFace2BombsGUID);
            break;
        case TYPE_GNOME_FACE_03:
            ActivateGnomeFace(2, false);
            ResetWalkingBombs(lGnomeFace3BombsGUID);
            break;
        case TYPE_GNOME_FACE_04:
            ActivateGnomeFace(3, false);
            ResetWalkingBombs(lGnomeFace4BombsGUID);
            break;
        case TYPE_GNOME_FACE_05:
            ActivateGnomeFace(4, false);
            ResetWalkingBombs(lGnomeFace5BombsGUID);
            break;
        case TYPE_GNOME_FACE_06:
            ActivateGnomeFace(5, false);
            ResetWalkingBombs(lGnomeFace6BombsGUID);
            break;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1];

        strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_gnomeregan::GetData(uint32 uiType)
{
	if (uiType < MAX_ENCOUNTER)
		return m_auiEncounter[uiType];

	return 0;
}
    
void instance_gnomeregan::ResetWalkingBombs(GUIDList m_uiList)
{
    if (!m_uiList.empty())
    {
        debug_log("SD0: Gnomeregan: Despawning %lu (not exactly value) Walking Bombs count.", m_uiList.size());

        for (GUIDList::iterator itr = m_uiList.begin(); itr != m_uiList.end(); ++itr)
            if (Creature* pBomb = instance->GetCreature((*itr)))
            {
                if (pBomb->isAlive())
                {
                    pBomb->DealDamage(pBomb, pBomb->GetHealth(), NULL, SPELL_DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
                    //pBomb->SetDeathState(JUST_DIED);
                }
                //itr = m_uiList.erase(itr);
            }
        m_uiList.clear();
    }
    else
        debug_log("SD0: Gnomeregan: Some button of \"Gnome Face\" gameobject pushed, but list of Walking Bombs of this gameobject is empty. No creature despawned.");
}

void instance_gnomeregan::ActivateGnomeFace(uint8 no_gnome_face, bool activate)
{
    if (no_gnome_face > 5)
        return;

    // Different log message by action
    if (activate)
        debug_log("SD0: Gnomeregan: Activating %uth Gnome Face in Gnomeregan and spawning Walking Bomb", no_gnome_face+1);
    else
        debug_log("SD0: Gnomeregan: Deactivating %uth Gnome Face in Gnomeregan.", no_gnome_face+1);

    // Initialize
    Creature* pMekgineer = NULL;
    Creature* pBomb = NULL;

    // If activate, summon Walking Bomb
    if (activate)
    {
		pMekgineer = GetSingleCreatureFromStorage(NPC_MEKGINEER_THERMAPLUG);
        if (!pMekgineer)
        {
            error_log("SD0: Gnomeregan: Mekgineer Thermaplug can't be selected! Returning..");
            return;
        }

        if ((pBomb = pMekgineer->SummonCreature(NPC_WALKING_BOMB, SpawnLocations[no_gnome_face][0], SpawnLocations[no_gnome_face][1], SpawnLocations[no_gnome_face][2], 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000)))
        {
            Unit* pTarget = pMekgineer->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            pBomb->AI()->AttackStart(pTarget ? pTarget : pMekgineer->getVictim());
        }
        else
        {
            error_log("SD0: Gnomeregan: Walking Bomb was not summoned! Returning..");
            return;
        }
    }

    if (GameObject* GoGFace = instance->GetGameObject(m_uiGnomeFaceGUID[no_gnome_face]))
        GoGFace->SetGoState(activate ? GO_STATE_ACTIVE : GO_STATE_READY);

    if (activate)
    {
        switch(no_gnome_face)
        {
            case 0:
				lGnomeFace1BombsGUID.push_back(pBomb->GetObjectGuid());
                break;
            case 1:
                lGnomeFace2BombsGUID.push_back(pBomb->GetObjectGuid());
                break;
            case 2:
                lGnomeFace3BombsGUID.push_back(pBomb->GetObjectGuid());
                break;
            case 3:
                lGnomeFace4BombsGUID.push_back(pBomb->GetObjectGuid());
                break;
            case 4:
                lGnomeFace5BombsGUID.push_back(pBomb->GetObjectGuid());
                break;
            case 5:
                lGnomeFace6BombsGUID.push_back(pBomb->GetObjectGuid());
                break;
        }
    }
}

void instance_gnomeregan::Update(uint32 uiDiff)
{
    if (GetData(TYPE_THERMAPLUG) == IN_PROGRESS)
    {
        // Summon Walking Bomb via opened Gnome Faces (if any)
        if (m_uiSummonWalkingBombTimer < uiDiff)
        {
            for (uint8 i = 0; i < 6; ++i)
                if (GameObject* GoGFace = instance->GetGameObject(m_uiGnomeFaceGUID[i]))
                    if (GoGFace->GetGoState() == GO_STATE_ACTIVE)
                        ActivateGnomeFace(i);
            m_uiSummonWalkingBombTimer = 10000;
        }
        else
            m_uiSummonWalkingBombTimer -= uiDiff;

        // Open closed Gnome Face(s) (max 2)
        if (m_uiActivateNewGnomeFaceTimer < uiDiff)
        {
            uint8 uiFreeFacesNo[6];
            uint8 uiFreeFacesNoCounter = 0;

            for(uint8 i = 0; i < 6; ++i)
                uiFreeFacesNo[i] = 0;

            for(uint8 i = 0; i < 6; ++i)
                if (GameObject* GoGFace = instance->GetGameObject(m_uiGnomeFaceGUID[i]))
                    if (GoGFace->GetGoState() == GO_STATE_READY)
                        uiFreeFacesNo[uiFreeFacesNoCounter++] = i;

            // If all Gnome Faces are active, return
            if (!uiFreeFacesNoCounter)
            {
                debug_log("SD0: Gnomeregan: All \"Gnome Face\" gameobjects are activated = nothing to open; returning..");
            }
            else
            {
                uint8 uiRand = uiFreeFacesNo[rand()%uiFreeFacesNoCounter];
                ActivateGnomeFace(uiRand);

                if (uiFreeFacesNoCounter > 1)
                {
                    uint8 uiRand2 = uiFreeFacesNo[rand()%uiFreeFacesNoCounter];
                    while (uiRand == uiRand2)
                    {
                        uiRand2 = uiFreeFacesNo[rand()%uiFreeFacesNoCounter];
                    }
                    ActivateGnomeFace(uiRand2);
                }

				if (Creature* pMekgineer = GetSingleCreatureFromStorage(NPC_MEKGINEER_THERMAPLUG))
                    DoScriptText(YELL_BOMB_SUMMON, pMekgineer);
            }

            m_uiActivateNewGnomeFaceTimer = 30000;
        }
        else
            m_uiActivateNewGnomeFaceTimer -= uiDiff;
    }
}

void instance_gnomeregan::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1];

    for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_gnomeregan(Map* pMap)
{
    return new instance_gnomeregan(pMap);
}

void AddSC_instance_gnomeregan()
{
    Script* pNewscript;
    pNewscript = new Script;
    pNewscript->Name = "instance_gnomeregan";
    pNewscript->GetInstanceData = &GetInstanceData_instance_gnomeregan;
    pNewscript->RegisterSelf();
}
