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
SDName: Undercity
SD%Complete: 95
SDComment: Quest support: 6628(Parqual Fintallas questions/'answers' might have more to it, need more info), 9180(post-event).
SDCategory: Undercity
EndScriptData */

/* ContentData
npc_lady_sylvanas_windrunner
npc_parqual_fintallas
EndContentData */

#include "precompiled.h"

/*######
## npc_lady_sylvanas_windrunner
######*/

#define SAY_LAMENT_END              -1000196
#define EMOTE_LAMENT_END            -1000197

#define SOUND_CREDIT                10896
#define ENTRY_HIGHBORNE_LAMENTER    21628
#define ENTRY_HIGHBORNE_BUNNY       21641

#define SPELL_HIGHBORNE_AURA        37090
#define SPELL_SYLVANAS_CAST         36568
#define SPELL_RIBBON_OF_SOULS       34432                   //the real one to use might be 37099

float HighborneLoc[4][3]=
{
    {1285.41f, 312.47f, 0.51f},
    {1286.96f, 310.40f, 1.00f},
    {1289.66f, 309.66f, 1.52f},
    {1292.51f, 310.50f, 1.99f},
};
#define HIGHBORNE_LOC_Y             -61.00
#define HIGHBORNE_LOC_Y_NEW         -55.50

struct MANGOS_DLL_DECL npc_lady_sylvanas_windrunnerAI : public ScriptedAI
{
    npc_lady_sylvanas_windrunnerAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 LamentEvent_Timer;
    bool LamentEvent;
    ObjectGuid targetGUID;

    float myX;
    float myY;
    float myZ;

    void Reset()
    {
        myX = m_creature->GetPositionX();
        myY = m_creature->GetPositionY();
        myZ = m_creature->GetPositionZ();

        LamentEvent_Timer = 5000;
        LamentEvent = false;
		targetGUID.Clear();
    }

    void JustSummoned(Creature *pSummoned)
    {
        if (pSummoned->GetEntry() == ENTRY_HIGHBORNE_BUNNY)
        {
            if (Creature* pBunny = pSummoned->GetMap()->GetCreature(targetGUID))
            {
                pBunny->NearTeleportTo(pBunny->GetPositionX(), pBunny->GetPositionY(), myZ+15.0f, 0.0f);
                pSummoned->CastSpell(pBunny,SPELL_RIBBON_OF_SOULS,false);
            }

            targetGUID = pSummoned->GetObjectGuid();
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (LamentEvent)
        {
            if (LamentEvent_Timer < diff)
            {
                float raX = myX;
                float raY = myY;
                float raZ = myZ;

                m_creature->GetRandomPoint(myX,myY,myZ,20.0f,raX,raY,raZ);
                m_creature->SummonCreature(ENTRY_HIGHBORNE_BUNNY,raX,raY,myZ,0,TEMPSUMMON_TIMED_DESPAWN,3000);

                LamentEvent_Timer = 2000;
                if (!m_creature->HasAura(SPELL_SYLVANAS_CAST, EFFECT_INDEX_0))
                {
                    DoScriptText(SAY_LAMENT_END, m_creature);
                    DoScriptText(EMOTE_LAMENT_END, m_creature);
                    LamentEvent = false;
                }
            }else LamentEvent_Timer -= diff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_npc_lady_sylvanas_windrunner(Creature* pCreature)
{
    return new npc_lady_sylvanas_windrunnerAI(pCreature);
}

bool QuestRewarded_npc_lady_sylvanas_windrunner(Player* /*pPlayer*/, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == 9180)
    {
        if (npc_lady_sylvanas_windrunnerAI* pSylvanAI = dynamic_cast<npc_lady_sylvanas_windrunnerAI*>(pCreature->AI()))
        {
            pSylvanAI->LamentEvent = true;
            pSylvanAI->DoPlaySoundToSet(pCreature, SOUND_CREDIT);
        }
        pCreature->CastSpell(pCreature,SPELL_SYLVANAS_CAST,false);

        for(uint8 i = 0; i < 4; ++i)
            pCreature->SummonCreature(ENTRY_HIGHBORNE_LAMENTER, HighborneLoc[i][0], HighborneLoc[i][1], HIGHBORNE_LOC_Y, HighborneLoc[i][2], TEMPSUMMON_TIMED_DESPAWN, 160000);
    }

    return true;
}

/*######
## npc_parqual_fintallas
######*/

#define SPELL_MARK_OF_SHAME 6767

bool GossipHello_npc_parqual_fintallas(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(6628) == QUEST_STATUS_INCOMPLETE && !pPlayer->HasAura(SPELL_MARK_OF_SHAME, EFFECT_INDEX_0))
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Gul'dan", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Kel'Thuzad", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Ner'zhul", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
        pPlayer->SEND_GOSSIP_MENU(5822, pCreature->GetObjectGuid());
    }
    else
        pPlayer->SEND_GOSSIP_MENU(5821, pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_parqual_fintallas(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->CastSpell(pPlayer,SPELL_MARK_OF_SHAME,false);
    }
    if (uiAction == GOSSIP_ACTION_INFO_DEF+2)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->AreaExploredOrEventHappens(6628);
    }
    return true;
}

/*####
# npc_bethor_iceshard
####*/

enum eBethor
{
	GUNTHER_SAY_1					 = -1720183,
	SPELL_NETHER_GEM				 = 7673,
	QUEST_ID_PRODIGAL_LICH_RETURNS	 = 411,
	NPC_GUNTHER_VISAGE				 = 5666
};

struct MANGOS_DLL_DECL npc_bethor_iceshardAI : public ScriptedAI
{
    npc_bethor_iceshardAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{ 
		Reset(); 
	}
	uint8 m_uiSpeechStep;
	uint32 m_uiSpeechTimer;
	bool m_bOutro411;

