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
SDName: Swamp_of_Sorrows
SD%Complete: 100
SDComment: Quest support: 1393
SDCategory: Swamp of Sorrows
EndScriptData */

/* ContentData
npc_galen_goodward
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"

/*######
## npc_galen_goodward
######*/

enum Galen
{
    QUEST_GALENS_ESCAPE     = 1393,

    GO_GALENS_CAGE          = 37118,

    SAY_PERIODIC            = -1000160,
    SAY_QUEST_ACCEPTED      = -1000161,
    SAY_ATTACKED_1          = -1000162,
    SAY_ATTACKED_2          = -1000163,
    SAY_QUEST_COMPLETE      = -1000164,
    EMOTE_WHISPER           = -1000165,
    EMOTE_DISAPPEAR         = -1000166
};

struct MANGOS_DLL_DECL npc_galen_goodwardAI : public npc_escortAI
{
    npc_galen_goodwardAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        Reset();
    }

    ObjectGuid m_uiGalensCageGUID;
    uint32 m_uiPeriodicSay;

    void Reset()
    {
        m_uiPeriodicSay = 6000;
    }

    void Aggro(Unit* pWho)
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
            DoScriptText(urand(0, 1) ? SAY_ATTACKED_1 : SAY_ATTACKED_2, m_creature, pWho);
    }

    void WaypointStart(uint32 uiPointId)
    {
        switch (uiPointId)
        {
            case 0:
            {
                GameObject* pCage = NULL;
                if (m_uiGalensCageGUID)
                    pCage = m_creature->GetMap()->GetGameObject(m_uiGalensCageGUID);
                else
                    pCage = GetClosestGameObjectWithEntry(m_creature, GO_GALENS_CAGE, INTERACTION_DISTANCE);
                if (pCage)
                {
                    pCage->UseDoorOrButton();
					m_uiGalensCageGUID = pCage->GetObjectGuid();
                }
                break;
            }
            case 21:
                DoScriptText(EMOTE_DISAPPEAR, m_creature);
                break;
        }
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch (uiPointId)
        {
            case 0:
                if (GameObject* pCage = m_creature->GetMap()->GetGameObject(m_uiGalensCageGUID))
                    pCage->ResetDoorOrButton();
                break;
            case 20:
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    m_creature->SetFacingToObject(pPlayer);
                    DoScriptText(SAY_QUEST_COMPLETE, m_creature, pPlayer);
                    DoScriptText(EMOTE_WHISPER, m_creature, pPlayer);
                    pPlayer->GroupEventHappens(QUEST_GALENS_ESCAPE, m_creature);
                }
                SetRun(true);
                break;
        }
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {

        if (m_uiPeriodicSay < uiDiff)
        {
            if (HasEscortState(STATE_ESCORT_NONE))
                DoScriptText(SAY_PERIODIC, m_creature);
            m_uiPeriodicSay = 6000;
        }
        else
            m_uiPeriodicSay -= uiDiff;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

bool QuestAccept_npc_galen_goodward(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_GALENS_ESCAPE)
    {
        if (npc_galen_goodwardAI* pEscortAI = dynamic_cast<npc_galen_goodwardAI*>(pCreature->AI()))
        {
            pEscortAI->Start(false, pPlayer, pQuest);
            pCreature->setFaction(FACTION_ESCORT_N_NEUTRAL_ACTIVE);
            DoScriptText(SAY_QUEST_ACCEPTED, pCreature);
        }
    }
    return true;
}

CreatureAI* GetAI_npc_galen_goodward(Creature* pCreature)
{
    return new npc_galen_goodwardAI(pCreature);
}

/*####
# npc_infiltrator_marksen
####*/

enum
{
	MARKSEN_SAY_1					 = -1720073,
	MARKSEN_SAY_2					 = -1720074,
	MARKSEN_SAY_3					 = -1720075,

	SPELL_TRANSFORM_ZOMBIE			 = 7293,

    QUEST_ID_NOTHING_BUT_THE_TRUTH	 = 1391,
};

