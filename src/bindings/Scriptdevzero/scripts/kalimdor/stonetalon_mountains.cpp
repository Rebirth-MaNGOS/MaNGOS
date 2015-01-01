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
SDName: Stonetalon_Mountains
SD%Complete: 95
SDComment: Quest support: 6627 (Braug Dimspirits questions/'answers' might have more to it, need more info),6523
SDCategory: Stonetalon Mountains
EndScriptData */

#include "precompiled.h"
#include "escort_ai.h"

/*######
## npc_braug_dimspirit
######*/

bool GossipHello_npc_braug_dimspirit(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(6627) == QUEST_STATUS_INCOMPLETE)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Ysera", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Neltharion", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Nozdormu", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Alexstrasza", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Malygos", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        pPlayer->SEND_GOSSIP_MENU(5820, pCreature->GetObjectGuid());
    }
    else
        pPlayer->SEND_GOSSIP_MENU(5819, pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_braug_dimspirit(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->CastSpell(pPlayer,6766,false);

    }
    if (uiAction == GOSSIP_ACTION_INFO_DEF+2)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->AreaExploredOrEventHappens(6627);
    }
    return true;
}

/*######
## npc_kaya
######*/

enum
{
    NPC_GRIMTOTEM_RUFFIAN       = 11910,
    NPC_GRIMTOTEM_BRUTE         = 11912,
    NPC_GRIMTOTEM_SORCERER      = 11913,

    SAY_START                   = -1000357,
    SAY_AMBUSH                  = -1000358,
    SAY_END                     = -1000359,

    QUEST_PROTECT_KAYA          = 6523
};

struct MANGOS_DLL_DECL npc_kayaAI : public npc_escortAI
{
    npc_kayaAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    void Reset() { }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->AI()->AttackStart(m_creature);
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            //Ambush
            case 16:
                //note about event here:
                //apparently NPC say _after_ the ambush is over, and is most likely a bug at you-know-where.
                //we simplify this, and make say when the ambush actually start.
                DoScriptText(SAY_AMBUSH, m_creature);
                m_creature->SummonCreature(NPC_GRIMTOTEM_RUFFIAN, -50.75f, -500.77f, -46.13f, 0.4f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                m_creature->SummonCreature(NPC_GRIMTOTEM_BRUTE, -40.05f, -510.89f,- 46.05f, 1.7f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                m_creature->SummonCreature(NPC_GRIMTOTEM_SORCERER, -32.21f, -499.20f, -45.35f, 2.8f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                break;
            // Award quest credit
            case 18:
                DoScriptText(SAY_END, m_creature);

                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_PROTECT_KAYA, m_creature);
                break;
        }
    }
};

CreatureAI* GetAI_npc_kaya(Creature* pCreature)
{
    return new npc_kayaAI(pCreature);
}

bool QuestAccept_npc_kaya(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    //Casting Spell and Starting the Escort quest is buggy, so this is a hack. Use the spell when it is possible.

    if (pQuest->GetQuestId() == QUEST_PROTECT_KAYA)
    {
        pCreature->setFaction(FACTION_ESCORT_H_PASSIVE);
        DoScriptText(SAY_START,pCreature);

        if (npc_kayaAI* pEscortAI = dynamic_cast<npc_kayaAI*>(pCreature->AI()))
            pEscortAI->Start(false, pPlayer, pQuest);
    }
    return true;
}

/*######
## npc_piznik
######*/

enum ePiznik
{
	QUEST_GERENZOS_ORDERS   = 1090,

	NPC_WINDSHEAR_DIGGER    = 3999,
	NPC_WINDSHEAR_GEOMANCER = 4003,
};

#define SPAWN_X 942.10f
#define SPAWN_Y -254.33f
#define SPAWN_Z -2.35f

struct MANGOS_DLL_DECL npc_piznikAI : public npc_escortAI
{
	npc_piznikAI(Creature* pCreature) : npc_escortAI(pCreature)
	{
		Reset();
	}

	Creature* WindshearKobold[12];
	uint32 WindshearKoboldCounter;
	uint32 EventPhase;
	uint32 EventTimer;
	bool CanWalk;

