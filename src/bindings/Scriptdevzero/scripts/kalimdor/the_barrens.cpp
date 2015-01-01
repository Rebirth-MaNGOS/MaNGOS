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
SDName: The_Barrens
SD%Complete: 90
SDComment: Quest support: 863, 898, 1719, 2381, 2458, 4021, 4921, 6981
SDCategory: Barrens
EndScriptData */

/* ContentData
npc_beaten_corpse
npc_gilthares
npc_sputtervalve
npc_taskmaster_fizzule
npc_twiggy_flathead
at_twiggy_flathead
npc_wizzlecrank_shredder
npc_wrenixs_gizmotronic_apparatus
npc_regthar_deathgate
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"

/*######
## npc_beaten_corpse
######*/

enum
{
    QUEST_LOST_IN_BATTLE    = 4921
};

bool GossipHello_npc_beaten_corpse(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_LOST_IN_BATTLE) == QUEST_STATUS_INCOMPLETE ||
        pPlayer->GetQuestStatus(QUEST_LOST_IN_BATTLE) == QUEST_STATUS_COMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT,"Examine corpse in detail...",GOSSIP_SENDER_MAIN,GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(3557, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_beaten_corpse(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF +1)
    {
        pPlayer->SEND_GOSSIP_MENU(3558, pCreature->GetObjectGuid());
        pPlayer->TalkedToCreature(pCreature->GetEntry(), pCreature->GetObjectGuid());
    }
    return true;
}

/*######
# npc_gilthares
######*/

enum
{
    SAY_GIL_START               = -1000370,
    SAY_GIL_AT_LAST             = -1000371,
    SAY_GIL_PROCEED             = -1000372,
    SAY_GIL_FREEBOOTERS         = -1000373,
    SAY_GIL_AGGRO_1             = -1000374,
    SAY_GIL_AGGRO_2             = -1000375,
    SAY_GIL_AGGRO_3             = -1000376,
    SAY_GIL_AGGRO_4             = -1000377,
    SAY_GIL_ALMOST              = -1000378,
    SAY_GIL_SWEET               = -1000379,
    SAY_GIL_FREED               = -1000380,

    QUEST_FREE_FROM_HOLD        = 898,
    AREA_MERCHANT_COAST         = 391
};

struct MANGOS_DLL_DECL npc_giltharesAI : public npc_escortAI
{
    npc_giltharesAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    void Reset() { }

    void WaypointReached(uint32 uiPointId)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch(uiPointId)
        {
            case 16:
                DoScriptText(SAY_GIL_AT_LAST, m_creature, pPlayer);
                break;
            case 17:
                DoScriptText(SAY_GIL_PROCEED, m_creature, pPlayer);
                break;
            case 18:
                DoScriptText(SAY_GIL_FREEBOOTERS, m_creature, pPlayer);
                break;
            case 37:
                DoScriptText(SAY_GIL_ALMOST, m_creature, pPlayer);
                break;
            case 47:
                DoScriptText(SAY_GIL_SWEET, m_creature, pPlayer);
                break;
            case 53:
                DoScriptText(SAY_GIL_FREED, m_creature, pPlayer);
                pPlayer->GroupEventHappens(QUEST_FREE_FROM_HOLD, m_creature);
                break;
        }
    }

    void Aggro(Unit* pWho)
    {
        //not always use
        if (urand(0, 3))
            return;

        //only aggro text if not player and only in this area
        if (pWho->GetTypeId() != TYPEID_PLAYER && m_creature->GetAreaId() == AREA_MERCHANT_COAST)
        {
            //appears to be pretty much random (possible only if escorter not in combat with pWho yet?)
            switch(urand(0, 3))
            {
                case 0: DoScriptText(SAY_GIL_AGGRO_1, m_creature, pWho); break;
                case 1: DoScriptText(SAY_GIL_AGGRO_2, m_creature, pWho); break;
                case 2: DoScriptText(SAY_GIL_AGGRO_3, m_creature, pWho); break;
                case 3: DoScriptText(SAY_GIL_AGGRO_4, m_creature, pWho); break;
            }
        }
    }
};

CreatureAI* GetAI_npc_gilthares(Creature* pCreature)
{
    return new npc_giltharesAI(pCreature);
}

bool QuestAccept_npc_gilthares(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_FREE_FROM_HOLD)
    {
        pCreature->setFaction(FACTION_ESCORT_H_NEUTRAL_ACTIVE);
        pCreature->SetStandState(UNIT_STAND_STATE_STAND);

        DoScriptText(SAY_GIL_START, pCreature, pPlayer);

        if (npc_giltharesAI* pEscortAI = dynamic_cast<npc_giltharesAI*>(pCreature->AI()))
            pEscortAI->Start(false, pPlayer, pQuest);
    }
    return true;
}

/*######
## npc_sputtervalve
######*/

bool GossipHello_npc_sputtervalve(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(6981) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT,"Can you tell me about this shard?",GOSSIP_SENDER_MAIN,GOSSIP_ACTION_INFO_DEF);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_sputtervalve(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF)
    {
        pPlayer->SEND_GOSSIP_MENU(2013, pCreature->GetObjectGuid());
        pPlayer->AreaExploredOrEventHappens(6981);
    }
    return true;
}

