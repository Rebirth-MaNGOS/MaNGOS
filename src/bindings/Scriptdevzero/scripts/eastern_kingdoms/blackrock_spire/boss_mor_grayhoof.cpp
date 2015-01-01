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
SDName: Boss Mor Grayhoof
SD%Complete:
SDComment:
SDCategory: Blackrock Spire
EndScriptData */

#include "precompiled.h"
#include "blackrock_spire.h"

enum Phases
{
    PHASE_DRUID                   = 0,
    PHASE_BEAR                    = 1,
    PHASE_CAT                     = 2,
    PHASE_DRAGON                  = 3,
};

enum Spells
{
    SPELL_ARCANE_EXPLOSION        = 22271,
    SPELL_CHAIN_LIGHTNING         = 27567,
    SPELL_HEALING_TOUCH           = 27527,
    SPELL_MOONFIRE                = 27737,
    SPELL_REFLECTION              = 27564,
    SPELL_REJUVENATON             = 27532,
    SPELL_SHOCK                   = 15605,
    SPELL_BEAR_FORM               = 27543,
    SPELL_DEMORALIZING_ROAR       = 27551,
    SPELL_MAUL                    = 27553,
    SPELL_SWIPE                   = 27554,
    SPELL_CAT_FORM                = 27545,
    SPELL_FEROCIOUS_BITE          = 27557,
    SPELL_RAKE                    = 27556,
    SPELL_SHRED                   = 27555,
    SPELL_FAERIE_DRAGON_FORM      = 27546,
};

struct MANGOS_DLL_DECL boss_mor_grayhoofAI : public ScriptedAI
{
    boss_mor_grayhoofAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_blackrock_spire*)pCreature->GetInstanceData();
        Reset();
    }

    instance_blackrock_spire* m_pInstance;

    uint32 m_uiPhase;
    uint32 m_uiCurrentPhase;
    uint32 m_uiPhaseTimer;
    uint32 m_uiSpell1Entry;
    uint32 m_uiSpell2Entry;
    uint32 m_uiSpell3Entry;
    uint32 m_uiSpell1Timer;
    uint32 m_uiSpell2Timer;
    uint32 m_uiSpell3Timer;
    uint32 m_uiRejuvenationTimer;

    void Reset()
    {
        m_creature->RemoveAurasDueToSpell(SPELL_BEAR_FORM);
        m_creature->RemoveAurasDueToSpell(SPELL_CAT_FORM);
        m_creature->RemoveAurasDueToSpell(SPELL_FAERIE_DRAGON_FORM);

        m_uiPhase = 0;
        m_uiPhaseTimer = 30000;
        m_uiCurrentPhase = 0;
        m_uiSpell1Entry = SPELL_MOONFIRE;
        m_uiSpell2Entry = SPELL_HEALING_TOUCH;
        m_uiSpell3Entry = SPELL_ARCANE_EXPLOSION;
        m_uiSpell1Timer = urand(4000,6000);
        m_uiSpell2Timer = urand(8000,10000);
        m_uiSpell3Timer = urand(15000,20000);
        m_uiRejuvenationTimer = urand(25000,30000);
    }

    void JustReachedHome()
    {
        m_creature->RemoveFromWorld();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MOR_GRAYHOOF, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MOR_GRAYHOOF, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MOR_GRAYHOOF, DONE);
    }

    void ChangePhase()
    {
        m_creature->InterruptNonMeleeSpells(false);
        m_uiCurrentPhase = m_uiPhase;

        while(m_uiCurrentPhase == m_uiPhase)
        {
            switch(urand(0,3))
            {
                case PHASE_DRUID:
                    m_uiPhase = PHASE_DRUID;
                    m_creature->RemoveAurasDueToSpell(SPELL_BEAR_FORM);
                    m_creature->RemoveAurasDueToSpell(SPELL_CAT_FORM);
                    m_creature->RemoveAurasDueToSpell(SPELL_FAERIE_DRAGON_FORM);
                    m_uiSpell1Entry = SPELL_MOONFIRE;
                    m_uiSpell2Entry = SPELL_HEALING_TOUCH;
                    m_uiSpell3Entry = SPELL_ARCANE_EXPLOSION;
                    break;
                case PHASE_BEAR:
                    m_uiPhase = PHASE_BEAR;
                    DoCastSpellIfCan(m_creature, SPELL_BEAR_FORM, CAST_INTERRUPT_PREVIOUS);
                    m_uiSpell1Entry = SPELL_MAUL;
                    m_uiSpell2Entry = SPELL_SWIPE;
                    m_uiSpell3Entry = SPELL_DEMORALIZING_ROAR;
                    break;
                case PHASE_CAT:
                    m_uiPhase = PHASE_CAT;
                    DoCastSpellIfCan(m_creature, SPELL_CAT_FORM, CAST_INTERRUPT_PREVIOUS);
                    m_uiSpell1Entry = SPELL_RAKE;
                    m_uiSpell2Entry = SPELL_SHRED;
                    m_uiSpell3Entry = SPELL_FEROCIOUS_BITE;
                    break;
                case PHASE_DRAGON:
                    m_uiPhase = PHASE_DRAGON;
                    DoCastSpellIfCan(m_creature, SPELL_FAERIE_DRAGON_FORM, CAST_INTERRUPT_PREVIOUS);
                    m_uiSpell1Entry = SPELL_SHOCK;
                    m_uiSpell2Entry = SPELL_CHAIN_LIGHTNING;
                    m_uiSpell3Entry = SPELL_REFLECTION;
                    break;
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {   
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Change Phase
        if (m_uiPhaseTimer < uiDiff)
        {
            ChangePhase();
            m_uiPhaseTimer = urand(40000,50000);
        }
        else
            m_uiPhaseTimer -= uiDiff;

        if (m_uiSpell1Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), m_uiSpell1Entry);
            m_uiSpell1Timer = urand(6000,8000);
        }
        else
            m_uiSpell1Timer -= uiDiff;

        if (m_uiSpell2Timer < uiDiff)
        {
            DoCastSpellIfCan(m_uiSpell2Entry == SPELL_HEALING_TOUCH ? m_creature : m_creature->getVictim(), m_uiSpell2Entry);
            m_uiSpell2Timer = urand(10000,12000);
        }
        else
            m_uiSpell2Timer -= uiDiff;

        if (m_uiSpell3Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), m_uiSpell3Entry);
            m_uiSpell3Timer = urand(15000,20000);
        }
        else
            m_uiSpell3Timer -= uiDiff;

        if (m_uiRejuvenationTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_REJUVENATON);
            m_uiRejuvenationTimer = urand(20000,25000);
        }
        else
            m_uiRejuvenationTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_mor_grayhoof(Creature* pCreature)
{
    return new boss_mor_grayhoofAI (pCreature);
}

void AddSC_boss_mor_grayhoof()
{
    Script* pNewscript;
    pNewscript = new Script;
    pNewscript->Name = "boss_mor_grayhoof";
    pNewscript->GetAI = &GetAI_boss_mor_grayhoof;
    pNewscript->RegisterSelf();
}
