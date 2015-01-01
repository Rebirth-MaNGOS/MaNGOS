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
SDName: Boss_Pyroguard_Emberseer
SD%Complete: 100
SDComment:
SDCategory: Blackrock Spire
EndScriptData */

#include "precompiled.h"
#include "blackrock_spire.h"

enum eEmberseer
{
    // Pyroguard
    SPELL_ENCAGED               = 15282,
    SPELL_FIRE_NOVA             = 16079,
    SPELL_FLAME_BUFFET          = 16536,
    SPELL_FULL_STRENGTH         = 16047,
    SPELL_PYROBLAST             = 17274,
    SAY_AGGRO                   = -1229000,

    // Blackhand Incarcerator
    SPELL_STRIKE                = 15580,
    SPELL_ENCAGE                = 15281,

    EVENT_SPELL_EMBERSEER_START = 4884,
};

struct MANGOS_DLL_DECL boss_pyroguard_emberseerAI : public ScriptedAI
{
    boss_pyroguard_emberseerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_blackrock_spire*)pCreature->GetInstanceData();
        Reset();
    }

    instance_blackrock_spire* m_pInstance;

    bool m_bGrowing;
    bool m_bGrowingStarted;
    uint32 m_uiFireNovaTimer;
    uint32 m_uiFlameBuffetTimer;
    uint32 m_uiPyroBlastTimer;
    uint32 m_uiGrowingTimer;

    void Reset()
    {
        m_bGrowing = false;
        m_bGrowingStarted = false;

        m_uiFireNovaTimer = urand(4000,6000);
        m_uiFlameBuffetTimer = 1000;
        m_uiGrowingTimer = 0;
        m_uiPyroBlastTimer = urand(10000,12000);

        m_creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_FIRE, true);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE + UNIT_FLAG_NOT_SELECTABLE);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_PYROGUARD_EMBERSEER, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_PYROGUARD_EMBERSEER, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_bGrowingStarted && m_creature->HasAura(SPELL_EMBERSEER_GROWING, EFFECT_INDEX_0))
        {
            m_uiGrowingTimer = 65000;
            m_bGrowing = true;
            m_bGrowingStarted = true;
        }

        // Growing phase
        if (m_bGrowing)
	{
            if (m_uiGrowingTimer <= uiDiff)
            {
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE + UNIT_FLAG_NOT_SELECTABLE);
                DoCastSpellIfCan(m_creature, SPELL_FULL_STRENGTH);
                m_creature->SetInCombatWithZone();
                m_bGrowing = false;
            }
            else
                m_uiGrowingTimer -= uiDiff;
	}

        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Fire Nova
        if (m_uiFireNovaTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FIRE_NOVA);
            m_uiFireNovaTimer = urand(3000,5000);
        }
        else
            m_uiFireNovaTimer -= uiDiff;

        // Flame Buffet
        if (m_uiFlameBuffetTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FLAME_BUFFET);
            m_uiFlameBuffetTimer = urand(9000,12000);
        }
        else
            m_uiFlameBuffetTimer -= uiDiff;

        // Pyroblast
        if (m_uiPyroBlastTimer <= uiDiff)
        {            
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_PYROBLAST);
            m_uiPyroBlastTimer = urand(10000,15000);
        }
        else
            m_uiPyroBlastTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_pyroguard_emberseer(Creature* pCreature)
{
    return new boss_pyroguard_emberseerAI(pCreature);
}

struct MANGOS_DLL_DECL mob_blackhand_incarceratorAI : public ScriptedAI
{
    mob_blackhand_incarceratorAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_blackrock_spire*)pCreature->GetInstanceData();
        Reset();
    }

    instance_blackrock_spire* m_pInstance;

    uint32 m_uiStrikeTimer;

    void Reset()
    {
        m_uiStrikeTimer = urand(3000,5000);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE + UNIT_FLAG_NOT_SELECTABLE);
    }

    void JustReachedHome()
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_PYROGUARD_EMBERSEER) != NOT_STARTED)
            m_pInstance->SetData(TYPE_PYROGUARD_EMBERSEER, NOT_STARTED);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (m_pInstance && !m_creature->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
                if (Creature* pEmberseer = m_pInstance->GetSingleCreatureFromStorage(NPC_PYROGUARD_EMBERSEER))
                    DoCastSpellIfCan(pEmberseer, SPELL_ENCAGE);

            return;
        }

        // Strike
        if (m_uiStrikeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_STRIKE);
            m_uiStrikeTimer = urand(4000,6000);
        }
        else
            m_uiStrikeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_blackhand_incarcerator(Creature* pCreature)
{
    return new mob_blackhand_incarceratorAI(pCreature);
}

bool ProcessEventId_event_spell_emberseer_start(uint32 uiEventId, Object* pSource, Object* /*pTarget*/, bool bIsStart)
{
    if (bIsStart && pSource->GetTypeId() == TYPEID_PLAYER && uiEventId == EVENT_SPELL_EMBERSEER_START)
    {
        instance_blackrock_spire* m_pInstance = (instance_blackrock_spire*)((Player*)pSource)->GetInstanceData();

        if (m_pInstance && m_pInstance->GetData(TYPE_PYROGUARD_EMBERSEER) == NOT_STARTED)
            m_pInstance->SetData(TYPE_PYROGUARD_EMBERSEER, IN_PROGRESS);
    }
    return true;
}

void AddSC_boss_pyroguard_emberseer()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_pyroguard_emberseer";
    pNewscript->GetAI = &GetAI_boss_pyroguard_emberseer;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_blackhand_incarcerator";
    pNewscript->GetAI = &GetAI_mob_blackhand_incarcerator;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "event_spell_emberseer_start";
    pNewscript->pProcessEventId = &ProcessEventId_event_spell_emberseer_start;
    pNewscript->RegisterSelf();
}
