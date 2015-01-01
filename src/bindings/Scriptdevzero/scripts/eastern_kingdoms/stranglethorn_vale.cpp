/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Stranglethorn_Vale
SD%Complete: 100
SDComment: Quest support: 592
SDCategory: Stranglethorn Vale
EndScriptData */

/* ContentData
mob_yenniku
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"
#include "Log.h"

/*######
## mob_yenniku
######*/

struct MANGOS_DLL_DECL mob_yennikuAI : public ScriptedAI
{
    mob_yennikuAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        bReset = false;
        Reset();
    }

    uint32 Reset_Timer;
    bool bReset;

    void Reset()
    {
        Reset_Timer = 0;
        m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_NONE);
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if (caster->GetTypeId() == TYPEID_PLAYER)
        {
                                                            //Yenniku's Release
            if(!bReset && ((Player*)caster)->GetQuestStatus(592) == QUEST_STATUS_INCOMPLETE && spell->Id == 3607)
            {
                m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_STUN);
                m_creature->CombatStop();                   //stop combat
                m_creature->DeleteThreatList();             //unsure of this
                m_creature->setFaction(83);                 //horde generic

                bReset = true;
                Reset_Timer = 60000;
            }
        }
        return;
    }

    void Aggro(Unit* /*pWho*/)
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if (bReset)
	{
            if(Reset_Timer < diff)
            {
                ResetToHome();
                bReset = false;
                m_creature->setFaction(28);                     //troll, bloodscalp
            }
            else
                Reset_Timer -= diff;
	}

        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_yenniku(Creature* pCreature)
{
    return new mob_yennikuAI(pCreature);
}

/*######
## mob_sv_gorilla
######*/

#define NPC_GORILLA_SPAWN 1557

struct MANGOS_DLL_DECL mob_sv_gorillaAI : public ScriptedAI
{
	mob_sv_gorillaAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{
		Reset();
		m_creature->GetMotionMaster()->MovePoint(0, -13734.66f + frand(-2,2), -30.0f + frand(-2,2), 44.96f, true);
		for (int i = 0; i < 3; i++)
			if (Creature* spawnedGorilla = m_creature->SummonCreature(NPC_GORILLA_SPAWN, -13769.20f + frand(-3,3), -6.32f + frand(-3,3), 43.02f, 5.66f,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,180000))
				spawnedGorilla->GetMotionMaster()->MovePoint(0, -13734.66f + frand(-2,2), -30.0f + frand(-2,2), 44.96f, true);
	}

	bool m_bEnraged;

	void Reset()
	{
		m_bEnraged = false;
	}

	//void JustReachedHome()
	//{
	//	m_creature->ForcedDespawn();
	//}

	void UpdateAI(const uint32 /*diff*/)
	{
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (m_creature->GetHealthPercent() < 30.0f && !m_bEnraged)
		{
			m_bEnraged = true;
			m_creature->CastSpell(m_creature,8599,false);
		}

        DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_mob_gorilla(Creature* pCreature)
{
    return new mob_sv_gorillaAI(pCreature);
}

/*######
## mob_unbagwa
######*/

bool reward_quest_stranglethorn_fever(Player* /*pPlayer*/, Creature* C, Quest const* Q)
{
	if (Q->GetQuestId() != 349)
		return true;

	for (int i = 0; i < 3; i++)
		if (Creature* spawnedGorilla = C->SummonCreature(NPC_GORILLA_SPAWN, -13769.20f + frand(-3,3), -6.32f + frand(-3,3), 43.02f, 5.66f,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,180000))
			spawnedGorilla->GetMotionMaster()->MovePoint(0, -13734.66f + frand(-2,2), -30.0f + frand(-2,2), 44.96f, true);

	return false;
}

/*######################
# Gurubashi Arena Event - Short Mithril Jones.
###########*/

enum short_mithril_jones
{
	EVENT_START_YELL = -1713000,
	TWO_HOURS = 7200000,
};

struct MANGOS_DLL_DECL npc_short_mithril_jones : public npc_escortAI
{
	npc_short_mithril_jones(Creature* pCreature) : npc_escortAI(pCreature) {
		b_eventStarted = false;
		Reset(); 
	}

	time_t t;
	tm* aTM;
	bool b_eventStarted;
	uint32 m_wait;
	GameObject *chest;

	void Reset() 
	{
		m_wait = 0;
	}

	void UpdateAI(const uint32 uiDiff)
	{
		t = time(nullptr);
		aTM = localtime(&t);

		if(!b_eventStarted && aTM->tm_hour % 3 == 0 && aTM->tm_min == 0 && aTM->tm_sec == 0)
		{
			overrideDistanceLimit(true);
			Start(false, 0, 0,true);
			basic_log("Gurubashi Arena Event started at %u:%u:%u", aTM->tm_hour, aTM->tm_min, aTM->tm_sec);
		}

		if((aTM->tm_hour + 1) % 3 == 0 && aTM->tm_min == 0 && aTM->tm_sec == 0 && b_eventStarted)
		{
			b_eventStarted = false;
		}
		

		if(b_eventStarted && !GetClosestGameObjectWithEntry(m_creature, 179697, 100.0f))
			b_eventStarted = false;

		if(m_wait)
		{
			if(m_wait <= uiDiff)
			{
				b_eventStarted = true;
				chest = m_creature->SummonGameObject(179697, TWO_HOURS, m_creature->GetPositionX() + 1.0f,
					m_creature->GetPositionY(), m_creature->GetPositionZ(), 4.24f);

				if(chest)
					chest->SetChestRestockTime(time(nullptr) + 10*DAY);

				SetEscortPaused(false);
				m_wait = 0;
			}
			else
				m_wait -= uiDiff;
		}

		npc_escortAI::UpdateAI(uiDiff);
	}

	 void WaypointReached(uint32 uiPointId)
	 {
		 switch(uiPointId)
		 {
		 case 16:
			 SetEscortPaused(true);
			 m_creature->MonsterYellToZone(EVENT_START_YELL, LANG_UNIVERSAL, nullptr);
			 m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
			 b_eventStarted = true;
			 m_wait = 5000;
			 break;
		 case 30:
			 break;
		 }
	 }
};

CreatureAI* GetAI_npc_short_mithril_jones(Creature* pCreature)
{
    return new npc_short_mithril_jones(pCreature);
}

void AddSC_stranglethorn_vale()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "mob_yenniku";
    pNewscript->GetAI = &GetAI_mob_yenniku;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_sv_gorilla";
    pNewscript->GetAI = &GetAI_mob_gorilla;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_unbagwa";
    pNewscript->pQuestRewardedNPC = &reward_quest_stranglethorn_fever;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_short_mithril_jones";
    pNewscript->GetAI = &GetAI_npc_short_mithril_jones;
    pNewscript->RegisterSelf();
}
