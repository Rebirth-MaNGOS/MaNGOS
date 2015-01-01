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
SDName: Instance_Molten_Core
SD%Complete: 90
SDComment:
SDCategory: Molten Core
EndScriptData */

/* Molten Core:
    Encounter 0 - Lucifron
    Encounter 1 - Magmadar
    Encounter 2 - Gehennas
    Encounter 3 - Garr
    Encounter 4 - Shazzrah
    Encounter 5 - Baron Geddon
    Encounter 6 - Sulfuron Harbinger
    Encounter 7 - Golemagg
    Encounter 8 - Majordomo Executus
    Encounter 9 - Ragnaros
    Encounter 10 - Cache of the Firelord spawned (like boolean)
*/

#include "precompiled.h"
#include "molten_core.h"
#include "MapPersistentStateMgr.h"

instance_molten_core::instance_molten_core(Map* pMap) : ScriptedInstance(pMap),

    m_uiDomoAddsDeadCount(0),
    m_uiInitiateAddsTimer(1000),
    m_uiRagnarosDespawnTimer(0),
    m_uiMajordomoRespawnTimer(0)
{
    Initialize();
}

void instance_molten_core::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

    lFiresworn.clear();
    lFlamewaker.clear();
    lFlamewakerProtector.clear();
    lFlamewakerPriest.clear();
    lFlamewakerHealerElite.clear();
    lLavaBomb.clear();
    lCoreRager.clear();
}

bool instance_molten_core::IsEncounterInProgress() const
{
    for(uint8 itr = 0; itr < MAX_ENCOUNTER; ++itr)
        if (m_auiEncounter[itr] == IN_PROGRESS)
            return true;

    return false;
}

void instance_molten_core::HandleRune(uint32 uiEntry, uint32 uiData)
{
    if (GameObject* pGo = GetSingleGameObjectFromStorage(uiEntry))
    {
		HandleRune(pGo, uiData);
	} 
    else
        debug_log("SD0: Molten Core: Rune not handled properly!");
}

void instance_molten_core::HandleRune(GameObject *pGo, uint32 uiData)
{
    if (uiData == DONE)
    {
        // Exception for Kress rune which includes killing two bosses
        if (pGo->GetEntry() == GO_RUNE_OF_KRESS)
        {
            if (m_auiEncounter[0] == uiData && m_auiEncounter[1] == uiData)
                pGo->SetGoState(GO_STATE_ACTIVE);
        }
        else
            pGo->SetGoState(GO_STATE_ACTIVE);
    }
    else
    {
        pGo->SetGoState(GO_STATE_READY);

        if (uiData == SPECIAL)
            DoSpawnMajordomoIfCan(false);
    }
}

bool instance_molten_core::CanSpawnDomoOrRagnaros(bool spawn_majordomo)
{
    // Majordomo IF
    if (spawn_majordomo)
    {
        // Debug: instant spawn of Majordomo
        //return m_auiEncounter[8] == DONE || m_auiEncounter[9] == DONE ? false : true;

        // Other bosses (except Ragnaros)
        for(uint8 i = 0; i < 8; ++i)
            if (m_auiEncounter[i] != SPECIAL)
                return false;

        // Majordomo and Ragnaros encounter
        if (m_auiEncounter[8] == DONE || m_auiEncounter[9] == DONE)
            return false;

        return true;
    }
    // Ragnaros IF
    else
    {
        // Majordomo's encounter must to be DONE
        // Ragnaros's  encounter must to be NOT_STARTED
        if (m_auiEncounter[8] != DONE || m_auiEncounter[9] == DONE || m_auiEncounter[9] == IN_PROGRESS)
            return false;

        return true;
    }
}

