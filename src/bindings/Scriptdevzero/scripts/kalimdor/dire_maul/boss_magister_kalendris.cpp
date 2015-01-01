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
SDName: Boss Magister Kalendris
SD%Complete:
SDComment:
SDCategory: Dire Maul
EndScriptData */

#include "precompiled.h"
#include "dire_maul.h"

enum Spells
{
    SPELL_DOMINATE_MIND     = 7645,
    SPELL_MIND_BLAST        = 17287,
    SPELL_MIND_FLAY         = 22919,
    SPELL_SHADOW_WORD_PAIN  = 17146,
    SPELL_SHADOWFORM        = 22917,
};

struct MANGOS_DLL_DECL boss_magister_kalendrisAI : public ScriptedAI
{
    boss_magister_kalendrisAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiDominateMindTimer;
    uint32 m_uiMindBlastTimer;
    uint32 m_uiMindFlayTimer;
    uint32 m_uiShadowWordPainTimer;

    void Reset()
    {
        m_uiDominateMindTimer = urand(10000,12000);
        m_uiMindBlastTimer = urand(1000,3000);
        m_uiMindFlayTimer = urand(6000,8000);
        m_uiShadowWordPainTimer = urand(4000,5000);
        DoCastSpellIfCan(m_creature, SPELL_SHADOWFORM);
    }

    void Aggro(Unit* /*pWho*/)
    {
    }

    void UpdateAI(const uint32 uiDiff)
    {       
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Dominate Mind
        if (m_uiDominateMindTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_DOMINATE_MIND);
            m_uiDominateMindTimer = urand(25000,35000);
        }
        else
            m_uiDominateMindTimer -= uiDiff;

        // Mind Blast
        if (m_uiMindBlastTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MIND_BLAST);
            m_uiMindBlastTimer = urand(4000,7000);
        }
        else
            m_uiMindBlastTimer -= uiDiff;

        // Mind Flay
        if (m_uiMindFlayTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_MIND_FLAY);
            m_uiMindFlayTimer = urand(15000,25000);
        }
        else
            m_uiMindFlayTimer -= uiDiff;

        // Shadow Word: Pain
        if (m_uiShadowWordPainTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_SHADOW_WORD_PAIN);
            m_uiShadowWordPainTimer = urand(5000,7000);
        }
        else
            m_uiShadowWordPainTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_magister_kalendris(Creature* pCreature)
{
    return new boss_magister_kalendrisAI(pCreature);
}

void AddSC_boss_magister_kalendris()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_magister_kalendris";
    pNewscript->GetAI = &GetAI_boss_magister_kalendris;
    pNewscript->RegisterSelf();
}
