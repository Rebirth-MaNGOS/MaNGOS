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
SDName: Boss_Ragnaros
SD%Complete: 80
SDComment: To do: Melt Weapon
SDCategory: Molten Core
EndScriptData */

#include "precompiled.h"
#include "molten_core.h"

enum eRagnaros
{
    SAY_SUMMON_FIRST            = -1409013,
    SAY_SUMMON                  = -1409014,
    SAY_MIGHT                   = -1409015,
    SAY_WRATH                   = -1409016,
    SAY_KILL                    = -1409017,
    SAY_MAGMA_BLAST             = -1409018,

    SPELL_ELEMENTAL_FIRE        = 20563,
	SPELL_ELEMENTAL_FIRE_DIRECT = 20564,
    SPELL_MIGHT_OF_RAGNAROS     = 21154,    
    SPELL_MAGMA_BLAST           = 20565,    
    SPELL_MELT_WEAPON           = 21387,
    SPELL_WRATH_OF_RAGNAROS     = 20566,    // Melee Knockback
    SPELL_LAVA_BURST            = 21908,    
    SPELL_RAGNAROS_EMERGE       = 20568,
    SPELL_RAGNAROS_SUBMERGE     = 21107,
    SPELL_RAGNAROS_SUBMERGE_    = 20567,
    SPELL_SONS_OF_FLAME         = 21110,
    SPELL_ROOT_SELF             = 23973,

    BOSS_STATE_NORMAL           = 0,
    BOSS_STATE_SUBMERGE         = 1,
    BOSS_STATE_MAGMA_BLAST      = 2
};

/*static float Sons[8][4]=
{
    { 848.74f, -816.10f, -229.74f, 2.61f },
    { 852.56f, -849.86f, -228.56f, 2.83f },
    { 808.71f, -852.84f, -227.91f, 0.96f },
    { 786.59f, -821.13f, -226.35f, 0.94f },
    { 796.21f, -800.94f, -226.01f, 0.56f },
    { 821.60f, -782.74f, -226.02f, 6.15f },
    { 844.92f, -769.45f, -225.52f, 4.45f },
    { 839.82f, -810.86f, -229.68f, 4.69f }
};*/

