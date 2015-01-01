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
SDName: Boss_Thekal
SD%Complete: 95
SDComment: Almost finished.
SDCategory: Zul'Gurub
EndScriptData */

#include "precompiled.h"
#include "zulgurub.h"

enum eThekal
{
    SAY_TRANSFORM             = -1309010,
    SAY_DEATH                 = -1309011,

    SPELL_CHARGE              = 24408,      // wowhead 24193 - not working?
    SPELL_FORCE_PUNCH         = 24189,
    SPELL_FRENZY              = 8269,
    SPELL_MORTAL_CLEAVE       = 22859,
    SPELL_RESURRECTION        = 24173,
    SPELL_SILENCE             = 22666,
    SPELL_SPEED_SLASH         = 24192,
    SPELL_SUMMON_TIGERS       = 24183,
    SPELL_THEKAL_TRANSFORM    = 24169,

    //Zealot Lor'Khan Spells
    SPELL_BLOODLUST           = 24185,
    SPELL_DISARM              = 6713,
    SPELL_DISPEL_MAGIC        = 17201,
    SPELL_GREAT_HEAL          = 24208,
    SPELL_LIGHTNING_SHIELD    = 20545,

    // Zealot Zath
    SPELL_BLIND               = 21060,
    SPELL_GOUGE               = 12540,
    SPELL_KICK                = 15614,
    SPELL_SINISTER_STRIKE     = 15581,
};

