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
SDName: Darkshore
SD%Complete: 100
SDComment: Quest support: 963, 731, 945, 2078, 5321, 5713
SDCategory: Darkshore
EndScriptData */

/* ContentData
npc_kerlonian
npc_prospector_remtravel
npc_threshwackonator
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"
#include "follower_ai.h"

/*####
# npc_kerlonian
####*/

enum
{
    SAY_KER_START               = -1000434,

    EMOTE_KER_SLEEP_1           = -1000435,
    EMOTE_KER_SLEEP_2           = -1000436,
    EMOTE_KER_SLEEP_3           = -1000437,

    SAY_KER_SLEEP_1             = -1000438,
    SAY_KER_SLEEP_2             = -1000439,
    SAY_KER_SLEEP_3             = -1000440,
    SAY_KER_SLEEP_4             = -1000441,

    EMOTE_KER_AWAKEN            = -1000445,

    SAY_KER_ALERT_1             = -1000442,
    SAY_KER_ALERT_2             = -1000443,

    SAY_KER_END                 = -1000444,

    SPELL_SLEEP_VISUAL          = 25148,
    SPELL_AWAKEN                = 17536,
    QUEST_SLEEPER_AWAKENED      = 5321,
    NPC_LILADRIS                = 11219                     //attackers entries unknown
};

//TODO: make concept similar as "ringo" -escort. Find a way to run the scripted attacks, _if_ player are choosing road.
struct MANGOS_DLL_DECL npc_kerlonianAI : public FollowerAI
{
    npc_kerlonianAI(Creature* pCreature) : FollowerAI(pCreature) {
        Reset();
    }

    uint32 m_uiFallAsleepTimer;

    void Reset()
    {
        m_uiFallAsleepTimer = urand(10000, 45000);
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        FollowerAI::MoveInLineOfSight(pWho);

        if (!m_creature->getVictim() && !HasFollowState(STATE_FOLLOW_COMPLETE) && pWho->GetEntry() == NPC_LILADRIS)
        {
            if (m_creature->IsWithinDistInMap(pWho, INTERACTION_DISTANCE*5))
            {
                if (Player* pPlayer = GetLeaderForFollower())
                {
                    if (pPlayer->GetQuestStatus(QUEST_SLEEPER_AWAKENED) == QUEST_STATUS_INCOMPLETE)
                        pPlayer->GroupEventHappens(QUEST_SLEEPER_AWAKENED, m_creature);

                    DoScriptText(SAY_KER_END, m_creature);
                }

                SetFollowComplete();
            }
        }
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell)
    {
        if (HasFollowState(STATE_FOLLOW_INPROGRESS | STATE_FOLLOW_PAUSED) && pSpell->Id == SPELL_AWAKEN)
            ClearSleeping();
    }

    void SetSleeping()
    {
        SetFollowPaused(true);

        switch(urand(0, 2))
        {
        case 0:
            DoScriptText(EMOTE_KER_SLEEP_1, m_creature);
            break;
        case 1:
            DoScriptText(EMOTE_KER_SLEEP_2, m_creature);
            break;
        case 2:
            DoScriptText(EMOTE_KER_SLEEP_3, m_creature);
            break;
        }

        switch(urand(0, 3))
        {
        case 0:
            DoScriptText(SAY_KER_SLEEP_1, m_creature);
            break;
        case 1:
            DoScriptText(SAY_KER_SLEEP_2, m_creature);
            break;
        case 2:
            DoScriptText(SAY_KER_SLEEP_3, m_creature);
            break;
        case 3:
            DoScriptText(SAY_KER_SLEEP_4, m_creature);
            break;
        }

        m_creature->SetStandState(UNIT_STAND_STATE_SLEEP);
        m_creature->CastSpell(m_creature, SPELL_SLEEP_VISUAL, false);
    }

    void ClearSleeping()
    {
        m_creature->RemoveAurasDueToSpell(SPELL_SLEEP_VISUAL);
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);

        DoScriptText(EMOTE_KER_AWAKEN, m_creature);

        SetFollowPaused(false);
    }

    void UpdateFollowerAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (!HasFollowState(STATE_FOLLOW_INPROGRESS))
                return;

            if (!HasFollowState(STATE_FOLLOW_PAUSED))
            {
                if (m_uiFallAsleepTimer < uiDiff)
                {
                    SetSleeping();
                    m_uiFallAsleepTimer = urand(25000, 90000);
                }
                else
                    m_uiFallAsleepTimer -= uiDiff;
            }

            return;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_kerlonian(Creature* pCreature)
{
    return new npc_kerlonianAI(pCreature);
}

