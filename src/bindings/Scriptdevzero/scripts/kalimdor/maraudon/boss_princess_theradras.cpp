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
SDName: Boss_Princess_Theradras
SD%Complete: 100
SDComment:
SDCategory: Maraudon
EndScriptData */

#include "precompiled.h"

enum eSpells
{
	SPELL_DUSTFIELD				= 21909,
	SPELL_BOULDER				= 21832,
	SPELL_THRASH				= 3391,
	SPELL_REPULSIVEGAZE			= 21869
};

struct MANGOS_DLL_DECL boss_ptheradrasAI : public ScriptedAI
{
    boss_ptheradrasAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 DustfieldTimer;
    uint32 BoulderTimer;
    uint32 ThrashTimer;
    uint32 RepulsiveGazeTimer;

    void Reset()
    {
        DustfieldTimer = 8000;
        BoulderTimer = 2000;
        ThrashTimer = 5000;
        RepulsiveGazeTimer = 23000;
    }

    void JustDied(Unit* /*pKiller*/)
    {
        m_creature->SummonCreature(12238,28.067f,61.875f,-123.405f,4.67f,TEMPSUMMON_TIMED_DESPAWN,600000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // DustfieldTimer
        if (DustfieldTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_DUSTFIELD);
            DustfieldTimer = 14000;
        }
		else
			DustfieldTimer -= uiDiff;

        // BoulderTimer
        if (BoulderTimer < uiDiff)
        {
            Unit* target = NULL;
            target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
            if (target)
                DoCastSpellIfCan(target, SPELL_BOULDER);
            BoulderTimer = 10000;
        }
		else
			BoulderTimer -= uiDiff;

        // RepulsiveGazeTimer
        if (RepulsiveGazeTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_REPULSIVEGAZE);
            RepulsiveGazeTimer = 20000;
        }
		else
			RepulsiveGazeTimer -= uiDiff;

        // ThrashTimer
        if (ThrashTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_THRASH);
            ThrashTimer = 18000;
        }
		else
			ThrashTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_ptheradras(Creature* pCreature)
{
    return new boss_ptheradrasAI(pCreature);
}

void AddSC_boss_ptheradras()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_princess_theradras";
    pNewscript->GetAI = &GetAI_boss_ptheradras;
    pNewscript->RegisterSelf();
}
