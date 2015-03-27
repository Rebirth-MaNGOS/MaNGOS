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
#include "TemporarySummon.h"

#include <vector>

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
        m_creature->SetActiveObjectState(true);
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


/*########################################################################
 * npc_pats_hellfire_guy                                                ##
 * AI for the mob spawning objects when the Heart of Hakkar is created. ##
########################################################################*/

/*
 * 4 sec after SB hit -> first channel
 * +2 s second and third channel
 * +2 s fourth channel and heart appears
 * +10 s clouds appear
 * +12 s explosion happens, event ends
 */

enum HakkarRitualSpell
{
    SPELL_CHANNEL = 24217, // The spell the summoning circle should cast. Its target is set in the DB.
    SPELL_EXPLOSION = 24207, // The spell that finishes the event and makes the explosion happen.
    SPELL_CREATE_RIFT = 24202, // The spell that summons the cloud animation.
    SPELL_CREATE_SUMMON_CIRCLE = 24602, // The spell that creates a swirly animation around the channelers. We don't use this one.
    SPELL_SUMMON_PAT = 24215, // Spell that can be used to summon Pat.
    SPELL_HEART_BOLT = 24214, // The Spell that Molthor uses to initiate the event.
    SPELL_LONGSHAKE = 24203,
    SPELL_SHORT_SHAKE = 24204,
    SPELL_BUFF = 24425
};

enum HakkarRitualObject
{
        OBJ_HEART = 180402,
        OBJ_SUMMONING_CIRCLE = 180404
};

struct MANGOS_DLL_DECL npc_pats_hellfire_guy : public ScriptedAI
{
    explicit npc_pats_hellfire_guy(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }
    