/*######
## npc_taskmaster_fizzule
######*/

enum
{
    FACTION_FRIENDLY_F  = 35,
    SPELL_FLARE         = 10113,
    SPELL_FOLLY         = 10137,
};

struct MANGOS_DLL_DECL npc_taskmaster_fizzuleAI : public ScriptedAI
{
    npc_taskmaster_fizzuleAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        factionNorm = pCreature->getFaction();
        Reset();
    }

    uint32 factionNorm;
    bool IsFriend;
    uint32 Reset_Timer;
    uint8 FlareCount;

    void Reset()
    {
        IsFriend = false;
        Reset_Timer = 120000;
        FlareCount = 0;
        m_creature->setFaction(factionNorm);
    }

    void DoFriend()
    {
        m_creature->RemoveAllAuras();
        m_creature->DeleteThreatList();
        m_creature->CombatStop(true);

        m_creature->StopMoving();
        m_creature->GetMotionMaster()->MoveIdle();

        m_creature->setFaction(FACTION_FRIENDLY_F);
        m_creature->HandleEmote(EMOTE_ONESHOT_SALUTE);
    }

    void SpellHit(Unit* /*caster*/, const SpellEntry *spell)
    {
        if (spell->Id == SPELL_FLARE || spell->Id == SPELL_FOLLY)
        {
            ++FlareCount;

            if (FlareCount >= 2)
                IsFriend = true;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (IsFriend)
        {
            if (Reset_Timer < diff)
            {
                ResetToHome();
            } else Reset_Timer -= diff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }

    void ReceiveEmote(Player* /*pPlayer*/, uint32 emote)
    {
        if (emote == TEXTEMOTE_SALUTE)
        {
            if (FlareCount >= 2)
            {
                if (m_creature->getFaction() == FACTION_FRIENDLY_F)
                    return;

                DoFriend();
            }
        }
    }
};

CreatureAI* GetAI_npc_taskmaster_fizzule(Creature* pCreature)
{
    return new npc_taskmaster_fizzuleAI(pCreature);
}

/*#####
## npc_twiggy_flathead
#####*/

#define SAY_BIG_WILL_READY                  -1000123
#define SAY_TWIGGY_BEGIN                    -1000124
#define SAY_TWIGGY_FRAY                     -1000125
#define SAY_TWIGGY_DOWN                     -1000126
#define SAY_TWIGGY_OVER                     -1000127

#define NPC_TWIGGY                          6248
#define NPC_BIG_WILL                        6238
#define NPC_AFFRAY_CHALLENGER               6240
#define QUEST_AFFRAY                        1719

float AffrayChallengerLoc[6][4]=
{
    {-1683.0f, -4326.0f, 2.79f, 0.0f},
    {-1682.0f, -4329.0f, 2.79f, 0.0f},
    {-1683.0f, -4330.0f, 2.79f, 0.0f},
    {-1680.0f, -4334.0f, 2.79f, 1.49f},
    {-1674.0f, -4326.0f, 2.79f, 3.49f},
    {-1677.0f, -4334.0f, 2.79f, 1.66f}
};

struct MANGOS_DLL_DECL npc_twiggy_flatheadAI : public ScriptedAI
{
    npc_twiggy_flatheadAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    bool EventInProgress;

    uint32 Event_Timer;
    uint32 Step;
    uint32 Challenger_Count;
    uint32 ChallengerDeath_Timer;

    ObjectGuid PlayerGUID;
    ObjectGuid BigWillGUID;
    ObjectGuid AffrayChallenger[6];

    void Reset()
    {
        EventInProgress = false;

        Event_Timer = 2000;
        Step = 0;
        Challenger_Count = 0;
        ChallengerDeath_Timer = 0;

		PlayerGUID.Clear();
		BigWillGUID.Clear();

        for(uint8 i = 0; i < 6; ++i)
			AffrayChallenger[i].Clear();
    }

    bool CanStartEvent(Player* pPlayer)
    {
        if (!EventInProgress)
        {
            EventInProgress = true;
            PlayerGUID = pPlayer->GetObjectGuid();
            DoScriptText(SAY_TWIGGY_BEGIN, m_creature, pPlayer);
            return true;
        }

        debug_log("SD2: npc_twiggy_flathead event already in progress, need to wait.");
        return false;
    }

    void SetChallengers()
    {
        for(uint8 i = 0; i < 6; ++i)
        {
            Creature* pCreature = m_creature->SummonCreature(NPC_AFFRAY_CHALLENGER, AffrayChallengerLoc[i][0], AffrayChallengerLoc[i][1], AffrayChallengerLoc[i][2], AffrayChallengerLoc[i][3], TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 600000);
            if (!pCreature)
            {
                debug_log("SD2: npc_twiggy_flathead event cannot summon challenger as expected.");
                continue;
            }

            pCreature->setFaction(35);
            pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pCreature->HandleEmote(EMOTE_ONESHOT_ROAR);
            AffrayChallenger[i] = pCreature->GetObjectGuid();
        }
    }

    void SetChallengerReady(Unit *pUnit)
    {
        pUnit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        pUnit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        pUnit->HandleEmote(EMOTE_ONESHOT_ROAR);
        pUnit->setFaction(14);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!EventInProgress)
            return;

        if (ChallengerDeath_Timer)
        {
            if (ChallengerDeath_Timer <= diff)
            {
                for(uint8 i = 0; i < 6; ++i)
                {
                    Creature *challenger = m_creature->GetMap()->GetCreature(AffrayChallenger[i]);
                    if (challenger && !challenger->isAlive() && challenger->isDead())
                    {
                        DoScriptText(SAY_TWIGGY_DOWN, m_creature);
                        challenger->RemoveCorpse();
						AffrayChallenger[i].Clear();
                        continue;
                    }
                }
                ChallengerDeath_Timer = 2500;
            }
			else
				ChallengerDeath_Timer -= diff;
        }

        if (Event_Timer < diff)
        {
            Player* pPlayer = m_creature->GetMap()->GetPlayer(PlayerGUID);

            if (!pPlayer || pPlayer->isDead())
                Reset();

            switch(Step)
            {
                case 0:
                    SetChallengers();
                    ChallengerDeath_Timer = 2500;
                    Event_Timer = 5000;
                    ++Step;
                    break;
                case 1:
                    DoScriptText(SAY_TWIGGY_FRAY, m_creature);
                    if (Creature *challenger = m_creature->GetMap()->GetCreature(AffrayChallenger[Challenger_Count]))
                        SetChallengerReady(challenger);
                    else
						Reset();
                    ++Challenger_Count;
                    Event_Timer = 25000;
                    if (Challenger_Count == 6)
                        ++Step;
                    break;
                case 2:
                    if (Unit *temp = m_creature->SummonCreature(NPC_BIG_WILL,-1713.79f,-4342.09f,6.05f,6.15f,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,300000))
                    {
						BigWillGUID = temp->GetObjectGuid();
                        temp->setFaction(35);
                        temp->GetMotionMaster()->MovePoint(0,-1682.31f,-4329.68f,2.78f);
                    }
                    Event_Timer = 15000;
                    ++Step;
                    break;
                case 3:
                    if (Creature* will = m_creature->GetMap()->GetCreature(BigWillGUID))
                    {
                        will->setFaction(32);
                        DoScriptText(SAY_BIG_WILL_READY, will, pPlayer);
                    }
                    Event_Timer = 5000;
                    ++Step;
                    break;
                case 4:
                    Creature* will = m_creature->GetMap()->GetCreature(BigWillGUID);
                    if (will && will->isDead())
                    {
                        DoScriptText(SAY_TWIGGY_OVER, m_creature);
                        Reset();
                    } else if (!will)
                        Reset();
                    Event_Timer = 5000;
                    break;
            }
        } else Event_Timer -= diff;
    }
};