struct MANGOS_DLL_DECL boss_ragnarosAI : public ScriptedAI
{
    boss_ragnarosAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_molten_core*)pCreature->GetInstanceData();
        SetCombatMovement(false);
        m_creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_FIRE, true);
        m_bossState = BOSS_STATE_NORMAL;
        Reset();
    }

    instance_molten_core* m_pInstance;

    bool m_bSubmergedOnce;

    uint32 m_uiEmergeTimer;
    uint32 m_uiMightOfRagnarosTimer;
    uint32 m_uiLavaBurstTimer;
	uint32 m_uiLavaBurstCount;
    uint32 m_uiSubmergeTimer;
    uint32 m_uiWrathOfRagnarosTimer;
	uint32 m_uiMeltWeaponTimer;
    uint32 m_uiGcdTimer;
    uint32 m_sonCheckTimer;

    uint8  m_bossState;
    uint8  m_uiEmergePhase;

    void Reset()
    {
        m_bSubmergedOnce = false;

        m_uiEmergeTimer = 0;
		m_uiMeltWeaponTimer = 10000;
        m_uiMightOfRagnarosTimer = 20000;
        m_uiLavaBurstTimer = urand(10000, 15000);
		m_uiLavaBurstCount = 0;
        m_uiSubmergeTimer = 180000;
        m_uiWrathOfRagnarosTimer = 25000;
        m_sonCheckTimer = 0;
        m_uiGcdTimer = 0;

        m_uiEmergePhase = 0;

		m_creature->RemoveAurasDueToSpell(SPELL_MELT_WEAPON);
        m_creature->RemoveAurasDueToSpell(SPELL_ELEMENTAL_FIRE);
        m_creature->RemoveAurasDueToSpell(SPELL_ROOT_SELF);

        std::list<Creature*> allSons;
        GetCreatureListWithEntryInGrid(allSons, m_creature, NPC_SON_OF_FLAME, 5000);

        if (!allSons.empty())
        {
            for (std::list<Creature*>::iterator i = allSons.begin(); i != allSons.end(); ++i)
            {
                (*i)->ForcedDespawn();
            }
        }

        if (m_bossState == BOSS_STATE_MAGMA_BLAST)
            m_bossState = BOSS_STATE_NORMAL;
    }

    void OverrideAttackDistance(float &attackDistance, Unit *pWho)
    {
        if (m_creature->CanReachWithMeleeAttack(pWho))
            attackDistance = 5000;
        else
            attackDistance = 0;
    }
	
    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_RAGNAROS, FAIL);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_RAGNAROS, DONE);
    }

    void KilledUnit(Unit* /*pVictim*/)
    {
        DoScriptText(SAY_KILL, m_creature);
    }

    void Aggro(Unit* /*pAttacker*/)
    {
		//Start combat with all players in order to prevent ooc rezzing!
		m_creature->SetInCombatWithZone();
		m_creature->CastSpell(m_creature, SPELL_MELT_WEAPON, true);
        m_creature->CastSpell(m_creature, SPELL_ELEMENTAL_FIRE, true);
        m_creature->CastSpell(m_creature, SPELL_ROOT_SELF, true);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_RAGNAROS, IN_PROGRESS);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetCreatureInfo()->Entry == NPC_SON_OF_FLAME)
            pSummoned->SetMeleeDamageSchool(SPELL_SCHOOL_FIRE);
    }

	void JustSummoned(GameObject* pGo) 
	{
		if (pGo->GetEntry() != 178088)
			return;

		pGo->SetOwnerGuid(m_creature->GetObjectGuid());
	}

    Player* DoSelectRandomManaPlayer()
    {
        if (!m_creature->CanHaveThreatList())
            return 0;

        GUIDVector vGuids;
        m_creature->FillGuidsListFromThreatList(vGuids);
        std::vector<Player*> vManaPlayers;

        if (!vGuids.empty())
        {
            for (GUIDVector::const_iterator itr = vGuids.begin(); itr != vGuids.end(); ++itr)
            {
                if (Unit* pTarget = m_creature->GetMap()->GetUnit(*itr))
                {
                    // We need only a player
                    if (!pTarget->IsCharmerOrOwnerPlayerOrPlayerItself())
                        continue;

                    // An enemy is a player with mana power
                    if (pTarget->getPowerType() == POWER_MANA)
                        vManaPlayers.push_back((Player*)pTarget);
                }
            }
        }

        if (!vManaPlayers.empty())
        {
            std::vector<Player*>::iterator i = vManaPlayers.begin();
            advance(i, (rand() % vManaPlayers.size()));
            return (*i);
        }
        
        return 0;
    }

	Player* DoGetPlayerInMeleeRangeByThreat()
	{
		std::vector<Player*> tmp_list;

		if (!m_creature->CanHaveThreatList())
			return NULL;

		GUIDVector vGuids;
		m_creature->FillGuidsListFromThreatList(vGuids);

		if (!vGuids.empty())
		{
			for (ObjectGuid current_guid : vGuids)
			{
				if (Unit* current_target = m_creature->GetMap()->GetUnit(current_guid))
				{
					// We need only a player
					if (!current_target->IsCharmerOrOwnerPlayerOrPlayerItself())
						continue;

					if (m_creature->CanReachWithMeleeAttack(current_target))
						tmp_list.push_back(dynamic_cast<Player*>(current_target));
				}
			}
		}

		// If there's just one player on the threat list we return that player.
		if (tmp_list.size() == 1)
			return tmp_list.front();

		// Sort the list from highest to lowest threat.
		std::sort(tmp_list.begin(), tmp_list.end(), [&]( Player* first, Player* second) -> bool { return m_creature->getThreatManager().getThreat(first) > m_creature->getThreatManager().getThreat(second); });

		return tmp_list.empty() ? NULL : tmp_list.front();
	}


    void HandleLavaBurst(uint32 diff)
    {
        if (m_uiLavaBurstTimer <= diff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_LAVA_BURST) == CAST_OK)
			{
				if (++m_uiLavaBurstCount % 3 == 0)
	                m_uiLavaBurstTimer = urand(10000, 15000);
				else
					m_uiLavaBurstTimer = 2000;
			}
        }
        else
            m_uiLavaBurstTimer -= diff;
    }

    void Submerge()
    {
        m_sonCheckTimer = 1000;
        m_creature->AttackStop();
        if (m_creature->IsNonMeleeSpellCasted(false))
            m_creature->InterruptNonMeleeSpells(false);
        //DoCastSpellIfCan(m_creature, 23973);
        //m_creature->setFaction(FACTION_FRIENDLY);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        if (!m_bSubmergedOnce)
        {
            DoScriptText(SAY_SUMMON_FIRST, m_creature);
            m_bSubmergedOnce = true;
        }
        else
            DoScriptText(SAY_SUMMON, m_creature);

        for (uint8 i = 0; i < 8; ++i)
		{
            m_creature->CastSpell(m_creature, SPELL_SONS_OF_FLAME + i, true);
		}
        //DoCastSpellIfCan(m_creature, SPELL_RAGNAROS_SUBMERGE_);   // Animation of submerge
        DoCastSpellIfCan(m_creature, SPELL_RAGNAROS_SUBMERGE, CAST_INTERRUPT_PREVIOUS);      // Passive

        m_bossState = BOSS_STATE_SUBMERGE;
        m_uiEmergeTimer = 90000;
        m_uiEmergePhase = 0;
    }

    void DoSubmergeState(uint32 diff)
    {
        // Just call this so that if there's a wipe we reset properly
        m_creature->SelectHostileTarget();

        if (m_sonCheckTimer <= diff)
            m_sonCheckTimer = 0;
        else
            m_sonCheckTimer -= diff;

        if (m_uiEmergePhase == 0 && m_uiEmergeTimer > 2000 && m_sonCheckTimer == 0)
        {
            m_sonCheckTimer = 2000;
            std::list<Creature*> allSons;
            GetCreatureListWithEntryInGrid(allSons, m_creature, NPC_SON_OF_FLAME, 5000);

            bool foundActiveSon = false;
            if (!allSons.empty())
            {
                for (std::list<Creature*>::iterator i = allSons.begin(); i != allSons.end(); ++i)
                {
                    if (Creature* pSon = *i)
                    {
                        if (!pSon->isAlive())
                            continue;

                        Unit::AuraList const& uAuraList = pSon->GetAurasByType(SPELL_AURA_MOD_STUN);

                        bool isBanished = false;
                        if (!uAuraList.empty())
                        {
                            for (Unit::AuraList::const_iterator itr = uAuraList.begin(); itr != uAuraList.end(); ++itr)
                            {
                                if ((*itr)->GetSpellProto()->Mechanic == MECHANIC_BANISH)
                                {
                                    // Banished!  Pretty much counts as dead
                                    isBanished = true;
                                    break;
                                }
                            }
                        }

                        if (!isBanished)
                        {
                            foundActiveSon = true;
                            break;
                        }
                    }
                }
            }

            if (!foundActiveSon)
                m_uiEmergeTimer = 2000;
        }

        // Emerge
        if (m_uiEmergeTimer <= diff)
        {
            if (++m_uiEmergePhase == 1)
            {
                // Emerge animation
                m_creature->RemoveAurasDueToSpell(SPELL_RAGNAROS_SUBMERGE);
                DoCastSpellIfCan(m_creature, SPELL_RAGNAROS_EMERGE);

                m_uiEmergeTimer = 3000; // maybe 2900 as cast time
            }
            else
            {
                // Ragnaros is fully emerged
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->setFaction(m_creature->GetCreatureInfo()->faction_A);

                m_bossState = BOSS_STATE_NORMAL;
                m_uiEmergePhase = 0;
                m_uiSubmergeTimer = 180000;
                m_uiGcdTimer = 3000;
            }
        }
        else
            m_uiEmergeTimer -= diff;
    }

    void DoNormalState(uint32 diff)
    {
        // Submerge
        if (m_uiSubmergeTimer <= diff)
        {
            Submerge();
            return;
        }
        else
            m_uiSubmergeTimer -= diff;

        if (m_uiGcdTimer > 0)
        {
            if (m_uiGcdTimer <= diff)
                m_uiGcdTimer = 0;
            else 
                m_uiGcdTimer -= diff;
        }

        // Wrath Of Ragnaros
        if (m_uiWrathOfRagnarosTimer <= diff)
        {
            m_creature->InterruptNonMeleeSpells(false);
            if (DoCastSpellIfCan(m_creature, SPELL_WRATH_OF_RAGNAROS) == CAST_OK)
            {
                DoScriptText(SAY_WRATH, m_creature);
                m_uiWrathOfRagnarosTimer = 25000;
                m_uiGcdTimer = 3000;

				// Threat wipe: done in core!
                /*GUIDVector vGuids;
                m_creature->FillGuidsListFromThreatList(vGuids);

                if (!vGuids.empty())
                {
                    // Threat wipe everyone affected by Wrath of Ragnaros
                    for (GUIDVector::const_iterator itr = vGuids.begin(); itr != vGuids.end(); ++itr)
                    {
                        if (Unit* pTarget = m_creature->GetMap()->GetUnit(*itr))
                        {
                            // Wrath of ragnaros's radius is 25
                            if (m_creature->IsWithinDistInMap(pTarget,25.0f-m_creature->GetObjectBoundingRadius()))
                                m_creature->getThreatManager().modifyThreatPercent(pTarget, -99);
                        }
                    }
                }*/
            }
        }
        else
            m_uiWrathOfRagnarosTimer -= diff;

		// Might of Ragnaros
        if (m_uiMightOfRagnarosTimer <= diff)
        {
            m_uiMightOfRagnarosTimer = 0;

            if (m_uiGcdTimer == 0 && m_uiWrathOfRagnarosTimer >= 3000)
            {
                if (Player* pManaPlayer = DoSelectRandomManaPlayer())
				    if (DoCastSpellIfCan(pManaPlayer, SPELL_MIGHT_OF_RAGNAROS) == CAST_OK)
				    {
                        DoScriptText(SAY_MIGHT, m_creature);
                        m_uiMightOfRagnarosTimer = 20000;
                        m_uiGcdTimer = 3000;
				    }
            }
        }
        else
            m_uiMightOfRagnarosTimer -= diff;

		// Cast the Elemental Fire effect on anyone in range who doesn't have the effect.
		DoCastSpellIfCan(m_creature->getVictim(), SPELL_ELEMENTAL_FIRE_DIRECT, CAST_AURA_NOT_PRESENT);


        // Lava Burst
        HandleLavaBurst(diff);

        if (m_bossState == BOSS_STATE_NORMAL)
        {
            if (!m_creature->CanReachWithMeleeAttack(m_creature->getVictim()))
            {
				if (Player* melee_player = DoGetPlayerInMeleeRangeByThreat())
				{
					// If there are players in melee range prioritise them.
					m_creature->SetTargetGuid(melee_player->GetGUID());
					if (m_creature->getVictim())
						this->DoMeleeAttackIfReady();
				}
				else
				{
					// Cannot find target- move to magma blast mode
					DoScriptText(SAY_MAGMA_BLAST, m_creature);
					m_bossState = BOSS_STATE_MAGMA_BLAST;
				}
            } 
			else
                this->DoMeleeAttackIfReady();
        } 
		else
        {
            if (m_creature->CanReachWithMeleeAttack(m_creature->getVictim()))
                m_bossState = BOSS_STATE_NORMAL;
            else if (m_uiGcdTimer == 0)
            {
		        if (Unit* bTarget = GetRandomPlayerInCurrentMap())
                {
					if ((bTarget->GetDistance(m_creature) > 150 || !m_creature->IsWithinLOS(bTarget->GetPositionX(), bTarget->GetPositionY(), bTarget->GetPositionZ())) && bTarget->isAlive())
						bTarget->NearTeleportTo(836.58f, -813.99f, -229.28f, 4.79f);
						
                    DoCastSpellIfCan(bTarget, SPELL_MAGMA_BLAST);
                    m_uiGcdTimer = 3000;
                }
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bossState == BOSS_STATE_SUBMERGE)
        {
            DoSubmergeState(uiDiff);
            return;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        switch (m_bossState)
        {
        case BOSS_STATE_NORMAL:
        case BOSS_STATE_MAGMA_BLAST:
           DoNormalState(uiDiff);
           break;
        }
    }
};

CreatureAI* GetAI_boss_ragnaros(Creature* pCreature)
{
    return new boss_ragnarosAI(pCreature);
}

void AddSC_boss_ragnaros()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "boss_ragnaros";
    pNewScript->GetAI = &GetAI_boss_ragnaros;
    pNewScript->RegisterSelf();
}