    uint32 m_uiEventTimer;
    uint32 m_uiEventStage;
    bool m_bDone;
    
    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_uiEventStage = 0;
        m_uiEventTimer = 4000;
        m_bDone = false;
    }
    
    void UpdateAI(const uint32 uiDiff)
    {           
        if (!m_bDone)
        {
            if (m_uiEventTimer <= uiDiff)
            {
                switch (m_uiEventStage)
                {
                case 0: // Spawn the heart
                    m_creature->SummonGameObject(HakkarRitualObject::OBJ_HEART, 22000, m_creature->GetPositionX() + 0.35f, m_creature->GetPositionY() - 0.35f, m_creature->GetPositionZ() + 0.5f, 0.f);
                    
                    m_uiEventTimer = 8000;
                    ++m_uiEventStage;
                    break;
                case 1: // Clouds appear
                    DoCast(m_creature, HakkarRitualSpell::SPELL_CREATE_RIFT, true);
                    
                    // Shake the ground
                    DoCast(m_creature, HakkarRitualSpell::SPELL_LONGSHAKE, true);
                    
                    m_uiEventTimer = 12000;
                    ++m_uiEventStage;
                    break;
                case 2: // Cast the explosion and despawn.
                    // Shake on explosion.
                    DoCast(m_creature, HakkarRitualSpell::SPELL_SHORT_SHAKE, true);
                    
                    DoCast(m_creature, HakkarRitualSpell::SPELL_EXPLOSION, true);
                    
                    
                    m_uiEventTimer = 8000;
                    ++m_uiEventStage;
                    break;
                    
                case 3:
                {
                    TemporarySummon* pTemp = dynamic_cast<TemporarySummon*>(m_creature);
                    if (pTemp)
                        pTemp->ForcedDespawn();
                    
                    m_bDone = true;
                    break;
                }
                }
            }
            else
                m_uiEventTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_pats_hellfire_guy(Creature* pCreature)
{
    return new npc_pats_hellfire_guy(pCreature);
}


/*###############
 * npc_molthor ##
 *#############*/

enum HakkarRitualQuest
{
    ID = 8183
};

enum HakkariRitualNPCs
{
    NPC_PATS_HELLFIRE_GUY = 15073,
    NPC_SERVANT_OF_THE_HAND = 15080
};

enum HakkariRitualTalk
{
    SAY_FIRST = -1720001,
    SHOUT = -1720002
};

const float fLocations[3][3] = { { -11820.f, 1249.f, 2.6f }, { -11836.f, 1294.f, 0.53f }, { -11825.f, 1324.f, 0.48f } };
const float fSummonerLocations[4][4] = { { -11817.63f, 1325.02f, 1.46f, 1.23f }, { -11801.25f, 1335.36f, 1.27f, 2.73f}, 
                                         { -11834.92f, 1349.61f, 2.01f, 5.86f }, { -11831.56f, 1331.34f, 1.84f, 1.15f } };

struct MANGOS_DLL_DECL npc_molthor : public ScriptedAI
{
    explicit npc_molthor(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_creature->SetActiveObjectState(true);
        Reset();
    }
    
    uint32 m_uiEventStage;
    uint32 m_uiEventTimer;
    
    std::vector<Creature*> m_Summons;
    
    void Reset()
    {
        m_uiEventStage = 0;
        m_uiEventTimer = 0;
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiEventStage)
        {
            if (m_uiEventTimer <= uiDiff)
            {
                switch (m_uiEventStage)
                {
                case 1:
                    DoScriptText(HakkariRitualTalk::SAY_FIRST, m_creature);
                    
                    m_uiEventTimer = 3000;
                    ++m_uiEventStage;
                    break;
                case 2:
                    m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                    m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    m_creature->GetMotionMaster()->MovePoint(0, fLocations[0][0], fLocations[0][1], fLocations[0][2]);
                    
                    for (short i = 0; i < 4; i++)
                        m_creature->SummonGameObject(HakkarRitualObject::OBJ_SUMMONING_CIRCLE, 50000, fSummonerLocations[i][0], 
                                                     fSummonerLocations[i][1], fSummonerLocations[i][2], fSummonerLocations[i][3]);
                    
                    m_uiEventTimer = 4000;
                    ++m_uiEventStage;
                    break;
                case 3:
                    for (short i = 0; i < 4; ++i)
                    {
                        Creature* summon = m_creature->SummonCreature(HakkariRitualNPCs::NPC_SERVANT_OF_THE_HAND, fSummonerLocations[i][0], fSummonerLocations[i][1], 
                                                                    fSummonerLocations[i][2], fSummonerLocations[i][3], TEMPSUMMON_TIMED_DESPAWN, 46000);
                        
                        if (summon)
                            m_Summons.push_back(summon);
                    }                   
                    
                    ++m_uiEventStage;
                    break;
                case 4:
                    // Wait for Molthor to reach his destination. m_uiEventStage is incremented in WaypointReached.
                    break;
                case 5:
                    DoCast(m_creature, HakkarRitualSpell::SPELL_HEART_BOLT);
                    
                    m_uiEventTimer = 6000;
                    ++m_uiEventStage;
                    break;
                case 6:
                    DoScriptText(HakkariRitualTalk::SHOUT, m_creature);
                    
                    m_uiEventTimer = 2000;
                    ++m_uiEventStage;
                    break;
                case 7:
                    m_creature->SummonCreature(HakkariRitualNPCs::NPC_PATS_HELLFIRE_GUY, -11818.61f, 1344.06f, 8.f, 0.f, TEMPSUMMON_TIMED_DESPAWN, 50000);
                    
                    if (m_Summons.size() == 4)
                        m_Summons[3]->CastSpell(m_Summons[3], HakkarRitualSpell::SPELL_CHANNEL, false);
                    
                    m_uiEventTimer = 2000;
                    ++m_uiEventStage;
                    break;
                case 8:
                    if (m_Summons.size() == 4)
                    {
                        m_Summons[0]->CastSpell(m_Summons[0], HakkarRitualSpell::SPELL_CHANNEL, false);
                        m_Summons[1]->CastSpell(m_Summons[1], HakkarRitualSpell::SPELL_CHANNEL, false);
                    }
                    
                    m_uiEventTimer = 2000;
                    ++m_uiEventStage;
                    break;
                case 9:
                    if (m_Summons.size() == 4)
                        m_Summons[2]->CastSpell(m_Summons[2], HakkarRitualSpell::SPELL_CHANNEL, false);
                    
                    m_uiEventTimer = 20000;
                    ++m_uiEventStage;
                    break;
                case 10:
                    for (Creature* current_creature : m_Summons)
                        current_creature->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL, true);
                    
                    m_Summons.clear();
                    
                    m_uiEventTimer = 8000;
                    ++m_uiEventStage;
                    break;
                case 11:
                    DoCast(m_creature, HakkarRitualSpell::SPELL_BUFF, true);
                    
                    m_uiEventTimer = 1000;
                    ++m_uiEventStage;
                    break;
                case 12:
                    m_creature->GetMotionMaster()->MovePoint(3, fLocations[1][0], fLocations[1][1], fLocations[1][2]);
                    
                    ++m_uiEventStage;
                    break;
                }
            }
            else
                m_uiEventTimer -= uiDiff;
        }
        
        if (m_creature->SelectHostileTarget() && m_creature->getVictim())
            DoMeleeAttackIfReady();
    }
    
    void MovementInform(uint32 /*uiMotionType*/, uint32 uiPointId)
    {
        if (uiPointId < 2)
        {
            m_creature->GetMotionMaster()->MovePoint(uiPointId + 1, fLocations[uiPointId + 1][0], 
                                                     fLocations[uiPointId + 1][1], fLocations[uiPointId + 1][2]);
        }
        else if (uiPointId == 2)
        {
            ++m_uiEventStage;
        }
        else if (uiPointId == 3)
            m_creature->GetMotionMaster()->MoveTargetedHome();
    }
    
    void JustReachedHome()
    {
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }
    
    void StartEvent()
    {
        m_uiEventStage = 1;
    }
};

