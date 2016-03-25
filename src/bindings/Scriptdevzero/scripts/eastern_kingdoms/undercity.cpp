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
# npc_thersa_windsong
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
## npc_brother_malach
######*/

enum
{
    SPELL_SUMMON = 3657, //8675,    // wrong spell, find a better, only want the visuals and we have no idea what the retail spell looks like

    NPC_HUMAN_MALE_CAPTIVE = 5680,
    NPC_HUMAN_FEMALE_CAPTIVE = 5681,
    
    NPC_CAPTIVE_GHOUL = 5685,
    NPC_CAPTIVE_ZOMBIE = 5686,
    NPC_CAPTIVE_ABOMINATION = 5687,
    
    NPC_LYSTA_BANCROFT = 5679,
    
    NPC_EDWARD = 5654,
    NPC_TYLER = 5653
};

struct Loc
{
    float x, y, z, o;
};

static Loc aSpawnLoc[]= 
{
    {1738.03f, 378.25f, -62.29, 3.77f},    // left
    {1736.07f, 378.05f, -62.29, 3.77f},   // mid
    {1735.89f, 381.46f, -62.29, 3.83f},   // right
};

struct MANGOS_DLL_DECL npc_brother_malachAI : public ScriptedAI
{
    npc_brother_malachAI(Creature* pCreature) : ScriptedAI(pCreature) 
    { 
        Reset(); 
    }

    uint8 m_uiDeadAdds;
    uint8 m_uiSpeechStep;
    uint32 m_uiSpeechTimer;
    
    uint32 m_uiEventTimer;

    bool m_bEvent;

    void Reset()
    {
        if(!m_bEvent)
        {
            m_uiSpeechStep = 1;
            m_uiSpeechTimer = 0;
            m_uiEventTimer = urand(10000, 60000);
        }
    }
    
    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->SetRespawnEnabled(false);            // make sure they won't respawn
        
        Creature* pEdward = GetClosestCreatureWithEntry(m_creature, NPC_EDWARD, 15.0f);
        Creature* pTyler = GetClosestCreatureWithEntry(m_creature, NPC_TYLER, 15.0f);
        
        int a = urand(0,1);
        if(pEdward && pTyler)
            pSummoned->AddThreat(a > 0 ? pEdward : pTyler, 0.1f);
        