struct MANGOS_DLL_DECL boss_thekalAI : public ScriptedAI
{
    boss_thekalAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_zulgurub*)pCreature->GetInstanceData();
        Reset();
    }

    instance_zulgurub* m_pInstance;

    bool m_bFakingDeath;
    bool m_bPhaseTwo;

    uint32 m_uiChargeTimer;
    uint32 m_uiForcePunchTimer;
    uint32 m_uiFrenzyTimer;
    uint32 m_uiMortalCleaveTimer;
    uint32 m_uiSilenceTimer;
    uint32 m_uiSpeedSlashTimer;
    uint32 m_uiSummonTigersTimer;

    void Reset()
    {
        m_bFakingDeath = false;
        m_bPhaseTwo = false;

        m_uiChargeTimer = 15000;
        m_uiForcePunchTimer = 8000;
        m_uiFrenzyTimer = 30000;
        m_uiMortalCleaveTimer = 4000;
        m_uiSilenceTimer = 8000;
        m_uiSpeedSlashTimer = 4000;
        m_uiSummonTigersTimer = 20000;

        // Set proper stats
        m_creature->clearUnitState(UNIT_STAT_CAN_NOT_MOVE);
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.0f);
    }

    void JustReachedHome()
    {
        // Respawn Zulian Tigers in cage
        std::list<Creature*> m_lTigers;
        GetCreatureListWithEntryInGrid(m_lTigers, m_creature, NPC_ZULIAN_TIGER, 40.0f);

        if (!m_lTigers.empty())
            for(std::list<Creature*>::iterator itr = m_lTigers.begin(); itr != m_lTigers.end(); ++itr)
                if ((*itr) && !(*itr)->isAlive())
                    (*itr)->Respawn();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_THEKAL, FAIL);
    }

    void Aggro(Unit* /*pWho*/)
    {
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32 &uiDamage)
    {
        if (m_bPhaseTwo)
            return;

        if (uiDamage >= m_creature->GetHealth() || HealthBelowPct(5))
        {
            uiDamage = 0;

            if (!m_bFakingDeath && m_pInstance)
            {
                m_bFakingDeath = true;
                m_creature->RemoveAllAuras();
                m_creature->AttackStop();
                m_creature->StopMoving();
                m_creature->addUnitState(UNIT_STAT_CAN_NOT_MOVE);
                m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

                m_creature->SetHealth(m_creature->GetMaxHealth());
                m_pInstance->SetData(TYPE_THEKAL, IN_PROGRESS);
            }
        }
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_THEKAL) == SPECIAL)
        {
            m_creature->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.0f);
            DoScriptText(SAY_DEATH, m_creature);
            m_pInstance->SetData(TYPE_THEKAL, DONE);
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
        if (!m_creature->getVictim() && !pTarget)
            return;

        pSummoned->AI()->AttackStart(pTarget ? pTarget : m_creature->getVictim());
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Start Phase Two
        if (m_bFakingDeath)
        {
            if (m_pInstance && m_pInstance->GetData(TYPE_THEKAL) == SPECIAL)
            {
                m_bFakingDeath = false;
                m_bPhaseTwo = true;

                m_creature->clearUnitState(UNIT_STAT_CAN_NOT_MOVE);
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                DoCastSpellIfCan(m_creature, SPELL_THEKAL_TRANSFORM, CAST_TRIGGERED + CAST_FORCE_TARGET_SELF);

                m_creature->SetHealth(m_creature->GetMaxHealth());
                m_creature->SetFloatValue(OBJECT_FIELD_SCALE_X, 3.0f);
                const CreatureInfo* cinfo = m_creature->GetCreatureInfo();
                m_creature->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, (cinfo->mindmg +((cinfo->mindmg/100) * 40)));
                m_creature->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, (cinfo->maxdmg +((cinfo->maxdmg/100) * 40)));
                m_creature->UpdateDamagePhysical(BASE_ATTACK);

                DoResetThreat();
                DoScriptText(SAY_TRANSFORM, m_creature);

                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    m_creature->AI()->AttackStart(pTarget);
                else
                    m_creature->SetInCombatWithZone();
            }
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_bPhaseTwo)
        {
            // Mortal Cleave
            if (m_uiMortalCleaveTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_MORTAL_CLEAVE);
                m_uiMortalCleaveTimer = urand(10000, 15000);
            }
            else
                m_uiMortalCleaveTimer -= uiDiff;

            // Silence
            if (m_uiSilenceTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_SILENCE);
                m_uiSilenceTimer = urand(15000, 20000);
            }
            else
                m_uiSilenceTimer -= uiDiff;
        }
        else
        {
            // Charge
            if (m_uiChargeTimer <= uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                    DoCastSpellIfCan(pTarget, SPELL_CHARGE);

                m_uiChargeTimer = urand(15000, 22000);
            }
            else
                m_uiChargeTimer -= uiDiff;

            // Force Puch
            if (m_uiForcePunchTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FORCE_PUNCH) == CAST_OK)
                {
                    Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);                    
                    DoResetThreat();
                    AttackStart(pTarget ? pTarget : m_creature->getVictim());
                }
                m_uiForcePunchTimer = urand(12000, 16000);
            }
            else
                m_uiForcePunchTimer -= uiDiff;

            // Frenzy
            if (m_uiFrenzyTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_FRENZY);
                m_uiFrenzyTimer = urand(25000, 35000);
            }
            else
                m_uiFrenzyTimer -= uiDiff;

            // Speed Slash
            if (m_uiSpeedSlashTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_SPEED_SLASH);
                m_uiSpeedSlashTimer = urand(10000, 15000);
            }
            else
                m_uiSpeedSlashTimer -= uiDiff;

            // Summon Tigers
            if (m_uiSummonTigersTimer <= uiDiff)
            {
                for(uint8 i = 0; i < 2; ++i)
                    DoSpawnCreature(NPC_ZULIAN_GUARDIAN, irand(-10,10), irand(-10,10), 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);

                m_uiSummonTigersTimer = 20000;
            }
            else
                m_uiSummonTigersTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_thekal(Creature* pCreature)
{
    return new boss_thekalAI(pCreature);
}