CreatureAI* GetAI_npc_twiggy_flathead(Creature* pCreature)
{
    return new npc_twiggy_flatheadAI(pCreature);
}

bool AreaTrigger_at_twiggy_flathead(Player* pPlayer, AreaTriggerEntry const* /*pAt*/)
{
    if (!pPlayer->isDead() && pPlayer->GetQuestStatus(QUEST_AFFRAY) == QUEST_STATUS_INCOMPLETE)
    {
        Creature* pCreature = GetClosestCreatureWithEntry(pPlayer, NPC_TWIGGY, 30.0f);

        if (!pCreature)
            return true;

        if (npc_twiggy_flatheadAI* pTwiggyAI = dynamic_cast<npc_twiggy_flatheadAI*>(pCreature->AI()))
        {
            if (pTwiggyAI->CanStartEvent(pPlayer))
                return false;                               //ok to let mangos process further
        }

        return true;
    }
    return true;
}

/*#####
## npc_wizzlecranks_shredder
#####*/

enum
{
    SAY_START           = -1000298,
    SAY_STARTUP1        = -1000299,
    SAY_STARTUP2        = -1000300,
    SAY_MERCENARY       = -1000301,
    SAY_PROGRESS_1      = -1000302,
    SAY_PROGRESS_2      = -1000303,
    SAY_PROGRESS_3      = -1000304,
    SAY_END             = -1000305,

    QUEST_ESCAPE        = 863,
    FACTION_RATCHET     = 637,
    NPC_PILOT_WIZZ      = 3451,
    NPC_MERCENARY       = 3282
};

