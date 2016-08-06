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
SDName: Boss_Mandokir
SD%Complete: 99
SDComment: test Threating Gaze
SDCategory: Zul'Gurub
EndScriptData */

#include "precompiled.h"
#include "zulgurub.h"
#include "SpellAuraDefines.h"
#include "Spell.h"

enum eMandokir
{
    EMOTE_RAGE          = -1309020,

    SAY_AGGRO           = -1309015,
    SAY_DING_KILL       = -1309016,
    SAY_GRATS_JINDO     = -1309017,
    SAY_WATCH           = -1309018,
    SAY_WATCH_WHISPER   = -1309019,

    SPELL_CHARGE        = 24315,
    SPELL_CHARGE2       = 24408,    // Charge randomly alone player
    SPELL_EXECUTE       = 7160,
    SPELL_FEAR          = 19134,
    SPELL_WHIRLWIND     = 15589,
	SPELL_WHIRLWIND_PREPARE = 29851,
    SPELL_MORTAL_STRIKE = 24573,
    SPELL_OVERPOWER     = 24407,
    SPELL_ENRAGE        = 24318,
    SPELL_WATCH         = 24314,
    SPELL_SUMMON_PLAYER = 25104,
    SPELL_LEVEL_UP      = 24312,

    SPELL_MOUNT         = 23243,    // This spell may not be correct, it's the spell used by item

    // Ohgans Spells
    SPELL_SUNDER_ARMOR  = 24317,

    // Chained Spirit Spells
    SPELL_REVIVE        = 24341,
};

static Loc aSpirits[]=
{
    {-12150.9f, -1956.24f, 133.41f, 2.57f},
    {-12157.1f, -1972.78f, 133.95f, 2.64f},
    {-12172.3f, -1982.63f, 134.06f, 1.48f},
    {-12194.0f, -1979.54f, 132.19f, 1.45f},
    {-12211.3f, -1978.49f, 133.58f, 1.35f},
    {-12228.4f, -1977.10f, 132.73f, 1.25f},
    {-12250.0f, -1964.78f, 135.07f, 0.92f},
    {-12264.0f, -1953.08f, 134.07f, 0.62f},
    {-12289.0f, -1924.10f, 132.62f, 5.37f},
    {-12267.3f, -1902.26f, 131.33f, 5.32f},
    {-12255.3f, -1893.53f, 134.03f, 5.06f},
    {-12229.9f, -1891.39f, 134.71f, 4.40f},
    {-12215.9f, -1889.09f, 137.27f, 4.70f},
    {-12200.5f, -1890.69f, 135.78f, 4.84f},
    {-12186.0f, -1890.12f, 134.26f, 4.36f},
    {-12246.3f, -1890.09f, 135.48f, 4.73f},
    {-12170.7f, -1894.85f, 133.85f, 3.51f},
    {-12279.0f, -1931.92f, 136.13f, 0.04f},
    {-12266.1f, -1940.72f, 132.61f, 0.70f},
};

/*######
## mob_chained_spirit
######*/

struct MANGOS_DLL_DECL mob_chained_spiritAI : public ScriptedAI
{
    mob_chained_spiritAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiPlayerCheck;
    ObjectGuid m_uiPlayerGUID;

    void Reset()
    {
        m_uiPlayerCheck = 60000;
        m_uiPlayerGUID.Clear();
    }
    
    void SetPlayerToRess(ObjectGuid Player)
    {
        m_uiPlayerGUID = Player;              
        m_uiPlayerCheck = 500;
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        // Revive
        if (m_uiPlayerCheck < uiDiff)
        {
            Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID);
            if (pPlayer && pPlayer->IsWithinDistInMap(m_creature, 2.0f))
            {
                m_creature->CastSpell(pPlayer, SPELL_REVIVE, false);
                
                m_creature->MonsterWhisper("I am released through you! Avenge me!", pPlayer, false);
                
                // Reset it
                m_uiPlayerGUID.Clear();                
                
                m_creature->ForcedDespawn(2000);
            }
            else
                m_uiPlayerCheck = 500;
        }
        else
            m_uiPlayerCheck -= uiDiff;
    }
};

CreatureAI* GetAI_mob_chained_spirit(Creature* pCreature)
{
    return new mob_chained_spiritAI(pCreature);
}

