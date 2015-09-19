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
SDComment: Quest support: 660, 665, 667
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

/*######
## npc_shakes_obreen
######*/

enum
{
	SAY_START							= -1720029,
	NAGA_YELL_1							= -1720030,
	SAY_SHAKES_HOLD						= -1720031,

	WAVE_W_TWO							= 2,
	WAVE_W_THREE						= 3,

	NPC_DAGGERSPINE_MARAUDER			= 2775,

	QUEST_DEATH_FROM_BELOW				= 667,
};

struct Loc
{
    float x, y, z;
};

static Loc aMobSpawnLoc[]= 
{
	{-2149.34f, -1974.08f, 14.86f},
	{-2154.04f, -1977.88f, 14.62f},
	{-2149.85f, -1978.95f, 13.86f},
};

struct MANGOS_DLL_DECL npc_shakes_obreenAI : public npc_escortAI
{
    npc_shakes_obreenAI(Creature* pCreature) : npc_escortAI(pCreature) 
	{ 
		Reset(); 
	}

	uint8 m_uiKilledCreatures;
	uint8 m_uiPhase;
	uint8 m_uiCurrentWave;

	uint32 m_uiEventTimer;

	ObjectGuid m_uiPlayerGUID;

	bool m_bNagaSay;

    void Reset() 
	{ 
	}

	void QuestEndReset()				// make sure another player can do the quest after it's done
	{
		m_uiEventTimer      = 0;
        m_uiKilledCreatures = 0;
		m_uiPhase           = 0;
		m_uiCurrentWave     = 0;
		m_uiPlayerGUID.Clear();
		m_bNagaSay = false;
	}

    void Aggro(Unit* /*pWho*/)
    {
    }

	void WaypointReached(uint32 uiPointId)
	{
	}

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->AI()->AttackStart(m_creature);
		pSummoned->SetRespawnDelay(-10);			// make sure they won't respawn
		if (!m_bNagaSay)
		{
			DoScriptText(NAGA_YELL_1, pSummoned);
			m_bNagaSay = true;
		}
    }

	void DoSummonWave(uint32 uiSummonId = 0)
    {
        if (uiSummonId == WAVE_W_TWO)
        {
            for (uint8 i = 0; i < 2; ++i)
            {
                m_creature->SummonCreature(NPC_DAGGERSPINE_MARAUDER, aMobSpawnLoc[i].x, aMobSpawnLoc[i].y, aMobSpawnLoc[i].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 20000);
            }

            ++m_uiCurrentWave;
        }
        else if (uiSummonId == WAVE_W_THREE)
        {
            for (uint8 i = 0; i < 3; ++i)
            {
                m_creature->SummonCreature(NPC_DAGGERSPINE_MARAUDER, aMobSpawnLoc[i].x, aMobSpawnLoc[i].y, aMobSpawnLoc[i].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 20000);
            }
			++m_uiCurrentWave;
        }
    }

	void JustDied(Unit* /*pKiller*/)			// fail quest if Shakes dies
	{
		if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID))
			pPlayer->FailQuest(QUEST_DEATH_FROM_BELOW);
		QuestEndReset();			// stop waves
	}

	void SummonedCreatureJustDied(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_DAGGERSPINE_MARAUDER)
        {
            ++m_uiKilledCreatures;
		}
		
		// Event ended
        if (m_uiKilledCreatures >= 8 && m_uiCurrentWave == 3)
        {
			if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID))			// give quest credit when all waves are dead
				pPlayer->AreaExploredOrEventHappens(QUEST_DEATH_FROM_BELOW);
		}
	}

	ObjectGuid DoStartEvent(ObjectGuid player_guid)
	{
		QuestEndReset();			// reset so next player can do quest again
		if (m_uiPlayerGUID)
            return ObjectGuid();

		m_uiEventTimer  =  15000;

		m_uiPlayerGUID = player_guid;
        
        return m_uiPlayerGUID;
	}

	void UpdateAI(const uint32 uiDiff)
	{
		npc_escortAI::UpdateAI(uiDiff);
		//return since we have no target
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
			return;

		DoMeleeAttackIfReady();
	}

	void UpdateEscortAI(const uint32 uiDiff)
    {
		if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID))
        {
			if (m_uiEventTimer)					// summon waves
			{
				if (m_uiEventTimer <= uiDiff)
				{
					switch (m_uiPhase)
					{
						case 0:
							DoSummonWave(WAVE_W_THREE);
							m_uiEventTimer = 35000;
							break;
						case 1:
							DoSummonWave(WAVE_W_TWO);
							m_uiEventTimer = 30000;
							break;
						case 2:
							DoScriptText(SAY_SHAKES_HOLD, m_creature);
							m_uiEventTimer = 15000;
							break;
						case 3:
							DoSummonWave(WAVE_W_THREE);
							m_uiEventTimer = 0;
							break;
					}
				++m_uiPhase;
				}
				else
				{
					m_uiEventTimer -= uiDiff;
				}
			}
		}
	}
};