	ObjectGuid m_uiPlayerGUID;

    void Reset()
	{
		m_bOutro411 = false;
		m_uiSpeechStep = 1;
		m_uiSpeechTimer = 0;
		m_uiPlayerGUID.Clear();
	}

	void StartOutro(ObjectGuid pPlayerGUID, uint32 uiQuestId = 0)
	{		
		if (!pPlayerGUID)
            return;

        m_uiPlayerGUID = pPlayerGUID;

		m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
		
		if (uiQuestId == 411)
		{
			m_bOutro411 = true; 
			m_uiSpeechTimer = 2000;
			m_uiSpeechStep = 1;
		}
	}

	void JustSummoned(Creature* pSummoned)
    {
        if(pSummoned->GetEntry() == NPC_GUNTHER_VISAGE)
        {
			pSummoned->SetSpeedRate(MOVE_WALK, 0.7f);
			pSummoned->GetMotionMaster()->MovePoint(0, 1768.88f,63.3057f,-46.3214f);
		}
	}

	void UpdateAI(const uint32 uiDiff)
    {
		if (m_uiSpeechTimer && m_bOutro411)							// handle RP at quest end 411
		{
			if(!m_uiSpeechStep)
				return;
		
			if(m_uiSpeechTimer <= uiDiff)
            {
				Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID);
				Creature* pGunther = GetClosestCreatureWithEntry(m_creature, NPC_GUNTHER_VISAGE, 10.0f);
                switch(m_uiSpeechStep)
                {					
                    case 1:
						m_creature->CastSpell(m_creature, SPELL_NETHER_GEM, false);
						m_creature->GenericTextEmote("Bethor Iceshard uses the Nether Gem to communicate with Gunther.", NULL, false);
						m_uiSpeechTimer = 4000;
                        break;
					case 2:
						m_creature->SummonCreature(NPC_GUNTHER_VISAGE,1772.62f,57.5082f,-46.3214f,2.188f,TEMPSUMMON_TIMED_DESPAWN, 23000, true);
                        m_uiSpeechTimer = 6000;
                        break;
					case 3:
						if(pGunther)
						{
							pGunther->SetFacingTo(2.36f);
							DoScriptText(GUNTHER_SAY_1, pGunther, pPlayer);
						}
                        m_uiSpeechTimer = 12000;
						break;
					case 4:
						if(pGunther)
							pGunther->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
						m_uiSpeechTimer = 3000;
						break;
					case 5:
						if(pGunther)
							pGunther->GetMotionMaster()->MovePoint(1,1772.62f,57.5082f,-46.3214f);
						m_creature->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
						m_uiSpeechTimer = 3000;
						break;
					case 6:
						m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
						m_bOutro411 = false;
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

		if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_bethor_iceshard(Creature* pCreature)
{
    return new npc_bethor_iceshardAI(pCreature);
}

bool OnQuestRewarded_npc_bethor_iceshard(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
	if (pQuest->GetQuestId() == QUEST_ID_PRODIGAL_LICH_RETURNS)
    {
		if (npc_bethor_iceshardAI* pBethorAI = dynamic_cast<npc_bethor_iceshardAI*>(pCreature->AI()))
			pBethorAI->StartOutro(pPlayer->GetObjectGuid(), 411);
	}
	return true;
}

/*####
# npc_chemist_cuely
####*/

enum eCuley
{
	QUEST_ID_SEEPING_CORRUPTION_1	= 3568,
	SAY_CULEY				= -1720232,
};

struct MANGOS_DLL_DECL npc_chemist_cuelyAI : public ScriptedAI
{
    npc_chemist_cuelyAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{ 
		Reset(); 
	}

	uint8 m_uiSpeechStep;
	uint32 m_uiSpeechTimer;
	bool m_bOutro3568;
	ObjectGuid m_uiPlayerGUID;

    void Reset()
	{
		m_bOutro3568 = false;
		m_uiSpeechStep = 1;
		m_uiSpeechTimer = 0;
		m_uiPlayerGUID.Clear();
	}

	void StartOutro(ObjectGuid pPlayerGUID, uint32 uiQuestId = 0)
	{		
		if (!pPlayerGUID)
            return;

        m_uiPlayerGUID = pPlayerGUID;

		m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
		
		if (uiQuestId == 3568)
		{
			m_bOutro3568 = true; 
			m_uiSpeechTimer = 2000;
			m_uiSpeechStep = 1;
		}
	}

	void UpdateAI(const uint32 uiDiff)
    {
		if (m_uiSpeechTimer && m_bOutro3568)							// handle RP at quest end 3568
		{
			if(!m_uiSpeechStep)
				return;
		
			if(m_uiSpeechTimer <= uiDiff)
            {				
                switch(m_uiSpeechStep)
                {					
                    case 1:
						m_creature->HandleEmoteState(EMOTE_STATE_USESTANDING);
						m_uiSpeechTimer = 6000;
                        break;
					case 2:
						m_creature->HandleEmoteState(EMOTE_STATE_NONE);
						m_uiSpeechTimer = 2000;
                        break;
					case 3:
						DoScriptText(SAY_CULEY,m_creature,NULL);
						m_uiSpeechTimer = 2000;
                        break;
					case 4:
						m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
						m_bOutro3568 = false;
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

		if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_chemist_cuely(Creature* pCreature)
{
    return new npc_chemist_cuelyAI(pCreature);
}

bool OnQuestRewarded_npc_chemist_cuely(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
	if (pQuest->GetQuestId() == QUEST_ID_SEEPING_CORRUPTION_1)
    {
		if (npc_chemist_cuelyAI* pCuleyAI = dynamic_cast<npc_chemist_cuelyAI*>(pCreature->AI()))
			pCuleyAI->StartOutro(pPlayer->GetObjectGuid(), 3568);
	}
	return true;
}

/*####
# npc_chemist_cuely
####*/

enum eThersa
{
	QUEST_ID_SEEPING_CORRUPTION_2	= 3569,
	SPELL_DRINK_DISEASE_BOTTLE		= 6355,

	SAY_THERSA				= -1720233,
	NPC_CHEMIST_CUELY		= 8390
};

struct MANGOS_DLL_DECL npc_thersa_windsongAI : public ScriptedAI
{
    npc_thersa_windsongAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{ 
		Reset(); 
	}

	uint8 m_uiSpeechStep;
	uint32 m_uiSpeechTimer;

	bool m_bOutro3569;

	ObjectGuid m_uiPlayerGUID;

    void Reset()
	{
		m_bOutro3569 = false;
		m_uiSpeechStep = 1;
		m_uiSpeechTimer = 0;
		m_uiPlayerGUID.Clear();
	}

	void StartOutro(ObjectGuid pPlayerGUID, uint32 uiQuestId = 0)
	{		
		if (!pPlayerGUID)
            return;

        m_uiPlayerGUID = pPlayerGUID;

		Creature* pCuely = GetClosestCreatureWithEntry(m_creature, NPC_CHEMIST_CUELY, 15.0f);
		if(pCuely)
			pCuely->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
		m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
		
		if (uiQuestId == 3569)
		{
			m_bOutro3569 = true; 
			m_uiSpeechTimer = 1000;
			m_uiSpeechStep = 1;
		}
	}

	void UpdateAI(const uint32 uiDiff)
    {
		if (m_uiSpeechTimer && m_bOutro3569)							// handle RP at quest end 3569
		{
			if(!m_uiSpeechStep)
				return;
		
			if(m_uiSpeechTimer <= uiDiff)
            {
				Creature* pCuely = GetClosestCreatureWithEntry(m_creature, NPC_CHEMIST_CUELY, 15.0f);
                switch(m_uiSpeechStep)
                {					
                    case 1:
						
						m_creature->CastSpell(m_creature, SPELL_DRINK_DISEASE_BOTTLE, false);
						m_uiSpeechTimer = 4000;
                        break;
					case 2:
						DoScriptText(SAY_THERSA,m_creature,NULL);
						m_uiSpeechTimer = 3000;
                        break;
					case 3:
						if(pCuely)
							pCuely->HandleEmote(EMOTE_ONESHOT_LAUGH);
						m_uiSpeechTimer = 3000;
                        break;
					case 4:						
						m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
						if(pCuely)
							pCuely->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
						m_bOutro3569 = false;
						// end the event after flags are set
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

		if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_thersa_windsong(Creature* pCreature)
{
    return new npc_thersa_windsongAI(pCreature);
}

bool OnQuestRewarded_npc_thersa_windsong(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
	if (pQuest->GetQuestId() == QUEST_ID_SEEPING_CORRUPTION_2)
    {
		if (npc_thersa_windsongAI* pThersaAI = dynamic_cast<npc_thersa_windsongAI*>(pCreature->AI()))
			pThersaAI->StartOutro(pPlayer->GetObjectGuid(), 3569);
	}
	return true;
}

/*######
## AddSC
######*/

void AddSC_undercity()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "npc_lady_sylvanas_windrunner";
    pNewscript->GetAI = &GetAI_npc_lady_sylvanas_windrunner;
    pNewscript->pQuestRewardedNPC = &QuestRewarded_npc_lady_sylvanas_windrunner;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_parqual_fintallas";
    pNewscript->pGossipHello = &GossipHello_npc_parqual_fintallas;
    pNewscript->pGossipSelect = &GossipSelect_npc_parqual_fintallas;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_bethor_iceshard";
    pNewscript->GetAI = &GetAI_npc_bethor_iceshard;
    pNewscript->pQuestRewardedNPC = &OnQuestRewarded_npc_bethor_iceshard;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_chemist_cuely";
    pNewscript->GetAI = &GetAI_npc_chemist_cuely;
    pNewscript->pQuestRewardedNPC = &OnQuestRewarded_npc_chemist_cuely;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_thersa_windsong";
    pNewscript->GetAI = &GetAI_npc_thersa_windsong;
    pNewscript->pQuestRewardedNPC = &OnQuestRewarded_npc_thersa_windsong;
    pNewscript->RegisterSelf();
}
