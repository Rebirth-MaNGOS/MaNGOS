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
SDName: Dustwallow_Marsh
SD%Complete: 95
SDComment: Quest support: 558, 1173, 1222, 1270, 1273, 1324. Vendor Nat Pagle
SDCategory: Dustwallow Marsh
EndScriptData */

/* ContentData
npc_lady_jaina_proudmoore
npc_pained
npc_archmage_tervosh
npc_morokk
npc_nat_pagle
npc_ogron
npc_private_hendel
npc_stinky_ignatz
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"

/*######
## npc_lady_jaina_proudmoore
######*/

enum
{
    QUEST_JAINAS_AUTOGRAPH = 558,
    SPELL_JAINAS_AUTOGRAPH = 23122
};

#define GOSSIP_ITEM_JAINA "I know this is rather silly but i have a young ward who is a bit shy and would like your autograph."

struct MANGOS_DLL_DECL npc_lady_jaina_proudmooreAI : public ScriptedAI
{
    npc_lady_jaina_proudmooreAI(Creature* pCreature) : ScriptedAI(pCreature)
	{ Reset();	}

	void Reset() 
	{
	}
	void UpdateAI(const uint32 uiDiff)
	{
    if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
    {
        return;
    }

    DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_lady_jaina_proudmoore(Creature* pCreature)
{
    return new npc_lady_jaina_proudmooreAI(pCreature);
}

bool GossipHello_npc_lady_jaina_proudmoore(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(QUEST_JAINAS_AUTOGRAPH) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_JAINA, GOSSIP_SENDER_MAIN, GOSSIP_SENDER_INFO);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_lady_jaina_proudmoore(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_SENDER_INFO)
    {
        pPlayer->SEND_GOSSIP_MENU(7012, pCreature->GetObjectGuid());
        pPlayer->CastSpell(pPlayer, SPELL_JAINAS_AUTOGRAPH, false);
    }
    return true;
}

/*######
## npc_pained
######*/

struct MANGOS_DLL_DECL npc_painedAI : public ScriptedAI							// used for missing diplomat
{
    npc_painedAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
    }
	void UpdateAI(const uint32 uiDiff)
    {
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_pained(Creature* pCreature)
{
    return new npc_painedAI(pCreature);
}

/*######
## npc_archmage_tervosh
######*/

enum
{
	SPELL_PROUDMOORES_DEFENCE			= 11971,
};

struct MANGOS_DLL_DECL npc_archmage_tervoshAI : public ScriptedAI							// used for missing diplomat
{
    npc_archmage_tervoshAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }
	uint32 m_uiProudmooresDefenceTimer;

    void Reset()
    {
		m_uiProudmooresDefenceTimer = 1000;
    }

	void UpdateAI(const uint32 uiDiff)
    {
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		 // Proudmoore's Defence 
        if (m_uiProudmooresDefenceTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_PROUDMOORES_DEFENCE, CAST_TRIGGERED);
            m_uiProudmooresDefenceTimer = 1800000;
        }
        else
            m_uiProudmooresDefenceTimer -= uiDiff;

        DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_archmage_tervosh(Creature* pCreature)
{
    return new npc_archmage_tervoshAI(pCreature);
}

/*######
## npc_morokk
######*/

enum
{
    SAY_MOR_CHALLENGE               = -1000499,
    SAY_MOR_SCARED                  = -1000500,

    QUEST_CHALLENGE_MOROKK          = 1173,

    FACTION_MOR_HOSTILE             = 168,
    FACTION_MOR_RUNNING             = 35
};

struct MANGOS_DLL_DECL npc_morokkAI : public npc_escortAI
{
    npc_morokkAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_bIsSuccess = false;
        Reset();
    }

    bool m_bIsSuccess;

    void Reset() {}

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 0:
                SetEscortPaused(true);
                break;
            case 1:
                if (m_bIsSuccess)
                    DoScriptText(SAY_MOR_SCARED, m_creature);
                else
                {
                    m_creature->SetDeathState(JUST_DIED);
                    m_creature->Respawn();
                }
                break;
        }
    }

    void AttackedBy(Unit* pAttacker)
    {
        if (m_creature->getVictim())
            return;

        if (m_creature->IsFriendlyTo(pAttacker))
            return;

        AttackStart(pAttacker);
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32 &uiDamage)
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
        {
            if (HealthBelowPct(30))
            {
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_CHALLENGE_MOROKK, m_creature);

                m_creature->setFaction(FACTION_MOR_RUNNING);

                m_bIsSuccess = true;
                ResetToHome();

                uiDamage = 0;
            }
        }
    }

    void UpdateEscortAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (HasEscortState(STATE_ESCORT_PAUSED))
            {
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    m_bIsSuccess = false;
                    DoScriptText(SAY_MOR_CHALLENGE, m_creature, pPlayer);
                    m_creature->setFaction(FACTION_MOR_HOSTILE);
                    AttackStart(pPlayer);
                }

                SetEscortPaused(false);
            }

            return;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_morokk(Creature* pCreature)
{
    return new npc_morokkAI(pCreature);
}

bool QuestAccept_npc_morokk(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_CHALLENGE_MOROKK)
    {
        if (npc_morokkAI* pEscortAI = dynamic_cast<npc_morokkAI*>(pCreature->AI()))
            pEscortAI->Start(true, pPlayer, pQuest);

        return true;
    }

    return false;
}
/*######
## npc_nat_pagle
######*/