struct MANGOS_DLL_DECL boss_mandokirAI : public ScriptedAI
{
    boss_mandokirAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiDefaultLevel = m_creature->getLevel();
        m_pInstance = (instance_zulgurub*)pCreature->GetInstanceData();
        Reset();
    }

    instance_zulgurub* m_pInstance;

    uint32 m_uiWatchTimer;
    uint32 m_uiChargeTimer;
    uint32 m_uiExecuteTimer;
    uint32 m_uiWhirlwindTimer;
    uint32 m_uiFearTimer;
    uint32 m_uiMortalStrikeTimer;
    uint32 m_uiOverpowerTimer;
    uint32 m_uiCheckTimer;
    uint32 m_uiWriteThreatTimer;

	uint32 m_uiSuspendTimer; // Timer used to allow the charge animation to occur correctly. Works most of the time but not always
    uint32 m_uiDefaultLevel;

    uint8 m_uiKillCount;
    bool m_bRaptorDead;

    float m_fTargetThreat;
    ObjectGuid m_uiWatchTargetGUID;

    void Reset()
    {
        m_uiWatchTimer = 33000;
        m_uiChargeTimer = urand(20000, 30000);    // <--- this time probably wrong // This should be better based on a youtube video of the encounter. (2013-06-18)
        m_uiExecuteTimer = 2000;
        m_uiWhirlwindTimer = 20000;
        m_uiFearTimer = 1000;
        m_uiMortalStrikeTimer = 1000;
        m_uiOverpowerTimer = 9000;
        m_uiCheckTimer = 1000;
        m_uiWriteThreatTimer = 1000;
		m_uiSuspendTimer = 0;

        m_uiKillCount = 0;

        m_bRaptorDead = false;

        m_fTargetThreat = 0.0f;
        m_uiWatchTargetGUID.Clear();

        m_creature->SetLevel(m_uiDefaultLevel);

        DoCastSpellIfCan(m_creature, SPELL_MOUNT);

		if (m_pInstance)
			m_pInstance->SetData(TYPE_OHGAN, NOT_STARTED);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MANDOKIR, NOT_STARTED);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MANDOKIR, DONE);
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() == TYPEID_PLAYER)
        {					
            ++m_uiKillCount;

            if (m_uiKillCount == 3)
            {
                DoScriptText(SAY_DING_KILL, m_creature);

                if (m_pInstance)
                {
                    Creature* pJindo = m_pInstance->GetSingleCreatureFromStorage(NPC_JINDO);
                    if (pJindo && pJindo->isAlive())
                        DoScriptText(SAY_GRATS_JINDO, pJindo);
                }

                DoCastSpellIfCan(m_creature, SPELL_LEVEL_UP, CAST_TRIGGERED);
                m_creature->SetLevel(m_creature->getLevel() + 1);
                m_uiKillCount = 0;
            }

			if (m_creature->isInCombat())
			{				 
                int r;
                r = irand(0,5);
                if (r > 0)
                {
                    if (Creature* pSpirit = GetClosestCreatureWithEntry(pVictim, NPC_CHAINED_SPIRIT, 80.0f))
                    {
                        if(pSpirit && pSpirit->GetMotionMaster()->GetCurrentMovementGeneratorType() != POINT_MOTION_TYPE)
                        {                        
                            mob_chained_spiritAI* pSpiritAI = dynamic_cast<mob_chained_spiritAI*>(pSpirit->AI());
                            
                            if(pSpiritAI)
                            {
                                pSpiritAI->SetPlayerToRess(pVictim->GetGUID());
                                // only move if we know the spirit is ok
                                pSpirit->GetMotionMaster()->MovePoint(1, pVictim->GetPositionX(),pVictim->GetPositionY(), pVictim->GetPositionZ());
                            }
                        }
                    }
                }
			}
        }
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MANDOKIR, IN_PROGRESS);

        for(uint8 i = 0; i < sizeof(aSpirits)/sizeof(Loc); ++i)
            m_creature->SummonCreature(NPC_CHAINED_SPIRIT, aSpirits[i].x, aSpirits[i].y, aSpirits[i].z, aSpirits[i].o, TEMPSUMMON_CORPSE_DESPAWN, 0);

        // At combat start Mandokir is mounted so we must unmount it first, and set his flags for attackable
        m_creature->RemoveAurasDueToSpell(SPELL_MOUNT);
        if (m_creature->IsMounted())
            m_creature->Unmount();

        // And summon his raptor
        DoSpawnCreature(NPC_OHGAN, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 35000);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (m_creature->getVictim() && pSummoned->GetEntry() == NPC_OHGAN)
            pSummoned->AI()->AttackStart(m_creature->getVictim());
    }

    void DisableGeneratingThreatOfAurasOfWatchedTarget()
    {
        if (!m_uiWatchTargetGUID)
            return;

        Player* pWatchedPlayer = m_creature->GetMap()->GetPlayer(m_uiWatchTargetGUID);
        if (!pWatchedPlayer)
            return;

        // Target must be a player
        if (!pWatchedPlayer->IsCharmerOrOwnerPlayerOrPlayerItself())
            return;

        CalculateNeededThreatBonus(m_creature, SPELL_AURA_PERIODIC_DAMAGE);             // Dots

        GUIDVector vGuids;
        m_creature->FillGuidsListFromThreatList(vGuids);
        if (!vGuids.empty())
        {
            for (GUIDVector::const_iterator itr = vGuids.begin(); itr != vGuids.end(); ++itr)
            {
                if (Unit* pUnit = m_creature->GetMap()->GetUnit(*itr))
                {
                    CalculateNeededThreatBonus(pUnit, SPELL_AURA_PERIODIC_HEAL);       // Hots  (e.g. Renew, ..)
                    CalculateNeededThreatBonus(pUnit, SPELL_AURA_PERIODIC_ENERGIZE);   // Buffs (e.g. Paladin's BoW, ..)
                }
            }
        }
    }

    float CalculateNeededThreatBonus(Unit* pWho, AuraType aura_type)
    {
        if (!pWho)
            return 0;

        Unit::AuraList const& uAuraList = pWho->GetAurasByType(aura_type);
        if (uAuraList.empty())
        {
            //error_log("SD0: Mandokir: AuraList seems to be empty! AuraList type: %u", uint32(aura_type));
            return 0;
        }

        float m_uiNeededThreat = 0.0f;

        for(Unit::AuraList::const_iterator itr = uAuraList.begin(); itr != uAuraList.end(); ++itr)
        {
            Aura* pAura = ((Aura*)(*itr));
            if (pAura == NULL)
            {
                //error_log("SD0: Mandokir: pAura is NULL! Continue..");
                continue;
            }

            if (!pAura->GetCaster())
                continue;

            if (pAura->GetCaster()->GetObjectGuid() != m_uiWatchTargetGUID)
            {
                //error_log("SD0: Mandokir: CasterGUID != WatchedTargetGUID! Continue..");
                continue;
            }

			if (pAura->GetAuraMaxDuration() == 0 || pAura->GetAuraMaxTicks() == 0)  // Prevent division by zero caused by items like the lifestone.
				continue;

            bool  m_bIsPositive         = pAura->IsPositive();
            float m_fOriginalDmgPerTick = float(pAura->GetModifier()->m_amount);    // Basic DAMAGE/HEAL (without spell bonuses)
            float m_fDamagePerTick      = 0.0f;     // Also HEAL/TICK when HEAL effect provided
            float m_fThreatPerTick      = 0.0f;     // HEAL effects generating 1/2 threat as DAMAGE effects

            uint32 m_uiAuraTicksRemainCount = pAura->GetAuraMaxTicks() - pAura->GetAuraTicks();
            uint32 m_uiMaxAllowedAuraTicks  = (6000 / (pAura->GetAuraMaxDuration() / pAura->GetAuraMaxTicks()));
            if (m_uiAuraTicksRemainCount > m_uiMaxAllowedAuraTicks)
                m_uiAuraTicksRemainCount = m_uiMaxAllowedAuraTicks;

            if (aura_type != SPELL_AURA_PERIODIC_ENERGIZE)
            {
                m_fDamagePerTick = float(pAura->GetCaster()->SpellDamageBonusDone(pAura->GetTarget(), pAura->GetSpellProto(), m_fOriginalDmgPerTick, DOT));
                m_fThreatPerTick = float((m_bIsPositive ? 0.5f : 1.0f) * m_fDamagePerTick);
            }
            else
            {
                m_fDamagePerTick = m_fOriginalDmgPerTick;
                m_fThreatPerTick = m_fOriginalDmgPerTick / 2;
            }

            m_uiNeededThreat = float(m_uiAuraTicksRemainCount * m_fThreatPerTick);

            /*error_log("Rmn.T:  %u  OrigDmg/T:  %f  NewDmg/T:  %f  Thrt/T:  %f", m_uiAuraTicksRemainCount, m_fOriginalDmgPerTick, m_fDamagePerTick, m_fThreatPerTick);
            error_log("Saved threat:        %f   (+ %f)", m_fTargetThreat, m_uiNeededThreat);
            error_log("Max allowed threat:  %f", m_fTargetThreat + m_uiNeededThreat);*/
        }

        m_fTargetThreat += m_uiNeededThreat;
        return m_uiNeededThreat;
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_WATCH && pTarget->GetTypeId() == TYPEID_PLAYER)
        {
            Player* pPlayer = (Player*)pTarget;
            DoScriptText(SAY_WATCH, m_creature, pPlayer);
            DoScriptText(SAY_WATCH_WHISPER, m_creature, pPlayer);

            m_uiWatchTargetGUID = pPlayer->GetObjectGuid();
            m_fTargetThreat = m_creature->getThreatManager().getThreat(pPlayer);
            m_uiWatchTimer = 6000;

            DisableGeneratingThreatOfAurasOfWatchedTarget();

            //Could use this instead of hard coded timer for the above (but no script access),
            //but would still a hack since we should better use the dummy, at aura removal
            //SpellDurationEntry* const pDuration = sSpellDurationStore.LookupEntry(pSpell->DurationIndex);
        }
    }

    Player* GetMaxAlonePlayer()
    {
        if (!m_creature->getVictim())
            return NULL;

        std::list<Player*> lPlayers;

        ObjectGuid m_uiMaxAlonePlayerGUID;
        float  m_fMaxAlonePlayerDist  = 4;			// 0 before, now spreading within 8yrds is ok, else he charges the player that is alone

        GUIDVector vGuids;
        m_creature->FillGuidsListFromThreatList(vGuids);
        if (vGuids.empty())
            return NULL;

        for (GUIDVector::const_iterator itr = vGuids.begin(); itr != vGuids.end(); ++itr)
            if (Unit* pUnit = m_creature->GetMap()->GetUnit(*itr))
                if (pUnit->isAlive() && pUnit->IsCharmerOrOwnerPlayerOrPlayerItself() && pUnit->GetObjectGuid() != m_creature->getVictim()->GetObjectGuid())
                    lPlayers.push_back((Player*)pUnit);

        if (lPlayers.empty())
            return NULL;

        for(std::list<Player*>::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
            for(std::list<Player*>::const_iterator jtr = lPlayers.begin(); jtr != lPlayers.end(); ++jtr)
                if ((*jtr)->GetDistance(*itr) > m_fMaxAlonePlayerDist)
                {
                    m_uiMaxAlonePlayerGUID = (*jtr)->GetObjectGuid();
                    m_fMaxAlonePlayerDist  = (*jtr)->GetDistance(*itr);
                }

        return m_creature->GetMap()->GetPlayer(m_uiMaxAlonePlayerGUID);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        // If watched player move, increas his saved threat.
        // In this fact will Mandokir on end of Threating Gaze buff charge this player.
        if (pWho->GetObjectGuid() == m_uiWatchTargetGUID)
            m_fTargetThreat += 1.0f;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void DoActThreatingGaze(Player* pPlayer)
    {
        if (pPlayer && pPlayer->isAlive() && pPlayer->IsCharmerOrOwnerPlayerOrPlayerItself())
        {
            if (!m_creature->IsWithinLOSInMap(pPlayer))
            {
                float x,y,z;
                m_creature->GetPosition(x,y,z);
                pPlayer->NearTeleportTo(x, y, z, 0.0f);
                m_creature->CastSpell(pPlayer, SPELL_SUMMON_PLAYER, true);
            }

            // Remove "Watch" Mark
            if (pPlayer->HasAura(SPELL_WATCH))
                pPlayer->RemoveAurasDueToSpell(SPELL_WATCH);

            DoCastSpellIfCan(pPlayer, SPELL_CHARGE);
        }

        m_uiWatchTargetGUID.Clear();
        m_uiWatchTimer = 14000;     // 20000
    }

    void UpdateAI(const uint32 uiDiff)
    {
		if (m_uiSuspendTimer > uiDiff)
        {
            m_uiSuspendTimer -= uiDiff;
            return;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiWatchTimer <= uiDiff)
        {
            // If someone is watched
            if (m_uiWatchTargetGUID)
            {
                Player* pWatchTarget = m_creature->GetMap()->GetPlayer(m_uiWatchTargetGUID);

				//If player is casting or autoAttacking, mandokir will charge
				if (pWatchTarget && pWatchTarget->isAlive() && (pWatchTarget->IsNonMeleeSpellCasted(false) || pWatchTarget->isAutoAttacking()))
                    DoActThreatingGaze(pWatchTarget);
                else
                {
                    m_fTargetThreat = 0;
                    m_uiWatchTargetGUID.Clear();
                }
            }
            else
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    if (Player* pPlayer = pTarget->GetCharmerOrOwnerPlayerOrPlayerItself())
                        m_creature->CastSpell(pPlayer, SPELL_WATCH, false);
            }

            m_uiWatchTimer = 14000;     // 20000
        }
        else
            m_uiWatchTimer -= uiDiff;

        if (!m_uiWatchTargetGUID)
        {
            // Charge (max alone player)
            if (m_uiChargeTimer <= uiDiff)
            {
                if (Player* pPlayer = GetMaxAlonePlayer())
                {			
		            if (DoCastSpellIfCan(pPlayer, SPELL_CHARGE2) == CAST_OK)
                    {
                        if (Unit* pSecondThreatUnit = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 1))
                        {
							uint32 m_uiMainTargetThreat = m_creature->getThreatManager().getThreat(m_creature->getVictim());
							uint32 m_uiSecondTargetTrheat = 0;

							if(Unit* pTank = m_creature->getVictim())
								m_creature->getThreatManager().modifyThreatPercent(pTank, -30);			// reduce the threat of the tank and add more to the 2nd highest dps

                            m_uiSecondTargetTrheat = m_creature->getThreatManager().getThreat(pSecondThreatUnit);
                            uint32 m_uiDeficitThreat = m_uiMainTargetThreat - m_uiSecondTargetTrheat;
                            m_creature->getThreatManager().addThreat(pSecondThreatUnit, (m_uiDeficitThreat + 100.0f));
							
                        }
						m_uiSuspendTimer = 2000;
                    }
                }
                m_uiChargeTimer = urand(20000, 30000);
            }
            else
                m_uiChargeTimer -= uiDiff;

            // Execute
            if (m_uiExecuteTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_EXECUTE);
                m_uiExecuteTimer = 11000;
            }
            else
                m_uiExecuteTimer -= uiDiff;

            // Whirlwind
            if (m_uiWhirlwindTimer <= uiDiff)
            {
				m_creature->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL, false);
                DoCastSpellIfCan(m_creature, SPELL_WHIRLWIND);
				m_uiWhirlwindTimer = 18000;
            }
            else
			{
				if (m_uiWhirlwindTimer <= 2000)
					DoCastSpellIfCan(m_creature, SPELL_WHIRLWIND_PREPARE);
                m_uiWhirlwindTimer -= uiDiff;
			}

            // If more then 3 targets in melee range mandokir will cast fear
            if (m_uiFearTimer <= uiDiff)
            {
                uint8 uiTargetInRangeCount = 0;

                GUIDVector vGuids;
                m_creature->FillGuidsListFromThreatList(vGuids);

                for (GUIDVector::const_iterator itr = vGuids.begin(); itr != vGuids.end(); ++itr)
                {
                    Unit* pTarget = m_creature->GetMap()->GetUnit(*itr);

                    if (pTarget && pTarget->GetTypeId() == TYPEID_PLAYER && m_creature->CanReachWithMeleeAttack(pTarget))
                        ++uiTargetInRangeCount;
                }

                if (uiTargetInRangeCount > 3)
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_FEAR);

                m_uiFearTimer = 4000;
            }
            else
                m_uiFearTimer -= uiDiff;

            // Mortal Strike
            if (HealthBelowPct(50))
            {
                if (m_uiMortalStrikeTimer <= uiDiff)
                {
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_MORTAL_STRIKE);
                    m_uiMortalStrikeTimer = 20000;
                }
                else
                    m_uiMortalStrikeTimer -= uiDiff;
            }

            // Overpower
            if (m_uiOverpowerTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_OVERPOWER);
                m_uiOverpowerTimer = urand(8000,16000);
            }
            else
                m_uiOverpowerTimer -= uiDiff;
        }

        // Checking if Ohgan is dead. If yes Mandokir will enrage.
        if (!m_bRaptorDead)
            if (m_pInstance && m_pInstance->GetData(TYPE_OHGAN) == DONE)
            {
                m_creature->CastSpell(m_creature, SPELL_ENRAGE, true);
                DoScriptText(EMOTE_RAGE, m_creature);
                m_bRaptorDead = true;
            }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_mandokir(Creature* pCreature)
{
    return new boss_mandokirAI(pCreature);
}