bool QuestRewardedMolthor(Player* /*pPlayer*/, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == HakkarRitualQuest::ID)
    {
        npc_molthor* pAI = dynamic_cast<npc_molthor*>(pCreature->AI());
        
        if (pAI)
            pAI->StartEvent();
    }
    return false; // We just do some extra things. Let the core handle the rewarding.
}

CreatureAI* GetAI_npc_molthor(Creature* pCreature)
{
    return new npc_molthor(pCreature);
}

/*####
# npc_kinweelay
####*/

enum
{
	KINWEELAY_SAY_1					 = -1720052,
	KINWEELAY_SAY_1_1240			 = -1720053,
	KINWEELAY_SAY_2_1240			 = -1720054,

	SPELL_SOUL_GEM					 = 3660,
	SPELL_SPEAK_WITH_HEADS			 = 3644,

    QUEST_ID_THE_MINDS_EYE			 = 591,
	QUEST_ID_THE_TROLL_WITCHDOCTOR	 = 1240,
	
};

struct MANGOS_DLL_DECL npc_kinweelayAI : public npc_escortAI
{
    npc_kinweelayAI(Creature* pCreature) : npc_escortAI(pCreature) 
	{ 
		Reset(); 
	}
	
	uint8 m_uiSpeechStep;
	uint32 m_uiSpeechTimer;
	bool m_bOutro591;

	uint8 m_uiSpeechStep2;
	uint32 m_uiSpeechTimer2;
	bool m_bOutro1240;

	ObjectGuid m_uiPlayerGUID;

    void Reset()
	{
		m_bOutro591 = false;
		m_bOutro1240 = false;
		m_uiSpeechStep = 1;
		m_uiSpeechStep2 = 1;
		m_uiSpeechTimer = 0;
		m_uiSpeechTimer2 = 0;
		m_uiPlayerGUID.Clear();
	}

	void WaypointReached(uint32 uiPointId)
    {
	}

	void JustStartedEscort()
    {
    }

	void StartOutro(ObjectGuid pPlayerGUID, uint32 uiQuestId = 0)
	{
		 if (!pPlayerGUID)
            return;

        m_uiPlayerGUID = pPlayerGUID;
		m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
		
		if (uiQuestId == 591)
		{
			m_bOutro591 = true; 
			m_uiSpeechTimer = 1000;
			m_uiSpeechStep = 1;
		}
		if (uiQuestId == 1240)
		{
			DoScriptText(KINWEELAY_SAY_1_1240, m_creature);
			m_bOutro1240 = true; 
			m_uiSpeechTimer2 = 2000;
			m_uiSpeechStep2 = 1;
		}
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
		if (m_uiSpeechTimer && m_bOutro591)							// handle RP at quest end 591
		{
			if (!m_uiSpeechStep)
				return;
		
			if (m_uiSpeechTimer <= uiDiff)
            {
				Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID);
                switch(m_uiSpeechStep)
                {					
                    case 1:
						m_creature->GenericTextEmote("Kin'weelay shows The Mind's Eye and Singing Crystals...", NULL, false);
						m_uiSpeechTimer = 4000;
                        break;
					case 2:
						m_creature->CastSpell(m_creature, SPELL_SOUL_GEM, false);
                        m_uiSpeechTimer = 3000;
                        break;
					case 3:
						DoScriptText(KINWEELAY_SAY_1, m_creature, pPlayer);
                        m_uiSpeechTimer = 2000;
						break;
					case 4:
						m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
						m_bOutro591 = false;
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

		if (m_uiSpeechTimer2 && m_bOutro1240)							// handle RP at quest end 1240
		{
			if (!m_uiSpeechStep2)
				return;
		
			if (m_uiSpeechTimer2 <= uiDiff)
            {
				Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID);
                switch(m_uiSpeechStep2)
                {					
                    case 1:
						m_creature->GetMotionMaster()->MovePoint(1, -12345.05f, 163.83f, 2.96f);
						m_uiSpeechTimer2 = 1500;
                        break;
					case 2:
						m_creature->SetFacingTo(0.34f);
						m_uiSpeechTimer2 = 1500;
						break;
					case 3:
						m_creature->HandleEmote(EMOTE_ONESHOT_ATTACK1H);
                        m_creature->GenericTextEmote("Kin'weelay places Marg's head in the cauldron.", NULL, false);
						m_uiSpeechTimer2 = 3000;
                        break;
					case 4:
						m_creature->CastSpell(m_creature, SPELL_SPEAK_WITH_HEADS, false);
                        m_uiSpeechTimer2 = 5000;
                        break;
					case 5:
						DoScriptText(KINWEELAY_SAY_2_1240, m_creature, pPlayer);
                        m_uiSpeechTimer2 = 3000;
						break;
					case 6:
						m_creature->GetMotionMaster()->MoveTargetedHome();
						m_uiSpeechTimer2 = 2000;
						break;
					case 7:
						m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
						m_bOutro1240 = false;
						break;
                    /*default:
                        m_uiSpeechStep = 0;
                        return;*/
                }
                ++m_uiSpeechStep2;
            }
            else
                m_uiSpeechTimer2 -= uiDiff;
		}
	}
};

CreatureAI* GetAI_npc_kinweelay(Creature* pCreature)
{
    return new npc_kinweelayAI(pCreature);
}

bool OnQuestRewarded_npc_kinweelay(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
	if (pQuest->GetQuestId() == QUEST_ID_THE_MINDS_EYE)
    {
		if (npc_kinweelayAI* pEscortAI = dynamic_cast<npc_kinweelayAI*>(pCreature->AI()))
			pEscortAI->StartOutro(pPlayer->GetObjectGuid(), 591);
	}
	if (pQuest->GetQuestId() == QUEST_ID_THE_TROLL_WITCHDOCTOR)
    {
		if (npc_kinweelayAI* pEscortAI = dynamic_cast<npc_kinweelayAI*>(pCreature->AI()))
			pEscortAI->StartOutro(pPlayer->GetObjectGuid(), 1240);
	}
	return true;
}

/*####
# npc_sea_wolf_mackinley
####*/

enum
{
	SEA_WOLF_SAY_1					= -1720059,