enum
{
    QUEST_NATS_MEASURING_TAPE = 8227
};

bool GossipHello_npc_nat_pagle(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pCreature->isVendor() && pPlayer->GetQuestRewardStatus(QUEST_NATS_MEASURING_TAPE))
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
        pPlayer->SEND_GOSSIP_MENU(7640, pCreature->GetObjectGuid());
    }
    else
        pPlayer->SEND_GOSSIP_MENU(7638, pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_nat_pagle(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_TRADE)
        pPlayer->SEND_VENDORLIST(pCreature->GetObjectGuid());

    return true;
}

/*######
## npc_ogron
######*/

enum
{
    SAY_OGR_START                       = -1000452,
    SAY_OGR_SPOT                        = -1000453,
    SAY_OGR_RET_WHAT                    = -1000454,
    SAY_OGR_RET_SWEAR                   = -1000455,
    SAY_OGR_REPLY_RET                   = -1000456,
    SAY_OGR_RET_TAKEN                   = -1000457,
    SAY_OGR_TELL_FIRE                   = -1000458,
    SAY_OGR_RET_NOCLOSER                = -1000459,
    SAY_OGR_RET_NOFIRE                  = -1000460,
    SAY_OGR_RET_HEAR                    = -1000461,
    SAY_OGR_CAL_FOUND                   = -1000462,
    SAY_OGR_CAL_MERCY                   = -1000463,
    SAY_OGR_HALL_GLAD                   = -1000464,
    EMOTE_OGR_RET_ARROW                 = -1000465,
    SAY_OGR_RET_ARROW                   = -1000466,
    SAY_OGR_CAL_CLEANUP                 = -1000467,
    SAY_OGR_NODIE                       = -1000468,
    SAY_OGR_SURVIVE                     = -1000469,
    SAY_OGR_RET_LUCKY                   = -1000470,
    SAY_OGR_THANKS                      = -1000471,

    QUEST_QUESTIONING                   = 1273,

    FACTION_GENERIC_FRIENDLY            = 35,
    FACTION_THER_HOSTILE                = 151,

    NPC_REETHE                          = 4980,
    NPC_CALDWELL                        = 5046,
    NPC_HALLAN                          = 5045,
    NPC_SKIRMISHER                      = 5044,

    SPELL_FAKE_SHOT                     = 7105,

    PHASE_INTRO                         = 0,
    PHASE_GUESTS                        = 1,
    PHASE_FIGHT                         = 2,
    PHASE_COMPLETE                      = 3
};

static float m_afSpawn[]= {-3383.501953f, -3203.383301f, 36.149f};
static float m_afMoveTo[]= {-3371.414795f, -3212.179932f, 34.210f};