struct MANGOS_DLL_DECL mob_zealot_lorkhanAI : public ScriptedAI
{
    mob_zealot_lorkhanAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_zulgurub*)pCreature->GetInstanceData();
        Reset();
    }

    instance_zulgurub* m_pInstance;

    uint32 m_uiBloodlustTimer;
    uint32 m_uiDisarmTimer;
    uint32 m_uiDispelMagicTimer;
    uint32 m_uiGreatHealTimer;
    uint32 m_uiLightningShieldTimer;

    void Reset()
    {
        m_uiBloodlustTimer = 4000;
        m_uiDisarmTimer = 6000;
        m_uiDispelMagicTimer = 16000;
        m_uiGreatHealTimer = 32000;
        m_uiLightningShieldTimer = 1000;
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LORKHAN, FAIL);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LORKHAN, IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Disarm
        if (m_uiDisarmTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_DISARM);
            m_uiDisarmTimer = urand(15000, 25000);
        }
        else
            m_uiDisarmTimer -= uiDiff;

        // Bloodlust
        if (m_uiBloodlustTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BLOODLUST);
            m_uiBloodlustTimer = urand(25000, 45000);
        }
        else
            m_uiBloodlustTimer -= uiDiff;

        // Dispel Magic
        if (m_uiDispelMagicTimer <= uiDiff)
        {
            if (m_pInstance)
            {
                Creature* pTarget = m_pInstance->GetSingleCreatureFromStorage(urand(0,1) ? NPC_THEKAL : NPC_ZATH);
                if (pTarget && m_creature->IsWithinDistInMap(pTarget, 15.0f))
                    DoCastSpellIfCan(pTarget, SPELL_DISPEL_MAGIC);
            }
            m_uiDispelMagicTimer = urand(15000, 25000);
        }
        else
            m_uiDispelMagicTimer -= uiDiff;

        // Great heal
        if (m_uiGreatHealTimer <= uiDiff)
        {
            if (m_pInstance)
            {
                Creature* pTarget = m_pInstance->GetSingleCreatureFromStorage(urand(0,1) ? NPC_THEKAL : NPC_ZATH);
                if (pTarget && m_creature->IsWithinDistInMap(pTarget, 15.0f))
                    DoCastSpellIfCan(pTarget, SPELL_GREAT_HEAL);
            }
            m_uiGreatHealTimer = urand(15000, 20000);
        }
        else
            m_uiGreatHealTimer -= uiDiff;

        // Lightning Shield
        if (m_uiLightningShieldTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_LIGHTNING_SHIELD);
            m_uiLightningShieldTimer = 61000;
        }
        else
            m_uiLightningShieldTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_zealot_lorkhan(Creature* pCreature)
{
    return new mob_zealot_lorkhanAI(pCreature);
}

struct MANGOS_DLL_DECL mob_zealot_zathAI : public ScriptedAI
{
    mob_zealot_zathAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_zulgurub*)pCreature->GetInstanceData();
        Reset();
    }

    instance_zulgurub* m_pInstance;

    uint32 m_uiBlindTimer;
    uint32 m_uiGougeTimer;
    uint32 m_uiKickTimer;
    uint32 m_uiSinisterStrikeTimer;

    void Reset()
    {
        m_uiBlindTimer = 5000;
        m_uiGougeTimer = 25000;
        m_uiKickTimer = 18000;
        m_uiSinisterStrikeTimer = 8000;
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ZATH, FAIL);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ZATH, IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Blind
        if (m_uiBlindTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BLIND);
            m_uiBlindTimer = urand(10000, 20000);
        }
        else
            m_uiBlindTimer -= uiDiff;

        // Gouge
        if (m_uiGougeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_GOUGE);

            if (m_creature->getThreatManager().getThreat(m_creature->getVictim()))
                m_creature->getThreatManager().modifyThreatPercent(m_creature->getVictim(),-100);

            m_uiGougeTimer = urand(17000, 27000);
        }
        else
            m_uiGougeTimer -= uiDiff;

        // Kick
        if (m_uiKickTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_KICK);
            m_uiKickTimer = urand(15000, 25000);
        }
        else
            m_uiKickTimer -= uiDiff;

        // Sinister Strike
        if (m_uiSinisterStrikeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SINISTER_STRIKE);
            m_uiSinisterStrikeTimer = urand(8000, 16000);
        }
        else
            m_uiSinisterStrikeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_zealot_zath(Creature* pCreature)
{
    return new mob_zealot_zathAI(pCreature);
}

void AddSC_boss_thekal()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_thekal";
    pNewScript->GetAI = &GetAI_boss_thekal;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_zealot_lorkhan";
    pNewScript->GetAI = &GetAI_mob_zealot_lorkhan;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_zealot_zath";
    pNewScript->GetAI = &GetAI_mob_zealot_zath;
    pNewScript->RegisterSelf();
}