struct MANGOS_DLL_DECL npc_wizzlecranks_shredderAI : public npc_escortAI
{
    npc_wizzlecranks_shredderAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_bIsPostEvent = false;
        m_uiPostEventTimer = 1000;
        m_uiPostEventCount = 0;
        Reset();
    }

    bool m_bIsPostEvent;
    uint32 m_uiPostEventTimer;
    uint32 m_uiPostEventCount;

    void Reset()
    {
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            if (m_creature->getStandState() == UNIT_STAND_STATE_DEAD)
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);

            m_bIsPostEvent = false;
            m_uiPostEventTimer = 1000;
            m_uiPostEventCount = 0;
        }
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 0:
                if (Player* pPlayer = GetPlayerForEscort())
                    DoScriptText(SAY_STARTUP1, m_creature, pPlayer);
                break;
            case 9:
                SetRun(false);
                break;
            case 17:
                if (Creature* pTemp = m_creature->SummonCreature(NPC_MERCENARY, 1128.489f, -3037.611f, 92.701f, 1.472f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000))
                {
                    DoScriptText(SAY_MERCENARY, pTemp);
                    m_creature->SummonCreature(NPC_MERCENARY, 1160.172f, -2980.168f, 97.313f, 3.690f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                }
                break;
            case 24:
                m_bIsPostEvent = true;
                break;
        }
    }

    void WaypointStart(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 9:
                if (Player* pPlayer = GetPlayerForEscort())
                    DoScriptText(SAY_STARTUP2, m_creature, pPlayer);
                break;
            case 18:
                if (Player* pPlayer = GetPlayerForEscort())
                    DoScriptText(SAY_PROGRESS_1, m_creature, pPlayer);
                SetRun();
                break;
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_PILOT_WIZZ)
            m_creature->SetStandState(UNIT_STAND_STATE_DEAD);

        if (pSummoned->GetEntry() == NPC_MERCENARY)
            pSummoned->AI()->AttackStart(m_creature);
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (m_bIsPostEvent)
            {
                if (m_uiPostEventTimer < uiDiff)
                {
                    switch(m_uiPostEventCount)
                    {
                        case 0:
                            DoScriptText(SAY_PROGRESS_2, m_creature);
                            break;
                        case 1:
                            DoScriptText(SAY_PROGRESS_3, m_creature);
                            break;
                        case 2:
                            DoScriptText(SAY_END, m_creature);
                            break;
                        case 3:
                            if (Player* pPlayer = GetPlayerForEscort())
                            {
                                pPlayer->GroupEventHappens(QUEST_ESCAPE, m_creature);
                                m_creature->SummonCreature(NPC_PILOT_WIZZ, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 180000);
                            }
                            break;
                    }

                    ++m_uiPostEventCount;
                    m_uiPostEventTimer = 5000;
                }
                else
                    m_uiPostEventTimer -= uiDiff;
            }

            return;
        }

        DoMeleeAttackIfReady();
    }
};

bool QuestAccept_npc_wizzlecranks_shredder(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ESCAPE)
    {
        DoScriptText(SAY_START, pCreature);
        pCreature->setFaction(FACTION_RATCHET);

        if (npc_wizzlecranks_shredderAI* pEscortAI = dynamic_cast<npc_wizzlecranks_shredderAI*>(pCreature->AI()))
            pEscortAI->Start(true, pPlayer, pQuest);
    }
    return true;
}

CreatureAI* GetAI_npc_wizzlecranks_shredder(Creature* pCreature)
{
    return new npc_wizzlecranks_shredderAI(pCreature);
}

/*######
## npc_wrenix_the_wretched
######*/

enum eWrenixAndHisBotPet
{
    QUEST_PLUNDERING_THE_PLUNDERERS   = 2381,

    SPELL_CONJURE_E_C_A_C             = 9977,
    SPELL_THIEVES_TOOLS_RACK_CONJURE  = 9949,
    SPELL_RED_FIREWORK                = 6668,

    NPC_WRENIXS_GIZMOTRONIC_APPARATUS = 7166
};

bool QuestAccept_npc_wrenix_the_wretched(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_PLUNDERING_THE_PLUNDERERS)
    {
        if (Creature* pWrenixPet = GetClosestCreatureWithEntry(pCreature, NPC_WRENIXS_GIZMOTRONIC_APPARATUS, DEFAULT_VISIBILITY_DISTANCE))
            pWrenixPet->CastSpell(pPlayer, SPELL_RED_FIREWORK, false);
        else
            debug_log("SD0: NPC Wrenix the Wretched can't find his Apparatus bot for cast Red Firework.");
    }
    return true;
}

/*######
## npc_wrenixs_gizmotronic_apparatus
######*/

bool GossipHello_npc_wrenixs_gizmotronic_apparatus(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_PLUNDERING_THE_PLUNDERERS) == QUEST_STATUS_INCOMPLETE ||
        pPlayer->GetQuestStatus(QUEST_PLUNDERING_THE_PLUNDERERS) == QUEST_STATUS_COMPLETE)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Give me some E.C.A.C", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Give me Thieves' Tools", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
    }

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_wrenixs_gizmotronic_apparatus(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pCreature->CastSpell(pPlayer, SPELL_CONJURE_E_C_A_C, false);
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pCreature->CastSpell(pPlayer, SPELL_THIEVES_TOOLS_RACK_CONJURE, false, 0, 0, pCreature->GetObjectGuid());
            break;
    }
    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

/*######
## mob_polly
######*/