struct MANGOS_DLL_DECL npc_ogronAI : public npc_escortAI
{
    npc_ogronAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        lCreatureList.clear();
        m_uiPhase = 0;
        m_uiPhaseCounter = 0;
        Reset();
    }

    std::list<Creature*> lCreatureList;

    uint32 m_uiPhase;
    uint32 m_uiPhaseCounter;
    uint32 m_uiGlobalTimer;

    void Reset()
    {
        m_uiGlobalTimer = 5000;

        if (HasEscortState(STATE_ESCORT_PAUSED) && m_uiPhase == PHASE_FIGHT)
            m_uiPhase = PHASE_COMPLETE;

        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            lCreatureList.clear();
            m_uiPhase = 0;
            m_uiPhaseCounter = 0;
        }
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING) && pWho->GetEntry() == NPC_REETHE && lCreatureList.empty())
            lCreatureList.push_back((Creature*)pWho);

        npc_escortAI::MoveInLineOfSight(pWho);
    }

    Creature* GetCreature(uint32 uiCreatureEntry)
    {
        if (!lCreatureList.empty())
        {
            for(std::list<Creature*>::iterator itr = lCreatureList.begin(); itr != lCreatureList.end(); ++itr)
            {
                if ((*itr)->GetEntry() == uiCreatureEntry && (*itr)->isAlive())
                    return (*itr);
            }
        }

        return NULL;
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 9:
                DoScriptText(SAY_OGR_SPOT, m_creature);
                break;
            case 10:
                if (Creature* pReethe = GetCreature(NPC_REETHE))
                    DoScriptText(SAY_OGR_RET_WHAT, pReethe);
                break;
            case 11:
                SetEscortPaused(true);
                break;
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        lCreatureList.push_back(pSummoned);

        pSummoned->setFaction(FACTION_GENERIC_FRIENDLY);

        if (pSummoned->GetEntry() == NPC_CALDWELL)
            pSummoned->GetMotionMaster()->MovePoint(0, m_afMoveTo[0], m_afMoveTo[1], m_afMoveTo[2]);
        else
        {
            if (Creature* pCaldwell = GetCreature(NPC_CALDWELL))
            {
                //will this conversion work without compile warning/error?
                size_t iSize = lCreatureList.size();
                pSummoned->GetMotionMaster()->MoveFollow(pCaldwell, 0.5f, (M_PI/2)*(int)iSize);
            }
        }
    }

    void DoStartAttackMe()
    {
        if (!lCreatureList.empty())
        {
            for(std::list<Creature*>::iterator itr = lCreatureList.begin(); itr != lCreatureList.end(); ++itr)
            {
                if ((*itr)->GetEntry() == NPC_REETHE)
                    continue;

                if ((*itr)->isAlive())
                {
                    (*itr)->setFaction(FACTION_THER_HOSTILE);
                    (*itr)->AI()->AttackStart(m_creature);
                }
            }
        }
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (HasEscortState(STATE_ESCORT_PAUSED))
            {
                if (m_uiGlobalTimer < uiDiff)
                {
                    m_uiGlobalTimer = 5000;

                    switch(m_uiPhase)
                    {
                        case PHASE_INTRO:
                        {
                            switch(m_uiPhaseCounter)
                            {
                                case 0:
                                    if (Creature* pReethe = GetCreature(NPC_REETHE))
                                        DoScriptText(SAY_OGR_RET_SWEAR, pReethe);
                                    break;
                                case 1:
                                    DoScriptText(SAY_OGR_REPLY_RET, m_creature);
                                    break;
                                case 2:
                                    if (Creature* pReethe = GetCreature(NPC_REETHE))
                                        DoScriptText(SAY_OGR_RET_TAKEN, pReethe);
                                    break;
                                case 3:
                                    DoScriptText(SAY_OGR_TELL_FIRE, m_creature);
                                    if (Creature* pReethe = GetCreature(NPC_REETHE))
                                        DoScriptText(SAY_OGR_RET_NOCLOSER, pReethe);
                                    break;
                                case 4:
                                    if (Creature* pReethe = GetCreature(NPC_REETHE))
                                        DoScriptText(SAY_OGR_RET_NOFIRE, pReethe);
                                    break;
                                case 5:
                                    if (Creature* pReethe = GetCreature(NPC_REETHE))
                                        DoScriptText(SAY_OGR_RET_HEAR, pReethe);

                                    m_creature->SummonCreature(NPC_CALDWELL, m_afSpawn[0], m_afSpawn[1], m_afSpawn[2], 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 300000);
                                    m_creature->SummonCreature(NPC_HALLAN, m_afSpawn[0], m_afSpawn[1], m_afSpawn[2], 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 300000);
                                    m_creature->SummonCreature(NPC_SKIRMISHER, m_afSpawn[0], m_afSpawn[1], m_afSpawn[2], 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 300000);
                                    m_creature->SummonCreature(NPC_SKIRMISHER, m_afSpawn[0], m_afSpawn[1], m_afSpawn[2], 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 300000);

                                    m_uiPhase = PHASE_GUESTS;
                                    break;
                            }
                            break;
                        }
                        case PHASE_GUESTS:
                        {
                            switch(m_uiPhaseCounter)
                            {
                                case 6:
                                    if (Creature* pCaldwell = GetCreature(NPC_CALDWELL))
                                        DoScriptText(SAY_OGR_CAL_FOUND, pCaldwell);
                                    break;
                                case 7:
                                    if (Creature* pCaldwell = GetCreature(NPC_CALDWELL))
                                        DoScriptText(SAY_OGR_CAL_MERCY, pCaldwell);
                                    break;
                                case 8:
                                    if (Creature* pHallan = GetCreature(NPC_HALLAN))
                                    {
                                        DoScriptText(SAY_OGR_HALL_GLAD, pHallan);

                                        if (Creature* pReethe = GetCreature(NPC_REETHE))
                                            pHallan->CastSpell(pReethe, SPELL_FAKE_SHOT, false);
                                    }
                                    break;
                                case 9:
                                    if (Creature* pReethe = GetCreature(NPC_REETHE))
                                    {
                                        DoScriptText(EMOTE_OGR_RET_ARROW, pReethe);
                                        DoScriptText(SAY_OGR_RET_ARROW, pReethe);
                                    }
                                    break;
                                case 10:
                                    if (Creature* pCaldwell = GetCreature(NPC_CALDWELL))
                                        DoScriptText(SAY_OGR_CAL_CLEANUP, pCaldwell);

                                    DoScriptText(SAY_OGR_NODIE, m_creature);
                                    break;
                                case 11:
                                    DoStartAttackMe();
                                    m_uiPhase = PHASE_FIGHT;
                                    break;
                            }
                            break;
                        }
                        case PHASE_COMPLETE:
                        {
                            switch(m_uiPhaseCounter)
                            {
                                case 12:
                                    if (Player* pPlayer = GetPlayerForEscort())
                                        pPlayer->GroupEventHappens(QUEST_QUESTIONING, m_creature);

                                    DoScriptText(SAY_OGR_SURVIVE, m_creature);
                                    break;
                                case 13:
                                    if (Creature* pReethe = GetCreature(NPC_REETHE))
                                        DoScriptText(SAY_OGR_RET_LUCKY, pReethe);
                                    break;
                                case 14:
                                    DoScriptText(SAY_OGR_THANKS, m_creature);
                                    SetRun();
                                    SetEscortPaused(false);
                                    break;
                            }
                            break;
                        }
                    }

                    if (m_uiPhase != PHASE_FIGHT)
                        ++m_uiPhaseCounter;
                }
                else
                    m_uiGlobalTimer -= uiDiff;
            }

            return;
        }

        DoMeleeAttackIfReady();
    }
};