bool QuestAccept_npc_kerlonian(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_SLEEPER_AWAKENED)
    {
        if (npc_kerlonianAI* pKerlonianAI = dynamic_cast<npc_kerlonianAI*>(pCreature->AI()))
        {
            pCreature->SetStandState(UNIT_STAND_STATE_STAND);            
            DoScriptText(SAY_KER_START, pCreature, pPlayer);
            pKerlonianAI->StartFollow(pPlayer, FACTION_ESCORT_N_FRIEND_PASSIVE, pQuest);            
            pCreature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
        }
    }

    return true;
}

/*####
# npc_prospector_remtravel
####*/

enum
{
    SAY_REM_START               = -1000327,
    SAY_REM_AGGRO               = -1000339,
    SAY_REM_RAMP1_1             = -1000328,
    SAY_REM_RAMP1_2             = -1000329,
    SAY_REM_BOOK                = -1000330,
    SAY_REM_TENT1_1             = -1000331,
    SAY_REM_TENT1_2             = -1000332,
    SAY_REM_MOSS                = -1000333,
    EMOTE_REM_MOSS              = -1000334,
    SAY_REM_MOSS_PROGRESS       = -1000335,
    SAY_REM_PROGRESS            = -1000336,
    SAY_REM_REMEMBER            = -1000337,
    EMOTE_REM_END               = -1000338,

    QUEST_ABSENT_MINDED_PT2     = 731,
    NPC_GRAVEL_SCOUT            = 2158,
    NPC_GRAVEL_BONE             = 2159,
    NPC_GRAVEL_GEO              = 2160
};

struct MANGOS_DLL_DECL npc_prospector_remtravelAI : public npc_escortAI
{
    npc_prospector_remtravelAI(Creature* pCreature) : npc_escortAI(pCreature) {
        Reset();
    }