/*######
## mob_ohgan
######*/

struct MANGOS_DLL_DECL mob_ohganAI : public ScriptedAI
{
    mob_ohganAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_zulgurub*)pCreature->GetInstanceData();
        Reset();
    }

    instance_zulgurub* m_pInstance;  

    uint32 m_uiSunderArmorTimer;

    void Reset()
    {
		m_creature->SetRespawnDelay(259200);
        m_uiSunderArmorTimer = 5000;
    }

    void JustReachedHome()
    {
        m_creature->ForcedDespawn();
        if (m_pInstance)
            m_pInstance->SetData(TYPE_OHGAN, NOT_STARTED);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_OHGAN, DONE);
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() == TYPEID_PLAYER && m_creature->isInCombat())
            if (Creature* pSpirit = GetClosestCreatureWithEntry(pVictim, NPC_CHAINED_SPIRIT, 50.0f))
                pSpirit->CastSpell(pVictim, SPELL_REVIVE, false);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Sunder Armor
        if (m_uiSunderArmorTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SUNDER_ARMOR);
            m_uiSunderArmorTimer = urand(10000, 15000);
        }
        else
            m_uiSunderArmorTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_ohgan(Creature* pCreature)
{
    return new mob_ohganAI(pCreature);
}

/*######
## mob_vilebranch_speaker
######*/