bool QuestAccept_npc_ogron(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_QUESTIONING)
    {
        if (npc_ogronAI* pEscortAI = dynamic_cast<npc_ogronAI*>(pCreature->AI()))
        {
            pEscortAI->Start(false, pPlayer, pQuest, true);
            pCreature->setFaction(FACTION_ESCORT_N_FRIEND_PASSIVE);
            DoScriptText(SAY_OGR_START, pCreature, pPlayer);
        }
    }

    return true;
}

CreatureAI* GetAI_npc_ogron(Creature* pCreature)
{
    return new npc_ogronAI(pCreature);
}

/*######
## npc_private_hendel
######*/

enum
{
    SAY_PROGRESS_1_TER          = -1000411,
    SAY_PROGRESS_2_HEN          = -1000412,
    SAY_PROGRESS_3_TER          = -1000413,
    SAY_PROGRESS_4_TER          = -1000414,
	SAY_BYE						= -1720017,
	SAY_AGGRO					= -1720018,
    EMOTE_SURRENDER             = -1000415,

    SPELL_ENCAGE				= 7081,						// this is the only spell that is right, but it looks ok
	SPELL_CAST_VISUAL           = 6421,						// unlock spell from SFK
	SPELL_IVUS_TELEPORT_VISUAL	= 21649,

    QUEST_MISSING_DIPLO_PT16    = 1324,
    FACTION_HOSTILE             = 168,                      //guessed, may be different

    NPC_SENTRY                  = 5184,                     //helps hendel
    NPC_JAINA                   = 4968,                     //appears once hendel gives up
    NPC_TERVOSH                 = 4967,
	NPC_HENDEL					= 4966,
	NPC_PAINED					= 4965,
};

struct Loc
{
    float x, y, z, o;
};

static Loc aSpawnLoc[]= 
{
	{-2875.6f, -3344.6f, 35.91f, 3.42f},	// tervosh
	{-2873.53f, -3340.4f, 36.28f, 3.49f},	// pained
	{-2872.49f, -3347.04f, 35.79f, 3.62f},	// jaina
};

static Loc aMoveLoc[]= 
{
	{-2883.25f, -3346.68f, 33.53f, 3.39f},		// tervosh
	{-2881.74f, -3344.78f, 34.03f, 3.39f},		// pained
	{-2880.99f, -3348.48f, 34.31f, 3.39f},		// jaina
};	

//TODO: develop this further, end event not created
struct MANGOS_DLL_DECL npc_private_hendelAI : public ScriptedAI
{
    npc_private_hendelAI(Creature* pCreature) : ScriptedAI(pCreature)
	{ 
		m_uiPlayerGUID.Clear();
		m_bOutro = false;
		//m_creature->SetVisibility(VISIBILITY_ON);
		m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		Reset(); 
	}

	uint32 m_uiSpeechTimer;
    uint8 m_uiSpeechStep;

	ObjectGuid m_uiPlayerGUID;

	bool m_bOutro;

    void Reset()
    {
		m_uiSpeechTimer = 0;
        m_uiSpeechStep = 1;

		m_creature->SetVisibility(VISIBILITY_ON);

        if (m_creature->getFaction() != m_creature->GetCreatureInfo()->faction_A)
            m_creature->setFaction(m_creature->GetCreatureInfo()->faction_A);

		if (Creature* pSentry = GetClosestCreatureWithEntry(m_creature, NPC_SENTRY, 40.0f))
			if (pSentry->getFaction() != pSentry->GetCreatureInfo()->faction_A)
				pSentry->setFaction(pSentry->GetCreatureInfo()->faction_A);
    }

	void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

	void JustSummoned(Creature* pSummoned)
    {
        switch(pSummoned->GetEntry())
        {
		case NPC_TERVOSH:
			pSummoned->GetMotionMaster()->MovePoint(0, aMoveLoc[0].x, aMoveLoc[0].y, aMoveLoc[0].z);
			pSummoned->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
			pSummoned->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
			break;
		case NPC_PAINED:
			pSummoned->GetMotionMaster()->MovePoint(0, aMoveLoc[1].x, aMoveLoc[1].y, aMoveLoc[1].z);
			break;
        case NPC_JAINA:
			pSummoned->GetMotionMaster()->MovePoint(0, aMoveLoc[2].x, aMoveLoc[2].y, aMoveLoc[2].z);
			pSummoned->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
			break;
		}
		pSummoned->SetRespawnDelay(-10);			// make sure they won't respawn randomly
	}

    void AttackedBy(Unit* pAttacker)
    {
        if (m_creature->getVictim())
            return;

        if (m_creature->IsFriendlyTo(pAttacker))
            return;

        AttackStart(pAttacker);
    }

