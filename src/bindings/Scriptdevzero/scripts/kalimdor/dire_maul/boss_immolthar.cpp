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
SDName: Boss Immolthar
SD%Complete:
SDComment:
SDCategory: Dire Maul
EndScriptData */

#include "precompiled.h"
#include "dire_maul.h"

enum Spells
{
    SPELL_FRENZY                    = 8269,
    SPELL_INFECTED_BITE             = 16128,
    SPELL_TRAMPLE                   = 5568,
    SPELL_PORTAL_OF_IMMOLTHAR       = 22950,
    SPELL_EYE_OF_IMMOLTHAR          = 22909,
};

struct MANGOS_DLL_DECL boss_immoltharAI : public ScriptedAI
{
    boss_immoltharAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_dire_maul*)pCreature->GetInstanceData();
        Reset();
    }

    instance_dire_maul* m_pInstance;

    bool m_bIsEnraged;
    uint32 m_uiInfectedBiteTimer;
    uint32 m_uiTrampleTimer;
    uint32 m_uiPortalOfImmortalTimer;
    uint32 m_uiSummonEyeOfImmortalTimer;

    void Reset()
    {
        m_bIsEnraged = false;
        m_uiInfectedBiteTimer = urand(5000,7000);
        m_uiTrampleTimer = urand(3000,5000);
        m_uiPortalOfImmortalTimer = urand(10000,12000);
        m_uiSummonEyeOfImmortalTimer = urand(5000,6000);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_IMMOLTHAR, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_IMMOLTHAR, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_IMMOLTHAR, DONE); 
    }

    void JustSummoned(Creature* pSummoned)
    {
        Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1);
        if (!m_creature->getVictim() && !pTarget)
            return;

        pSummoned->AI()->AttackStart(pTarget ? pTarget : m_creature->getVictim());
    }

    void UpdateAI(const uint32 uiDiff)
    {            
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Frenzy
        if (!m_bIsEnraged && HealthBelowPct(40))
        {
            DoCastSpellIfCan(m_creature, SPELL_FRENZY);
            m_bIsEnraged = true;
        }

        // Infected Bite
        if (m_uiInfectedBiteTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_INFECTED_BITE);
            m_uiInfectedBiteTimer = urand(6000,8000);
        }
        else
            m_uiInfectedBiteTimer -= uiDiff;

        // Trample
        if (m_uiTrampleTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_TRAMPLE);
            m_uiTrampleTimer = urand(7000,9000);
        }
        else
            m_uiTrampleTimer -= uiDiff;

        // Portal of Immorlhar
        if (m_uiPortalOfImmortalTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_PORTAL_OF_IMMOLTHAR);
            m_uiPortalOfImmortalTimer = urand(12000,15000);
        }
        else
            m_uiPortalOfImmortalTimer -= uiDiff;

        // Summon Eye of Immolthar
        if (m_uiSummonEyeOfImmortalTimer <= uiDiff)
        {
            DoSpawnCreature(NPC_EYE_OF_IMMOLTHAR, irand(-2,2), irand(-2,2), 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
            m_uiSummonEyeOfImmortalTimer = urand(8000,10000);
        }
        else
            m_uiSummonEyeOfImmortalTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_immolthar(Creature* pCreature)
{
    return new boss_immoltharAI(pCreature);
}

struct MANGOS_DLL_DECL mob_eye_of_immoltharAI : public ScriptedAI
{
    mob_eye_of_immoltharAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiEyeOfImmoltharTimer;

    void Reset()
    {
        m_uiEyeOfImmoltharTimer = urand(5000,7000);
    }

    void Aggro(Unit* /*pWho*/)
    {
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Eye of Immolthar
        if (m_uiEyeOfImmoltharTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_EYE_OF_IMMOLTHAR);
            m_uiEyeOfImmoltharTimer = urand(6000,8000);
        }
        else
            m_uiEyeOfImmoltharTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_eye_of_immolthar(Creature* pCreature)
{
    return new mob_eye_of_immoltharAI(pCreature);
}

void AddSC_boss_immolthar()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_immolthar";
    pNewscript->GetAI = &GetAI_boss_immolthar;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_eye_of_immolthar";
    pNewscript->GetAI = &GetAI_mob_eye_of_immolthar;
    pNewscript->RegisterSelf();
}
