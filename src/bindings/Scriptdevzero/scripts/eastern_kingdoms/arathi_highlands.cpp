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
SDName: Arathi Highlands
SD%Complete: 100
SDComment: Quest support: 660, 665
SDCategory: Arathi Highlands
EndScriptData */

/* ContentData
npc_professor_phizzlethorpe
npc_kinelory
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"

/*######
## npc_professor_phizzlethorpe
######*/

enum
{
    SAY_PROGRESS_1          = -1000264,
    SAY_PROGRESS_2          = -1000265,
    SAY_PROGRESS_3          = -1000266,
    EMOTE_PROGRESS_4        = -1000267,
    SAY_AGGRO               = -1000268,
    SAY_PROGRESS_5          = -1000269,
    SAY_PROGRESS_6          = -1000270,
    SAY_PROGRESS_7          = -1000271,
    EMOTE_PROGRESS_8        = -1000272,
    SAY_PROGRESS_9          = -1000273,

    QUEST_SUNKEN_TREASURE   = 665,
    ENTRY_VENGEFUL_SURGE    = 2776
};

struct MANGOS_DLL_DECL npc_professor_phizzlethorpeAI : public npc_escortAI
{
    npc_professor_phizzlethorpeAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    void Reset() { }

    void WaypointReached(uint32 uiPointId)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch(uiPointId)
        {
            case 4: DoScriptText(SAY_PROGRESS_2, m_creature, pPlayer); break;
            case 5: DoScriptText(SAY_PROGRESS_3, m_creature, pPlayer); break;
            case 8: DoScriptText(EMOTE_PROGRESS_4, m_creature); break;
            case 9:
                m_creature->SummonCreature(ENTRY_VENGEFUL_SURGE, -2056.41f, -2144.01f, 20.59f, 5.70f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 600000);
                m_creature->SummonCreature(ENTRY_VENGEFUL_SURGE, -2050.17f, -2140.02f, 19.54f, 5.17f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 600000);
                break;
            case 10: DoScriptText(SAY_PROGRESS_5, m_creature, pPlayer); break;
            case 11:
                DoScriptText(SAY_PROGRESS_6, m_creature, pPlayer);
                SetRun();
                break;
            case 19: DoScriptText(SAY_PROGRESS_7, m_creature, pPlayer); break;
            case 20:
                DoScriptText(EMOTE_PROGRESS_8, m_creature);
                DoScriptText(SAY_PROGRESS_9, m_creature, pPlayer);
                pPlayer->GroupEventHappens(QUEST_SUNKEN_TREASURE, m_creature);
                break;
        }
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->AI()->AttackStart(m_creature);
    }
};

bool QuestAccept_npc_professor_phizzlethorpe(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_SUNKEN_TREASURE)
    {
        pCreature->setFaction(FACTION_ESCORT_N_NEUTRAL_PASSIVE);
        DoScriptText(SAY_PROGRESS_1, pCreature, pPlayer);

        if (npc_professor_phizzlethorpeAI* pEscortAI = dynamic_cast<npc_professor_phizzlethorpeAI*>(pCreature->AI()))
            pEscortAI->Start(false, pPlayer, pQuest, true);
    }
    return true;
}

CreatureAI* GetAI_npc_professor_phizzlethorpe(Creature* pCreature)
{
    return new npc_professor_phizzlethorpeAI(pCreature);
}

/*######
## npc_kinelory
######*/

enum uKinelory
{
	SPELL_KINELORY_BEAR_FORM          = 4948,
	SPELL_REJUVENATION                = 3627,
	
	SAY_KINELORY                      = -1000548,

	QUEST_HINTS_OF_A_NEW_PLAGUE_PART4 = 660,
	NPC_FORSAKEN_COURIER              = 2714,
	NPC_FORSAKEN_BODYGUARD            = 2721
};

struct MANGOS_DLL_DECL npc_kineloryAI : public npc_escortAI
{
	npc_kineloryAI(Creature* pCreature) : npc_escortAI(pCreature) {Reset();}

	uint32 m_uiKineloryBearForm_Timer;

	void Reset()
	{
		if (HasEscortState(STATE_ESCORT_ESCORTING))
			return;
		m_uiKineloryBearForm_Timer = urand(1000, 3000);
	}

	void Aggro(Unit *)
	{
		if (!urand(0,2))
			DoScriptText(SAY_KINELORY, m_creature);
	}

	void JustDied(Unit *)
	{
		if (Player* pPlayer = GetPlayerForEscort())
			pPlayer->FailQuest(QUEST_HINTS_OF_A_NEW_PLAGUE_PART4);
	}

	void WaypointReached(uint32 uiPointId)
	{
		Player* pPlayer = GetPlayerForEscort();
		if (!pPlayer)
			return;

		switch (uiPointId)
		{
			case 13:
				m_creature->SummonCreature(NPC_FORSAKEN_COURIER, -1553.33f, -3028.65f, 13.64f, 0.f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5*MINUTE*IN_MILLISECONDS);
				for (uint8 i = 0; i < 4; ++i)
					m_creature->SummonCreature(NPC_FORSAKEN_BODYGUARD, (-1553.33f + urand(0.f, 3.f)), (-3028.65f + urand(0.f, 3.f)), 13.64f, 0.f, TEMPSUMMON_DEAD_DESPAWN, 5*MINUTE*IN_MILLISECONDS);
				break;
			case 31:
				pPlayer->GroupEventHappens(QUEST_HINTS_OF_A_NEW_PLAGUE_PART4, m_creature);
				m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
				break;
		}
	}

	void UpdateAI(const uint32 uiDiff)
	{
		npc_escortAI::UpdateAI(uiDiff);

		//return since we have no target
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
			return;

		//m_uiKineloryBearForm_Timer
		if (m_uiKineloryBearForm_Timer <= uiDiff)
		{
			DoCastSpellIfCan(m_creature, SPELL_KINELORY_BEAR_FORM);
			m_uiKineloryBearForm_Timer = urand(30000, 38000);
		}
			else m_uiKineloryBearForm_Timer -= uiDiff;

		//m_uiRejuvenation_Timer
		if (Unit* pTarget = DoSelectLowestHpFriendly(40.0f, 800.0f))
		{
			if (pTarget && !pTarget->HasAura(SPELL_REJUVENATION, EFFECT_INDEX_0))
				DoCastSpellIfCan(m_creature, SPELL_REJUVENATION);
		}
		else if ((m_creature->GetMaxHealth() - 800) < m_creature->GetHealth())
			if (!m_creature->HasAura(SPELL_REJUVENATION, EFFECT_INDEX_0))
				DoCastSpellIfCan(m_creature, SPELL_REJUVENATION);

		DoMeleeAttackIfReady();
	}
};
CreatureAI* GetAI_npc_kinelory(Creature* pCreature)
{
	return new npc_kineloryAI (pCreature);
}

bool QuestAccept_npc_kinelory(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
	if (pQuest->GetQuestId() == QUEST_HINTS_OF_A_NEW_PLAGUE_PART4)
		if (npc_kineloryAI* pEscortAI = dynamic_cast<npc_kineloryAI*>(pCreature->AI()))
			pEscortAI->Start(true, pPlayer, pQuest, true, false);
	return true;
}

void AddSC_arathi_highlands()
{
    Script * pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "npc_professor_phizzlethorpe";
    pNewscript->GetAI = &GetAI_npc_professor_phizzlethorpe;
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_professor_phizzlethorpe;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_kinelory";
    pNewscript->GetAI = &GetAI_npc_kinelory;
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_kinelory;
    pNewscript->RegisterSelf();
}