	void SummonOutro()
	{
		m_creature->SummonCreature(NPC_TERVOSH, aSpawnLoc[0].x, aSpawnLoc[0].y,aSpawnLoc[0].z,aSpawnLoc[0].o, TEMPSUMMON_CORPSE_DESPAWN, 5000, true);
		m_creature->SummonCreature(NPC_PAINED, aSpawnLoc[1].x, aSpawnLoc[1].y,aSpawnLoc[1].z,aSpawnLoc[1].o, TEMPSUMMON_CORPSE_DESPAWN, 5000, true);
		m_creature->SummonCreature(NPC_JAINA, aSpawnLoc[2].x, aSpawnLoc[2].y,aSpawnLoc[2].z,aSpawnLoc[2].o, TEMPSUMMON_CORPSE_DESPAWN, 5000, true);
	}

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (uiDamage > m_creature->GetHealth() || HealthBelowPct(20))
        {
            uiDamage = 0;

			if (Creature* pSentry = GetClosestCreatureWithEntry(m_creature, NPC_SENTRY, 40.0f))		// reset the sentries
			{
				if (pSentry->getFaction() != pSentry->GetCreatureInfo()->faction_A)
					pSentry->setFaction(pSentry->GetCreatureInfo()->faction_A);
				pSentry->AttackStop();
				pSentry->CombatStop();
				pSentry->GetMotionMaster()->MoveTargetedHome();				
			}

			m_bOutro = true;
            DoScriptText(EMOTE_SURRENDER, m_creature);
            ResetToHome();
        }
    }

	void StartEvent(ObjectGuid uiPlayerGUID)
    {
		m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_uiPlayerGUID  = uiPlayerGUID;
		m_creature->setFaction(FACTION_HOSTILE);

		if (Creature* pSentry = GetClosestCreatureWithEntry(m_creature, NPC_SENTRY, 20.0f))
			pSentry->SetFactionTemporary(FACTION_HOSTILE);

	}

	void UpdateAI(const uint32 uiDiff)					// handle Rp at end of Missing diplomat
    {
		if (m_bOutro)
		{
			if (!m_uiSpeechStep)
				return;
		
			if (m_uiSpeechTimer < uiDiff)
			{
				Creature* pTervosh = GetClosestCreatureWithEntry(m_creature, NPC_TERVOSH, 40.0f);
				Creature* pJaina = GetClosestCreatureWithEntry(m_creature, NPC_JAINA, 40.0f);
				Creature* pPained = GetClosestCreatureWithEntry(m_creature, NPC_PAINED, 40.0f);
				Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID);

				switch(m_uiSpeechStep)
				{
					case 1:					// spawn the 3 of them + move forward
						SummonOutro();
						m_creature->HandleEmoteState(EMOTE_STATE_STUN);
						m_creature->SetFacingTo(0.29f);			// face towards tervosh
						m_uiSpeechTimer = 3000;
						break;
					case 2:					// archmage cast spell on hendel
						if (npc_archmage_tervoshAI* pTervoshAI = dynamic_cast<npc_archmage_tervoshAI*>(pTervosh->AI()))
							pTervosh->CastSpell(m_creature, SPELL_ENCAGE, false);
						m_uiSpeechTimer = 3000;
						break;
					case 3:
						if (pTervosh)
							DoScriptText(SAY_PROGRESS_1_TER, pTervosh);
						m_uiSpeechTimer = 5000;
						break;
					case 4:
						DoScriptText(SAY_PROGRESS_2_HEN, m_creature);
						m_uiSpeechTimer = 1000;
						break;
					case 5:						// cast spell to "teleport" hendel, then give quest credit FIND A SPELL WITH SHORTER CAST, 1sec~~! Full cast for now
						if (npc_archmage_tervoshAI* pTervoshAI = dynamic_cast<npc_archmage_tervoshAI*>(pTervosh->AI()))
							pTervosh->CastSpell(pTervosh, SPELL_CAST_VISUAL, true);
						m_uiSpeechTimer = 5000;
						break;
					case 6:						// hendel disapears
						if (pPlayer)
							pPlayer->GroupEventHappens(QUEST_MISSING_DIPLO_PT16, m_creature);
						m_creature->SetVisibility(VISIBILITY_OFF);
						m_creature->HandleEmoteState(EMOTE_STATE_NONE);
						if (pTervosh)
							pTervosh->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
						if (pJaina)
							pJaina->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
						m_uiSpeechTimer = 4000;
						break;
					case 7:
						if (pTervosh)
							DoScriptText(SAY_PROGRESS_3_TER, pTervosh);
						m_uiSpeechTimer = 8000;
						break;
					case 8:
						if (pTervosh)
							DoScriptText(SAY_PROGRESS_4_TER, pTervosh);
						m_uiSpeechTimer = urand(60000, 90000);			// long time until they despawn
						break;
					case 9:	// despawn event
						if (pTervosh)
							DoScriptText(SAY_BYE, pTervosh);
						m_uiSpeechTimer = 3000;
						break;
					case 10:
						if (pJaina)
							pJaina->HandleEmote(EMOTE_ONESHOT_WAVE);
						m_uiSpeechTimer = 5000;
						break;
					case 11:
						if (npc_lady_jaina_proudmooreAI* pJainaAI = dynamic_cast<npc_lady_jaina_proudmooreAI*>(pJaina->AI()))
							pJaina->CastSpell(pJaina, SPELL_IVUS_TELEPORT_VISUAL, false);
						
						if (npc_painedAI* pPainedAI = dynamic_cast<npc_painedAI*>(pPained->AI()))
							pPained->CastSpell(pPained, SPELL_IVUS_TELEPORT_VISUAL, false);

						if (npc_archmage_tervoshAI* pTervoshAI = dynamic_cast<npc_archmage_tervoshAI*>(pTervosh->AI()))
							pTervosh->CastSpell(pTervosh, SPELL_IVUS_TELEPORT_VISUAL, false);

						m_uiSpeechTimer = 2000;
						break;
					case 12:
						if (pTervosh)
						{
							pTervosh->SetVisibility(VISIBILITY_OFF);
							pTervosh->ForcedDespawn(1000);
						}
						if (pJaina)
						{
							pJaina->SetVisibility(VISIBILITY_OFF);
							pJaina->ForcedDespawn(1000);
						}
						if (pPained)
						{
							pPained->SetVisibility(VISIBILITY_OFF);
							pPained->ForcedDespawn(1000);
						}
						m_creature->ForcedDespawn(5000);

						m_bOutro = false;
						m_uiSpeechStep = 0;
						return;
					default:
						m_uiSpeechStep = 0;
						return;
				}
				++m_uiSpeechStep;
			}
			else
				m_uiSpeechTimer -= uiDiff;
		}
		
		DoMeleeAttackIfReady();
	}
};