enum ePolly
{
    SPELL_STEALTH                   = 8822,
    SPELL_SUMMON_POLLY_JR_          = 9998,
    SPELL_POLLY_EATS_THE_E_C_A_C_   = 9976,

    FACTION_FRIENDLY                = 35,
    FACTION_HOSTILE                 = 14,

    SAY_POLLY_1                     = -1000353,
    SAY_POLLY_2                     = -1000354,
    SAY_POLLY_3                     = -1000355,
};

struct MANGOS_DLL_DECL mob_pollyAI : public ScriptedAI
{
    mob_pollyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        DoScriptText(SAY_POLLY_1, m_creature);
        bFeeded = false;
        pHoldedTarget = 0;
        Reset();
    }

    uint32 m_uiStealthTimer;

    uint32 EventTimer;
    uint32 EventPhase;

    Unit* pHoldedTarget;
    bool bFeeded;

    void Reset()
    {
        m_uiStealthTimer = 73000;
        
        EventTimer = 0;
        EventPhase = 0;
    }

    void Aggro(Unit* pVictim)
    {
        if (m_creature->getFaction() != FACTION_FRIENDLY)
            DoCastSpellIfCan(pVictim, SPELL_SUMMON_POLLY_JR_);
    }

    void SpellHit(Unit* pUnit, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_POLLY_EATS_THE_E_C_A_C_)
            if (!bFeeded)
            {
                m_creature->setFaction(FACTION_FRIENDLY);
                m_creature->RemoveAllAuras();
                m_creature->CombatStop(true);
                m_creature->DeleteThreatList();

                DoScriptText(SAY_POLLY_2, m_creature);

                EventTimer = 10000;
                EventPhase = 1;
                
                pHoldedTarget = pUnit;
                bFeeded = true;
            }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (EventTimer)
        {
            if (EventTimer <= uiDiff)
            {
                switch(EventPhase)
                {
                    case 1:
                        DoScriptText(SAY_POLLY_3, m_creature);
                        EventTimer = 1500;
                        break;
                    case 2:
                        m_creature->HandleEmote(EMOTE_ONESHOT_CHEER);
                        EventTimer = 800;
                        break;
                    case 3:
                        m_creature->HandleEmote(EMOTE_ONESHOT_CHEER);
                        EventTimer = 800;
                        break;
                    case 4:
                        m_creature->HandleEmote(EMOTE_ONESHOT_CHEER);
                        EventTimer = 800;
                        break;
                    case 5:
                        m_creature->HandleEmote(EMOTE_ONESHOT_CHEER);
                        EventTimer = 800;
                        break;
                    case 6:
                        m_creature->HandleEmote(EMOTE_ONESHOT_CHEER);
                        EventTimer = 800;
                        break;
                    case 7:
                        m_creature->setFaction(FACTION_HOSTILE);
                        if (pHoldedTarget)
                            AttackStart(pHoldedTarget);
                        break;
                }
                ++EventPhase;
            }
            else
                EventTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            // Stealth
            if (m_uiStealthTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_STEALTH);
                m_uiStealthTimer = 60000;
            }
            else
                m_uiStealthTimer -= uiDiff;

            return;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_polly(Creature* pCreature)
{
    return new mob_pollyAI(pCreature);
}

/*######
## npc_regthar_deathgate
######*/

enum eRegtharDeathgate
{
    QUEST_COUNTERATTACK             = 4021,

    SAY_REGTHAR_BEWARE              = -1000677,
    YELL_REGTHAR_RETREATING         = -1000678,
    SAY_REGTHAR_DEFENDER_FALLEN     = -1000679,
    YELL_WARLORD_KROMZAR            = -1000680,
    YELL_LANTIGAH                   = -1000681,

    NPC_KOLKAR_INVADER              = 9524,
    NPC_KOLKAR_STORMSEER            = 9523,
    NPC_WARLORD_KROMZAR             = 9456,
    NPC_HORDE_DEFENDER              = 9457,
    NPC_HORDE_AXE_THROWER           = 9458,
    NPC_REGTHAR                     = 3389,

    SPELL_CREATE_KROMZAR_BANNER     = 13965,

    MAX_REGTHAR_EVENT_TIME          = 15 * MINUTE * IN_MILLISECONDS,
    MAX_POSITIONS                   = 19,
};

#define GOSSIP_ITEM_REGTHAR "Where is Warlord Krom'zar?"

float static SpawnPosition[MAX_POSITIONS][3]=
{
    // West
    {-292.87f, -1902.36f, 91.66f},
    {-282.26f, -1901.07f, 91.66f},
    {-270.97f, -1900.77f, 91.66f},
    {-272.80f, -1891.35f, 91.66f},
    {-287.75f, -1887.86f, 92.50f},
    {-297.58f, -1882.00f, 91.81f},
    {-289.30f, -1878.99f, 92.63f},
    {-277.64f, -1878.60f, 92.59f},
    // Too far away
    {-288.73f, -1869.65f, 92.67f},
    {-298.90f, -1869.86f, 92.51f},
    {-311.14f, -1870.11f, 93.00f},
    {-332.28f, -1869.14f, 93.78f},
    {-272.18f, -1856.07f, 93.15f},
    {-290.78f, -1850.82f, 92.63f},
    {-281.01f, -1846.74f, 92.68f},

    // North
    {-240.23f, -1915.00f, -92.14f},
    {-230.42f, -1902.68f, 91.70f},
    {-231.31f, -1934.65f, 93.70f},
    {-246.66f, -1928.84f, 92.38f}
};