        if(pEdward)
            pEdward->AddThreat(pSummoned, 0.1f);
        if(pTyler)
            pTyler->AddThreat(pSummoned, 0.1f);
    }

    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        ++m_uiDeadAdds;
        
        if(m_uiDeadAdds == 3 || m_uiDeadAdds == 5 || m_uiDeadAdds == 6)
            m_uiSpeechTimer = 1000;
    }
    
    void DoSummonWave(uint32 uiSummonId = 0)
    {
        if (uiSummonId == 1)
        {            
            m_creature->SummonCreature(NPC_HUMAN_MALE_CAPTIVE, aSpawnLoc[0].x, aSpawnLoc[0].y, aSpawnLoc[0].z, aSpawnLoc[0].o, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
            m_creature->SummonCreature(NPC_HUMAN_FEMALE_CAPTIVE, aSpawnLoc[1].x, aSpawnLoc[1].y, aSpawnLoc[1].z, aSpawnLoc[1].o, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
            m_creature->SummonCreature(NPC_HUMAN_MALE_CAPTIVE, aSpawnLoc[2].x, aSpawnLoc[2].y, aSpawnLoc[2].z, aSpawnLoc[2].o, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
        }        
        else if (uiSummonId == 2)
        {
            m_creature->SummonCreature(NPC_CAPTIVE_GHOUL, aSpawnLoc[0].x, aSpawnLoc[0].y, aSpawnLoc[0].z, aSpawnLoc[0].o, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);            
            m_creature->SummonCreature(NPC_CAPTIVE_ZOMBIE, aSpawnLoc[2].x, aSpawnLoc[2].y, aSpawnLoc[2].z, aSpawnLoc[2].o, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
        }        
        else if (uiSummonId == 3)
            m_creature->SummonCreature(NPC_CAPTIVE_ABOMINATION, aSpawnLoc[1].x, aSpawnLoc[1].y, aSpawnLoc[1].z, aSpawnLoc[1].o, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);        
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiEventTimer <= uiDiff)
        {
            if(!m_bEvent)
            {
                m_bEvent = true;
                m_uiSpeechTimer = 1000;
                m_uiSpeechStep = 1;
            }
            // start event again after 5-10 min
            m_uiEventTimer = urand(300000, 600000);
        }
        else
            m_uiEventTimer -= uiDiff;
        
        if (m_uiSpeechTimer && m_bEvent)
        {
            if(!m_uiSpeechStep)
                return;
        
            if(m_uiSpeechTimer <= uiDiff)
            {
                Creature* pLysta = GetClosestCreatureWithEntry(m_creature, NPC_LYSTA_BANCROFT, 10.0f);
                switch(m_uiSpeechStep)
                {                   
                    case 1:                        
                        // empty for now
                        m_uiSpeechTimer = 1000;
                        break;
                    case 2:                        
                        m_creature->MonsterSay("Edward, Tyler. Prepare for your first challenge.", LANG_GUTTERSPEAK, NULL);
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        m_uiSpeechTimer = 10000;
                        break;                        
                    case 3:                        
                        m_creature->MonsterSay("Lysta, summon in the captives.", LANG_GUTTERSPEAK, NULL);
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        m_uiSpeechTimer = 2000;
                        break;
                    case 4:
                        if(pLysta)
                            pLysta->CastSpell(pLysta, SPELL_SUMMON, false);
                        m_uiSpeechTimer = 6000;
                        break;
                    case 5:
                        // max one minute or we'll move on the event
                        DoSummonWave(1);
                        m_uiSpeechTimer = 60000;
                        break;
                    case 6:                        
                        m_creature->MonsterSay("Not a challenge at all it seems. Let us see how you handle your second test. Lysta, bring forth minions of the Lich King.", LANG_GUTTERSPEAK, NULL);
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        m_uiSpeechTimer = 10000;
                        break;
                    case 7:                        
                        m_creature->MonsterSay("Lysta, summon in undead captives.", LANG_GUTTERSPEAK, NULL);
                        m_creature->HandleEmote(EMOTE_ONESHOT_POINT);
                        m_uiSpeechTimer = 2000;
                        break;
                    case 8:
                        if(pLysta)
                            pLysta->CastSpell(pLysta, SPELL_SUMMON, false);
                        m_uiSpeechTimer = 6000;
                        break;
                    case 9:
                        // max one minute or we'll move on the event
                        DoSummonWave(2);
                        m_uiSpeechTimer = 60000;
                        break;
                    case 10:                        
                        m_creature->MonsterSay("It is time to face your final challenge, young warriors! Prepare for your hardest fight yet.", LANG_GUTTERSPEAK, NULL);
                        m_creature->HandleEmote(EMOTE_ONESHOT_EXCLAMATION);
                        m_uiSpeechTimer = 10000;
                        break;
                    case 11:                        
                        m_creature->MonsterSay("Lysta, summon forth... the abomination!", LANG_GUTTERSPEAK, NULL);
                        m_creature->HandleEmote(EMOTE_ONESHOT_LAUGH);
                        m_uiSpeechTimer = 2000;
                        break;
                    case 12:
                        if(pLysta)
                            pLysta->CastSpell(pLysta, SPELL_SUMMON, false);
                        m_uiSpeechTimer = 6000;
                        break;
                    case 13:
                        // max 1.5  minute or we'll move on the event
                        DoSummonWave(3);
                        m_uiSpeechTimer = 90000;
                        break;
                    case 14:
                        m_creature->MonsterSay("Well done Edward and Tyler. You are progressing along in your training quite nicely. We shall test your mettle again soon.", LANG_GUTTERSPEAK, NULL);
                        m_creature->HandleEmote(EMOTE_ONESHOT_APPLAUD);
                        m_uiDeadAdds = 0;
                        m_bEvent = false;
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

CreatureAI* GetAI_npc_brother_malach(Creature* pCreature)
{
    return new npc_brother_malachAI(pCreature);
}

/*######
## npc_samantha_shackleton
######*/

struct MANGOS_DLL_DECL npc_samantha_shackletonAI : public ScriptedAI
{
    npc_samantha_shackletonAI(Creature* pCreature) : ScriptedAI(pCreature) 
    { 
        m_uiSpeechTimer = 0;
        Reset(); 
    }

    uint8 textID;
    uint8 OldtextID;
    uint32 m_uiSpeechTimer;
    
    void Reset()
    {
    }
    
    void HandleQuote()
    {
        short attempts = 0;
        while(attempts < 20)
        {
            textID = urand(0, 15);
            
            // if we get a new text move on
            if(textID != OldtextID)
                break;
            
            ++attempts;
        }
                
        switch(textID)
        {
            case 0:
                m_creature->MonsterSay("If I could just find \"Magic and the Ways of Power\". Ahh here it is, what was it doing over here?", LANG_GUTTERSPEAK, NULL);
                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                break;
             case 1:
                m_creature->MonsterSay("That wasn't very helpful. Let me check the other one.", LANG_GUTTERSPEAK, NULL);
                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                break;
            case 2:
                m_creature->MonsterSay("Ahh, \"Mystical Conjurings of the Archmages of Dalaran.\" Perfect.", LANG_GUTTERSPEAK, NULL);
                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                break;
            case 3:
                m_creature->MonsterSay("But this can't be right. Where did I put that other book?", LANG_GUTTERSPEAK, NULL);
                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                break;
            case 4:
                m_creature->MonsterSay("\"Magic Maladies\", no. \"Magic Matricies\", no. Ahh here we go \"Magic Mysteries\".", LANG_GUTTERSPEAK, NULL);
                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                break;
            case 5:
                m_creature->MonsterSay("If this is correct, then I have a lot more research to do.", LANG_GUTTERSPEAK, NULL);
                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                break;
            case 6:
                m_creature->MonsterSay("\"Magical Flows and How They Effect the World\", by Maginor Dumas. That's the one I was looking for.", LANG_GUTTERSPEAK, NULL);
                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                break;
            case 7:
                m_creature->MonsterSay("That's what I wanted to know. Now I just need to find where the flux variance is discussed.", LANG_GUTTERSPEAK, NULL);
                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                break;
            case 8:
                m_creature->MonsterSay("So I was right about the energies. Let's see what the other tome has to say on the subject.", LANG_GUTTERSPEAK, NULL);
                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                break;
            case 9:
                m_creature->MonsterSay("Ahh there it is.", LANG_GUTTERSPEAK, NULL);
                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                break;
            case 10:
                m_creature->MonsterSay("Back to the drawing board.", LANG_GUTTERSPEAK, NULL);
                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                break;
            case 11:
                m_creature->MonsterSay("There's the mystical tome I was looking for.", LANG_GUTTERSPEAK, NULL);
                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                break;
            case 12:
                m_creature->MonsterSay("\"Khadgar's Mystical Journal, Volume 8.\" I think this is the one I wanted.", LANG_GUTTERSPEAK, NULL);
                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                break;
            case 13:
                m_creature->MonsterSay("I really need a better system for filing these books. Ahh that's the one I wanted.", LANG_GUTTERSPEAK, NULL);
                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                break;
            case 14:
                m_creature->MonsterSay("Yes, yes. Hmm, it seems to cross reference another tome. Guess I have to find that one too.", LANG_GUTTERSPEAK, NULL);
                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                break;
            case 15:
                m_creature->MonsterSay("Very interesting. But if that's the case then my theory is wrong. I must reread the other sections to see what I missed.", LANG_GUTTERSPEAK, NULL);
                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                break;
        }
        
        // save the text we just did as old
        OldtextID = textID;
    }

    void UpdateAI(const uint32 uiDiff)
    {

        if(m_uiSpeechTimer <= uiDiff)
        {                  
            HandleQuote();                       
            m_uiSpeechTimer = urand(30000, 60000);
        }
        else
            m_uiSpeechTimer -= uiDiff;

        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_samantha_shackleton(Creature* pCreature)
{
    return new npc_samantha_shackletonAI(pCreature);
}

/*######
## npc_jezelle_pruitt
######*/

enum
{
    SPELL_SUMMON_ = 7794,

    NPC_IMP = 5730,
    NPC_VOIDWALKER = 5729,    
    NPC_SUCCUBUS = 5728,
    NPC_FELHUNTER = 5726,
    NPC_FELSTEED = 5727   
};


static Loc aSumLoc[]= 
{
    {1793.60f, 129.38f, -63.84, 3.77f}
};

struct MANGOS_DLL_DECL npc_jezelle_pruittAI : public ScriptedAI
{
    npc_jezelle_pruittAI(Creature* pCreature) : ScriptedAI(pCreature) 
    { 
        Reset(); 
    }

    uint8 m_uiSpeechStep;
    uint32 m_uiSpeechTimer;
    
    uint32 m_uiEventTimer;

    bool m_bEvent;

    void Reset()
    {
        if(!m_bEvent)
        {
            m_uiSpeechStep = 1;
            m_uiSpeechTimer = 0;
            m_uiEventTimer = urand(10000, 60000);
        }
    }
    
    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->SetRespawnEnabled(false);            // make sure they won't respawn
        pSummoned->GetMotionMaster()->MoveRandom();
    }

    void DoSummonWave(uint32 uiSummonId = 0)
    {
        switch(uiSummonId)
        {
            case 0:
                m_creature->SummonCreature(NPC_IMP, aSumLoc[0].x, aSumLoc[0].y, aSumLoc[0].z, aSumLoc[0].o, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                break;
            case 1:
                m_creature->SummonCreature(NPC_VOIDWALKER, aSumLoc[0].x, aSumLoc[0].y, aSumLoc[0].z, aSumLoc[0].o, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                break;
            case 2:
                m_creature->SummonCreature(NPC_SUCCUBUS, aSumLoc[0].x, aSumLoc[0].y, aSumLoc[0].z, aSumLoc[0].o, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                break;
            case 3:
                m_creature->SummonCreature(NPC_FELHUNTER, aSumLoc[0].x, aSumLoc[0].y, aSumLoc[0].z, aSumLoc[0].o, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                break;
            case 4:
                m_creature->SummonCreature(NPC_FELSTEED, aSumLoc[0].x, aSumLoc[0].y, aSumLoc[0].z, aSumLoc[0].o, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                break;
        }
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiEventTimer <= uiDiff)
        {
            if(!m_bEvent)
            {
                m_bEvent = true;
                m_uiSpeechTimer = 1000;
                m_uiSpeechStep = 1;
            }
            // start event again after 5-10 min
            m_uiEventTimer = urand(300000, 600000);
        }
        else
            m_uiEventTimer -= uiDiff;
        
        if (m_uiSpeechTimer && m_bEvent)
        {
            if(!m_uiSpeechStep)
                return;
        
            if(m_uiSpeechTimer <= uiDiff)
            {
                switch(m_uiSpeechStep)
                {                   
                    case 1:              
                        m_creature->MonsterSay("If you're here, then it means you are prepared to begin this study of summoning demonic cohorts to do your bidding. We will start with the lowliest creatures you will be able to call and continue from there. Let us begin.", LANG_GUTTERSPEAK, NULL);
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        m_uiSpeechTimer = 10000;
                        break;
                    case 2:                        
                        m_creature->MonsterSay("The easiest creature for you to summon is the imp. You should already be able to bring forth this minion but for completeness' sake I will start with him.", LANG_GUTTERSPEAK, NULL);         
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        m_uiSpeechTimer = 3000;
                        break;                        
                    case 3:                        
                        m_creature->CastSpell(m_creature, SPELL_SUMMON_, false);
                        m_uiSpeechTimer = 1500;
                        break;
                    case 4:
                        DoSummonWave(0);
                        m_uiSpeechTimer = 3000;
                        break;
                    case 5:
                        m_creature->MonsterSay("This foul little beast is the imp. It is small and weak, making it almost useless as a meatshield, and its damage output is mediocre at best. This creature is best used for support of a larger group.", LANG_GUTTERSPEAK, NULL);         
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        m_uiSpeechTimer = 30000;
                        break;
                    case 6:                        
                        m_creature->MonsterSay("Now that you have had a chance to study the imp, let us move on the next minion you will be able to summon, the voidwalker.", LANG_GUTTERSPEAK, NULL);
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        m_uiSpeechTimer = 3000;
                        break;
                    case 7:                        
                        m_creature->CastSpell(m_creature, SPELL_SUMMON_, false);
                        m_uiSpeechTimer = 1500;
                        break;
                    case 8:
                        DoSummonWave(1);
                        m_uiSpeechTimer = 3000;
                        break;
                    case 9:
                        m_creature->MonsterSay("This demonic entity is know as the Voidwalker. Its strenght and endurance are significant, making it ideal for defense. Send it to attack your enemy, then use a shield while you use your spells and abilities to drain away your opponent's life.", LANG_GUTTERSPEAK, NULL);         
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        m_uiSpeechTimer = 30000;
                        break;
                    case 10:                        
                        m_creature->MonsterSay("If you've never seen one, it is a sight to behold. A very impressive creature both on and off the field of battle. Next, let us take a look at what I am sure all you male students have been waiting for. The succubus.", LANG_GUTTERSPEAK, NULL);
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        m_uiSpeechTimer = 3000;
                        break;
                    case 11:                        
                        m_creature->CastSpell(m_creature, SPELL_SUMMON_, false);
                        m_uiSpeechTimer = 1500;
                        break;
                    case 12:
                        DoSummonWave(2);
                        m_uiSpeechTimer = 3000;
                        break;
                    case 13:
                        m_creature->MonsterSay("All right now. Aside from the obvious distractions a minion like this will provide against your masculine foes, she is also capable of dealing out impressive amounts of damage. However, her fragile endurance makes her almost useless as a shield.", LANG_GUTTERSPEAK, NULL);         
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        m_uiSpeechTimer = 30000;
                        break;
                    case 14:                        
                        m_creature->MonsterSay("Study hard and you might one day be able to summon one on your own, but for now it's time to move to the felhunter.", LANG_GUTTERSPEAK, NULL);
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        m_uiSpeechTimer = 3000;
                        break;
                       case 15:                        
                        m_creature->CastSpell(m_creature, SPELL_SUMMON_, false);
                        m_uiSpeechTimer = 1500;
                        break;
                    case 16:
                        DoSummonWave(3);
                        m_uiSpeechTimer = 3000;
                        break;
                    case 17:
                        m_creature->MonsterSay("What you see before you is a felhunter. This creature's natural talents include spell lock and other abilities which make it unequalled when facing a magically attuned opponent.", LANG_GUTTERSPEAK, NULL);         
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        m_uiSpeechTimer = 30000;
                        break;
                    case 18:                        
                        m_creature->MonsterSay("When facing a spellcaster of any kind, this feral beast will be your best friend. Now, let us take a look at something a bit different. This next creature will aid your travels and make your future journeys much easier. Let's take a look at a felsteed.", LANG_GUTTERSPEAK, NULL);
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        m_uiSpeechTimer = 3000;
                        break; 
                    case 19:                        
                        m_creature->CastSpell(m_creature, SPELL_SUMMON_, false);
                        m_uiSpeechTimer = 1500;
                        break;
                    case 20:
                        DoSummonWave(4);
                        m_uiSpeechTimer = 3000;
                        break;
                    case 21:
                        m_creature->MonsterSay(" I doubt you have had much occasion to see such a creature. These demonic equines will make your travels much faster by acting as your mount as long as you control them. However, they are difficult to control, so be sure you are ready before attempting it.", LANG_GUTTERSPEAK, NULL);         
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        m_uiSpeechTimer = 30000;
                        break;
                    case 22:                        
                        m_creature->MonsterSay("There you have it. Our lesson on summoning has come to an end. A new class will begin shortly, so if you wish to brush up, feel free to stay around.", LANG_GUTTERSPEAK, NULL);
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        m_bEvent = false;
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

CreatureAI* GetAI_npc_jezelle_pruitt(Creature* pCreature)
{
    return new npc_jezelle_pruittAI(pCreature);
}

/*######
## npc_apothecary_keever
######*/

enum
{    
    NPC_SQUIRREL = 5739,
    NPC_RABBIT = 5741,
    NPC_TOAD = 5742,
    NPC_SHEEP = 5743,
    SPELL_EXPLODE = 7670,
};

struct MANGOS_DLL_DECL npc_apothecary_keeverAI : public ScriptedAI
{
    npc_apothecary_keeverAI(Creature* pCreature) : ScriptedAI(pCreature) 
    { 
        Reset(); 
    }

    uint8 m_uiSpeechStep;
    uint32 m_uiSpeechTimer;
    
    uint32 m_uiEventTimer;

    bool m_bEvent;

    void Reset()
    {
        m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
        if(!m_bEvent)
        {
            m_uiSpeechStep = 1;
            m_uiSpeechTimer = 0;
            m_uiEventTimer = urand(10000, 60000);
        }
    }
    
    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->SetRespawnEnabled(false);            // make sure they won't respawn
        pSummoned->SetFactionTemporary(68);
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiEventTimer <= uiDiff)
        {
            if(!m_bEvent)
            {
                m_bEvent = true;
                m_uiSpeechTimer = 1000;
                m_uiSpeechStep = 1;
            }
            // start event again after 5-10 min
            m_uiEventTimer = urand(300000, 600000);
        }
        else
            m_uiEventTimer -= uiDiff;
        
        if (m_uiSpeechTimer && m_bEvent)
        {
            if(!m_uiSpeechStep)
                return;
        
            if(m_uiSpeechTimer <= uiDiff)
            {
                Creature* pHuman = GetClosestCreatureWithEntry(m_creature, NPC_HUMAN_MALE_CAPTIVE, 10.0f);
                Creature* pToad = GetClosestCreatureWithEntry(m_creature, NPC_TOAD, 10.0f);
                Creature* pSquirrel = GetClosestCreatureWithEntry(m_creature, NPC_SQUIRREL, 10.0f);
                Creature* pRabbit = GetClosestCreatureWithEntry(m_creature, NPC_RABBIT, 10.0f);
                Creature* pSheep = GetClosestCreatureWithEntry(m_creature, NPC_SHEEP, 10.0f);
                switch(m_uiSpeechStep)
                {                   
                    case 1:                        
                        m_creature->MonsterSay("Hmm, it would seem Keever needs a new subject. If that fool, Abernathy, keeps taking Keever's subjects, Keever may have to have a word with him.", LANG_GUTTERSPEAK, NULL);
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        m_uiSpeechTimer = 5000;
                        break;
                    case 2:                        
                        m_creature->CastSpell(m_creature, SPELL_SUMMON_, false);
                        m_uiSpeechTimer = 1500;
                        break;                
                    case 3:                 
                        m_creature->SummonCreature(NPC_HUMAN_MALE_CAPTIVE, 1400.85f, 363.242f, -84.868f, 1.11701f, TEMPSUMMON_TIMED_DESPAWN, 120000);
                        m_uiSpeechTimer = 2000;
                        break;
                    case 4:
                        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                        m_uiSpeechTimer = 6000;
                        break;
                    case 5:
                        m_creature->MonsterSay("Ahh, there we go, now Keever must try this vial and see if it works.", LANG_GUTTERSPEAK, NULL);
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        m_uiSpeechTimer = 8000;
                        break;
                    case 6:         
                        m_creature->GenericTextEmote("Apothecary Keever forces the fluid down the weary man's throat.", NULL, false);
                        m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                        m_uiSpeechTimer = 2000;
                        break;
                    case 7:              
                        if(pHuman)
                        {
                            pHuman->SetEntry(NPC_TOAD);
                            pHuman->SetDisplayId(901);
                            pHuman->UpdateModelData();
                        }
                        m_uiSpeechTimer = 8000;
                        break;
                    case 8:
                        m_creature->GenericTextEmote("Apothecary Keever pokes the small toad.", NULL, false);
                        m_uiSpeechTimer = 8000;
                        break;
                    case 9:
                        m_creature->MonsterSay("Not what Keever was hoping for. Keever may have added too much earthroot. Let's see if the second serum will do what I need.", LANG_GUTTERSPEAK, NULL);
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                         m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                        m_uiSpeechTimer = 10000;
                        break;
                    case 10:                        
                        m_creature->GenericTextEmote("Apothecary Keever feeds the toad some of the strange liquid.", NULL, false);
                        m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                        m_uiSpeechTimer = 2000;
                        break;
                    case 11:              
                        if(pToad)
                        {
                            pToad->SetEntry(NPC_SQUIRREL);
                            pToad->SetDisplayId(134);
                        }
                        m_uiSpeechTimer = 5000;
                        break;
                    case 12:                        
                        m_creature->GenericTextEmote("Apothecary Keever pokes the fuzzy squirrel with obvious disappointment.", NULL, false);
                        m_uiSpeechTimer = 10000;
                        break;
                    case 13:
                        m_creature->MonsterSay("Well, this is just not right. The creature is far too small. Let us see what Keever's third batch will do.", LANG_GUTTERSPEAK, NULL);
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                         m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                        m_uiSpeechTimer = 10000;
                        break;
                    case 14:                        
                        m_creature->GenericTextEmote("Apothecary Keever feeds the squirrel some of the viscous fluid.", NULL, false);
                        m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                        m_uiSpeechTimer = 2000;
                        break;
                    case 15:              
                        if(pSquirrel)
                        {
                            pSquirrel->SetEntry(NPC_RABBIT);
                            pSquirrel->SetDisplayId(328);
                        }
                        m_uiSpeechTimer = 5000;
                        break;
                    case 16:                        
                        m_creature->GenericTextEmote("Apothecary Keever pokes the skittish rabbit.", NULL, false);
                        m_uiSpeechTimer = 10000;
                        break;
                    case 17:
                        m_creature->MonsterSay("Keever is unhappy with this. Prehaps if Keever were to try a larger dose, that may fix this dilemma.", LANG_GUTTERSPEAK, NULL);
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                         m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                        m_uiSpeechTimer = 10000;
                        break;
                    case 18:                        
                        m_creature->GenericTextEmote("Apothecary Keever grabs the rabbit and pours the liquid down its throat.", NULL, false);
                        m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                        m_uiSpeechTimer = 2000;
                        break;
                    case 19:              
                        if(pRabbit)
                        {
                            pRabbit->SetEntry(NPC_SHEEP);
                            pRabbit->SetDisplayId(856);
                        }
                        m_uiSpeechTimer = 5000;
                        break;
                    case 20:
                        m_creature->MonsterSay("What is this? Did Keever ask for a sheep? Keever wanted a weapon of great power and all he got was this sheep. Keever is very disappointed.", LANG_GUTTERSPEAK, NULL);
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                         m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                        m_uiSpeechTimer = 10000;
                        break;
                    case 21:                        
                        m_creature->GenericTextEmote("Apothecary Keever pokes the wooly sheep repeatedly.", NULL, false);
                        m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                        m_uiSpeechTimer = 2000;
                        break;
                    case 22:              
                        if(pSheep)
                            pSheep->CastSpell(pHuman, SPELL_EXPLODE, true);
                        m_uiSpeechTimer = 3000;
                        break;    
                    case 23:
                        m_creature->MonsterSay("Keever is most pleased.", LANG_GUTTERSPEAK, NULL);
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        m_bEvent = false;
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

CreatureAI* GetAI_npc_apothecary_keever(Creature* pCreature)
{
    return new npc_apothecary_keeverAI(pCreature);
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
    
    pNewscript = new Script;
    pNewscript->Name = "npc_brother_malach";
    pNewscript->GetAI = &GetAI_npc_brother_malach;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_samantha_shackleton";
    pNewscript->GetAI = &GetAI_npc_samantha_shackleton;
    pNewscript->RegisterSelf();
    
    pNewscript = new Script;
    pNewscript->Name = "npc_jezelle_pruitt";
    pNewscript->GetAI = &GetAI_npc_jezelle_pruitt;
    pNewscript->RegisterSelf(); 
    
    pNewscript = new Script;
    pNewscript->Name = "npc_apothecary_keever";
    pNewscript->GetAI = &GetAI_npc_apothecary_keever;
    pNewscript->RegisterSelf(); 
}
