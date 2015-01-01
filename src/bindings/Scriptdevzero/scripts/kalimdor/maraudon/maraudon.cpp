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
SDName: Maraudon
SD%Complete: 100
SDComment:
SDCategory: Maraudon
EndScriptData */

#include "precompiled.h"
#include "escort_ai.h"
#include "maraudon.h"

enum Enums
{
    SAY_ESCORT_READY                  = -1349000,
    SAY_ESCORT_PROGRESS               = -1349001,
    SAY_SCEPTER_START                 = -1349002,
    SAY_SCEPTER_PROGRESS_1            = -1349003,
    SAY_SCEPTER_PROGRESS_2            = -1349005,
    SAY_SCEPTER_PROGRESS_3            = -1349006,
    SAY_SCEPTER_FINISH                = -1349007,
    
    EMOTE_SCEPTER_START               = -1349004,
    
    QUEST_THE_SCEPTER_OF_CELEBRAS     = 7046,
    SPELL_CREATE_SCEPTER              = 21939
};

struct MANGOS_DLL_DECL npc_celebras_the_redeemedAI : public npc_escortAI
{
    npc_celebras_the_redeemedAI(Creature *pCreature) : npc_escortAI(pCreature) {Reset();}
    
    bool Escort;
    void Reset()
    {
        Escort = true;
    }

    void WaypointReached(uint32 uiWaypointId)
    {
        Player* pPlayer = GetPlayerForEscort();
        if (!pPlayer)
            return;

        switch (uiWaypointId)
        {
            case 1:
                DoScriptText(SAY_ESCORT_PROGRESS, m_creature);
                break;
            case 2:
                DoScriptText(SAY_SCEPTER_START, m_creature);
                break;
            case 3:
                DoScriptText(SAY_SCEPTER_PROGRESS_1, m_creature);
                break;
            case 4:
                DoScriptText(SAY_SCEPTER_PROGRESS_2, m_creature);
                DoScriptText(EMOTE_SCEPTER_START, m_creature);
                break;
            case 5:
                DoScriptText(SAY_SCEPTER_PROGRESS_3, m_creature);
                break;
            case 6:
                DoScriptText(SAY_SCEPTER_FINISH, m_creature);
                DoCastSpellIfCan(m_creature, SPELL_CREATE_SCEPTER);
                break;
            case 7:
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
                if (pPlayer && pPlayer->GetTypeId() == TYPEID_PLAYER)
                    pPlayer->GroupEventHappens(QUEST_THE_SCEPTER_OF_CELEBRAS, m_creature);
                break;
        }
    }

    void Aggro(Unit* /*who*/)
    {
    }

    void JustDied(Unit* /*killer*/)
    {
        if (Player* pPlayer = GetPlayerForEscort())
            pPlayer->FailQuest(QUEST_THE_SCEPTER_OF_CELEBRAS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (Escort)
            npc_escortAI::UpdateAI(uiDiff);
        
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
    }
};
CreatureAI* GetAI_npc_celebras_the_redeemed(Creature *pCreature)
{
    return new npc_celebras_the_redeemedAI (pCreature);
}

bool QuestAccept_npc_celebras_the_redeemed(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
	if (pQuest->GetQuestId() == QUEST_THE_SCEPTER_OF_CELEBRAS)
	{
		if (npc_celebras_the_redeemedAI* pEscortAI = dynamic_cast<npc_celebras_the_redeemedAI*>(pCreature->AI()))
		{
        pEscortAI->Start(false, pPlayer, pQuest, true);
        DoScriptText(SAY_ESCORT_READY, pCreature, pPlayer);
    }
	}
	return true;
}

enum GIZLOCK_SPELLS
{
	SPELL_BLITZSTRAHLBOMBE			= 29419,
	SPELL_BOMBE						= 9143,
	SPELL_GOBLINDRACHENWERFER		= 21833,
	SPELL_SCHIESSEN					= 16100
};

struct MANGOS_DLL_DECL boss_tueftler_gizlockAI : public ScriptedAI
{
    boss_tueftler_gizlockAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

	uint32 blitzstrahl_timer;
	uint32 bombe_timer;
	uint32 werfer_timer;

    void Reset()
    {
		blitzstrahl_timer = urand(30000,45000);
		bombe_timer = urand(12000,20000);
		werfer_timer = urand(8000,25000);
    }