struct npc_regthar_deathgateAI : public ScriptedAI
{
    npc_regthar_deathgateAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiCancelEventTimer = 0;
        m_uiEventTimer = 0;
        m_uiEventPhase = 0;

        m_bKromzarSpawned = false;
        m_bKromzarDeath = false;

        Reset();
    }

    uint32 m_uiCancelEventTimer;
    uint32 m_uiEventTimer;
    uint8  m_uiEventPhase;

    ObjectGuid m_uiPlayerGUID;
    ObjectGuid m_uiKromzarGUID;

    bool m_bKromzarSpawned;
    bool m_bKromzarDeath;

    GUIDList m_lSummonedNPCs;

    void Reset()
	{
		m_uiPlayerGUID.Clear();
		m_uiKromzarGUID.Clear();

		m_lSummonedNPCs.clear();
	}

    bool StartEvent(ObjectGuid pPlayerGUID)
    {
        // Encounter must to be to idle
        if (PlayerExists())
            return false;

        m_uiPlayerGUID = pPlayerGUID;
        m_uiEventPhase = 10;
        m_uiEventTimer = 2000;

        m_uiCancelEventTimer = MAX_REGTHAR_EVENT_TIME;

        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        return true;
    }

    void ResetEvent()
    {
        m_uiCancelEventTimer = 0;
        m_uiEventTimer = 0;
        m_uiEventPhase = 0;

		m_uiPlayerGUID.Clear();
		m_uiKromzarGUID.Clear();

        m_bKromzarSpawned = false;
        m_bKromzarDeath = false;

        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        // Despawn all summoned NPCs
        if (m_lSummonedNPCs.size())
            for(GUIDList::iterator itr = m_lSummonedNPCs.begin(); itr != m_lSummonedNPCs.end(); ++itr)
                if (Creature* pCreature = m_creature->GetMap()->GetCreature(*itr))
                    pCreature->ForcedDespawn();
    }

    bool PlayerExists()
    {
        if (m_creature->GetMap()->GetPlayer(m_uiPlayerGUID))
            return true;

        ResetEvent();
        return false;
    }

    ObjectGuid SummonRegtharNpc(uint8 position_id, bool invader_true__defender_false = true)
    {
        uint32 uiEntry = 0;
        if (invader_true__defender_false)
            uiEntry = rand()%2 ? NPC_KOLKAR_INVADER : NPC_KOLKAR_STORMSEER;
        else
            uiEntry = rand()%2 ? NPC_HORDE_DEFENDER : NPC_HORDE_AXE_THROWER;
        Creature* pSummoned = m_creature->SummonCreature(uiEntry, SpawnPosition[position_id][0], SpawnPosition[position_id][1], SpawnPosition[position_id][2], 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0);
        return pSummoned ? pSummoned->GetObjectGuid() : ObjectGuid();
    }

    void JustSummoned(Creature* pSummoned)
    {
        m_lSummonedNPCs.push_back(pSummoned->GetObjectGuid());
    }

    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_HORDE_DEFENDER || pSummoned->GetEntry() == NPC_HORDE_AXE_THROWER)
            DoScriptText(SAY_REGTHAR_DEFENDER_FALLEN, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiPlayerGUID)
        {
            if (m_uiCancelEventTimer <= uiDiff)
                ResetEvent();
            else
                m_uiCancelEventTimer -= uiDiff;

            // This line will reset event, if player for quest does not exists anymore
            if (!PlayerExists())
                return;

            if (m_uiEventTimer)
            {
                if (m_uiEventTimer <= uiDiff)
                {
                    m_uiEventTimer = 0;
                    switch(++m_uiEventPhase)
                    {
                        case 11:    // first
                            // spawn troops of the battlefield
                            for(uint8 itr = 0; itr < MAX_POSITIONS; ++itr)
                                SummonRegtharNpc(itr, itr % 2);
                            m_uiEventTimer = 15000;
                            break;
                        case 12:
                            for(uint8 itr = MAX_POSITIONS; itr < MAX_POSITIONS/2; ++itr)
                                if (itr % 2 == 0)
                                    SummonRegtharNpc(itr, true);
                            // +2 friendly horde defenders :)
                            SummonRegtharNpc(4, false);
                            SummonRegtharNpc(5, false);
                            m_uiEventTimer = 15000;
                            break;
                        case 13:
                            for(uint8 itr = 0; itr < MAX_POSITIONS/2; ++itr)
                                if (itr % 2)
                                    SummonRegtharNpc(itr, true);
                            m_uiEventTimer = 10000;
                            break;
                        case 14:
                            for(uint8 itr = 0; itr < MAX_POSITIONS/2; ++itr)
                                if (itr % 2)
                                    SummonRegtharNpc(itr, true);
                            m_uiEventTimer = 30000;
                            break;
                        case 15:
                            // Warlord + 2 adds
                            SummonRegtharNpc(4, true);
                            SummonRegtharNpc(6, true);
                            if (Creature* pKromzar = m_creature->SummonCreature(NPC_WARLORD_KROMZAR, SpawnPosition[5][0], SpawnPosition[5][1], SpawnPosition[5][2], 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0))
                            {
                                m_uiKromzarGUID = pKromzar->GetObjectGuid();
                                m_bKromzarSpawned = true;
                                DoScriptText(YELL_WARLORD_KROMZAR, pKromzar);
                            }
                            m_uiEventPhase = 0;
                            m_uiEventTimer = 0;
                            break;
                    }
                }
                else
                    m_uiEventTimer -= uiDiff;
            }

            if (m_bKromzarSpawned && !m_bKromzarDeath)
                if (Creature* pKromzar = m_creature->GetMap()->GetCreature(m_uiKromzarGUID))
                    if (pKromzar->isDead())
                    {
                        m_bKromzarDeath = true;
                        DoScriptText(YELL_REGTHAR_RETREATING, m_creature);
                    }
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_regthar_deathgate(Creature* pCreature)
{
    return new npc_regthar_deathgateAI(pCreature);
}

bool GossipHello_npc_regthar_deathgate(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(QUEST_COUNTERATTACK) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_REGTHAR, GOSSIP_SENDER_MAIN, (GOSSIP_ACTION_INFO_DEF + 1));

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_regthar_deathgate(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiSender == GOSSIP_SENDER_MAIN && uiAction == (GOSSIP_ACTION_INFO_DEF + 1))
    {
        if (npc_regthar_deathgateAI* pRetgharAI = dynamic_cast<npc_regthar_deathgateAI*>(pCreature->AI()))
        {
			if (pRetgharAI->StartEvent(pPlayer->GetObjectGuid()))
            {
                DoScriptText(SAY_REGTHAR_BEWARE, pCreature, pPlayer);
                pPlayer->SEND_GOSSIP_MENU(2534, pCreature->GetObjectGuid());
                return true;
            }
            else
                pCreature->MonsterSay("Not now, $N..", LANG_UNIVERSAL, pPlayer);
        }
    }

    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

bool QuestRewarded_npc_regthar_deathgate(Player* /*pPlayer*/, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_COUNTERATTACK)
        if (npc_regthar_deathgateAI* pRetgharAI = dynamic_cast<npc_regthar_deathgateAI*>(pCreature->AI()))
            pRetgharAI->ResetEvent();
    return true;
}

