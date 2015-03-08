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
SDName: Southshore
SD%Complete:
SDComment: Quest support: 0.
SDCategory: Southshore
EndScriptData */

/* ContentData
EndContentData */

#include "precompiled.h"

/*######
## npc_southshore_crier
######*/

struct MANGOS_DLL_DECL npc_southshore_crierAI : public ScriptedAI
{
    npc_southshore_crierAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
		m_creature->SetActiveObjectState(true);
        Reset();
    }

    void Reset()
    {
    }

	void UpdateAI(const uint32 uiDiff)
    {
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_southshore_crier(Creature* pCreature)
{
    return new npc_southshore_crierAI(pCreature);
}

void AddSC_southshore()
{
    Script* pNewscript;

	pNewscript = new Script;
	pNewscript->Name = "npc_southshore_crier";
	pNewscript->GetAI = &GetAI_npc_southshore_crier;
	pNewscript->RegisterSelf();
}
