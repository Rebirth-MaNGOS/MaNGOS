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
SDName: Hillsbrad_Foothills
SD%Complete: 100
SDComment: Quest support: 0
SDCategory: Hillsbrad Foothils
EndScriptData */

/* ContentData
go_open_to_pass_your_rite
EndContentData */

#include "precompiled.h"

/*######
## go_open_to_pass_your_rite
######*/

enum
{
    NPC_MILTON_BEATS     = 13082,
    SPELL_CONJURE_MILTON = 21078,
};

bool GOUse_go_open_to_pass_your_rite(Player* pPlayer, GameObject* /*pGo*/)
{
    // Summon Milton Beats (randomly to 3 yards)
    pPlayer->CastSpell(pPlayer, SPELL_CONJURE_MILTON, true);

    return false;
}

/*######
## npc_stanley
######*/

enum
{
	QUEST_ID_ELIXIR_OF_PAIN = 502,
	SPELL_ENRAGE			= 3258,
	NPC_ENRAGED_STANLEY		= 2275,
};

struct MANGOS_DLL_DECL npc_stanleyAI : public ScriptedAI
{
    npc_stanleyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
		m_uiDefaultEntry    = pCreature->GetEntry();
        Reset();
    }

	uint32 m_uiDefaultEntry;

    void Reset()
    {
		m_creature->UpdateEntry(m_uiDefaultEntry);
    }

	void transform()
	{
		m_creature->UpdateEntry(NPC_ENRAGED_STANLEY);
	}

	void UpdateAI(const uint32 uiDiff)
    {
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (HealthBelowPct(33))
        {
			m_creature->CastSpell(m_creature, SPELL_ENRAGE, true);
		}

        DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_stanley(Creature* pCreature)
{
    return new npc_stanleyAI(pCreature);
}

bool OnQuestRewarded_npc_stanley(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
	if (pQuest->GetQuestId() == QUEST_ID_ELIXIR_OF_PAIN)
    {
		if (npc_stanleyAI* pEscortAI = dynamic_cast<npc_stanleyAI*>(pCreature->AI()))
		{
			pEscortAI->transform();
			pEscortAI->AttackStart(pPlayer);
		}
	}
	return true;
}

/*######
## AddSC
######*/

void AddSC_hillsbrad_foothills()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "go_open_to_pass_your_rite";
    pNewscript->pGOUse = &GOUse_go_open_to_pass_your_rite;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_stanley";
    pNewscript->GetAI = &GetAI_npc_stanley;
    pNewscript->pQuestRewardedNPC = &OnQuestRewarded_npc_stanley;
    pNewscript->RegisterSelf();
}