bool QuestAccept_npc_shakes_obreen(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_DEATH_FROM_BELOW)
    {
        DoScriptText(SAY_START, pCreature, pPlayer);

        if (npc_shakes_obreenAI* pEscortAI = dynamic_cast<npc_shakes_obreenAI*>(pCreature->AI()))
        { 
			pEscortAI->Start(false, pPlayer, pQuest, true);
			pEscortAI->DoStartEvent(pPlayer->GetGUID());
		}
			
    }
    return true;
}

CreatureAI* GetAI_npc_shakes_obreen(Creature* pCreature)
{
    return new npc_shakes_obreenAI(pCreature);
}

/*####
# npc_gormul
####*/

enum
{
	GORMUL_SAY_1					 = -1720049,
	GORMUL_SAY_2					 = -1720050,

	SPELL_GUILE_OF_THE_RAPTOR		 = 4153,

    QUEST_ID_GUILE_OF_THE_RAPTOR	 = 702,
};

struct MANGOS_DLL_DECL npc_gormulAI : public npc_escortAI
{
    npc_gormulAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }
	
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

	void Aggro(Unit* /*pWho*/)
    {
        m_creature->CastSpell(m_creature, SPELL_GUILE_OF_THE_RAPTOR, true);
    }

	void StartOutro()
	{
		m_bOutro = true; 
		m_uiSpeechTimer = 3000;
		m_uiSpeechStep = 1;
		m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
		m_creature->CastSpell(m_creature, SPELL_GUILE_OF_THE_RAPTOR, false);
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
                        m_creature->GenericTextEmote("Gor'mul watches as his blood begins to stir, and the guile of the raptor slips through him.", NULL, false);
						m_uiSpeechTimer = 5000;
                        break;
					case 2:
						DoScriptText(GORMUL_SAY_1, m_creature);
                        m_uiSpeechTimer = 3000;
                        break;
					case 3:
						DoScriptText(GORMUL_SAY_2, m_creature);
                        m_uiSpeechTimer = 2500;
						break;
					case 4:
						m_creature->HandleEmote(EMOTE_ONESHOT_FLEX);
						m_uiSpeechTimer = 3000;
						break;
					case 5:
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

CreatureAI* GetAI_npc_gormul(Creature* pCreature)
{
    return new npc_gormulAI(pCreature);
}

bool OnQuestRewarded_npc_gormul(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
	if (pQuest->GetQuestId() == QUEST_ID_GUILE_OF_THE_RAPTOR)
    {
		if (npc_gormulAI* pEscortAI = dynamic_cast<npc_gormulAI*>(pCreature->AI()))
			pEscortAI->StartOutro();
	}
	return true;
}

/*######
## npc_kovork
######*/

enum eKovork
{
	SPELL_FRENZY		= 8269,
	EMOTE_FRENZY		= -1000002,
};

struct MANGOS_DLL_DECL npc_kovorkAI : public ScriptedAI
{
	npc_kovorkAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

	bool m_bFrenzy;

	void Reset() 
	{
		m_bFrenzy = false;
	}

	void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_FRENZY)
		{
			DoScriptText(EMOTE_FRENZY,m_creature,NULL);
			m_bFrenzy = true;
		}
    }

	void JustDied(Unit* /*pKiller*/)
    {
		uint32 respawn_time = urand(18000,28800);
	
		m_creature->SetRespawnDelay(respawn_time);
		m_creature->SetRespawnTime(respawn_time);
		m_creature->SaveRespawnTime();
    }

	void UpdateAI(const uint32 uiDiff)
	{
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
			return;

		if(HealthBelowPct(50) && !m_bFrenzy)
			m_creature->CastSpell(m_creature, SPELL_FRENZY,true);

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_kovork(Creature* pCreature)
{
    return new npc_kovorkAI(pCreature);
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

	pNewscript = new Script;
    pNewscript->Name = "npc_shakes_obreen";
    pNewscript->GetAI = &GetAI_npc_shakes_obreen;
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_shakes_obreen;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_gormul";
    pNewscript->GetAI = &GetAI_npc_gormul;
    pNewscript->pQuestRewardedNPC = &OnQuestRewarded_npc_gormul;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_kovork";
    pNewscript->GetAI = &GetAI_npc_kovork;
    pNewscript->RegisterSelf();
}