void instance_molten_core::DoSpawnMajordomoIfCan(bool m_bKillAndRespawn, bool force)
{
    error_log("SD0: Molten Core - DoSpawnMajordomoIfCan() called.");

    // Un-hide Majordomo and his adds
    if (CanSpawnDomoOrRagnaros(true) || force)
    {
        if (force)
		{
			// Force all encounters to be set as dowsed.
			for (uint8 i = 0; i < 8; ++i)  
				m_auiEncounter[i] = SPECIAL;
		}
	    
	// Majordomo
        if (Creature* pDomo = GetSingleCreatureFromStorage(NPC_MAJORDOMO))
        {
            SetMoltenCoreCreatureVisibility(pDomo, ObjectGuid(), true, m_bKillAndRespawn);

            m_uiDomoAddsDeadCount = 0;

            if (pDomo->isDead())
                pDomo->Respawn();

            pDomo->SetUInt32Value(UNIT_DYNAMIC_FLAGS, 0);
            pDomo->SetStandState(UNIT_STAND_STATE_STAND);
            pDomo->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            //TODO: Yell again after 12 hour respawn?
            if (pDomo->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
                pDomo->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

			this->DoOrSimulateScriptTextForThisInstance(SAY_DOMO_SPAWN, NPC_MAJORDOMO);
        }
        else
            error_log("SD0: Molten Core: DoSpawnMajordomoIfCan() - unable to select Majordomo.");

        // Majordomo adds
        if (!lFlamewakerHealerElite.empty())
            for(GUIDList::iterator itr = lFlamewakerHealerElite.begin(); itr != lFlamewakerHealerElite.end(); ++itr)
                SetMoltenCoreCreatureVisibility(0, (*itr), true);

        RespawnBossAdds(lFlamewakerHealerElite);
    }
}

void instance_molten_core::SetMoltenCoreCreatureVisibility(Creature* pCreature, ObjectGuid pCreatureGUID, bool set_visible, bool m_bKillAndRespawn)
{
    if (!pCreature)
        pCreature = instance->GetCreature(pCreatureGUID);

    if (pCreature)
    {
        if (m_bKillAndRespawn)
        {
            if (pCreature->isAlive())
            {
                pCreature->SetHealth(0);
                pCreature->SetDeathState(JUST_DIED);
            }
			pCreature->Update(1,time(NULL));
			pCreature->Respawn();
        }

        if (set_visible)
        {
            //pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            pCreature->setFaction(pCreature->GetCreatureInfo()->faction_A);
            pCreature->SetVisibility(VISIBILITY_ON);
        }
        else
        {
            if (pCreature->isInCombat())
                pCreature->AI()->ResetToHome();
            //pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            pCreature->setFaction(FACTION_FRIENDLY);
            pCreature->SetVisibility(VISIBILITY_OFF);
        }
    }
    else
        debug_log("SD0: Molten Core: SetMoltenCoreCreatureVisibility() - Can't select creature!");
}

void instance_molten_core::RespawnBossAdds(GUIDList list)
{
    debug_log("SD0: Molten Core: RespawnBossAdds()");

    if (!list.empty())
    {
        for(GUIDList::iterator itr = list.begin(); itr != list.end(); ++itr)
        {
            if (Creature* pCreature = instance->GetCreature(*itr))
            {
                pCreature->Respawn();

                // Flamewaker Healer and Flamewaker Elite - Majordomo adds
                // Reset scale, damage and immunity at polymorph effect (only healer!)
                if (pCreature->GetEntry() == NPC_FLAMEWAKER_HEALER || pCreature->GetEntry() == NPC_FLAMEWAKER_ELITE)
                {
                    const CreatureInfo* cinfo = pCreature->GetCreatureInfo();
                    pCreature->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, cinfo->mindmg);
                    pCreature->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, cinfo->maxdmg);
                    pCreature->UpdateDamagePhysical(BASE_ATTACK);

                    pCreature->SetFloatValue(OBJECT_FIELD_SCALE_X, pCreature->GetCreatureInfo()->scale);

                    if (pCreature->GetEntry() == NPC_FLAMEWAKER_HEALER)
                        pCreature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, false);
                }
            }
        }
    }
    else
        debug_log("SD0: Molten Core: RespawnBossAdds() called, but list of adds seems to be empty!");
}