bool QuestAccept_npc_private_hendel(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_MISSING_DIPLO_PT16)
	{ 
		if (npc_private_hendelAI* pHendelAI = dynamic_cast<npc_private_hendelAI*>(pCreature->AI()))
			pHendelAI->StartEvent(pPlayer->GetObjectGuid());		
	}
	
    return true;
}

CreatureAI* GetAI_npc_private_hendel(Creature* pCreature)
{
    return new npc_private_hendelAI(pCreature);
}

/*######
## npc_stinky_ignatz
######*/

enum eStinkyIgnatz
{
    QUEST_STINKYS_ESCAPE_A          = 1222,
    QUEST_STINKYS_ESCAPE_H          = 1270,

    NPC_STINKYS_AMBUSHER            = 4352,         // probably wrong npc entry

    SAY_STINKY_NOPE_ITS_NOT_HERE    = -1000696,
    SAY_STINKY_FOUND_BOBGEAN_PLANT  = -1000697,
    SAY_STINKY_THANKS_COMPLETE_A    = -1000698,
    SAY_STINKY_THANKS_COMPLETE_H    = -1000699
};

struct MANGOS_DLL_DECL npc_stinky_ignatzAI : public npc_escortAI
{
    npc_stinky_ignatzAI(Creature* pCreature) : npc_escortAI(pCreature) {Reset();}

    uint32 m_uiWaitTimer;
	uint32 m_uiContinuteTimer;
	uint32 m_uiTrurnAroundTimer;
    bool m_bBobgeanPlant;
	bool m_bFinishedEscort;

    void Reset()
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
            return;