	void Reset()
	{
		if (HasEscortState(STATE_ESCORT_ESCORTING))
			return;

		for(uint8 i = 0; i < 12; ++i)
			WindshearKobold[i] = 0;

		WindshearKoboldCounter = 0;
		EventPhase = 0;
		EventTimer = 0;

		CanWalk = true;
	}

	void WaypointReached(uint32 uiPoint)
	{
		Player* pPlayer = GetPlayerForEscort();
		if (!pPlayer)
			return;

		switch(uiPoint)
		{
			case 0:
				EventTimer = 1;
				EventPhase = 1;
				CanWalk = false;
				break;
		}
	}

	void JustSummoned(Creature* pSummon)
	{
		if (pSummon)
			pSummon->AI()->AttackStart(m_creature);
	}

	void SummonWindshearKobold()
	{
		switch(urand(1,2))
		{
			case 1: WindshearKobold[++WindshearKoboldCounter] = m_creature->SummonCreature(NPC_WINDSHEAR_DIGGER, SPAWN_X, SPAWN_Y, SPAWN_Z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000); break;
			case 2: WindshearKobold[++WindshearKoboldCounter] = m_creature->SummonCreature(NPC_WINDSHEAR_GEOMANCER, SPAWN_X, SPAWN_Y, SPAWN_Z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000); break;
		}
	}

	bool CanCompletePiznikQuest()
	{
		if (WindshearKobold[9] && WindshearKobold[9]->isAlive() && !WindshearKobold[9]->isDead())
			return false;
		if (WindshearKobold[10] && WindshearKobold[10]->isAlive() && !WindshearKobold[10]->isDead())
			return false;
		if (WindshearKobold[11] && WindshearKobold[11]->isAlive() && !WindshearKobold[11]->isDead())
			return false;

		return true;
	}

	void UpdateAI(const uint32 uiDiff)
	{
		Player* pPlayer = GetPlayerForEscort();

		if (EventTimer && pPlayer && m_creature->isAlive())
		{
			if (EventTimer <= uiDiff)
			{
				switch(EventPhase)
				{
					case 1:
						SummonWindshearKobold();
						SummonWindshearKobold();
						SummonWindshearKobold();
						EventTimer = 30000;
						break;
					case 2:
						SummonWindshearKobold();
						SummonWindshearKobold();
						SummonWindshearKobold();
						EventTimer = 30000;
						break;
					case 3:
						SummonWindshearKobold();
						SummonWindshearKobold();
						SummonWindshearKobold();
						EventTimer = 30000;
						break;
					case 4:
						SummonWindshearKobold();
						SummonWindshearKobold();
						SummonWindshearKobold();
						EventTimer = 1;
						break;
					case 5:
						if (CanCompletePiznikQuest())
						{
							m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
							pPlayer->AreaExploredOrEventHappens(QUEST_GERENZOS_ORDERS);
							EventTimer = 0; // event done
							CanWalk = true;
						}
						else
						{
							EventPhase = 4; // repeat this phase
							EventTimer = 1; // holding event repeat
						}
						break;
				}
				++EventPhase;
			}
				else EventTimer -= uiDiff;
		}

		if (CanWalk)
			npc_escortAI::UpdateAI(uiDiff);

		if (m_creature->SelectHostileTarget() || m_creature->getVictim())
			DoMeleeAttackIfReady();
	}
};
CreatureAI* GetAI_npc_piznik(Creature* pCreature)
{
    return new npc_piznikAI (pCreature);
}

bool QuestAccept_npc_piznik(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_GERENZOS_ORDERS)
    {
        if (npc_piznikAI* pEscortAI = dynamic_cast<npc_piznikAI*>(pCreature->AI()))
            pEscortAI->Start(false, pPlayer, pQuest, true, false);
    }
    return true;
}

/*######
## AddSC
######*/

void AddSC_stonetalon_mountains()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "npc_braug_dimspirit";
    pNewscript->pGossipHello = &GossipHello_npc_braug_dimspirit;
    pNewscript->pGossipSelect = &GossipSelect_npc_braug_dimspirit;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_kaya";
    pNewscript->GetAI = &GetAI_npc_kaya;
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_kaya;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_piznik";
    pNewscript->GetAI = &GetAI_npc_piznik;
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_piznik;
    pNewscript->RegisterSelf();
}