void instance_molten_core::StopSpawningAllTrashMobs(uint32 type)
{
	std::pair<std::multimap<uint32,ObjectGuid>::const_iterator,std::multimap<uint32,ObjectGuid>::const_iterator> creatureGuids = this->m_trashGuids.equal_range(type);

	for(std::multimap<uint32,ObjectGuid>::const_iterator creatureGuid = creatureGuids.first;creatureGuid != creatureGuids.second; ++creatureGuid)
	{
		Creature *pCreature = instance->GetCreature((*creatureGuid).second);

		if (pCreature == NULL)
		{
			//Try to update the respawn time if the creature is dead in the db
			//If it doesn't work, then the creature is not around because it's unloaded, and it will
			//be fixed when it loads back in
			instance->ExtendDeadCreatureRespawn((*creatureGuid).second.GetCounter(),604800);
		} else if (!pCreature->isAlive())
		{
			//Push the respawn a week out
			pCreature->SetRespawnTime(604800);
			pCreature->SaveRespawnTime();
		} else
			pCreature->SetRespawnDelay(604800);
	}
}

void instance_molten_core::OnPlayerEnter(Player* pPlayer)
{
    SetLavaState(pPlayer, true);
}

void instance_molten_core::OnPlayerLeave(Player* pPlayer)
{
    SetLavaState(pPlayer, false);
}

void instance_molten_core::OnObjectCreate(GameObject* pGo)
{
    switch(pGo->GetEntry())
    {
        case GO_LAVA_BOMB:
            lLavaBomb.push_back(pGo->GetObjectGuid());
            return;
        case GO_RUNE_OF_KRESS:                                   // Magmadar
            HandleRune(pGo, m_auiEncounter[1]);
            break;
        case GO_RUNE_OF_MOHN:                                    // Gehennas
            HandleRune(pGo, m_auiEncounter[2]);
            break;
        case GO_RUNE_OF_BLAZ:                                    // Garr
            HandleRune(pGo, m_auiEncounter[3]);
            break;
        case GO_RUNE_OF_MAZJ:                                    // Shazzrah
            HandleRune(pGo, m_auiEncounter[4]);
            break;
        case GO_RUNE_OF_ZETH:                                    // Geddon
            HandleRune(pGo, m_auiEncounter[5]);
            break;
        case GO_RUNE_OF_KORO:                                    // Sulfuron
            HandleRune(pGo, m_auiEncounter[6]);
            break;
        case GO_RUNE_OF_THERI:                                   // Golemagg
            HandleRune(pGo, m_auiEncounter[7]);
            break;
        case GO_FIRELORD_CACHE:									// Majordomo's chest
            break;
		default:
			return;
    }

	m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_molten_core::OnCreatureCreate(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_LUCIFRON:
        case NPC_MAGMADAR:
        case NPC_GEHENNAS:
        case NPC_GARR:
        case NPC_SHAZZRAH:
        case NPC_GEDDON:
        case NPC_SULFURON:
        case NPC_GOLEMAGG:
            break;
        case NPC_MAJORDOMO:
            if (!CanSpawnDomoOrRagnaros(true))
                SetMoltenCoreCreatureVisibility(pCreature, ObjectGuid(), false);
            break;
        case NPC_RAGNAROS:
            if (!CanSpawnDomoOrRagnaros(false))
                SetMoltenCoreCreatureVisibility(pCreature, ObjectGuid(), false);
            break;
        // Boss adds
        case NPC_FLAMEWAKER:
			lFlamewaker.push_back(pCreature->GetObjectGuid());
            return;
        case NPC_FLAMEWAKER_PROTECTOR:
            lFlamewakerProtector.push_back(pCreature->GetObjectGuid());
            return;
        case NPC_FLAMEWAKER_PRIEST:
            lFlamewakerPriest.push_back(pCreature->GetObjectGuid());
            return;
        case NPC_FLAMEWAKER_HEALER:
        case NPC_FLAMEWAKER_ELITE:
            lFlamewakerHealerElite.push_back(pCreature->GetObjectGuid());
            if (!CanSpawnDomoOrRagnaros(true))
                SetMoltenCoreCreatureVisibility(pCreature, ObjectGuid(), false);
            return;
        case NPC_FIRESWORN:
            lFiresworn.push_back(pCreature->GetObjectGuid());
            return;
        case NPC_CORE_RAGER:
            lCoreRager.push_back(pCreature->GetObjectGuid());
            return;
		case NPC_ANCIENT_CORE_HOUND:
			//If magmadar is dead, give all core hounds a week respawn so they won't respawn
			if (GetData(TYPE_MAGMADAR) >= DONE)
			{
				pCreature->SetRespawnDelay(604800);
				if (!pCreature->isAlive())
				{
					pCreature->SetRespawnTime(604800);
					pCreature->SaveRespawnTime();
				}
				else
					pCreature->ForcedDespawn();
			}
			this->m_trashGuids.insert(std::pair<uint32,ObjectGuid>(pCreature->GetEntry(),pCreature->GetObjectGuid()));
			break;
		case NPC_LAVA_SURGER:
			//Same with garr and lava surgers- a week respawn once he's dead so they won't come back
			if (GetData(TYPE_GARR) >= DONE)
			{
				pCreature->SetRespawnDelay(604800);
				if (!pCreature->isAlive())
				{
					pCreature->SetRespawnTime(604800);
					pCreature->SaveRespawnTime();
				}
			}
			this->m_trashGuids.insert(std::pair<uint32,ObjectGuid>(pCreature->GetEntry(),pCreature->GetObjectGuid()));
			break;
    }

	m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
}