    void WaypointReached(uint32 i)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch(i)
        {
        case 0:
            DoScriptText(SAY_REM_START, m_creature, pPlayer);
            break;
        case 5:
            DoScriptText(SAY_REM_RAMP1_1, m_creature, pPlayer);
            break;
        case 6:
            DoSpawnCreature(NPC_GRAVEL_SCOUT, -10.0f, 5.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
            DoSpawnCreature(NPC_GRAVEL_BONE, -10.0f, 7.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
            break;
        case 9:
            DoScriptText(SAY_REM_RAMP1_2, m_creature, pPlayer);
            break;
        case 14:
            //depend quest rewarded?
            DoScriptText(SAY_REM_BOOK, m_creature, pPlayer);
            break;
        case 15:
            DoScriptText(SAY_REM_TENT1_1, m_creature, pPlayer);
            break;
        case 16:
            DoSpawnCreature(NPC_GRAVEL_SCOUT, -10.0f, 5.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
            DoSpawnCreature(NPC_GRAVEL_BONE, -10.0f, 7.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
            break;
        case 17:
            DoScriptText(SAY_REM_TENT1_2, m_creature, pPlayer);
            break;
        case 26:
            DoScriptText(SAY_REM_MOSS, m_creature, pPlayer);
            break;
        case 27:
            DoScriptText(EMOTE_REM_MOSS, m_creature, pPlayer);
            break;
        case 28:
            DoScriptText(SAY_REM_MOSS_PROGRESS, m_creature, pPlayer);
            break;
        case 29:
            DoSpawnCreature(NPC_GRAVEL_SCOUT, -15.0f, 3.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
            DoSpawnCreature(NPC_GRAVEL_BONE, -15.0f, 5.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
            DoSpawnCreature(NPC_GRAVEL_GEO, -15.0f, 7.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
            break;
        case 31:
            DoScriptText(SAY_REM_PROGRESS, m_creature, pPlayer);
            break;
        case 41:
            DoScriptText(SAY_REM_REMEMBER, m_creature, pPlayer);
            break;
        case 42:
            DoScriptText(EMOTE_REM_END, m_creature, pPlayer);
            pPlayer->GroupEventHappens(QUEST_ABSENT_MINDED_PT2, m_creature);
            break;
        }
    }

    void Reset() { }

    void Aggro(Unit* who)
    {
        if (urand(0, 1))
            DoScriptText(SAY_REM_AGGRO, m_creature, who);
    }

    void JustSummoned(Creature* /*pSummoned*/)
    {
        //unsure if it should be any
        //pSummoned->AI()->AttackStart(m_creature);
    }
};

CreatureAI* GetAI_npc_prospector_remtravel(Creature* pCreature)
{
    return new npc_prospector_remtravelAI(pCreature);
}

bool QuestAccept_npc_prospector_remtravel(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ABSENT_MINDED_PT2)
    {
        pCreature->setFaction(FACTION_ESCORT_A_NEUTRAL_PASSIVE);

        if (npc_prospector_remtravelAI* pEscortAI = dynamic_cast<npc_prospector_remtravelAI*>(pCreature->AI()))
            pEscortAI->Start(false, pPlayer, pQuest, true);
    }

    return true;
}

/*####
# npc_threshwackonator
####*/

enum
{
    EMOTE_START             = -1000325,
    SAY_AT_CLOSE            = -1000326,
    QUEST_GYROMAST_REV      = 2078,
    NPC_GELKAK              = 6667,
    FACTION_HOSTILE         = 14
};

#define GOSSIP_ITEM_INSERT_KEY  "[PH] Insert key"

struct MANGOS_DLL_DECL npc_threshwackonatorAI : public FollowerAI
{
    npc_threshwackonatorAI(Creature* pCreature) : FollowerAI(pCreature) {
        Reset();
    }

    void Reset() {}

    void MoveInLineOfSight(Unit* pWho)
    {
        FollowerAI::MoveInLineOfSight(pWho);

        if (!m_creature->getVictim() && !HasFollowState(STATE_FOLLOW_COMPLETE) && pWho->GetEntry() == NPC_GELKAK)
        {
            if (m_creature->IsWithinDistInMap(pWho, 10.0f))
            {
                DoScriptText(SAY_AT_CLOSE, pWho);
                DoAtEnd();
            }
        }
    }

    void DoAtEnd()
    {
        m_creature->setFaction(FACTION_HOSTILE);

        if (Player* pHolder = GetLeaderForFollower())
            m_creature->AI()->AttackStart(pHolder);

        SetFollowComplete();
    }
};

CreatureAI* GetAI_npc_threshwackonator(Creature* pCreature)
{
    return new npc_threshwackonatorAI(pCreature);
}

bool GossipHello_npc_threshwackonator(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_GYROMAST_REV) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_INSERT_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_threshwackonator(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        if (npc_threshwackonatorAI* pThreshAI = dynamic_cast<npc_threshwackonatorAI*>(pCreature->AI()))
        {
            DoScriptText(EMOTE_START, pCreature);
            pThreshAI->StartFollow(pPlayer);
        }
    }

    return true;
}

/*####
# npc_volcor
####*/

enum
{
    QUEST_ESCAPE_THROUGH_FORCE = 994,
    HELLO_GRIMCLAW = -1713002,
    ENDING_TEXT = -1713001,
};

struct MANGOS_DLL_DECL npc_volcorAI : public npc_escortAI
{
    npc_volcorAI(Creature* pCreature) : npc_escortAI(pCreature) {
        Reset();
    }

    uint32 ui_ending_delay;
    uint32 ui_runDelay;

    void Reset()
    {
        ui_ending_delay = 0;
        ui_runDelay = 0;
    }

    void WaypointReached(uint32 i)
    {
        switch(i)
        {
        case 17:
            SetEscortPaused(true);
            ui_ending_delay = 3000;

            Creature *GrimClaw = GetClosestCreatureWithEntry(m_creature, 3695, 40.0f);

            if(GrimClaw)
            {
                m_creature->SetFacingToObject(GrimClaw);
                DoScriptText(HELLO_GRIMCLAW, m_creature);
            }

            break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(ui_ending_delay)
        {
            if(ui_ending_delay <= uiDiff)
            {
                Player *pPlayer = GetPlayerForEscort();

                if(pPlayer)
                {
                    m_creature->SetFacingToObject(pPlayer);
                    DoScriptText(ENDING_TEXT, m_creature, pPlayer);
                    pPlayer->GroupEventHappens(QUEST_ESCAPE_THROUGH_FORCE, m_creature);
                }
                ui_runDelay = 4000;
                SetRun(true);
                ui_ending_delay = 0;
            }
            else
                ui_ending_delay -= uiDiff;
        }

        if(ui_runDelay)
        {
            if(ui_runDelay <= uiDiff)
            {
                SetEscortPaused(false);
            }
            else
                ui_runDelay -= uiDiff;
        }

        npc_escortAI::UpdateAI(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }

    void Aggro(Unit* /*who*/)
    {
    }
};

CreatureAI* GetAI_npc_volcor(Creature* pCreature)
{
    return new npc_volcorAI(pCreature);
}

bool QuestAccept_npc_volcor(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ESCAPE_THROUGH_FORCE)
    {
        pCreature->setFaction(FACTION_ESCORT_A_NEUTRAL_PASSIVE);

        if (npc_volcorAI* pEscortAI = dynamic_cast<npc_volcorAI*>(pCreature->AI()))
        {
            pCreature->SetStandState(UNIT_STAND_STATE_STAND);
            pEscortAI->Start(false, pPlayer, pQuest, true);
        }
    }

    return true;
}

/*####
# npc_grimclaw
####*/

struct MANGOS_DLL_DECL npc_grimclaw : public ScriptedAI
{
    npc_grimclaw(Creature* pCreature) : ScriptedAI(pCreature) {
        Reset();
    }

    uint32 ui_sniffTimer;
    uint32 ui_turnPlayerTimer;
    uint32 ui_sniffReturnTimer;
    uint32 ui_responseCooldown;
    Player *player;

    void Reset()
    {
        player = nullptr;
        ui_sniffTimer = 0;
        ui_sniffReturnTimer = 0;
        ui_turnPlayerTimer = 0;
        ui_responseCooldown = 0;
    }

    void ReceiveEmote(Player *pPlayer, uint32 uiTextEmote)
    {
        if(uiTextEmote == TEXTEMOTE_WAVE && !ui_responseCooldown)
        {
            if(pPlayer)
            {
                player = pPlayer;
                m_creature->GenericTextEmote("Grimclaw growls in your direction before taking time to sniff you.", nullptr);
                m_creature->SetFacingToObject(pPlayer);
                ui_sniffTimer = 3000;
                ui_responseCooldown = 15000;
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(ui_sniffTimer)
        {
            if(ui_sniffTimer <= uiDiff)
            {
                m_creature->GenericTextEmote("Grimclaw faces southeast and whimpers before looking back at you.", nullptr);
				m_creature->SetFacingTo(3.9f);
                ui_sniffTimer = 0;
                ui_turnPlayerTimer = 3000;
            }
            else
                ui_sniffTimer -= uiDiff;
        }

        if(ui_turnPlayerTimer)
        {
            if(ui_turnPlayerTimer <= uiDiff)
            {
                if(player)
                    m_creature->SetFacingToObject(player);
                else
                    m_creature->SetFacingTo(2.1f);

                ui_sniffReturnTimer = 2000;
                ui_turnPlayerTimer = 0;
                player = nullptr;
            }
            else
                ui_turnPlayerTimer -= uiDiff;
        }

        if(ui_sniffReturnTimer)
        {
            if(ui_sniffReturnTimer <= uiDiff)
            {
                ui_sniffReturnTimer = 0;
                m_creature->SetFacingTo(2.1f);
            }
            else
                ui_sniffReturnTimer -= uiDiff;
        }

        if(ui_responseCooldown)
        {
            if(ui_responseCooldown <= uiDiff)
                ui_responseCooldown = 0;
            else
                ui_responseCooldown -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_grimclaw(Creature* pCreature)
{
    return new npc_grimclaw(pCreature);
}

/*####
# mob_rabid_thistle_bear
####*/

enum
{
	RABID_THISTLE_BEAR = 2164,
	CAPTURED_THISTLE_BEAR = 11836,
	CAPTURED_THISTLE_BEAR_DISPLAY_ID = 8840,
	NPC_THARNARIUN = 3701,
	QUEST_PLAGUED_LANDS = 2118,
	BEAR_TRAP = 111148,
};

struct MANGOS_DLL_DECL mob_rabid_thistle_bear : public ScriptedAI
{
    mob_rabid_thistle_bear(Creature* pCreature) : ScriptedAI(pCreature) {
        Reset();
    }

	GameObject *trap;
	Player *trapOwner;
	Creature *tharnariun;
	bool captured;
	uint32 ui_despawnTimer;

    void Reset()
    {
        if(m_creature->GetEntry() == CAPTURED_THISTLE_BEAR)
        {
            captured = true;
        }
        else
        {
            captured = false;
            trapOwner = nullptr;
            ui_despawnTimer = 0;
        }

		trap = nullptr;
        tharnariun = nullptr;
    }

    void UpdateAI(const uint32 uiDiff)
    {
		if(!captured)
		{
			trap = GetClosestGameObjectWithEntry(m_creature, BEAR_TRAP, 2.0f);

			if(trap)
			{
				trapOwner = (Player*)trap->GetOwner();

				if(trapOwner)
				{
					if(trapOwner->GetQuestStatus(QUEST_PLAGUED_LANDS) == QUEST_STATUS_INCOMPLETE)
					{
						captured = true;
						trapOwner->KilledMonsterCredit(CAPTURED_THISTLE_BEAR);
                        m_creature->DeleteThreatList();
                        m_creature->CombatStop(true);
                        m_creature->setFaction(FACTION_ESCORT_A_PASSIVE);
                        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
						m_creature->SetEntry(CAPTURED_THISTLE_BEAR);
						m_creature->SetDisplayId(CAPTURED_THISTLE_BEAR_DISPLAY_ID);
						m_creature->UpdateVisibilityAndView();
						m_creature->GetMotionMaster()->MoveFollow(trapOwner, 0, 0);
						m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
						ui_despawnTimer = 120000;
					}
				}
			}
		} else {
			tharnariun = GetClosestCreatureWithEntry(m_creature, NPC_THARNARIUN, 10.0f);
					
			if(tharnariun)
			{
				m_creature->ForcedDespawn();
			}
            
			if(ui_despawnTimer <= uiDiff)
				m_creature->ForcedDespawn();
			else
				ui_despawnTimer -= uiDiff;

			return;
		}
        
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();

    }
};

CreatureAI* GetAI_mob_rabid_thistle_bear(Creature* pCreature)
{
    return new mob_rabid_thistle_bear(pCreature);
}

/*######
## npc_sentinel_aynasha
######*/

enum SentinelAynasha
{
	SAY_START							= -1720003,
	SAY_ARROWS							= -1720004,
	SAY_END_1							= -1720005,
	SAY_END_2							= -1720006,
	SAY_END_3							= -1720007,

	SPELL_AYNASHAS_BOW					= 19767,
	SPELL_ROOT_SELF						= 23973,			// taken from rag, make sure she doesn't move

	NPC_SENTINEL_AYNASHA				= 11711,
	NPC_BLACKWOOD_TRACKER				= 11713,
	NPC_MAROSH_THE_DEVIOUS				= 11714,

	WAVE_ONE							= 1,
	WAVE_TWO							= 2,
	WAVE_BOSS							= 3,

	QUEST_ONE_SHOT_ONE_KILL				= 5713,
};

struct Loc
{
    float x, y, z;
};

static Loc aMobSpawnLoc[]= 
{
	{4369.34f, -25.11f, 71.27f},
	{4363.90f, -27.64f, 71.47f},
	{4368.92f, -30.12f, 72.40f},
};

struct MANGOS_DLL_DECL npc_sentinel_aynashaAI : public npc_escortAI
{
    npc_sentinel_aynashaAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
		m_uiPlayerGUID = ObjectGuid();
		m_creature->CastSpell(m_creature, SPELL_ROOT_SELF, true);
		SetCombatMovement(false);

		m_creature->SetSheath(SHEATH_STATE_RANGED);
		
        Reset();
    }

    uint8 m_uiKilledCreatures;
	uint8 m_uiPhase;
	uint8 m_uiCurrentWave;
	uint8 m_uiSpeechStep;

    uint32 m_startTimer;
	uint32 m_uiEventTimer;
	uint32 m_despawnTimer;
	uint32 m_uiSpeechTimer;

	uint32 m_uiCanShootTimer;
	uint32 m_uiShootTimer;

	ObjectGuid m_uiPlayerGUID;

	bool m_bCanShoot;
	bool m_bOutro;

    void Reset()
    {
        if (!m_creature->isActiveObject() && m_creature->isAlive())
            m_creature->SetActiveObjectState(true);
    }

	void QuestEndReset()				// make sure another player can do the quest after it's done
	{
		m_uiSpeechTimer		= 0;
		m_uiEventTimer      = 0;
        m_uiKilledCreatures = 0;
		m_uiPhase           = 0;
		m_uiCurrentWave     = 0;
        m_startTimer		= 0;
		m_uiCanShootTimer	= 0;
		m_uiShootTimer		= 0;
		m_despawnTimer		= 0;
		m_uiPlayerGUID.Clear();
		m_bCanShoot			= true;
		m_bOutro			= false;
		m_uiSpeechStep		= 1;
	}

	void WaypointReached(uint32 uiPoint)
    {
	}

	void DoSummonWave(uint32 uiSummonId = 0)
    {
        if (uiSummonId == WAVE_ONE)
        {
            for (uint8 i = 0; i < 2; ++i)
            {
                m_creature->SummonCreature(NPC_BLACKWOOD_TRACKER, aMobSpawnLoc[i].x, aMobSpawnLoc[i].y, aMobSpawnLoc[i].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
            }

            ++m_uiCurrentWave;
        }
        else if (uiSummonId == WAVE_TWO)
        {
            for (uint8 i = 0; i < 3; ++i)
            {
                m_creature->SummonCreature(NPC_BLACKWOOD_TRACKER, aMobSpawnLoc[i].x, aMobSpawnLoc[i].y, aMobSpawnLoc[i].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
            }
			++m_uiCurrentWave;
        }
        else if (uiSummonId == WAVE_BOSS)
        {
            for (uint8 i = 0; i < 1; ++i)
            {
                m_creature->SummonCreature(NPC_MAROSH_THE_DEVIOUS, aMobSpawnLoc[i].x, aMobSpawnLoc[i].y, aMobSpawnLoc[i].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
            }
			++m_uiCurrentWave;
        }
    }

	void AttackStart(Unit* pWho)
    {
        if (!pWho)
            return;

        if (m_creature->Attack(pWho, false))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);

            m_creature->GetMotionMaster()->MoveChase(pWho, 30.0f);
        }
    }

	void JustDied(Unit* /*pKiller*/)			// fail quest if aynasha dies
	{
		if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID))
			pPlayer->FailQuest(QUEST_ONE_SHOT_ONE_KILL);
		QuestEndReset();			// stop waves
	}

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->AI()->AttackStart(m_creature);
		pSummoned->SetRespawnDelay(-10);			// make sure they won't respawn
    }

	void SummonedCreatureJustDied(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_BLACKWOOD_TRACKER || pSummoned->GetEntry() == NPC_MAROSH_THE_DEVIOUS)
        {
            ++m_uiKilledCreatures;
		}
		
		// Event ended
        if (m_uiKilledCreatures >= 6 && m_uiCurrentWave == 3)
        {
			if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID))			// give quest credit when all waves are dead
			{
				pPlayer->AreaExploredOrEventHappens(QUEST_ONE_SHOT_ONE_KILL);
			
			}
			m_despawnTimer = 0;			// despawn timer shouldn't be needed anymore
			m_bOutro = true;
			m_uiSpeechTimer = 2000;
			m_uiSpeechStep		= 1;		// need this here so the outro can be repeated more than once.
        }
	}

	ObjectGuid DoStartEvent(ObjectGuid player_guid)
	{
		QuestEndReset();			// this first, reset timers so the quest can be done again
		if (m_uiPlayerGUID)
            return ObjectGuid();

        m_startTimer	= 3000; // Do say and start event after 3 sec
		m_uiEventTimer  = 5000;
		m_despawnTimer  = 600000; // despawn after 10min if she didn't reach the outro RP
		
		m_bCanShoot		= true;

		m_uiPlayerGUID = player_guid;
        
        return m_uiPlayerGUID;
	}

	void MoveInLineOfSight(Unit* pWho)		// don't move to target, NOT WORKING
    {
        if (!pWho)
            return;
	}

	void UpdateEscortAI(const uint32 uiDiff)
    {
		if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID))
        {
			if(m_startTimer)
			{
				if(m_startTimer <= uiDiff)
				{
					DoScriptText(SAY_START, m_creature);
					m_uiCanShootTimer = 65000;
					m_uiShootTimer = 1000;
					m_startTimer = 0;
				}
				else
					m_startTimer -= uiDiff;
			}

			if (m_uiEventTimer)					// summon waves every 60 sec
			{
				if (m_uiEventTimer <= uiDiff)
				{
					switch (m_uiPhase)
					{
						case 0:
							DoSummonWave(WAVE_ONE);
							m_uiEventTimer = 60000;
							break;
						case 1:
							DoSummonWave(WAVE_TWO);
							m_uiEventTimer = 60000;
							break;
						case 2:
							DoSummonWave(WAVE_BOSS);
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

			if (m_despawnTimer)							// despawn after 10min if quest somehow bugged
			{
				if(m_despawnTimer <= uiDiff)
				{
					m_creature->ForcedDespawn();
					m_despawnTimer = 0;
				}
				else
					m_despawnTimer -= uiDiff;
			}

			if (m_uiCanShootTimer)						// do say and no more bow attacks
			{
				if (m_uiCanShootTimer <= uiDiff)
				{
					DoScriptText(SAY_ARROWS, m_creature);
					m_creature->SetSheath(SHEATH_STATE_MELEE);
					m_uiCanShootTimer = 0;
					m_bCanShoot = false;
				}
				else
					m_uiCanShootTimer -= uiDiff;
			}

			if (m_uiShootTimer && m_bCanShoot)
			{
				if (m_uiShootTimer <= uiDiff)
				{
					for(uint8 i = 0; i < 6; ++i)
						if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, i))
							if (!pTarget->IsWithinDist(m_creature, 10.0f))
							{
								DoCastSpellIfCan(pTarget, SPELL_AYNASHAS_BOW);
								m_uiShootTimer = 2000;
							}
				}
				else
					m_uiShootTimer -= uiDiff;
			}
		}

		if (m_uiSpeechTimer && m_bOutro)							// handle RP at the end
		{
			if (!m_uiSpeechStep)
                return;

            if (m_uiSpeechTimer <= uiDiff)
            {
                switch(m_uiSpeechStep)
                {
                    case 1:
                        DoScriptText(SAY_END_1, m_creature);
                        m_uiSpeechTimer = 5000;
                        break;
					case 2:
						DoScriptText(SAY_END_2, m_creature);
						m_uiSpeechTimer = 5000;
						break;
					case 3:
						DoScriptText(SAY_END_3, m_creature);
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

		// Return since we have no target
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
		{
			return;
		}
		
		DoMeleeAttackIfReady();
		
	}
};

CreatureAI* GetAI_npc_sentinel_aynasha(Creature* pCreature)
{
    return new npc_sentinel_aynashaAI(pCreature);
}

bool QuestAccept_npc_sentinel_aynasha(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ONE_SHOT_ONE_KILL)
    {
        if (npc_sentinel_aynashaAI* pEscortAI = dynamic_cast<npc_sentinel_aynashaAI*>(pCreature->AI()))
        {
            pEscortAI->Start(false, pPlayer, pQuest);
			pEscortAI->DoStartEvent(pPlayer->GetGUID());
        }

        return true;
    }

    return false;
}

/*####
# npc_therylune
####*/

enum
{
    SAY_THERYLUNE_START              = -1720008,
    SAY_THERYLUNE_FINISH             = -1720009,

    QUEST_ID_THERYLUNE_ESCAPE        = 945,
};

struct MANGOS_DLL_DECL npc_theryluneAI : public npc_escortAI
{
    npc_theryluneAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    void Reset()
	{
	}

	void JustStartedEscort()
    {
        if (GetPlayerForEscort())
		    m_creature->SetFactionTemporary(10);			// added temp faction so mobs can hit her
    }

	void JustDied(Unit* /*pKiller*/)			// fail quest if Therylune dies
	{
		if (Player* pPlayer = GetPlayerForEscort())
        {
			pPlayer->FailQuest(QUEST_ID_THERYLUNE_ESCAPE);
        }
	}

    void WaypointReached(uint32 uiPointId)
    {
        switch (uiPointId)
        {
            case 18:
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    pPlayer->GroupEventHappens(QUEST_ID_THERYLUNE_ESCAPE, m_creature);
                }
                break;
            case 19:
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    DoScriptText(SAY_THERYLUNE_FINISH, m_creature, pPlayer);
                }
                SetRun();
                break;
			case 20:
				m_creature->ForcedDespawn();
				break;
        }
    }
};

CreatureAI* GetAI_npc_therylune(Creature* pCreature)
{
    return new npc_theryluneAI(pCreature);
}

bool QuestAccept_npc_therylune(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ID_THERYLUNE_ESCAPE)
    {
        if (npc_theryluneAI* pEscortAI = dynamic_cast<npc_theryluneAI*>(pCreature->AI()))
        {
            pEscortAI->Start(false, pPlayer, pQuest);
            DoScriptText(SAY_THERYLUNE_START, pCreature, pPlayer);
        }
    }

    return true;
}

/*####
# npc_cerellean_whiteclaw
####*/

enum
{
	CERELLAN_SAY_1					 = -1720032,
	CERELLAN_SAY_2					 = -1720033,
	CERELLAN_SAY_3					 = -1720034,
	CERELLAN_SAY_4					 = -1720035,
	CERELLAN_SAY_5					 = -1720036,

	NPC_ANAYA						 = 3843,

    QUEST_ID_FOR_LOVE_ETERNAL        = 963,
};

struct MANGOS_DLL_DECL npc_cerellean_whiteclawAI : public npc_escortAI
{
    npc_cerellean_whiteclawAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }
	
	uint8 m_uiSpeechStep;
	uint32 m_uiSpeechTimer;
	bool m_bOutro;

    void Reset()
	{
		m_bOutro = false;
		m_uiSpeechStep = 1;;
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
		m_uiSpeechTimer = 6000;
		m_uiSpeechStep = 1;
		m_creature->SummonCreature(NPC_ANAYA, 6427.54f, 604.45f, 9.46f, 4.19f, TEMPSUMMON_TIMED_DESPAWN, 56000, true);
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
                        DoScriptText(CERELLAN_SAY_1, m_creature);
						m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        m_uiSpeechTimer = 14000;
                        break;
					case 2:
						DoScriptText(CERELLAN_SAY_2, m_creature);
						m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
						m_uiSpeechTimer = 5000;
						break;
					case 3:
						m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
						m_uiSpeechTimer = 4000;
						break;
					case 4:
						DoScriptText(CERELLAN_SAY_3, m_creature);
						m_uiSpeechTimer = 20000;
						break;
					case 5:
						DoScriptText(CERELLAN_SAY_4, m_creature);
						m_uiSpeechTimer = 10000;
						break;
					case 6:
						m_creature->GenericTextEmote("Anaya's soft voice trails away into the mists. \"Know that I love you always...\" ", NULL, false);
						m_uiSpeechTimer = 4000;
						break;
					case 7:
						DoScriptText(CERELLAN_SAY_5, m_creature);
						m_uiSpeechTimer = 40000;
						break;
					case 8:
						m_creature->SetStandState(UNIT_STAND_STATE_STAND);
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

CreatureAI* GetAI_npc_cerellean_whiteclaw(Creature* pCreature)
{
    return new npc_cerellean_whiteclawAI(pCreature);
}

bool OnQuestRewarded_npc_cerellan(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
	if (pQuest->GetQuestId() == QUEST_ID_FOR_LOVE_ETERNAL)
    {
		if (npc_cerellean_whiteclawAI* pEscortAI = dynamic_cast<npc_cerellean_whiteclawAI*>(pCreature->AI()))
		{
			pEscortAI->Start(false, pPlayer, pQuest);
			pEscortAI->StartOutro();
		}
	}
	return true;
}

void AddSC_darkshore()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "npc_kerlonian";
    pNewscript->GetAI = &GetAI_npc_kerlonian;
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_kerlonian;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_prospector_remtravel";
    pNewscript->GetAI = &GetAI_npc_prospector_remtravel;
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_prospector_remtravel;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_threshwackonator";
    pNewscript->GetAI = &GetAI_npc_threshwackonator;
    pNewscript->pGossipHello = &GossipHello_npc_threshwackonator;
    pNewscript->pGossipSelect = &GossipSelect_npc_threshwackonator;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_volcor";
    pNewscript->GetAI = &GetAI_npc_volcor;
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_volcor;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_grimclaw";
    pNewscript->GetAI = &GetAI_npc_grimclaw;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "mob_rabid_thistle_bear";
    pNewscript->GetAI = &GetAI_mob_rabid_thistle_bear;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_sentinel_aynasha";
    pNewscript->GetAI = &GetAI_npc_sentinel_aynasha;
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_sentinel_aynasha;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_therylune";
    pNewscript->GetAI = &GetAI_npc_therylune;
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_therylune;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_cerellean_whiteclaw";
    pNewscript->GetAI = &GetAI_npc_cerellean_whiteclaw;
    pNewscript->pQuestRewardedNPC = &OnQuestRewarded_npc_cerellan;
    pNewscript->RegisterSelf();
}