        m_uiWaitTimer = 0;
		m_uiContinuteTimer = 0;
		m_uiTrurnAroundTimer = 0;
		m_bFinishedEscort = false;
        m_bBobgeanPlant = false;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_STINKYS_AMBUSHER)
            if (Player* pPlayer = GetPlayerForEscort())
                pSummoned->AI()->AttackStart(pPlayer);
    }

    void WaypointReached(uint32 uiPointId)
    {
        Player* pPlayer = GetPlayerForEscort();
        if (!pPlayer)
            return;

        switch(uiPointId)
        {
            case 7:
				SetEscortPaused(true);
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                m_uiWaitTimer = 3000;
                break;
            case 9:
				break;
            case 10:
				break;
            case 11:
				break;
            case 13:
				SetEscortPaused(true);
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                m_uiWaitTimer = 3000;
                break;
			case 17:
				SetEscortPaused(true);
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                m_uiWaitTimer = 3000;
				break;
			case 21:
				SetEscortPaused(true);
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
				if(GetClosestGameObjectWithEntry(m_creature, 20939, 20.0f))
				{
					GetClosestGameObjectWithEntry(m_creature, 20939, 20.0f)->Respawn();
				}
                m_bBobgeanPlant = true;
                m_uiWaitTimer = 3000;
				break;
            case 29:
				m_bFinishedEscort = true;
				SetEscortPaused(true);
				m_uiContinuteTimer = 6000;
				m_creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
				if(pPlayer)
				{
					m_creature->SetFacingToObject(pPlayer);
					DoScriptText(pPlayer->GetTeam() == ALLIANCE ? SAY_STINKY_THANKS_COMPLETE_A : SAY_STINKY_THANKS_COMPLETE_H, m_creature, pPlayer);
					if(pPlayer->GetTeam() == ALLIANCE)
					{
						pPlayer->GroupEventHappens(QUEST_STINKYS_ESCAPE_A, m_creature);
					}
					else
					{
						pPlayer->GroupEventHappens(QUEST_STINKYS_ESCAPE_H, m_creature);
					}
				}
                break;
			case 36:
				break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiWaitTimer)
        {
            if (m_uiWaitTimer <= uiDiff)
            {
                Player* pPlayer = GetPlayerForEscort();
                if (m_bBobgeanPlant && pPlayer && !m_bFinishedEscort)
				{
					DoScriptText(SAY_STINKY_FOUND_BOBGEAN_PLANT, m_creature);
					m_creature->SetStandState(UNIT_STAND_STATE_STAND);
					m_uiTrurnAroundTimer = 1000;
					SetRun(true);
					m_uiContinuteTimer = 3000;
				}
				else
				{
					DoScriptText(SAY_STINKY_NOPE_ITS_NOT_HERE, m_creature);
					m_creature->SetStandState(UNIT_STAND_STATE_STAND);
					m_uiContinuteTimer = 2000;
				}
				m_uiWaitTimer = 0;
            }
            else
                m_uiWaitTimer -= uiDiff;
        }

		if(m_uiContinuteTimer)
		{

			if (m_uiContinuteTimer <= uiDiff && !m_bFinishedEscort)
			{
				SetEscortPaused(false);
				m_uiContinuteTimer = 0;
			}
			else if(m_uiContinuteTimer <= uiDiff && m_bFinishedEscort)
			{
				SetEscortPaused(false);
				m_uiContinuteTimer = 0;
			}
			else
			{
				m_uiContinuteTimer -= uiDiff;
			}
		}

		if(m_uiTrurnAroundTimer)
		{
			Player* pPlayer = GetPlayerForEscort();

			if(m_uiTrurnAroundTimer <= uiDiff)
			{
				if(pPlayer)
				{
					m_creature->SetFacingToObject(pPlayer);
				}

				m_uiTrurnAroundTimer = 0;
			}
			else
			{
				m_uiTrurnAroundTimer -= uiDiff;
			}
		}

        npc_escortAI::UpdateAI(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_stinky_ignatz(Creature* pCreature)
{
    return new npc_stinky_ignatzAI(pCreature);
}

bool QuestAccept_npc_stinky_ignatz(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pPlayer && ((pPlayer->GetTeam() == ALLIANCE && pQuest->GetQuestId() == QUEST_STINKYS_ESCAPE_A) ||
        (pPlayer->GetTeam() == HORDE && pQuest->GetQuestId() == QUEST_STINKYS_ESCAPE_H)))
    {
        if (npc_stinky_ignatzAI* pEscortAI = dynamic_cast<npc_stinky_ignatzAI*>(pCreature->AI()))
        {
            pCreature->SetStandState(UNIT_STAND_STATE_STAND);
            pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
            pEscortAI->Start(false, pPlayer, pQuest);
        }
    }
    return true;
}

/*#######
## npc_bogbean_plant_dummy
####*/

enum
{
	QUEST_NPC_STINKY = 4880,
	QUEST_OBJECT_BOGBEAN_PLANT = 20939,
};

struct MANGOS_DLL_DECL npc_bogbean_plant_dummy : public ScriptedAI
{
    npc_bogbean_plant_dummy(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

	uint32 m_uiResetTimer;
	uint32 m_uiInitTimer;

	void Reset() 
	{
		m_creature->SetVisibility(VISIBILITY_OFF);
		m_uiResetTimer = 0;
		m_uiInitTimer = 2000;
	}

	void UpdateAI(const uint32 uiDiff)
	{
		if(GetClosestGameObjectWithEntry(m_creature, QUEST_OBJECT_BOGBEAN_PLANT, 20.0f) && GetClosestGameObjectWithEntry(m_creature, QUEST_OBJECT_BOGBEAN_PLANT, 20.0f)->getLootState() != GO_JUST_DEACTIVATED && 
			!m_uiResetTimer)
		{
			m_uiResetTimer = 30000;	
		}

		if(m_uiResetTimer)
		{
			if(m_uiResetTimer <= uiDiff)
			{
				if(GetClosestGameObjectWithEntry(m_creature, QUEST_OBJECT_BOGBEAN_PLANT, 20.0f))
				{			
					GetClosestGameObjectWithEntry(m_creature, QUEST_OBJECT_BOGBEAN_PLANT, 20.0f)->SetLootState(GO_JUST_DEACTIVATED);
					GetClosestGameObjectWithEntry(m_creature, QUEST_OBJECT_BOGBEAN_PLANT, 20.0f)->UpdateObjectVisibility();
				}
				m_uiResetTimer = 0;
			}
			else
			{
				m_uiResetTimer -= uiDiff;
			}
		}

		if(m_uiInitTimer)
		{
			if(m_uiInitTimer <= uiDiff)
			{
				if(GetClosestGameObjectWithEntry(m_creature, QUEST_OBJECT_BOGBEAN_PLANT, 20.0f))
				{			
					GetClosestGameObjectWithEntry(m_creature, QUEST_OBJECT_BOGBEAN_PLANT, 20.0f)->SetLootState(GO_JUST_DEACTIVATED);
					GetClosestGameObjectWithEntry(m_creature, QUEST_OBJECT_BOGBEAN_PLANT, 20.0f)->UpdateObjectVisibility();
				}
				m_uiInitTimer = 0;
			}
			else
			{
				m_uiInitTimer -= uiDiff;
			}
		}

	}
};

CreatureAI* GetAI_npc_bogbean_plant_dummy(Creature* pCreature)
{
    return new npc_bogbean_plant_dummy(pCreature);
}

/*####
# npc_krog
####*/

enum
{
	KROG_SAY_1					 = -1720055,
	KROG_SAY_2					 = -1720056,
	KAGORO_SAY					 = -1720057,

	NPC_KAGORO					 = 4972,

    QUEST_ID_SUSPICIOUS_HOOFPRINTS = 1268,
};

struct MANGOS_DLL_DECL npc_krogAI : public npc_escortAI
{
    npc_krogAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }
	
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
		DoScriptText(KROG_SAY_1, m_creature);
		m_creature->SummonCreature(NPC_KAGORO, -3156.64f, -2900.26f, 34.0025f,0,TEMPSUMMON_DEAD_DESPAWN, 60000, true);
		m_bOutro = true; 
		m_uiSpeechTimer = 5000;
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
				Creature* pKagoro = GetClosestCreatureWithEntry(m_creature, NPC_KAGORO, 10.0f);

                switch(m_uiSpeechStep)
                {
                    case 1:
                        DoScriptText(KROG_SAY_2, m_creature);
						m_uiSpeechTimer = 4000;
                        break;
					case 2:
						DoScriptText(KAGORO_SAY, pKagoro);
                        m_uiSpeechTimer = 3000;
                        break;
					case 3:
						pKagoro->HandleEmote(EMOTE_ONESHOT_SALUTE);
						m_uiSpeechTimer = 3000;
                        break;
					case 4:
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

CreatureAI* GetAI_npc_krog(Creature* pCreature)
{
    return new npc_krogAI(pCreature);
}

bool OnQuestRewarded_npc_krog(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
	if (pQuest->GetQuestId() == QUEST_ID_SUSPICIOUS_HOOFPRINTS)
    {
		if (npc_krogAI* pEscortAI = dynamic_cast<npc_krogAI*>(pCreature->AI()))
		{
			//pEscortAI->Start(false, pPlayer, pQuest);
			pEscortAI->StartOutro();
		}
	}
	return true;
}

enum doctor_weavil
{
    SPELL_MIND_SHATTER = 25774,
    SPELL_CREATURE_OF_NIGHTMARE = 25772,
};

struct MANGOS_DLL_DECL boss_doctor_weavil : public ScriptedAI
{
    boss_doctor_weavil(Creature* pCreature) : ScriptedAI(pCreature) {
        Reset();
    }

    uint32 m_uiMindShatterTimer;
    uint32 m_uiMindControlTimer;

    void Reset()
    {   
        m_uiMindShatterTimer = urand(5000, 15000);
        m_uiMindControlTimer = urand(15000, 18000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_uiMindShatterTimer)
        {
            if(m_uiMindShatterTimer <= uiDiff)
            {
                m_uiMindShatterTimer = urand(10000, 15000);
                DoCast(m_creature, SPELL_MIND_SHATTER, true);
            }
            else
                m_uiMindShatterTimer -= uiDiff;
        }

        if(m_uiMindControlTimer)
        {
            if(m_uiMindControlTimer <= uiDiff)
            {
                m_uiMindControlTimer = urand(35000, 50000);

                const ThreatList& threatList = m_creature->getThreatManager().getThreatList();

                for (HostileReference* currentReference : threatList)
                {
                    Unit* target = currentReference->getTarget();
                    if (target)
                    {

                        DoCast(target, SPELL_CREATURE_OF_NIGHTMARE, true);

                        if (!m_creature->getVictim())
                            return;

                        break;
                    }
                }
            }
            else
                m_uiMindControlTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_doctor_weavil(Creature* pCreature)
{
    return new boss_doctor_weavil(pCreature);
}

/* AddSC */

void AddSC_dustwallow_marsh()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_lady_jaina_proudmoore";
	pNewScript->GetAI = &GetAI_npc_lady_jaina_proudmoore;
    pNewScript->pGossipHello = &GossipHello_npc_lady_jaina_proudmoore;
    pNewScript->pGossipSelect = &GossipSelect_npc_lady_jaina_proudmoore;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
	pNewScript->Name = "npc_pained";
	pNewScript->GetAI = &GetAI_npc_pained;
	pNewScript->RegisterSelf();

	pNewScript = new Script;
	pNewScript->Name = "npc_archmage_tervosh";
	pNewScript->GetAI = &GetAI_npc_archmage_tervosh;
	pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_morokk";
    pNewScript->GetAI = &GetAI_npc_morokk;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_morokk;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_nat_pagle";
    pNewScript->pGossipHello = &GossipHello_npc_nat_pagle;
    pNewScript->pGossipSelect = &GossipSelect_npc_nat_pagle;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_ogron";
    pNewScript->GetAI = &GetAI_npc_ogron;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_ogron;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_private_hendel";
    pNewScript->GetAI = &GetAI_npc_private_hendel;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_private_hendel;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_stinky_ignatz";
    pNewScript->GetAI = &GetAI_npc_stinky_ignatz;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_stinky_ignatz;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
	pNewScript->Name = "npc_bogbean_plant_dummy";
	pNewScript->GetAI = &GetAI_npc_bogbean_plant_dummy;
	pNewScript->RegisterSelf();

	pNewScript = new Script;
    pNewScript->Name = "npc_krog";
    pNewScript->GetAI = &GetAI_npc_krog;
    pNewScript->pQuestRewardedNPC = &OnQuestRewarded_npc_krog;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
	pNewScript->Name = "boss_doctor_weavil";
	pNewScript->GetAI = &GetAI_boss_doctor_weavil;
	pNewScript->RegisterSelf();
}