void instance_molten_core::OnCreatureDeath(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_FLAMEWAKER_HEALER:
        case NPC_FLAMEWAKER_ELITE:
        {
            ++m_uiDomoAddsDeadCount;

            if (m_uiDomoAddsDeadCount == 4)
            {
                if (!lFlamewakerHealerElite.empty())
                {
                    for(GUIDList::iterator itr = lFlamewakerHealerElite.begin(); itr != lFlamewakerHealerElite.end(); ++itr)
                    {
                        Creature* pDomoAdd = instance->GetCreature(*itr);
                        if (pDomoAdd && pDomoAdd->isAlive())
                        {
                            // Un-sheep sheeped Adds
                            if (pDomoAdd->IsPolymorphed())
                                pDomoAdd->RemoveAurasAtMechanicImmunity(1<<(MECHANIC_POLYMORPH-1), 0, true);

                            // Add immunity on polymorph effect
                            pDomoAdd->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                        }
                    }
                }
            }
            else if (m_uiDomoAddsDeadCount == 7)
            {
                // Boost Majordomo and his last alive add
                if (Creature* pDomo = GetSingleCreatureFromStorage(NPC_MAJORDOMO))
                {
                    const CreatureInfo* domocinfo = pDomo->GetCreatureInfo();
                    pDomo->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, (domocinfo->mindmg * 1.7f));
                    pDomo->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, (domocinfo->maxdmg * 1.8f));
                    pDomo->UpdateDamagePhysical(BASE_ATTACK);
                }

                if (!lFlamewakerHealerElite.empty())
                {
                    for(GUIDList::iterator itr = lFlamewakerHealerElite.begin(); itr != lFlamewakerHealerElite.end(); ++itr)
                    {
                        Creature* pDomoAdd = instance->GetCreature(*itr);
                        if (pDomoAdd && pDomoAdd->isAlive())
                        {
                            pDomoAdd->SetFloatValue(OBJECT_FIELD_SCALE_X, (pDomoAdd->GetFloatValue(OBJECT_FIELD_SCALE_X) + 0.5f));

                            const CreatureInfo* cinfo = pDomoAdd->GetCreatureInfo();
                            pDomoAdd->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, (cinfo->mindmg * 2));
                            pDomoAdd->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, (cinfo->maxdmg * 2));
                            pDomoAdd->UpdateDamagePhysical(BASE_ATTACK);
                        }
                    }
                }
            }
            else if (m_uiDomoAddsDeadCount == 8 && GetData(TYPE_MAJORDOMO) == IN_PROGRESS)
                SetData(TYPE_MAJORDOMO, SPECIAL);
            break;
        }
    }
}