enum eVilebranchSpeaker
{
    SPELL_CLEAVE             = 15284,
    SPELL_DEMORALIZING_SHOUT = 13730,
    //SPELL_ENRAGE             = 8599
};

struct MANGOS_DLL_DECL mob_vilebranch_speakerAI : public ScriptedAI
{
    mob_vilebranch_speakerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_zulgurub*)pCreature->GetInstanceData();
        Reset();
    }

    instance_zulgurub* m_pInstance;

    uint32 m_uiCleaveTimer;
    bool m_bEnraged;

    void Reset()
    {
        m_uiCleaveTimer = 5000;
        m_bEnraged = false;
    }

    void Aggro(Unit* pVictim)
    {
        DoCastSpellIfCan(pVictim, SPELL_DEMORALIZING_SHOUT);
    }

    void JustDied(Unit* pKiller)
    {
        Creature* pMandokir = NULL;

        if (m_pInstance)
            pMandokir = m_pInstance->GetSingleCreatureFromStorage(NPC_MANDOKIR);

        if (!pMandokir)
            pMandokir = GetClosestCreatureWithEntry(m_creature, NPC_MANDOKIR, DEFAULT_VISIBILITY_INSTANCE);
        
        if (!pMandokir)
            debug_log("SD0: Zul'Gurub: Vilebranch Speaker (in front of boss Bloodlord Mandokir) can't select Bloodlord Mandokir for start event.");
        else
            pMandokir->AI()->AttackStart(pKiller);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_bEnraged && HealthBelowPct(30))
        {
            m_bEnraged = true;
            DoCastSpellIfCan(m_creature, SPELL_ENRAGE);
        }

        // Cleave
        if (m_uiCleaveTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE);
            m_uiCleaveTimer = 5000;
        }
        else
            m_uiCleaveTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_vilebranch_speaker(Creature* pCreature)
{
    return new mob_vilebranch_speakerAI(pCreature);
}

/*######
## AddSC
######*/

void AddSC_boss_mandokir()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_mandokir";
    pNewScript->GetAI = &GetAI_boss_mandokir;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_ohgan";
    pNewScript->GetAI = &GetAI_mob_ohgan;
    pNewScript->RegisterSelf();
    
    pNewScript = new Script;
    pNewScript->Name = "mob_chained_spirit";
    pNewScript->GetAI = &GetAI_mob_chained_spirit;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_vilebranch_speaker";
    pNewScript->GetAI = &GetAI_mob_vilebranch_speaker;
    pNewScript->RegisterSelf();
}