	void shoot()
	{
		if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
			m_creature->CastSpell(pTarget,SPELL_SCHIESSEN,true);
	}

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (blitzstrahl_timer < uiDiff)
		{
			m_creature->CastSpell(m_creature->getVictim(),SPELL_BLITZSTRAHLBOMBE,true);
			blitzstrahl_timer = urand(30000,45000);
			shoot();
		}
		else
			blitzstrahl_timer -= uiDiff;

		if (bombe_timer < uiDiff)
		{
			if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
			{
				m_creature->CastSpell(pTarget,SPELL_BOMBE,true);
				bombe_timer = urand(12000,20000);
				shoot();
			}
		}
		else
			bombe_timer -= uiDiff;

		if (werfer_timer < uiDiff)
		{
			if(DoCastSpellIfCan(m_creature->getVictim(),SPELL_GOBLINDRACHENWERFER) == CAST_OK)
			{
				werfer_timer = urand(8000,25000);
				shoot();
			}
		}
		else
			werfer_timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

enum SCHLANGENZUNGE_SPELLS
{
	SPELL_BLINZELN				= 21655,
	SPELL_MEHRFACHSCHUSS		= 21390,
	SPELL_RAUCHBOMBE			= 7964,

	NPC_SHADOWSTALKER			= 11792,
	ADD1						= 54675,
	ADD2						= 54676
};	

struct MANGOS_DLL_DECL boss_lord_schlangenzungeAI : public ScriptedAI
{
    boss_lord_schlangenzungeAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
		m_pInstance = (instance_maraudon*)pCreature->GetInstanceData();
        Reset();
    }

	instance_maraudon* m_pInstance;

	uint32 blinzeln_timer;
	uint32 schuss_timer;
	uint32 bombe_timer;

    void Reset()
    {
		blinzeln_timer = urand(15000,30000);
		schuss_timer = urand(8000,18000);
		bombe_timer = urand(12000,25000);

		ReviveAdds();
    }

	void Aggro(Unit* /*pwho*/)
    {
		m_creature->CallForHelp(40.0f);
    }

	void ReviveAdds()
	{
		std::list<Creature*> ADList;
		GetCreatureListWithEntryInGrid(ADList,m_creature,NPC_SHADOWSTALKER,40.0f);
		for(std::list<Creature*>::iterator i = ADList.begin(); i != ADList.end(); ++i)
		{
			if(Creature* pTemp = *i)
			{
				if(pTemp->GetGUID() == ADD1 || pTemp->GetGUID() == ADD2)
				{
					if(pTemp->isDead())
						pTemp->Respawn();
				}
			}
		}
	}

	void shoot()
	{
		if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
			m_creature->CastSpell(pTarget,SPELL_SCHIESSEN,true);
	}

	void UpdateAI(const uint32 uiDiff)
    {
        // Return if we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (blinzeln_timer < uiDiff)
		{
			if(DoCastSpellIfCan(m_creature,SPELL_BLINZELN) == CAST_OK)
			{
				blinzeln_timer = urand(15000,30000);
				shoot();
			}
		}
		else
			blinzeln_timer -= uiDiff;

		if (schuss_timer < uiDiff)
		{
			if(DoCastSpellIfCan(m_creature->getVictim(),SPELL_MEHRFACHSCHUSS) == CAST_OK)
			{
				schuss_timer = urand(8000,18000);
				shoot();
			}
		}
		else
			schuss_timer -= uiDiff;

		if (bombe_timer < uiDiff)
		{
			shoot();
			if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
			{
				if(DoCastSpellIfCan(pTarget,SPELL_RAUCHBOMBE) == CAST_OK)
				{
					bombe_timer = urand(12000,25000);
					shoot();
				}
			}
		}
		else
			bombe_timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_lord_schlangenzunge(Creature* pCreature)
{
    return new boss_lord_schlangenzungeAI(pCreature);
}

CreatureAI* GetAI_boss_tueftler_gizlock(Creature* pCreature)
{
    return new boss_tueftler_gizlockAI(pCreature);
}

void AddSC_maraudon()
{
    Script *newscript;
        
    newscript = new Script;
    newscript->Name="npc_celebras_the_redeemed";
    newscript->pQuestAcceptNPC = &QuestAccept_npc_celebras_the_redeemed; 
    newscript->GetAI = &GetAI_npc_celebras_the_redeemed;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "boss_tueftler_gizlock";
    newscript->GetAI = &GetAI_boss_tueftler_gizlock;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "boss_lord_schlangenzunge";
    newscript->GetAI = &GetAI_boss_lord_schlangenzunge;
    newscript->RegisterSelf();
}