	QUEST_ID_VOODOO_DUES			= 609,
};

struct MANGOS_DLL_DECL npc_sea_wolf_mackinleyAI : public npc_escortAI
{
    npc_sea_wolf_mackinleyAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }
	
	uint8 m_uiSpeechStep;
	uint32 m_uiSpeechTimer;
	bool m_bOutro;

	ObjectGuid m_uiPlayerGUID;

    void Reset()
	{
		m_bOutro = false;
		m_uiSpeechStep = 1;
		m_uiSpeechTimer = 0;
		m_uiPlayerGUID.Clear();
	}

	void WaypointReached(uint32 uiPointId)
    {
	}

	void JustStartedEscort()
    {
    }

	void StartOutro(ObjectGuid pPlayerGUID)
	{
		if (!pPlayerGUID)
            return;

        m_uiPlayerGUID = pPlayerGUID;

		m_bOutro = true; 
		m_uiSpeechTimer = 5000;
		m_uiSpeechStep = 1;
		m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
		m_creature->GenericTextEmote("\"Sea Wolf\" MacKinley tries opening Maury's Clubbed Foot...", NULL, false);
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
				Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID);
                switch(m_uiSpeechStep)
                {
					case 1:
						DoScriptText(SEA_WOLF_SAY_1, m_creature, pPlayer);
                        m_uiSpeechTimer = 1000;
					case 2:
						m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
						m_bOutro = false;
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

CreatureAI* GetAI_npc_sea_wolf_mackinley(Creature* pCreature)
{
    return new npc_sea_wolf_mackinleyAI(pCreature);
}

bool OnQuestRewarded_npc_sea_wolf_mackinley(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
	if (pQuest->GetQuestId() == QUEST_ID_VOODOO_DUES)
    {
		if (npc_sea_wolf_mackinleyAI* pEscortAI = dynamic_cast<npc_sea_wolf_mackinleyAI*>(pCreature->AI()))
			pEscortAI->StartOutro(pPlayer->GetObjectGuid());
	}
	return true;
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
    
    pNewscript = new Script;
    pNewscript->Name = "npc_pats_hellfire_guy";
    pNewscript->GetAI = &GetAI_npc_pats_hellfire_guy;
    pNewscript->RegisterSelf();
    
    pNewscript = new Script;
    pNewscript->Name = "npc_molthor";
    pNewscript->GetAI = &GetAI_npc_molthor;
    pNewscript->pQuestRewardedNPC = &QuestRewardedMolthor;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_kinweelay";
    pNewscript->GetAI = &GetAI_npc_kinweelay;
    pNewscript->pQuestRewardedNPC = &OnQuestRewarded_npc_kinweelay;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_sea_wolf_mackinley";
    pNewscript->GetAI = &GetAI_npc_sea_wolf_mackinley;
    pNewscript->pQuestRewardedNPC = &OnQuestRewarded_npc_sea_wolf_mackinley;
    pNewscript->RegisterSelf();
}