void instance_molten_core::Update(uint32 uiDiff)
{
    if (m_uiInitiateAddsTimer)
    {
        if (m_uiInitiateAddsTimer <= uiDiff)
        {
            m_uiInitiateAddsTimer = 0;

            // Lucifron encounter
            if (GetData(TYPE_LUCIFRON) == NOT_STARTED || GetData(TYPE_LUCIFRON) == FAIL)
                if (Creature* pLuci = GetSingleCreatureFromStorage(NPC_LUCIFRON))
                    if (pLuci->isAlive())
                        RespawnBossAdds(lFlamewakerProtector);

            // Gehennas encounter
            if (GetData(TYPE_GEHENNAS) == NOT_STARTED || GetData(TYPE_GEHENNAS) == FAIL)
                if (Creature* pGehennas = GetSingleCreatureFromStorage(NPC_GEHENNAS))
                    if (pGehennas->isAlive())
                        RespawnBossAdds(lFlamewaker);

            // Garr encounter
            if (GetData(TYPE_GARR) == NOT_STARTED || GetData(TYPE_GARR) == FAIL)
                if (Creature* pGarr = GetSingleCreatureFromStorage(NPC_GARR))
                    if (pGarr->isAlive())
                        RespawnBossAdds(lFiresworn);

            // Golemagg the Incinerator encounter
            if (GetData(TYPE_GOLEMAGG) == NOT_STARTED || GetData(TYPE_GOLEMAGG) == FAIL)
                if (Creature* pGolemagg = GetSingleCreatureFromStorage(NPC_GOLEMAGG))
                    if (pGolemagg->isAlive())
                        RespawnBossAdds(lCoreRager);

            // Sulfuron Harbinger encounter
            if (GetData(TYPE_SULFURON) == NOT_STARTED || GetData(TYPE_SULFURON) == FAIL)
                if (Creature* pSulfuron = GetSingleCreatureFromStorage(NPC_SULFURON))
                    if (pSulfuron->isAlive())
                        RespawnBossAdds(lFlamewakerPriest);

            // Majordomo Executus encounter
            DoSpawnMajordomoIfCan(false);

            // Ragnaros
            if (CanSpawnDomoOrRagnaros(false))
                if (Creature* pRagnaros = GetSingleCreatureFromStorage(NPC_RAGNAROS))
                    if (pRagnaros->isDead())
                        //pRagnaros->Respawn();
                        SetMoltenCoreCreatureVisibility(pRagnaros, ObjectGuid(), true, true);
        }
        else
            m_uiInitiateAddsTimer -= uiDiff;
    }

    // Despawn Ragnaros
    if (m_uiRagnarosDespawnTimer && GetData(TYPE_RAGNAROS) != DONE)
    {
        if (m_uiRagnarosDespawnTimer <= uiDiff)
        {
            if (Creature* pRagnaros = GetSingleCreatureFromStorage(NPC_RAGNAROS))
            {
                if (pRagnaros->isAlive() && !pRagnaros->isInCombat())
                {
                    debug_log("SD0: Molten Core: Despawning Ragnaros after 2 hours. It's time.. :-)");
                    m_uiRagnarosDespawnTimer = 0;

                    SetData(TYPE_RAGNAROS, NOT_STARTED);
                    SetMoltenCoreCreatureVisibility(pRagnaros, ObjectGuid(), false);
                }
                else
                    m_uiRagnarosDespawnTimer = 100;     // Try again every update tick
            }
            else
                error_log("SD0: Molten Core: Despawn Ragnaros and respawn Majordomo, but Ragnaros cannot be selected.");
        }
        else
            m_uiRagnarosDespawnTimer -= uiDiff;
    }

    // Respawn Majordomo after 12 hours
    if (m_uiMajordomoRespawnTimer)
    {
        if (m_uiMajordomoRespawnTimer <= uiDiff)
        {
            m_uiMajordomoRespawnTimer = 0;

            SetData(TYPE_MAJORDOMO, NOT_STARTED);
            DoSpawnMajordomoIfCan(false);
        }
        else
            m_uiMajordomoRespawnTimer -= uiDiff;
    }

    DoLavaDamage(uiDiff);
}