struct MANGOS_DLL_DECL npc_infiltrator_marksenAI : public npc_escortAI
{
    npc_infiltrator_marksenAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }
	
	uint8 m_uiSpeechStep;
	uint32 m_uiSpeechTimer;
	bool m_bOutro;

    void Reset()
	{
		m_bOutro = false;
		m_uiSpeechStep = 1;
		m_uiSpeechTimer = 0;
	}

	void WaypointReached(uint32 uiPointId)
    {
	}

	void JustStartedEscort()
    {
    }

	void StartOutro()
	{
		m_bOutro = true; 
		m_uiSpeechTimer = 2000;
		m_uiSpeechStep = 1;
		m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
	}

	void UpdateAI(const uint32 uiDiff)
    {
		npc_escortAI::UpdateAI(uiDiff);

		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
	}

	void UpdateEscortAI(const uint32 uiDiff)
    {
		if (m_uiSpeechTimer && m_bOutro)							// handle RP at quest end
		{
			if (!m_uiSpeechStep)
				return;
		
			if (m_uiSpeechTimer <= uiDiff)
            {
                switch(m_uiSpeechStep)
                {
                    case 1:
                        m_creature->GenericTextEmote("Infiltrator Marksen takes a big swig of ale.", NULL, false);
						m_creature->HandleEmote(EMOTE_ONESHOT_EAT);
						m_uiSpeechTimer = 3000;
                        break;
					case 2:
						DoScriptText(MARKSEN_SAY_1, m_creature);
                        m_uiSpeechTimer = 4000;
                        break;
					case 3:
						DoScriptText(MARKSEN_SAY_2, m_creature);
                        m_uiSpeechTimer = 1000;
						break;
					case 4:						
						m_creature->HandleEmote(EMOTE_STATE_STUN);
						m_uiSpeechTimer = 5000;
						break;
					case 5:
						m_creature->HandleEmote(EMOTE_STATE_NONE);						
						m_uiSpeechTimer = 1000;
						break;
					case 6:
						m_creature->HandleEmote(EMOTE_ONESHOT_WOUND);
						m_creature->GenericTextEmote("Infiltrator Marksen writhes in pain.", NULL, false);
						m_uiSpeechTimer = 3000;
						break;
					case 7:						
						m_creature->CastSpell(m_creature, SPELL_TRANSFORM_ZOMBIE, false);
                        m_uiSpeechTimer = 3000;
                        break;
					case 8:
						m_creature->GetMotionMaster()->MoveConfused();
						m_uiSpeechTimer = 5000;
                        break;
					case 9:			
						DoScriptText(MARKSEN_SAY_3, m_creature);
						m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
						m_bOutro = false;
						// Kill self
						m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);						
						break;
                    /*default:
                        m_uiSpeechStep = 0;
                        return;*/
                }
                ++m_uiSpeechStep;
            }
            else
                m_uiSpeechTimer -= uiDiff;
		}
	}
};

CreatureAI* GetAI_npc_infiltrator_marksen(Creature* pCreature)
{
    return new npc_infiltrator_marksenAI(pCreature);
}

bool OnQuestRewarded_npc_infiltrator_marksen(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
	if (pQuest->GetQuestId() == QUEST_ID_NOTHING_BUT_THE_TRUTH)
    {
		if (npc_infiltrator_marksenAI* pEscortAI = dynamic_cast<npc_infiltrator_marksenAI*>(pCreature->AI()))
			pEscortAI->StartOutro();
	}
	return true;
}

void AddSC_swamp_of_sorrows()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "npc_galen_goodward";
    pNewscript->GetAI = &GetAI_npc_galen_goodward;
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_galen_goodward;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_infiltrator_marksen";
    pNewscript->GetAI = &GetAI_npc_infiltrator_marksen;
    pNewscript->pQuestRewardedNPC = &OnQuestRewarded_npc_infiltrator_marksen;
    pNewscript->RegisterSelf();
}