/*######
## npc_lantigah
######*/

struct MANGOS_DLL_DECL npc_lantigahAI : public ScriptedAI
{
    npc_lantigahAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset()
    {
    }

    void Aggro(Unit* /*pAttacker*/)
    {
        if (Creature* pRegthar = GetClosestCreatureWithEntry(m_creature, NPC_REGTHAR, DEFAULT_VISIBILITY_DISTANCE))
            if (npc_regthar_deathgateAI* pRegtharAI = dynamic_cast<npc_regthar_deathgateAI*>(pRegthar->AI()))
                if (pRegtharAI->PlayerExists())
                    DoScriptText(YELL_LANTIGAH, m_creature);
    }
};

CreatureAI* GetAI_npc_lantigah(Creature* pCreature)
{
    return new npc_lantigahAI(pCreature);
}

/*######
## mob_warlord_kromzar
######*/

enum eWarlordKromzar
{
    SPELL_STRIKE                    = 11976,
    //SPELL_CREATE_KROMZAR_BANNER     = 13965,
};

struct MANGOS_DLL_DECL mob_warlord_kromzarAI : public ScriptedAI
{
    mob_warlord_kromzarAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiStrikeTimer;

    void Reset()
    {
        m_uiStrikeTimer = 10000;
    }

    void JustDied(Unit* /*pKiller*/)
    {
        m_creature->CastSpell(m_creature, SPELL_CREATE_KROMZAR_BANNER, true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiStrikeTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_STRIKE))
                m_uiStrikeTimer = urand(15000, 20000);
        }
        else
            m_uiStrikeTimer -= uiDiff;
    }
};

CreatureAI* GetAI_mob_warlord_kromzar(Creature* pCreature)
{
    return new mob_warlord_kromzarAI(pCreature);
}

/*######
## NPCs for rogue quest 2478
######*/

struct MANGOS_DLL_DECL mob_mutated_droneAI : public ScriptedAI
{
	mob_mutated_droneAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

	void Reset()
	{
	}

	void SpellHit(Unit* U, const SpellEntry* pSpell) 
	{
		if (!U || !pSpell)
			return;

		//Instant kill when ambushed
		if (pSpell->SpellIconID == 856 && pSpell->SpellVisual == 155)
			U->DealDamage(m_creature,1000,NULL,DIRECT_DAMAGE,SPELL_SCHOOL_MASK_NORMAL,0,false);
	}

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		DoMeleeAttackIfReady();
	}

};