void instance_molten_core::SetData(uint32 uiType, uint32 uiData)
{
    switch(uiType)
    {
        case TYPE_LUCIFRON:
            m_auiEncounter[0] = uiData;
            HandleRune(GO_RUNE_OF_KRESS, uiData);
            if (uiData == FAIL)
                RespawnBossAdds(lFlamewakerProtector);
            break;
        case TYPE_MAGMADAR:
            m_auiEncounter[1] = uiData;
            HandleRune(GO_RUNE_OF_KRESS, uiData);
            if (!lLavaBomb.empty() && (uiData == FAIL || uiData == DONE))
            {
                for(GUIDList::iterator itr = lLavaBomb.begin(); itr != lLavaBomb.end(); ++itr)
                {
                    if (GameObject* pBomb = instance->GetGameObject(*itr))
                        pBomb->RemoveFromWorld();
                }
                lLavaBomb.clear();
            }

			if (uiData == DONE)
			{
				//Set any already-dead ancient core hounds to not bother respawning
				StopSpawningAllTrashMobs(NPC_ANCIENT_CORE_HOUND);
			}

            break;
        case TYPE_GEHENNAS:
            m_auiEncounter[2] = uiData;
            HandleRune(GO_RUNE_OF_MOHN, uiData);
            if (uiData == FAIL)
                RespawnBossAdds(lFlamewaker);
            break;
        case TYPE_GARR:
            m_auiEncounter[3] = uiData;
            HandleRune(GO_RUNE_OF_BLAZ, uiData);
            if (uiData == FAIL)
                RespawnBossAdds(lFiresworn);

			if (uiData == DONE)
			{
				//Set any already-dead ancient core hounds to not bother respawning
				StopSpawningAllTrashMobs(NPC_LAVA_SURGER);
			}
            break;
        case TYPE_SHAZZRAH:
            m_auiEncounter[4] = uiData;
            HandleRune(GO_RUNE_OF_MAZJ, uiData);
            break;
        case TYPE_GEDDON:
            m_auiEncounter[5] = uiData;
            HandleRune(GO_RUNE_OF_ZETH, uiData);
            break;
        case TYPE_SULFURON:
            m_auiEncounter[6] = uiData;
            HandleRune(GO_RUNE_OF_KORO, uiData);
            if (uiData == FAIL)
                RespawnBossAdds(lFlamewakerPriest);
            break;
        case TYPE_GOLEMAGG:
            m_auiEncounter[7] = uiData;
            HandleRune(GO_RUNE_OF_THERI, uiData);
            if (uiData == FAIL)
                RespawnBossAdds(lCoreRager);
            else if (uiData == DONE)    // Despawn Golemagg's adds
                if (!lCoreRager.empty())
                    for (GUIDList::iterator itr = lCoreRager.begin(); itr != lCoreRager.end(); ++itr)
                        if (Creature* target = instance->GetCreature(*itr))
                            target->ForcedDespawn();
            break;
        case TYPE_MAJORDOMO:
            m_auiEncounter[8] = uiData;
            switch(uiData)
            {
                case FAIL:
                    RespawnBossAdds(lFlamewakerHealerElite);
                    m_uiDomoAddsDeadCount = 0;
                    break;
                case DONE:
                    // Majordomo will be respawned after 12 hours,
                    // but his loot from cache should not be already obtainable
                    if (GetData(TYPE_MAJORDOMO_FIRST_SPAWN) == NOT_STARTED)
                        DoRespawnGameObject(GO_FIRELORD_CACHE, 2*HOUR);
                    break;
            }
            break;
        case TYPE_RAGNAROS:
            m_auiEncounter[9] = uiData;
            if (uiData == IN_PROGRESS && !m_uiRagnarosDespawnTimer)
            {
                //m_uiRagnarosDespawnTimer  = 2 * HOUR * IN_MILLISECONDS;
                //m_uiMajordomoRespawnTimer = 12 * HOUR * IN_MILLISECONDS;
                m_uiRagnarosDespawnTimer  = 2 * 3600 * 1000;
                m_uiMajordomoRespawnTimer = 12 * 3600 * 1000;
                //m_uiRagnarosDespawnTimer  = 10000;
                //m_uiMajordomoRespawnTimer = 15000;
            }
            else if (uiData == FAIL && !m_uiRagnarosDespawnTimer)
                m_uiRagnarosDespawnTimer = 100;
            else if (uiData == DONE)
            {
                // Ragnaros has been defeated, so Majordomo is no more needed..
                if (m_uiMajordomoRespawnTimer)
                    m_uiMajordomoRespawnTimer = 0;

                if (Creature* pDomo = GetSingleCreatureFromStorage(NPC_MAJORDOMO))
                {
                    if (pDomo->isAlive())
                    {
                        if (pDomo)
                            pDomo->AI()->ResetToHome();
                        pDomo->SetHealth(0);
                        pDomo->SetDeathState(JUST_DIED);
                    }
                }

                for(GUIDList::iterator itr = lFlamewakerHealerElite.begin(); itr != lFlamewakerHealerElite.end(); ++itr)
                {
                    if (Creature* pDomoAdd = instance->GetCreature(*itr))
                    {
                        if (pDomoAdd->isAlive())
                        {
                            if (pDomoAdd)
                                pDomoAdd->AI()->ResetToHome();
                            pDomoAdd->SetHealth(0);
                            pDomoAdd->SetDeathState(JUST_DIED);
                        }
                    }
                }

                SetData(TYPE_MAJORDOMO, DONE);
            }
            break;
        case TYPE_MAJORDOMO_FIRST_SPAWN:
            m_auiEncounter[10] = uiData;
            break;
    }

    OUT_SAVE_INST_DATA;

    std::ostringstream saveStream;
    saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
	<< m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " "
	<< m_auiEncounter[6] << " " << m_auiEncounter[7] << " " << m_auiEncounter[8] << " "
	<< m_auiEncounter[9] << " " << m_auiEncounter[10];

    strInstData = saveStream.str();

    SaveToDB();
    OUT_SAVE_INST_DATA_COMPLETE;
}

uint32 instance_molten_core::GetData(uint32 uiType)
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_molten_core::Load(const char* chrIn)
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
        >> m_auiEncounter[8] >> m_auiEncounter[9] >> m_auiEncounter[10];

    for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        if (m_auiEncounter[i] == IN_PROGRESS)           // Do not load an encounter as "In Progress" - reset it instead.
            m_auiEncounter[i] = NOT_STARTED;

    if (m_auiEncounter[8] == SPECIAL)                   // Do not load Majordomo's encounter as "Special" - reset.
        m_auiEncounter[8] = NOT_STARTED;

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_molten_core(Map* pMap)
{
    return new instance_molten_core(pMap);
}

void AddSC_instance_molten_core()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "instance_molten_core";
    pNewScript->GetInstanceData = &GetInstanceData_instance_molten_core;
    pNewScript->RegisterSelf();
}
