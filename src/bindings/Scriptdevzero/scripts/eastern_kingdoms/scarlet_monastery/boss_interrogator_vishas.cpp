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
SDName: Boss_Interrogator_Vishas
SD%Complete: 100
SDComment:
SDCategory: Scarlet Monastery
EndScriptData */

#include "precompiled.h"
#include "scarlet_monastery.h"

enum
{
    SAY_AGGRO               = -1189011,
    SAY_HEALTH1             = -1189012,
    SAY_HEALTH2             = -1189013,
    SAY_KILL                = -1189014,
    SAY_TRIGGER_VORREL      = -1189015,

    SPELL_SHADOW_WORD_PAIN  = 2767,
	SPELL_IMMOLATE_MELEE	= 18542,
};

struct MANGOS_DLL_DECL boss_interrogator_vishasAI : public ScriptedAI
{
    boss_interrogator_vishasAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_scarlet_monastery*)pCreature->GetInstanceData();
        Reset();
    }

    instance_scarlet_monastery* m_pInstance;

    bool bYell25;
    bool bYell75;
    uint32 m_uiShadowWordPainTimer;

    void Reset()
    {
        bYell25 = false;
        bYell75 = false;
        m_uiShadowWordPainTimer = 5000;
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(SAY_AGGRO, m_creature);
		DoCastSpellIfCan(m_creature, SPELL_IMMOLATE_MELEE, CAST_TRIGGERED);			// passive aura to procc Flame Lash on melee
    }

    void KilledUnit(Unit* /*pVictim*/)
    {
        DoScriptText(SAY_KILL, m_creature);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (!m_pInstance)
            return;

        // Any other actions to do with vorrel? setStandState?
		if (Creature* pVorrel = m_pInstance->GetSingleCreatureFromStorage(NPC_VORREL))
            DoScriptText(SAY_TRIGGER_VORREL, pVorrel);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // If we are low on hp Do sayings
        if (!bYell75 && HealthBelowPct(75))
        {
            DoScriptText(SAY_HEALTH1, m_creature);
            bYell75 = true;
        }

        if (!bYell25 && HealthBelowPct(25))
        {
            DoScriptText(SAY_HEALTH2, m_creature);
            bYell25 = true;
        }

        // Shadow word: Pain
        if (m_uiShadowWordPainTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_WORD_PAIN);
            m_uiShadowWordPainTimer = urand(5000, 15000);
        }
        else 
            m_uiShadowWordPainTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_interrogator_vishas(Creature* pCreature)
{
    return new boss_interrogator_vishasAI(pCreature);
}

void AddSC_boss_interrogator_vishas()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "boss_interrogator_vishas";
    pNewScript->GetAI = &GetAI_boss_interrogator_vishas;
    pNewScript->RegisterSelf();
}