CreatureAI* GetAI_mob_mutated_drone(Creature* pCreature)
{
    return new mob_mutated_droneAI(pCreature);
}

struct MANGOS_DLL_DECL mob_rogue_patrollerAI : public ScriptedAI
{
	mob_rogue_patrollerAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

	void Reset()
	{
	}

	void SpellHit(Unit* U, const SpellEntry* pSpell) 
	{
		if (!U || !pSpell)
			return;

		//Additional damage from Rupture
		if (pSpell->SpellFamilyName == SPELLFAMILY_ROGUE && pSpell->SpellFamilyFlags & UI64LIT(0x000000000000100000))
			m_creature->CastSpell(m_creature,10266,true,0,0,U->GetObjectGuid());
	}

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		DoMeleeAttackIfReady();
	}

};

CreatureAI* GetAI_mob_rogue_patroller(Creature* pCreature)
{
    return new mob_rogue_patrollerAI(pCreature);
}

struct MANGOS_DLL_DECL mob_rogue_lookoutAI : public ScriptedAI
{
	mob_rogue_lookoutAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

	void Reset()
	{
	}

	void SpellHit(Unit* U, const SpellEntry* pSpell) 
	{
		if (!U || !pSpell)
			return;

		// Workaround: Kill when hit by eviscerate
		if (pSpell->Id != 27611 && pSpell->SpellIconID == 514 && pSpell->SpellVisual == 671)
			U->DealDamage(m_creature,1000,NULL,DIRECT_DAMAGE,SPELL_SCHOOL_MASK_NORMAL,0,false);
	}

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		DoMeleeAttackIfReady();
	}

};

CreatureAI* GetAI_mob_rogue_lookout(Creature* pCreature)
{
    return new mob_rogue_lookoutAI(pCreature);
}

struct MANGOS_DLL_DECL mob_gallywixAI : public ScriptedAI
{
	mob_gallywixAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

	void Reset()
	{
	}

	void SpellHit(Unit* U, const SpellEntry* pSpell) 
	{
		if (!U || !pSpell)
			return;

		//Ambush deals additional damage
		if (pSpell->SpellIconID == 856 && pSpell->SpellVisual == 155)
			U->DealDamage(m_creature,800,NULL,DIRECT_DAMAGE,SPELL_SCHOOL_MASK_NORMAL,0,false);
	}

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		DoMeleeAttackIfReady();
	}

};

CreatureAI* GetAI_mob_gallywix(Creature* pCreature)
{
    return new mob_gallywixAI(pCreature);
}

/* go_ */

void AddSC_the_barrens()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "npc_beaten_corpse";
    pNewscript->pGossipHello = &GossipHello_npc_beaten_corpse;
    pNewscript->pGossipSelect = &GossipSelect_npc_beaten_corpse;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_gilthares";
    pNewscript->GetAI = &GetAI_npc_gilthares;
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_gilthares;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_sputtervalve";
    pNewscript->pGossipHello = &GossipHello_npc_sputtervalve;
    pNewscript->pGossipSelect = &GossipSelect_npc_sputtervalve;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_taskmaster_fizzule";
    pNewscript->GetAI = &GetAI_npc_taskmaster_fizzule;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_twiggy_flathead";
    pNewscript->GetAI = &GetAI_npc_twiggy_flathead;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "at_twiggy_flathead";
    pNewscript->pAreaTrigger = &AreaTrigger_at_twiggy_flathead;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_wizzlecranks_shredder";
    pNewscript->GetAI = &GetAI_npc_wizzlecranks_shredder;
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_wizzlecranks_shredder;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_wrenix_the_wretched";
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_wrenix_the_wretched;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_wrenixs_gizmotronic_apparatus";
    pNewscript->pGossipHello = &GossipHello_npc_wrenixs_gizmotronic_apparatus;
    pNewscript->pGossipSelect = &GossipSelect_npc_wrenixs_gizmotronic_apparatus;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_polly";
    pNewscript->GetAI = &GetAI_mob_polly;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_regthar_deathgate";
    pNewscript->GetAI = &GetAI_npc_regthar_deathgate;
    pNewscript->pGossipHello = &GossipHello_npc_regthar_deathgate;
    pNewscript->pGossipSelect = &GossipSelect_npc_regthar_deathgate;
    pNewscript->pQuestRewardedNPC = &QuestRewarded_npc_regthar_deathgate;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_lantigah";
    pNewscript->GetAI = &GetAI_npc_lantigah;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_warlord_kromzar";
    pNewscript->GetAI = &GetAI_mob_warlord_kromzar;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_mutated_drone";
    pNewscript->GetAI = &GetAI_mob_mutated_drone;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_rogue_patroller";
    pNewscript->GetAI = &GetAI_mob_rogue_patroller;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_rogue_lookout";
    pNewscript->GetAI = &GetAI_mob_rogue_lookout;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_gallywix";
    pNewscript->GetAI = &GetAI_mob_gallywix;
    pNewscript->RegisterSelf();
}
