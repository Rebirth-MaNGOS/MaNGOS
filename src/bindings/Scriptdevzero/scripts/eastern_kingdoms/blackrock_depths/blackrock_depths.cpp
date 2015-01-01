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
SDName: Blackrock_Depths
SD%Complete: 60
SDComment: Quest support: 4001, 4322, 4342, 7604, 9015. Vendor Lokhtos Darkbargainer.
SDCategory: Blackrock Depths
EndScriptData */

/* ContentData
npc_marshal_windsor
npc_commander_gorshak
go_shadowforge_lock
go_shadowforge_brazier
go_dark_keeper_portrait
go_relic_coffer_door
mob_warbringer_construct
the_grim_guzzler_patrons
go_thunderbrew_lager_keg
at_ring_of_law
npc_grimstone
npc_theldren_trigger
mob_phalanx
npc_kharan_mighthammer
npc_lokhtos_darkbargainer
npc_rocknot
npc_mistress_nagmara
EndContentData */

#include "precompiled.h"
#include "blackrock_depths.h"
#include "escort_ai.h"
#include "follower_ai.h"

/*######
## npc_marshal_windsor
######*/

enum eMarshalWindsor
{
    SAY_WINDSOR_1                       = -1230010,
    SAY_WINDSOR_2                       = -1230012,
    SAY_WINDSOR_3                       = -1230015,
    SAY_WINDSOR_4                       = -1230016,
    SAY_WINDSOR_5                       = -1230018,

    SAY_REGINALD_WINDSOR_1              = -1230019,
    SAY_REGINALD_WINDSOR_2              = -1230020,
    SAY_REGINALD_WINDSOR_3              = -1230021,
    SAY_REGINALD_WINDSOR_4              = -1230022,
    SAY_REGINALD_WINDSOR_5              = -1230023,
    SAY_REGINALD_WINDSOR_6              = -1230024,
    SAY_REGINALD_WINDSOR_7              = -1230026,
    SAY_REGINALD_WINDSOR_8              = -1230027,
    SAY_REGINALD_WINDSOR_9              = -1230028,
    SAY_REGINALD_WINDSOR_10             = -1230029,
    SAY_REGINALD_WINDSOR_11             = -1230031,
    SAY_REGINALD_WINDSOR_12             = -1230032,
    SAY_REGINALD_WINDSOR_13             = -1230033,
    SAY_REGINALD_WINDSOR_14             = -1230035,
    SAY_REGINALD_WINDSOR_15             = -1230036,
    SAY_REGINALD_WINDSOR_16             = -1230037,

    SAY_DUGHAL                          = -1230014,
    SAY_SHILL                           = -1230025,
    SAY_CREST                           = -1230030,
    SAY_TOBIAS                          = -1230034,

    SAY_CHECK_THAT_CELL                 = -1230011,
    SAY_LOCKED_WRONG_MARSHAL            = -1230017,
    SAY_I_BET_YOU                       = -1230013,

    QUEST_JAIL_BREAK                    = 4322,

    SPELL_DUST_CLOUD                    = 7272,
    SPELL_DEVOTION_AURA                 = 10293,

    MODELID_MARSHAL_REGINALD_WINDSOR    = 9052,
};

struct MANGOS_DLL_DECL npc_marshal_windsorAI : public npc_escortAI
{
    npc_marshal_windsorAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_pInstance = (instance_blackrock_depths*)pCreature->GetInstanceData();
        Reset();
    }

    instance_blackrock_depths* m_pInstance;

    uint8  m_uiEventPhase;
    uint32 m_uiEventTimer;

    uint8 m_uiLastReachedWaypoint;

    bool m_bCanWalk;

    void Reset()
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
            return;

        m_uiEventPhase = 0;
        m_uiEventTimer = 0;

        m_uiLastReachedWaypoint = 0;

        m_bCanWalk = true;
    }

    void Aggro(Unit* pUnit)
    {
        DoScriptText(rand()%2 == 0 ? SAY_LOCKED_WRONG_MARSHAL : SAY_I_BET_YOU, m_creature, pUnit);
    }

    void WaypointReached(uint32 uiPointId)
    {
        m_uiLastReachedWaypoint = uiPointId;

        switch(uiPointId)
        {
        case 8:
            m_bCanWalk = false;
            m_uiEventPhase = 1;
            m_uiEventTimer = 2000;
            break;

        case 12:
            m_bCanWalk = false;
            m_uiEventPhase = 10;
            m_uiEventTimer = 2000;
            break;
        case 15:
            DoScriptText(SAY_WINDSOR_5, m_creature);
            m_creature->SetEntry(NPC_MARSHAL_REGINALD_WINDSOR);
            m_creature->SetDisplayId(MODELID_MARSHAL_REGINALD_WINDSOR);
            m_creature->SetSheath(SHEATH_STATE_MELEE);
            DoCastSpellIfCan(m_creature, SPELL_DUST_CLOUD, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_DEVOTION_AURA, CAST_TRIGGERED);
            break;
        case 16:
            SetRun(false);
            break;
        case 17:
            m_bCanWalk = false;
            m_uiEventPhase = 20;
            m_uiEventTimer = 100;
            break;

        case 21:
            m_bCanWalk = false;
            m_uiEventPhase = 30;
            m_uiEventTimer = 2000;
            break;

        case 22:
            DoSpawnGuards(597.22f, -130.08f, -69.23f);
            break;

        case 23:
            m_bCanWalk = false;
            m_uiEventPhase = 40;
            m_uiEventTimer = 2000;
            break;

        case 27:
            DoSpawnGuards(645.70f, -260.62f, -81.98f, 3.40f);
            break;

        case 30:
            m_bCanWalk = false;
            m_uiEventPhase = 50;
            m_uiEventTimer = 2000;
            break;

        case 31:
            m_bCanWalk = false;
            m_uiEventPhase = 60;
            m_uiEventTimer = 2000;
            break;

        case 36:
            DoSpawnGuards(470.87f, -9.29f, -69.81f, 1.86f, 0, 3);
            break;

        case 38:
            m_bCanWalk = false;
            m_creature->SetFacingTo(5.780610f);
            if (GetPlayerForEscort())
                GetPlayerForEscort()->GroupEventHappens(QUEST_JAIL_BREAK, m_creature);
            m_uiEventPhase = 70;
            m_uiEventTimer = 2000;
            break;
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_DOOMFORGE_DRAGOON)
        {
            if (rand()%2 == 1 && GetPlayerForEscort() && GetPlayerForEscort()->isAlive())
                pSummoned->AI()->AttackStart(GetPlayerForEscort());
            else
                pSummoned->AI()->AttackStart(m_creature);
        }
    }

    bool DoCheckCellDoorOpenState(uint32 uiIdentifier)
    {
        if (!m_pInstance)
        {
            error_log("SD0: Blackrock Depths - Marshal Windsor - DoCheckCellDoorOpenState(): m_pInstance is NULL. Event can't continue!");
            return false;
        }

        if (GameObject* GoCellDoor = m_pInstance->GetSingleGameObjectFromStorage(uiIdentifier))
        {
            if (GoCellDoor->GetGoState() == GO_STATE_READY)
            {
                // Door are still not opened. Try later..
                --m_uiEventPhase;
                m_uiEventTimer = 2500;
                return false;
            }
            else
                // Continue to the next event phase
                m_uiEventTimer = 1000;
        }
        else
            error_log("SD0: Blackrock Depths - Marshal Windsor - DoCheckCellDoorOpenState(): Can't select a cell Door (enum: %u). Event can't continue!", uiIdentifier);

        return true;
    }

    void DoSpawnGuards(float x = 0, float y = 0, float z = 0, float o = 0, uint32 uiEntryToSpawn = NPC_DOOMFORGE_DRAGOON,  uint8 uiCountToSpawn = 2)
    {
        if (!uiEntryToSpawn || !uiCountToSpawn)
            return;

        for(uint8 itr = 0; itr < uiCountToSpawn; ++itr)
            m_creature->SummonCreature(uiEntryToSpawn, (x + irand(-2,2)), (y + irand(-2,2)), (z + irand(0,2)), o, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance)
        {
            error_log("SD0: Blackrock Depths - Marshal Windsor - UpdateAI(): m_pInstance is NULL.");
            return;
        }

        if (m_uiEventTimer)
        {
            if (m_uiEventTimer <= uiDiff)
            {
                m_uiEventTimer = 0;     // reset event timer (do not delete!)

                Creature* pDughal   = m_pInstance->GetSingleCreatureFromStorage(NPC_DUGHAL_STORMWING);
                Creature* pJaz      = m_pInstance->GetSingleCreatureFromStorage(NPC_JAZ);
                Creature* pOgrabisi = m_pInstance->GetSingleCreatureFromStorage(NPC_OGRABISI);
                Creature* pShill    = m_pInstance->GetSingleCreatureFromStorage(NPC_SHILL_DINGER);
                Creature* pCrest    = m_pInstance->GetSingleCreatureFromStorage(NPC_CREST_KILLER);
                Creature* pTobias   = m_pInstance->GetSingleCreatureFromStorage(NPC_TOBIAS_SEECHER);

                switch(m_uiEventPhase)
                {
                case 1:
                    DoScriptText(SAY_CHECK_THAT_CELL, m_creature, GetPlayerForEscort());
                    if (pDughal)
                        m_creature->SetFacingToObject(pDughal);
                    m_creature->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
                    m_uiEventTimer = 3000;
                    break;
                case 2:
                    DoSpawnGuards(315.95f, -76.81f, -75.67f, 3.65f);
                    m_uiEventTimer = 2500;
                    break;
                case 3:
                    DoCheckCellDoorOpenState(GO_CELL_DOOR__DUGHAL);
                    break;
                case 4:
                    DoScriptText(SAY_WINDSOR_2, m_creature);
                    m_creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                    m_uiEventTimer = 1500;
                    break;
                case 5:
                    if (pDughal)
                    {
                        DoScriptText(SAY_DUGHAL, pDughal, GetPlayerForEscort());
                        pDughal->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);
                    }
                    m_uiEventTimer = 2500;
                    break;
                case 6:
                    DoScriptText(SAY_WINDSOR_3, m_creature, GetPlayerForEscort());
                    m_creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                    m_uiEventTimer = 2000;
                    break;
                case 7:
                    if (pDughal)
                        pDughal->SetVisibility(VISIBILITY_OFF);
                    m_bCanWalk = true;
                    break;

                case 10:
                    DoScriptText(SAY_WINDSOR_4, m_creature, GetPlayerForEscort());
                    m_creature->SetFacingTo(1.308706f);
                    m_uiEventTimer = 5000;
                    break;
                case 11:
                    SetRun(true);
                    if (GameObject* GoCellDoor = m_pInstance->GetSingleGameObjectFromStorage(GO_SUPPLY_ROOM_DOOR))
                        if (GoCellDoor->GetGoState() == GO_STATE_READY)
                            GoCellDoor->UseDoorOrButton();
                    DoSpawnGuards(406.65f, -49.23f, -63.93f, 4.38f);
                    m_bCanWalk = true;
                    break;

                case 20:
                    DoScriptText(SAY_REGINALD_WINDSOR_1, m_creature, GetPlayerForEscort());
                    m_creature->SetFacingToObject(GetPlayerForEscort());
                    m_creature->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
                    m_uiEventTimer = 1500;
                    break;
                case 21:
                    m_creature->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
                    m_uiEventTimer = 3500;
                    break;
                case 22:
                    DoScriptText(SAY_REGINALD_WINDSOR_2, m_creature);
                    m_creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                    m_uiEventTimer = 2500;
                    break;
                case 23:
                    m_bCanWalk = true;
                    break;

                case 30:
                    DoScriptText(SAY_REGINALD_WINDSOR_3, m_creature);
                    if (pJaz && pOgrabisi)
                    {
                        m_creature->SetFacingToObject(pJaz);
                        pJaz->SetFacingToObject(m_creature);
                        pOgrabisi->SetFacingToObject(m_creature);
                    }
                    m_creature->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
                    m_uiEventTimer = 2500;
                    break;
                case 31:
                    DoCheckCellDoorOpenState(GO_CELL_DOOR__JAZ__OGRABISI);
                    break;
                case 32:
                    DoScriptText(SAY_REGINALD_WINDSOR_4, m_creature);
                    m_creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                    m_uiEventTimer = 3000;
                    break;
                case 33:
                    DoScriptText(SAY_REGINALD_WINDSOR_5, m_creature);
                    m_creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                    m_uiEventTimer = 3000;
                    break;
                case 34:
                    m_bCanWalk = true;
                    if (pJaz && pOgrabisi)
                    {
                        pJaz->SetVisibility(VISIBILITY_OFF);
                        pOgrabisi->SetVisibility(VISIBILITY_OFF);
                    }
                    break;

                case 40:
                    DoScriptText(SAY_REGINALD_WINDSOR_6, m_creature);
                    if (pShill)
                    {
                        if (pShill->isDead())
                            pShill->Respawn();
                        m_creature->SetFacingToObject(pShill);
                    }
                    m_creature->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
                    m_uiEventTimer = 2500;
                    break;
                case 41:
                    DoCheckCellDoorOpenState(GO_CELL_DOOR__SHILL);
                    break;
                case 42:
                    if (pShill)
                    {
                        DoScriptText(SAY_SHILL, pShill);
                        pShill->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                    }
                    m_uiEventTimer = 8000;
                    break;
                case 43:
                    if (pShill)
                    {
                        DoScriptText(SAY_REGINALD_WINDSOR_7, m_creature);
                        pShill->setFaction(FACTION_ENEMY);
                        pShill->AI()->AttackStart(m_creature);
                        AttackStart(pShill);
                    }
                    m_uiEventTimer = 2500;
                    break;
                case 44:
                    // Shill Dinger must be died for continue
                    if (pShill && pShill->isAlive())
                        --m_uiEventPhase;
                    m_uiEventTimer = 2500;
                    break;
                case 45:
                    DoScriptText(SAY_REGINALD_WINDSOR_8, m_creature);
                    if (pShill)
                        m_creature->SetFacingToObject(pShill);
                    m_uiEventTimer = 10000;
                    break;
                case 46:
                    DoScriptText(SAY_REGINALD_WINDSOR_9, m_creature);
                    m_uiEventTimer = 3000;
                    break;
                case 47:
                    m_bCanWalk = true;
                    break;

                case 50:
                    DoScriptText(SAY_REGINALD_WINDSOR_10, m_creature);
                    if (pCrest)
                    {
                        if (pCrest->isDead())
                            pCrest->Respawn();
                        m_creature->SetFacingToObject(pCrest);
                    }
                    m_uiEventTimer = 10000;
                    break;
                case 51:
                    DoCheckCellDoorOpenState(GO_CELL_DOOR__CREST);
                    break;
                case 52:
                    if (pCrest)
                    {
                        DoScriptText(SAY_CREST, pCrest);
                        pShill->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                    }
                    m_uiEventTimer = 7000;
                    break;
                case 53:
                    if (pCrest)
                    {
                        DoScriptText(SAY_REGINALD_WINDSOR_11, m_creature);
                        pCrest->setFaction(FACTION_ENEMY);
                        pCrest->AI()->AttackStart(m_creature);
                        AttackStart(pCrest);
                    }
                    m_uiEventTimer = 2500;
                    break;
                case 54:
                    if (pCrest && pCrest->isAlive())
                        --m_uiEventPhase;
                    m_uiEventTimer = 2500;
                    break;
                case 55:
                    m_bCanWalk = true;
                    break;

                case 60:
                    if (pTobias)
                    {
                        m_creature->SetFacingToObject(pTobias);
                        pTobias->SetFacingToObject(m_creature);
                    }
                    DoScriptText(SAY_REGINALD_WINDSOR_12, m_creature);
                    m_creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                    m_uiEventTimer = 9000;
                    break;
                case 61:
                    m_uiEventTimer = 100;       // just skip this step
                    break;
                case 62:
                    m_creature->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
                    m_uiEventTimer = 2500;
                    break;
                case 63:
                    DoCheckCellDoorOpenState(GO_CELL_DOOR__TOBIAS);
                    break;
                case 64:
                    DoScriptText(SAY_REGINALD_WINDSOR_13, m_creature);
                    m_creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                    m_uiEventTimer = 3000;
                    break;
                case 65:
                    if (pTobias)
                    {
                        DoScriptText(SAY_TOBIAS, pTobias);
                        m_creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                    }
                    m_uiEventTimer = 4000;
                    break;
                case 66:
                    m_creature->SetFacingToObject(GetPlayerForEscort());
                    DoScriptText(SAY_REGINALD_WINDSOR_14, m_creature, GetPlayerForEscort());
                    m_creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                    m_uiEventTimer = 7000;
                    break;
                case 67:
                    if (pTobias)
                        pTobias->SetVisibility(VISIBILITY_OFF);
                    m_bCanWalk = true;
                    break;

                case 70:
                    DoScriptText(SAY_REGINALD_WINDSOR_15, m_creature);
                    m_creature->SetFacingToObject(GetPlayerForEscort());
                    m_creature->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);
                    m_uiEventTimer = 2000;
                    break;
                case 71:
                    DoScriptText(SAY_REGINALD_WINDSOR_16, m_creature);
                    m_creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                    m_uiEventTimer = 60000;
                    break;
                case 72:
                    m_bCanWalk = true;      // escort ende
                    break;
                }
                ++m_uiEventPhase;
            }
            else
                m_uiEventTimer -= uiDiff;
        }

        if (m_bCanWalk)
            npc_escortAI::UpdateAI(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_marshal_windsor(Creature* pCreature)
{
    return new npc_marshal_windsorAI(pCreature);
}

bool QuestAccept_npc_marshal_windsor(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_JAIL_BREAK)
    {
        if (npc_marshal_windsorAI* pEscortAI = dynamic_cast<npc_marshal_windsorAI*>(pCreature->AI()))
        {
            DoScriptText(SAY_WINDSOR_1, pCreature);
            pCreature->setFaction(FACTION_ESCORT_A_NEUTRAL_ACTIVE);     // use 250 if Marshal no aggroing
            pEscortAI->Start(false, pPlayer, pQuest, false, false);
        }

        if (GameObject* GoCellDoor = GetClosestGameObjectWithEntry(pCreature, GO_CELL_DOOR__WINDSOR, 20.0f))
            if (GoCellDoor->GetGoState() == GO_STATE_READY)
                GoCellDoor->UseDoorOrButton();
    }
    return true;
}

/*######
## npc_commander_gorshak
######*/

enum eGorShak
{
    QUEST_WHAT_IS_GOING_ON      = 3982,
    MAX_WAVE_COUNT              = 3,
};

struct MANGOS_DLL_DECL npc_commander_gorshakAI : public FollowerAI
{
    npc_commander_gorshakAI(Creature* pCreature) : FollowerAI(pCreature) {
        Reset();
    }

    uint8 m_uiEventPhase;
    uint32 m_uiEventTimer;
    uint32 m_uiGuardsCount;

    void Reset()
    {
        if (HasFollowState(STATE_FOLLOW_INPROGRESS))
            return;

        m_uiEventPhase = 0;
        m_uiEventTimer = 0;
        m_uiGuardsCount = 0;
    }

    void JustDied(Unit *)
    {
        if (GetLeaderForFollower())
            GetLeaderForFollower()->FailQuest(QUEST_WHAT_IS_GOING_ON);
    }

    // JustSummoned() is not working yet

    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_ANVILRAGE_GUARDSMAN)
        {
            if (--m_uiGuardsCount == 0 && m_uiEventPhase > MAX_WAVE_COUNT)
            {
                if (Player* pPlayer = GetLeaderForFollower())
                {
                    pPlayer->GroupEventHappens(QUEST_WHAT_IS_GOING_ON, pPlayer);
                    SetFollowComplete();
                    m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
                    ResetToHome();
                }
            }
        }
    }

    void StartEvent()
    {
        m_uiEventPhase = 1;
        m_uiEventTimer = 5000;
        SetFollowPaused(true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiEventTimer)
        {
            if (m_uiEventTimer <= uiDiff)
            {
                m_uiEventTimer = 30000;

                switch(m_uiEventPhase)
                {
                case 1:
                case 2:
                case 3:
                    for(uint8 itr = 0; itr < 3; ++itr)
                    {
                        if (Creature* pSummon = m_creature->SummonCreature(NPC_ANVILRAGE_GUARDSMAN, 379.50f, -191.97f, -69.71f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5*MINUTE*IN_MILLISECONDS))
                        {
                            ++m_uiGuardsCount;

                            Player* pPlayer = GetLeaderForFollower();
                            if (pPlayer && pPlayer->isAlive())
                                ((Creature*)pSummon)->AI()->AttackStart(pPlayer);
                            else
                                ((Creature*)pSummon)->AI()->AttackStart(m_creature);
                        }
                    }
                    break;
                case 4:
                    // Prevent counting to infinity
                    --m_uiEventPhase;
                    break;
                }
                ++m_uiEventPhase;
            }
            else
                m_uiEventTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_commander_gorshak(Creature* pCreature)
{
    return new npc_commander_gorshakAI(pCreature);
}

bool QuestAccept_npc_commander_gorshak(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_WHAT_IS_GOING_ON)
    {
        if (npc_commander_gorshakAI* pCreatureAI = dynamic_cast<npc_commander_gorshakAI*>(pCreature->AI()))
        {
            pCreatureAI->StartFollow(pPlayer);
            pCreatureAI->StartEvent();
        }

        if (instance_blackrock_depths* m_pInstance = (instance_blackrock_depths*)pCreature->GetInstanceData())
            if (GameObject* GoCellDoor = m_pInstance->GetSingleGameObjectFromStorage(GO_CELL_DOOR__GORSHAK))
                if (GoCellDoor->GetGoState() == GO_STATE_READY)
                    GoCellDoor->UseDoorOrButton();
    }
    return true;
}

/*######
## go_shadowforge_lock
######*/

bool GOUse_go_shadowforge_lock(Player* /*pPlayer*/, GameObject* pGo)
{
    if (instance_blackrock_depths* m_pInstance = (instance_blackrock_depths*)pGo->GetInstanceData())
        m_pInstance->SetData(TYPE_GIANT_DOOR, DONE);

    return false;
}

/*######
## go_shadowforge_brazier
######*/

bool GOUse_go_shadowforge_brazier(Player* /*pPlayer*/, GameObject* pGo)
{
    if (instance_blackrock_depths* pInstance = (instance_blackrock_depths*)pGo->GetInstanceData())
    {
        if (pInstance->GetData(TYPE_LYCEUM) == IN_PROGRESS)
            pInstance->SetData(TYPE_LYCEUM, DONE);
        else
            pInstance->SetData(TYPE_LYCEUM, IN_PROGRESS);
    }
    return false;
}

/*######
## go_dark_keeper_portrait
######*/

bool GOUse_go_dark_keeper_portrait(Player* pPlayer, GameObject* pGo)
{
    if (instance_blackrock_depths* m_pInstance = ((instance_blackrock_depths*)pGo->GetInstanceData()))
    {
        uint8 uiRand = rand()%6;

        //TODO: summon guards with Darkkeepers
        switch(uiRand)
        {
        case 0:
            m_pInstance->DoRespawnGameObject(GO_DARK_KEEPER_NAMEPLATE1, 120*MINUTE);
            pPlayer->SummonCreature(NPC_VORFALK, 902.425f, -140.241f, -49.756f, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0);
            break;
        case 1:
            m_pInstance->DoRespawnGameObject(GO_DARK_KEEPER_NAMEPLATE2, 120*MINUTE);
            pPlayer->SummonCreature(NPC_BETHEK, 799.778f, -346.608f, -50.196f, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0);
            break;
        case 2:
            m_pInstance->DoRespawnGameObject(GO_DARK_KEEPER_NAMEPLATE3, 120*MINUTE);
            pPlayer->SummonCreature(NPC_UGGEL, 962.34f, -396.09f, -60.83f, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0);
            break;
        case 3:
            m_pInstance->DoRespawnGameObject(GO_DARK_KEEPER_NAMEPLATE4, 120*MINUTE);
            pPlayer->SummonCreature(NPC_ZIMREL, 596.518f, -232.087f, -38.356f, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0);
            break;
        case 4:
            m_pInstance->DoRespawnGameObject(GO_DARK_KEEPER_NAMEPLATE5, 120*MINUTE);
            pPlayer->SummonCreature(NPC_OFGUT, 652.572f, 19.288f, -60.059f, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0);
            break;
        case 5:
            // The Domicile
            m_pInstance->DoRespawnGameObject(GO_DARK_KEEPER_NAMEPLATE6, 120*MINUTE);
            pPlayer->SummonCreature(NPC_PELVER, 768.112f, -250.812f, -43.344f, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0);
            break;
        }
    }
    return false;
}

/*######
## go_relic_coffer_door
######*/

bool GOUse_go_relic_coffer_door(Player* pPlayer, GameObject* pGo)
{
    instance_blackrock_depths* m_pInstance = (instance_blackrock_depths*)pPlayer->GetInstanceData();

    if (m_pInstance)
    {
        if (m_pInstance->GetData(TYPE_VAULT) != DONE || m_pInstance->GetData(TYPE_VAULT) != SPECIAL)
            m_pInstance->SetData(TYPE_VAULT, IN_PROGRESS);

        if (m_pInstance->GetData(TYPE_VAULT) == SPECIAL)
        {
            pGo->SummonCreature(NPC_DOOMGRIP, 813.46f, -347.74f, -50.577f, 3.90f, TEMPSUMMON_DEAD_DESPAWN, 0);
        }

        GameObject* pCoffer = m_pInstance->GetClosestRelicCoffer(pGo); // Add the coffer when the door is opened.
        pCoffer->AddToWorld();
        pCoffer->UpdateVisibilityAndView();
    }

    return false;
}

GameObject* instance_blackrock_depths::GetClosestRelicCoffer(GameObject* pObj)
{
    GameObject* return_object;
    float minDist = 99999;

    for (std::vector<GameObject*>::iterator itr = m_uiRelicCoffers.begin(); itr != m_uiRelicCoffers.end(); ++itr) // Loop through the coffers and see which one is closest to the door.
    {
        GameObject* current_object = *itr;

        if (!current_object)
            continue;

        float current_dist = pObj->GetDistance(current_object);

        if (current_dist < minDist)
        {
            return_object = current_object;
            minDist = current_dist;
        }
    }
    
    // Increse the count of how many doors have been opened.
    ++m_uiOpenCofferDoors;

    return return_object;
}

/*######
## mob_warbringer_construct
######*/

enum eWarbringerConstruct
{
    SPELL_SKULL_CRACK = 15621,
};

struct MANGOS_DLL_DECL mob_warbringer_constructAI : public ScriptedAI
{
    mob_warbringer_constructAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_blackrock_depths*)pCreature->GetInstanceData();
        Reset();
    }

    instance_blackrock_depths* m_pInstance;
    uint32 m_uiSkullCrackTimer;
    uint32 m_uiCheckEventStatusTimer;
    bool bIsInTheVault;
    bool bIsInTheVaultChecked;

    void Reset()
    {
        m_uiSkullCrackTimer = 3000;
        m_uiCheckEventStatusTimer = 100;
        bIsInTheVault = false;
        bIsInTheVaultChecked = false;
    }

    void JustDied(Unit *)
    {
        if (bIsInTheVault)
            if (m_pInstance)
                m_pInstance->SetData(TYPE_VAULT, IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiCheckEventStatusTimer <= uiDiff)
        {
            if (!bIsInTheVaultChecked)
            {
                bIsInTheVaultChecked = true;

                if (GetClosestGameObjectWithEntry(m_creature, GO_DARK_COFFER, 20.0f))
                {
                    bIsInTheVault = true;

                    //Done by aura of spell 10255
                    /*if (m_pInstance && m_pInstance->GetData(TYPE_VAULT) != SPECIAL)
                    {
                        ResetToHome();
                        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE + UNIT_FLAG_NOT_SELECTABLE);
                        m_creature->addUnitState(UNIT_STAT_CAN_NOT_MOVE);
                        m_creature->addUnitState(UNIT_STAT_ROOT);
                    }*/
                }
            }

            if (m_pInstance && m_pInstance->GetData(TYPE_VAULT) == SPECIAL)
            {
                m_creature->RemoveAura(10255, EFFECT_INDEX_0);
                /*m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE + UNIT_FLAG_NOT_SELECTABLE);
                m_creature->clearUnitState(UNIT_STAT_CAN_NOT_MOVE);
                m_creature->clearUnitState(UNIT_STAT_ROOT);*/
            }

            m_uiCheckEventStatusTimer = 1500;      // check only one time..
        }
        else
            m_uiCheckEventStatusTimer -= uiDiff;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Skull crack
        if (m_uiSkullCrackTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SKULL_CRACK);
            m_uiSkullCrackTimer = urand(10000, 12000);
        }
        else
            m_uiSkullCrackTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_warbringer_construct(Creature* pCreature)
{
    return new mob_warbringer_constructAI(pCreature);
}

/*######
## mob_watchman_doomgrip
######*/

enum eDoomgrip
{
    SPELL_DRINK_HEALING_POTION = 15504,
    SPELL_SUNDER_ARMOR         = 11971
};

struct MANGOS_DLL_DECL mob_watchman_doomgripAI : public ScriptedAI
{
    mob_watchman_doomgripAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_blackrock_depths*)pCreature->GetInstanceData();
        Reset();
    }

    instance_blackrock_depths* m_pInstance;
    uint32 m_uiDrinkHealingPotionTimer;
    uint32 m_uiSunderArmorTimer;

    void Reset()
    {
        m_uiDrinkHealingPotionTimer = 0;
        m_uiSunderArmorTimer = 10000;
    }

    void JustDied(Unit *)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_VAULT, IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Drink Healing Potion
        if (m_uiDrinkHealingPotionTimer <= uiDiff && HealthBelowPct(70))
        {
            DoCastSpellIfCan(m_creature, SPELL_DRINK_HEALING_POTION);
            m_uiDrinkHealingPotionTimer = 10000;
        }
        else
            m_uiDrinkHealingPotionTimer -= uiDiff;

        // Sunder Armor
        if (m_uiSunderArmorTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SUNDER_ARMOR);
            m_uiSunderArmorTimer = 10000;
        }
        else
            m_uiSunderArmorTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_watchman_doomgrip(Creature* pCreature)
{
    return new mob_watchman_doomgripAI(pCreature);
}

/*######
## mob_plugger_spazzrink
######*/

enum eSpazzringSpells
{
    SPELL_BANISH            = 8994,
    SPELL_CURSE_OF_TONGUES  = 13338,
    SPELL_DEMONARMOR        = 13787,
    SPELL_IMMOLATE          = 12742,
    SPELL_SHADOW_BOLT       = 12739,
};

struct MANGOS_DLL_DECL mob_plugger_spazzringAI : public ScriptedAI
{
    mob_plugger_spazzringAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_blackrock_depths*)pCreature->GetInstanceData();
        Reset();
    }

    instance_blackrock_depths* m_pInstance;
    uint32 m_uiBanishTimer;
    uint32 m_uiCurseOfTonguesTimer;
    uint32 m_uiDemonArmorTimer;
    uint32 m_uiImmolateTimer;
    uint32 m_uiShadowBoltTimer;

    void Reset()
    {
        m_uiBanishTimer = 10000;
        m_uiCurseOfTonguesTimer = 3000;
        m_uiDemonArmorTimer = 0;
        m_uiImmolateTimer = 7000;
        m_uiShadowBoltTimer = 5000;
    }

    void Aggro(Unit* /*pVictim*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BAR5, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Banish
        if (m_uiBanishTimer <= uiDiff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(target, SPELL_BANISH);
            m_uiBanishTimer = 25000;
        }
        else
            m_uiBanishTimer -= uiDiff;

        // Curse of Tongues
        if (m_uiCurseOfTonguesTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CURSE_OF_TONGUES);
            m_uiCurseOfTonguesTimer = 25000;
        }
        else
            m_uiCurseOfTonguesTimer -= uiDiff;

        // Demon Armor
        if (m_uiDemonArmorTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_DEMONARMOR);
            m_uiDemonArmorTimer = 32*MINUTE*IN_MILLISECONDS;
        }
        else
            m_uiDemonArmorTimer -= uiDiff;

        // Immolate
        if (m_uiImmolateTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_IMMOLATE);
            m_uiImmolateTimer = 30000;
        }
        else
            m_uiImmolateTimer -= uiDiff;

        // Shadow Bolt
        if (m_uiShadowBoltTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_BOLT);
            m_uiShadowBoltTimer = 5000;
        }
        else
            m_uiShadowBoltTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_plugger_spazzring(Creature* pCreature)
{
    return new mob_plugger_spazzringAI (pCreature);
}

/*######
## npc_ribbly_crony
######*/

enum eRibblyCrony
{
    SPELL_EVISCERATE      = 15692,
    SPELL_SINISTER_STRIKE = 15581,
};

struct MANGOS_DLL_DECL npc_ribbly_cronyAI : public ScriptedAI
{
    npc_ribbly_cronyAI(Creature* pCreature) : ScriptedAI(pCreature) {
        Reset();
    }

    uint32 m_uiEviscerateTimer;
    uint32 m_uiSinisterStrikeTimer;

    void Reset()
    {
        m_uiEviscerateTimer = urand(5000, 8000);
        m_uiSinisterStrikeTimer = urand(0,2500);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Eviscerate
        if (m_uiEviscerateTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_EVISCERATE);
            m_uiEviscerateTimer = 10000;
        }
        else
            m_uiEviscerateTimer -= uiDiff;

        // Sinister Strike
        if (m_uiSinisterStrikeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SINISTER_STRIKE);
            m_uiSinisterStrikeTimer = 8000;
        }
        else
            m_uiSinisterStrikeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_ribbly_crony(Creature* pCreature)
{
    return new npc_ribbly_cronyAI(pCreature);
}

bool GossipHello_npc_ribbly_crony(Player* pPlayer, Creature* pCreature)
{
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Fight!", GOSSIP_SENDER_MAIN, (GOSSIP_ACTION_INFO_DEF + 1));
    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_ribbly_crony(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == (GOSSIP_ACTION_INFO_DEF + 1))
    {
        if (Creature* pRibbly = GetClosestCreatureWithEntry(pCreature, 9543, DEFAULT_VISIBILITY_INSTANCE))
        {
            pRibbly->setFaction(FACTION_ENEMY);
            pRibbly->AI()->AttackStart(pPlayer);
        }

        pCreature->setFaction(FACTION_ENEMY);
        pCreature->AI()->AttackStart(pPlayer);

        std::list<Creature*> lRibblyCrones;
        GetCreatureListWithEntryInGrid(lRibblyCrones, pCreature, pCreature->GetEntry(), 40.0f);

        if (!lRibblyCrones.empty())
        {
            for(std::list<Creature*>::iterator i = lRibblyCrones.begin(); i != lRibblyCrones.end(); ++i)
            {
                if (Creature* pRibblyCrone = *i)
                {
                    pRibblyCrone->setFaction(FACTION_ENEMY);
                    pRibblyCrone->AI()->AttackStart(pPlayer);
                }
            }
        }
    }
    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

/*######
## the_grim_guzzler_patrons
######*/

enum eBarSecurity
{
    //Patrons' spells
    SPELL_KICK			 = 15610,
    SPELL_BACKHAND		 = 6253,
    SPELL_CURSE_OF_AGONY = 14868,
    //SPELL_SHADOW_BOLT    = 20825
};

static float Locations[6][3]=
{
    {872.274f, -231.280f, -43.750f},    // 1x Fireguard Destroyer
    {873.364f, -232.896f, -43.752f},	// 3x Anvilrage Officer

    {865.257f, -219.078f, -43.700f},    // Fireguard Destroyer
    {868.760f, -220.710f, -43.700f},	// Anvilrage Officer
    {866.693f, -221.182f, -43.700f},	// Anvilrage Officer
    {865.026f, -222.306f, -43.700f},    // Anvilrage Officer
};

struct MANGOS_DLL_DECL the_grim_guzzler_patronsAI : public ScriptedAI
{
    the_grim_guzzler_patronsAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_blackrock_depths*)pCreature->GetInstanceData();
        Reset();
    }

    instance_blackrock_depths* m_pInstance;
    uint32 m_uiKickTimer;
    uint32 m_uiBackHandTimer;
    uint32 m_uiCurseOfAgonyTimer;
    uint32 m_uiShadowBoltTimer;
    uint32 m_uiGoToAleMugTrapTimer;

    void Reset()
    {
        m_uiKickTimer = 5000;
        m_uiBackHandTimer = urand(2500, 5000);
        m_uiCurseOfAgonyTimer = 0;
        m_uiShadowBoltTimer = urand(3000, 6000);
        m_uiGoToAleMugTrapTimer = 0;
    }

    void JustDied(Unit *)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_BAR2) == NOT_STARTED)
        {
            m_pInstance->SetData(TYPE_BAR2, DONE);
            m_pInstance->SetData(TYPE_BAR5, DONE);
        }
        else
            return;

        if (Creature* pSummon1 = m_creature->SummonCreature(NPC_FIREGUARD_DESTROYER, Locations[0][0], Locations[0][1], Locations[0][2], 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0))
        {
            pSummon1->GetMotionMaster()->MovePoint(0, Locations[2][0], Locations[2][1], Locations[2][2]);
            CreatureCreatePos pos(pSummon1->GetMap(), Locations[2][0], Locations[2][1], Locations[2][2], 0.0f);
            pSummon1->SetSummonPoint(pos);
        }

        if (Creature* pSummon2 = m_creature->SummonCreature(NPC_ANVILRAGE_OFFICER, Locations[1][0], Locations[1][1], Locations[1][2], 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0))
        {
            pSummon2->GetMotionMaster()->MovePoint(0, Locations[3][0], Locations[3][1], Locations[3][2]);
            CreatureCreatePos pos(pSummon2->GetMap(), Locations[2][0], Locations[2][1], Locations[2][2], 0.0f);
            pSummon2->SetSummonPoint(pos);
        }

        if (Creature* pSummon3 = m_creature->SummonCreature(NPC_ANVILRAGE_OFFICER, Locations[1][0], Locations[1][1], Locations[1][2], 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0))
        {
            pSummon3->GetMotionMaster()->MovePoint(0, Locations[4][0], Locations[4][1], Locations[4][2]);
            CreatureCreatePos pos(pSummon3->GetMap(), Locations[2][0], Locations[2][1], Locations[2][2], 0.0f);
            pSummon3->SetSummonPoint(pos);
        }

        if (Creature* pSummon4 = m_creature->SummonCreature(NPC_ANVILRAGE_OFFICER, Locations[1][0], Locations[1][1], Locations[1][2], 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0))
        {
            pSummon4->GetMotionMaster()->MovePoint(0, Locations[5][0], Locations[5][1], Locations[5][2]);
            CreatureCreatePos pos(pSummon4->GetMap(), Locations[2][0], Locations[2][1], Locations[2][2], 0.0f);
            pSummon4->SetSummonPoint(pos);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        switch(m_creature->GetEntry())
        {
        case NPC_GRIM_PATRON:
            // Kick
            if (m_uiKickTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_KICK);
                m_uiKickTimer = 10000;
            }
            else
                m_uiKickTimer -= uiDiff;
            break;
        case NPC_HAMMERED_PATRON:
            // Back Hand
            if (m_uiBackHandTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_BACKHAND);
                m_uiBackHandTimer = urand(7500, 10000);
            }
            else
                m_uiBackHandTimer -= uiDiff;
            break;
        case NPC_GUZZLING_PATRON:
            // Curse of Agony
            if (m_uiCurseOfAgonyTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_CURSE_OF_AGONY);
                m_uiCurseOfAgonyTimer = 10000;
            }
            else
                m_uiCurseOfAgonyTimer -= uiDiff;

            // Shadow bolt
            if (m_uiShadowBoltTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_BOLT);
                m_uiShadowBoltTimer = 10000;
            }
            else
                m_uiShadowBoltTimer -= uiDiff;
            break;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_the_grim_guzzler_patrons(Creature* pCreature)
{
    return new the_grim_guzzler_patronsAI(pCreature);
}

/*######
## go_dark_iron_ale_mug_and_grim_guzzler_boar
######*/

bool GOUse_go_dark_iron_ale_mug_and_grim_guzzler_boar(Player* /*pPlayer*/, GameObject* pGo)
{
    if (instance_blackrock_depths* m_pInstance = (instance_blackrock_depths*)pGo->GetInstanceData())
        if (m_pInstance->GetData(TYPE_BAR5) != DONE)
            m_pInstance->SetData(TYPE_BAR5, DONE);
    return false;
}

/*######
## go_thunderbrew_lager_keg
######*/

bool GOUse_go_thunderbrew_lager_keg(Player* pPlayer, GameObject* pGo)
{
    if (instance_blackrock_depths* m_pInstance = (instance_blackrock_depths*)pGo->GetInstanceData())
    {
        if (m_pInstance->GetData(TYPE_BAR4) != DONE)
        {
            m_pInstance->SetData(TYPE_BAR4, IN_PROGRESS);

            if (m_pInstance->GetData(TYPE_BAR4) == SPECIAL)
            {
                m_pInstance->SetData(TYPE_BAR4, DONE);

                if (Creature* pHurley = pGo->SummonCreature(9537, 894.08f, -130.60f, -49.74f, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0))
                    pHurley->AI()->AttackStart(pPlayer);
                if (Creature* pBodyGuard1 = pGo->SummonCreature(8901, 894.80f, -126.48f, -49.74f, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0))
                    pBodyGuard1->AI()->AttackStart(pPlayer);
                if (Creature* pBodyGuard2 = pGo->SummonCreature(8901, 889.58f, -129.23f, -49.74f, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0))
                    pBodyGuard2->AI()->AttackStart(pPlayer);
                if (Creature* pBodyGuard3 = pGo->SummonCreature(8901, 891.61f, -127.00f, -49.74f, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0))
                    pBodyGuard3->AI()->AttackStart(pPlayer);
            }
        }
    }
    return false;
}

/*######
## npc_grimstone
######*/

enum
{
    NPC_GRIMSTONE       = 10096,
    //NPC_THELDREN        = 16059,

    //4 or 6 in total? 1+2+1 / 2+2+2 / 3+3. Depending on this, code should be changed.
    MAX_MOB_AMOUNT      = 4
};

uint32 RingMob[]=
{
    8925,                                                   // Dredge Worm
    8926,                                                   // Deep Stinger
    8927,                                                   // Dark Screecher
    8928,                                                   // Burrowing Thundersnout
    8933,                                                   // Cave Creeper
    8932,                                                   // Borer Beetle
};

uint32 RingBoss[]=
{
    9027,                                                   // Gorosh
    9028,                                                   // Grizzle
    9029,                                                   // Eviscerator
    9030,                                                   // Ok'thor
    9031,                                                   // Anub'shiah
    9032,                                                   // Hedrum
};

bool AreaTrigger_at_ring_of_law(Player* pPlayer, AreaTriggerEntry const* /*pAt*/)
{
    if (instance_blackrock_depths* m_pInstance = (instance_blackrock_depths*)pPlayer->GetInstanceData())
    {
        if (m_pInstance->GetData(TYPE_RING_OF_LAW) == NOT_STARTED)
        {
            m_pInstance->SetData(TYPE_RING_OF_LAW, IN_PROGRESS);
            pPlayer->SummonCreature(NPC_GRIMSTONE, 625.559f, -205.618f, -52.735f, 2.609f, TEMPSUMMON_DEAD_DESPAWN, 0);
        }
    }
    return false;
}

/*######
## npc_grimstone
######*/

enum eGrimstone
{
    SAY_GRIMSTONE_1 = -1230004,
    SAY_GRIMSTONE_2 = -1230005,
    SAY_GRIMSTONE_3 = -1230006,
    SAY_GRIMSTONE_4 = -1230007,
    SAY_GRIMSTONE_5 = -1230008,
    SAY_GRIMSTONE_6 = -1230009
};

struct MANGOS_DLL_DECL npc_grimstoneAI : public npc_escortAI
{
    npc_grimstoneAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_pInstance = (instance_blackrock_depths*)pCreature->GetInstanceData();
        MobSpawnId = urand(0, 5);
        Reset();
    }

    instance_blackrock_depths* m_pInstance;

    uint8 EventPhase;
    uint32 EventTimer;

    uint8 MobSpawnId;
    uint8 MobCount;
    uint8 BossCount;
    uint32 MobDeathTimer;

    ObjectGuid RingMobGUID[MAX_MOB_AMOUNT];
    ObjectGuid RingBossGUID[6];

    bool CanWalk;

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        EventPhase = 0;
        EventTimer = 1000;

        MobCount = 0;
        BossCount = 0;
        MobDeathTimer = 0;

        for(uint8 i = 0; i < MAX_MOB_AMOUNT; ++i)
            RingMobGUID[i].Clear();
        for(uint8 i = 0; i < 6; ++i)
            RingBossGUID[i].Clear();

        CanWalk = false;
    }

    void DoGate(uint32 id, uint32 state)
    {
        if (!m_pInstance)
            return;

        if (GameObject* pGo = m_pInstance->GetSingleGameObjectFromStorage(id))
            pGo->SetGoState(GOState(state));

        debug_log("SD2: npc_grimstone, arena gate update state.");
    }

    void SummonRingMob()
    {
        if (Creature* tmp = m_creature->SummonCreature(RingMob[MobSpawnId],608.960f,-235.322f,-53.907f,1.857f,TEMPSUMMON_DEAD_DESPAWN,0))
            RingMobGUID[MobCount] = tmp->GetObjectGuid();

        ++MobCount;

        if (MobCount == MAX_MOB_AMOUNT)
            MobDeathTimer = 2500;
    }

    void SummonRingBoss()
    {
        if (m_pInstance)
        {
            if (m_pInstance->GetData(TYPE_RING_OF_LAW) == SPECIAL)
            {
                // Theldren's gladiator team (summon four random Theldren's gladiators)
                uint32 uiRand[4];

                for(uint8 i = 0; i < 4; ++i)
                    uiRand[i] = urand(0,7);

                while(uiRand[1] == uiRand[0])
                    uiRand[1] = urand(0,7);

                while(uiRand[2] == uiRand[0] || uiRand[2] == uiRand[1])
                    uiRand[2] = urand(0,7);

                while(uiRand[3] == uiRand[0] || uiRand[3] == uiRand[1] || uiRand[3] == uiRand[2])
                    uiRand[3] = urand(0,7);

                for(uint8 i = 0; i < 4; ++i)
                {
                    uint32 uiCreatureEntry = 0;

                    switch(uiRand[i])
                    {
                    case 0:
                        uiCreatureEntry = NPC_LEFTY;
                        break;
                    case 1:
                        uiCreatureEntry = NPC_SNOKH_BLACKSPINE;
                        break;
                    case 2:
                        uiCreatureEntry = NPC_ROTFANG;
                        break;
                    case 3:
                        uiCreatureEntry = NPC_MALGEN_LONGSPEAR;
                        break;
                    case 4:
                        uiCreatureEntry = NPC_KORV;
                        break;
                    case 5:
                        uiCreatureEntry = NPC_REZZNIK;
                        break;
                    case 6:
                        uiCreatureEntry = NPC_VAJASHNI;
                        break;
                    case 7:
                        uiCreatureEntry = NPC_VOLIDA;
                        break;
                    }

                    if (Creature* pSummon = m_creature->SummonCreature(uiCreatureEntry, 644.300f, -175.989f, -53.739f, 3.418f, TEMPSUMMON_DEAD_DESPAWN, 0))
                    {
                        RingBossGUID[BossCount++] = pSummon->GetObjectGuid();

                        if (uiCreatureEntry == NPC_MALGEN_LONGSPEAR)
                        {
                            if (Creature* pMalgensPet = m_creature->SummonCreature(NPC_GNASHJAW, 644.300f, -175.989f, -53.739f, 3.418f, TEMPSUMMON_DEAD_DESPAWN, 0))
                            {
                                RingBossGUID[BossCount++] = pMalgensPet->GetObjectGuid();
                                pMalgensPet->SetOwnerGuid(pSummon->GetObjectGuid());
                            }
                        }
                    }
                }

                if (Creature* pSummon = m_creature->SummonCreature(NPC_THELDREN, 644.300f, -175.989f, -53.739f, 3.418f, TEMPSUMMON_DEAD_DESPAWN, 0))
                    RingBossGUID[BossCount++] = pSummon->GetObjectGuid();

                MobDeathTimer = 2500;
                return;
            }
        }

        if (Creature* pSummon = m_creature->SummonCreature(RingBoss[rand()%6], 644.300f, -175.989f, -53.739f, 3.418f, TEMPSUMMON_DEAD_DESPAWN, 0))
            RingBossGUID[BossCount++] = pSummon->GetObjectGuid();

        MobDeathTimer = 2500;
    }

    void JustSummoned(Creature* pSummoned)
    {
        float newPosX = 596.82f + irand(-3, 3);
        float newPosY = -188.77f + irand(-3, 3);
        float newPosZ = -54.15f;

        pSummoned->GetMotionMaster()->MovePoint(0, newPosX, newPosY, newPosZ);
        CreatureCreatePos pos(pSummoned->GetMap(), newPosX, newPosY, newPosZ, 5.67f);
        pSummoned->SetSummonPoint(pos);
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
        case 0:
            DoScriptText(SAY_GRIMSTONE_2, m_creature);
            CanWalk = false;
            EventTimer = 5000;
            break;
        case 1:
            DoScriptText(SAY_GRIMSTONE_4, m_creature);
            CanWalk = false;
            EventTimer = 5000;
            break;
        case 2:
            CanWalk = false;
            break;
        case 3:
            DoScriptText(SAY_GRIMSTONE_5, m_creature);
            break;
        case 4:
            switch (rand()%3)
            {
            case 0:
                DoScriptText(SAY_GRIMSTONE_4, m_creature);
                break;
            case 1:
                DoScriptText(SAY_GRIMSTONE_5, m_creature);
                break;
            case 2:
                DoScriptText(SAY_GRIMSTONE_6, m_creature);
                break;
            }
            CanWalk = false;
            EventTimer = 5000;
            break;
        case 5:
            if (m_pInstance)
            {
                m_pInstance->SetData(TYPE_RING_OF_LAW, DONE);
                debug_log("SD2: npc_grimstone: event reached end and set complete.");
            }
            break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance)
            return;

        if (MobDeathTimer)
        {
            if (MobDeathTimer <= uiDiff)
            {
                MobDeathTimer = 2500;

                for(uint8 i = 0; i < 6; ++i)
                {
                    if (RingBossGUID[i])
                    {
                        Creature* pBoss = m_creature->GetMap()->GetCreature(RingBossGUID[i]);
                        if (pBoss && !pBoss->isAlive() && pBoss->isDead())
                        {
                            RingBossGUID[i].Clear();
                            --BossCount;

                            if (!BossCount)
                            {
                                EventTimer = 5000;
                                MobDeathTimer = 0;
                                break;
                                //return;
                            }
                        }
                    }
                }

                for(uint8 i = 0; i < MAX_MOB_AMOUNT; ++i)
                {
                    Creature* pMob = m_creature->GetMap()->GetCreature(RingMobGUID[i]);
                    if (pMob && !pMob->isAlive() && pMob->isDead())
                    {
                        RingMobGUID[i].Clear();
                        --MobCount;

                        //seems all are gone, so set timer to continue and discontinue this
                        if (!MobCount)
                        {
                            EventTimer = 5000;
                            MobDeathTimer = 0;
                            break;
                        }
                    }
                }
            }
            else
                MobDeathTimer -= uiDiff;
        }

        if (EventTimer)
        {
            if (EventTimer <= uiDiff)
            {
                switch(EventPhase)
                {
                case 0:
                    DoScriptText(SAY_GRIMSTONE_1, m_creature);
                    DoGate(GO_ARENA4, GO_STATE_READY);
                    Start();
                    CanWalk = true;
                    EventTimer = 0;
                    break;
                case 1:
                    CanWalk = true;
                    EventTimer = 0;
                    break;
                case 2:
                    EventTimer = 2000;
                    break;
                case 3:
                    DoGate(GO_ARENA1, GO_STATE_ACTIVE);
                    EventTimer = 3000;
                    break;
                case 4:
                    CanWalk = true;
                    m_creature->SetVisibility(VISIBILITY_OFF);
                    SummonRingMob();
                    EventTimer = 8000;
                    break;
                case 5:
                    SummonRingMob();
                    SummonRingMob();
                    EventTimer = 8000;
                    break;
                case 6:
                    SummonRingMob();
                    EventTimer = 0;
                    break;
                case 7:
                    m_creature->SetVisibility(VISIBILITY_ON);
                    DoGate(GO_ARENA1, GO_STATE_READY);
                    DoScriptText(SAY_GRIMSTONE_4, m_creature);
                    CanWalk = true;
                    EventTimer = 0;
                    break;
                case 8:
                    DoGate(GO_ARENA2, GO_STATE_ACTIVE);
                    EventTimer = 5000;
                    break;
                case 9:
                    m_creature->SetVisibility(VISIBILITY_OFF);
                    SummonRingBoss();
                    EventTimer = 0;
                    break;
                case 10:
                    //if quest, complete
                    DoGate(GO_ARENA2, GO_STATE_READY);
                    DoGate(GO_ARENA3, GO_STATE_ACTIVE);
                    DoGate(GO_ARENA4, GO_STATE_ACTIVE);
                    CanWalk = true;
                    EventTimer = 0;
                    break;
                }
                ++EventPhase;
            }
            else
                EventTimer -= uiDiff;
        }

        if (CanWalk)
            npc_escortAI::UpdateAI(uiDiff);
    }
};

CreatureAI* GetAI_npc_grimstone(Creature* pCreature)
{
    return new npc_grimstoneAI(pCreature);
}

/*######
## npc_theldren_trigger
######*/

struct MANGOS_DLL_DECL npc_theldren_triggerAI : public ScriptedAI
{
    npc_theldren_triggerAI(Creature* pCreature) : ScriptedAI(pCreature) {
        Reset();
    }

    void Reset() {}
    void MoveInLineOfSight(Unit *) {
        /* ignore aggro */
    }
    void AttackStart(Unit *)
    {
        ResetToHome();
    }
};

CreatureAI* GetAI_npc_theldren_trigger(Creature* pCreature)
{
    return new npc_theldren_triggerAI(pCreature);
}

/*######
## mob_phalanx
######*/

enum ePhalanx
{
    SPELL_THUNDERCLAP       = 15588,
    SPELL_FIREBALLVOLLEY    = 15285,
    SPELL_MIGHTYBLOW        = 14099,
};

struct MANGOS_DLL_DECL mob_phalanxAI : public ScriptedAI
{
    mob_phalanxAI(Creature* pCreature) : ScriptedAI(pCreature) {
        Reset();
    }

    uint32 m_uiThunderClapTimer;
    uint32 m_uiFireballVolleyTimer;
    uint32 m_uiMightyBlowTimer;

    void Reset()
    {
        m_uiThunderClapTimer = 12000;
        m_uiFireballVolleyTimer = 0;
        m_uiMightyBlowTimer = 15000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Thunder Clap
        if (m_uiThunderClapTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(),SPELL_THUNDERCLAP);
            m_uiThunderClapTimer = 10000;
        }
        else
            m_uiThunderClapTimer -= uiDiff;

        // Fireball Volley
        if (HealthBelowPct(51))
        {
            if (m_uiFireballVolleyTimer < uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(),SPELL_FIREBALLVOLLEY);
                m_uiFireballVolleyTimer = 15000;
            }
            else
                m_uiFireballVolleyTimer -= uiDiff;
        }

        // Mighty Blow
        if (m_uiMightyBlowTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(),SPELL_MIGHTYBLOW);
            m_uiMightyBlowTimer = 10000;
        }
        else
            m_uiMightyBlowTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_phalanx(Creature* pCreature)
{
    return new mob_phalanxAI(pCreature);
}

/*######
## npc_kharan_mighthammer
######*/

#define QUEST_4001              4001
#define QUEST_4342              4342

#define GOSSIP_ITEM_KHARAN_1    "I need to know where the princess are, Kharan!"
#define GOSSIP_ITEM_KHARAN_2    "All is not lost, Kharan!"

#define GOSSIP_ITEM_KHARAN_3    "Gor'shak is my friend, you can trust me."
#define GOSSIP_ITEM_KHARAN_4    "Not enough, you need to tell me more."
#define GOSSIP_ITEM_KHARAN_5    "So what happened?"
#define GOSSIP_ITEM_KHARAN_6    "Continue..."
#define GOSSIP_ITEM_KHARAN_7    "So you suspect that someone on the inside was involved? That they were tipped off?"
#define GOSSIP_ITEM_KHARAN_8    "Continue with your story please."
#define GOSSIP_ITEM_KHARAN_9    "Indeed."
#define GOSSIP_ITEM_KHARAN_10   "The door is open, Kharan. You are a free man."

bool GossipHello_npc_kharan_mighthammer(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(QUEST_4001) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KHARAN_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    if (pPlayer->GetQuestStatus(4342) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KHARAN_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);

    if (pPlayer->GetTeam() == HORDE)
        pPlayer->SEND_GOSSIP_MENU(2473, pCreature->GetObjectGuid());
    else
        pPlayer->SEND_GOSSIP_MENU(2474, pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_kharan_mighthammer(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
    case GOSSIP_ACTION_INFO_DEF+1:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KHARAN_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
        pPlayer->SEND_GOSSIP_MENU(2475, pCreature->GetObjectGuid());
        break;
    case GOSSIP_ACTION_INFO_DEF+2:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KHARAN_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
        pPlayer->SEND_GOSSIP_MENU(2476, pCreature->GetObjectGuid());
        break;

    case GOSSIP_ACTION_INFO_DEF+3:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KHARAN_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
        pPlayer->SEND_GOSSIP_MENU(2477, pCreature->GetObjectGuid());
        break;
    case GOSSIP_ACTION_INFO_DEF+4:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KHARAN_6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
        pPlayer->SEND_GOSSIP_MENU(2478, pCreature->GetObjectGuid());
        break;
    case GOSSIP_ACTION_INFO_DEF+5:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KHARAN_7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
        pPlayer->SEND_GOSSIP_MENU(2479, pCreature->GetObjectGuid());
        break;
    case GOSSIP_ACTION_INFO_DEF+6:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KHARAN_8, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+7);
        pPlayer->SEND_GOSSIP_MENU(2480, pCreature->GetObjectGuid());
        break;
    case GOSSIP_ACTION_INFO_DEF+7:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KHARAN_9, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+8);
        pPlayer->SEND_GOSSIP_MENU(2481, pCreature->GetObjectGuid());
        break;
    case GOSSIP_ACTION_INFO_DEF+8:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KHARAN_10, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+9);
        pPlayer->SEND_GOSSIP_MENU(2482, pCreature->GetObjectGuid());
        break;
    case GOSSIP_ACTION_INFO_DEF+9:
        pPlayer->CLOSE_GOSSIP_MENU();
        if (pPlayer->GetTeam() == HORDE)
            pPlayer->AreaExploredOrEventHappens(QUEST_4001);
        else
            pPlayer->AreaExploredOrEventHappens(QUEST_4342);
        break;
    }
    return true;
}

/*######
## npc_lokhtos_darkbargainer
######*/

#define ITEM_THRORIUM_BROTHERHOOD_CONTRACT               18628
#define ITEM_SULFURON_INGOT                              17203
#define QUEST_A_BINDING_CONTRACT                         7604
#define SPELL_CREATE_THORIUM_BROTHERHOOD_CONTRACT_DND    23059

#define GOSSIP_ITEM_SHOW_ACCESS     "Show me what I have access to, Lothos."
#define GOSSIP_ITEM_GET_CONTRACT    "Get Thorium Brotherhood Contract"

bool GossipHello_npc_lokhtos_darkbargainer(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pCreature->isVendor() && pPlayer->GetReputationRank(59) >= REP_FRIENDLY)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_ITEM_SHOW_ACCESS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

    if (pPlayer->GetQuestRewardStatus(QUEST_A_BINDING_CONTRACT) != 1 &&
            !pPlayer->HasItemCount(ITEM_THRORIUM_BROTHERHOOD_CONTRACT, 1, true) &&
            pPlayer->HasItemCount(ITEM_SULFURON_INGOT, 1))
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GET_CONTRACT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    }

    if (pPlayer->GetReputationRank(59) < REP_FRIENDLY)
        pPlayer->SEND_GOSSIP_MENU(3673, pCreature->GetObjectGuid());
    else
        pPlayer->SEND_GOSSIP_MENU(3677, pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_lokhtos_darkbargainer(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer, SPELL_CREATE_THORIUM_BROTHERHOOD_CONTRACT_DND, false);
    }

    if (uiAction == GOSSIP_ACTION_TRADE)
        pPlayer->SEND_VENDORLIST(pCreature->GetObjectGuid());

    return true;
}

/*######
## npc_rocknot
######*/

enum eRocknot
{
    SAY_GOT_BEER        = -1230000,
    SPELL_DRUNKEN_RAGE  = 14872,
    SPELL_LOVE          = 27741,
    QUEST_ALE           = 4295,
};

struct MANGOS_DLL_DECL npc_rocknotAI : public npc_escortAI
{
    npc_rocknotAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_pInstance = (instance_blackrock_depths*)pCreature->GetInstanceData();
        Reset();
    }

    instance_blackrock_depths* m_pInstance;

    uint32 m_uiBreakKegTimer;
    uint32 m_uiBreakDoorTimer;
    uint32 m_uiAlternateStartTimer;

    void Reset()
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
            return;

        m_uiBreakKegTimer = 0;
        m_uiBreakDoorTimer = 0;
        m_uiAlternateStartTimer = 0;
    }

    void DoGo(uint32 id, uint32 state)
    {
        if (!m_pInstance)
            return;

        if (GameObject* pGo = m_pInstance->GetSingleGameObjectFromStorage(id))
        {
            if (state != 2)
                pGo->SetGoState(GOState(state));
            else if (state == 2 && pGo->GetGoState() != GO_STATE_ACTIVE)
                pGo->SetGoState(GOState(state));
        }
    }

    void WaypointReached(uint32 i)
    {
        if (!m_pInstance)
            return;

        switch(i)
        {
        case 1:
            m_creature->HandleEmote(EMOTE_ONESHOT_KICK);
            break;
        case 2:
            m_creature->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);
            break;
        case 3:
            m_creature->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);
            break;
        case 4:
            m_creature->HandleEmote(EMOTE_ONESHOT_KICK);
            break;
        case 5:
            m_creature->HandleEmote(EMOTE_ONESHOT_KICK);
            m_uiBreakKegTimer = 2000;
            break;
        case 7:
            // Force cancel escort
            // Next waypoints (8 and higher) are an alternate way
            // for an another event
            // CHECK: this maybe wont work
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            m_creature->Respawn();
            break;

        case 15:
            if (Creature* pNagmara = m_pInstance->GetSingleCreatureFromStorage(NPC_MISTRESS_NAGMARA))
                m_creature->SetFacingToObject(pNagmara);
            DoCastSpellIfCan(m_creature, SPELL_LOVE);
            break;
        }
    }

    void StartAlternateWay()
    {
        m_uiAlternateStartTimer = 3000;
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if (!m_pInstance)
            return;

        if (m_uiAlternateStartTimer)
        {
            if (m_uiAlternateStartTimer <= uiDiff)
            {
                m_uiAlternateStartTimer = 0;

                Start();
                SetEscortPaused(true);
                SetCurrentWaypoint(8);
                SetEscortPaused(false);
            }
            else
                m_uiAlternateStartTimer -= uiDiff;
        }

        if (m_uiBreakKegTimer)
        {
            if (m_uiBreakKegTimer <= uiDiff)
            {
                DoGo(GO_BAR_KEG_SHOT,0);
                m_uiBreakKegTimer = 0;
                m_uiBreakDoorTimer = 1000;
            }
            else
                m_uiBreakKegTimer -= uiDiff;
        }

        if (m_uiBreakDoorTimer)
        {
            if (m_uiBreakDoorTimer <= uiDiff)
            {
                DoGo(GO_BAR_DOOR,2);
                DoGo(GO_BAR_KEG_TRAP,0);               //doesn't work very well, leaving code here for future
                //spell by trap has effect61, this indicate the bar go hostile

                //TODO: When Keg explode, nears Patrons say some text..

                if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_PHALANX))
                    pTemp->setFaction(14);

                //for later, this event(s) has alot more to it.
                //optionally, DONE can trigger bar to go hostile.
                m_pInstance->SetData(TYPE_BAR, DONE);

                m_uiBreakDoorTimer = 0;
            }
            else
                m_uiBreakDoorTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_rocknot(Creature* pCreature)
{
    return new npc_rocknotAI(pCreature);
}

bool QuestRewarded_npc_rocknot(Player* /*pPlayer*/, Creature* pCreature, const Quest* pQuest)
{
    instance_blackrock_depths* pInstance = (instance_blackrock_depths*)pCreature->GetInstanceData();

    if (!pInstance)
        return true;

    if (pInstance->GetData(TYPE_BAR) == DONE || pInstance->GetData(TYPE_BAR) == SPECIAL)
        return true;

    if (pQuest->GetQuestId() == QUEST_ALE)
    {
        if (pInstance->GetData(TYPE_BAR) != IN_PROGRESS)
            pInstance->SetData(TYPE_BAR,IN_PROGRESS);

        pInstance->SetData(TYPE_BAR,SPECIAL);

        //keep track of amount in instance script, returns SPECIAL if amount ok and event in progress
        if (pInstance->GetData(TYPE_BAR) == SPECIAL)
        {
            DoScriptText(SAY_GOT_BEER, pCreature);
            pCreature->CastSpell(pCreature,SPELL_DRUNKEN_RAGE,false);

            if (npc_rocknotAI* pEscortAI = dynamic_cast<npc_rocknotAI*>(pCreature->AI()))
                pEscortAI->Start(false, 0, NULL, true);
        }
    }

    return true;
}

/*######
## npc_mistress_nagmara
######*/

#define Q_THE_LOVE_POTION   4201
#define GOSSIP_ITEM_NAGMARA "Why don't you and Rocknot go find somewhere private.."

struct MANGOS_DLL_DECL npc_mistress_nagmaraAI : public npc_escortAI
{
    npc_mistress_nagmaraAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_pInstance = (instance_blackrock_depths*)pCreature->GetInstanceData();
        Reset();
    }

    instance_blackrock_depths* m_pInstance;

    uint32 m_uiKissRocknotTimer;
    uint32 m_uiOpenBarDoorTimer;

    bool m_bStairsReached;

    void Reset()
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
            return;

        m_uiKissRocknotTimer = 2000;
        m_uiOpenBarDoorTimer = 0;

        m_bStairsReached = false;
    }

    Creature* GetRocknot()
    {
        if (!m_pInstance)
            return GetClosestCreatureWithEntry(m_creature, NPC_PRIVATE_ROCKNOT, DEFAULT_VISIBILITY_INSTANCE);

        return m_pInstance->GetSingleCreatureFromStorage(NPC_PRIVATE_ROCKNOT);
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
        case 0:
            if (Creature* pRocknot = GetRocknot())
                if (npc_rocknotAI* pRocknotAI = dynamic_cast<npc_rocknotAI*>(pRocknot->AI()))
                    pRocknotAI->StartAlternateWay();
            break;
        case 4:
            m_uiOpenBarDoorTimer = 2000;
            break;
        case 9:
            m_bStairsReached = true;
            DoCastSpellIfCan(m_creature, SPELL_LOVE);
            break;
        default:
            break;
        }
    }

    void JustStartedEscort()
    {
        if (Creature* pRocknot = GetRocknot())
        {
            pRocknot->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
        }
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if (!m_pInstance)
            return;

        if (HasEscortState(STATE_ESCORT_ESCORTING))
        {
            if (m_uiOpenBarDoorTimer)
            {
                if (m_uiOpenBarDoorTimer <= uiDiff)
                {
                    m_uiOpenBarDoorTimer = 0;
                    m_pInstance->SetData(TYPE_BAR3, DONE);
                }
                else
                    m_uiOpenBarDoorTimer -= uiDiff;
            }

            if (m_bStairsReached)
            {
                // Kiss Rocknot
                if (m_uiKissRocknotTimer <= uiDiff)
                {
                    if (Creature* pRocknot = m_pInstance->GetSingleCreatureFromStorage(NPC_PRIVATE_ROCKNOT))
                        if (m_creature->IsWithinDist(pRocknot, 20.0f))
                            m_creature->SetFacingToObject(pRocknot);
                    //TODO: emote command kiss
                    m_uiKissRocknotTimer = 2500;
                }
                else
                    m_uiKissRocknotTimer -= uiDiff;
            }
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_mistress_nagmara(Creature* pCreature)
{
    return new npc_mistress_nagmaraAI(pCreature);
}

bool GossipHello_npc_mistress_nagmara(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestRewardStatus(Q_THE_LOVE_POTION))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NAGMARA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_mistress_nagmara(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == (GOSSIP_ACTION_INFO_DEF + 1))
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        if (instance_blackrock_depths* m_pInstance = (instance_blackrock_depths*)pCreature->GetInstanceData())
        {
            if (Creature* pRocknot = m_pInstance->GetSingleCreatureFromStorage(NPC_PRIVATE_ROCKNOT))
            {
                if (npc_rocknotAI* pRocknotAI = dynamic_cast<npc_rocknotAI*>(pRocknot->AI()))
                {
                    if (!pRocknotAI->HasEscortState(STATE_ESCORT_ESCORTING))
                    {
                        if (npc_mistress_nagmaraAI* pEscortAI = dynamic_cast<npc_mistress_nagmaraAI*>(pCreature->AI()))
                            pEscortAI->Start();
                    }
                    else
                        pCreature->MonsterSay("Wait, Rocknot is busy for a while.", LANG_UNIVERSAL, NULL);
                }
            }
        }
    }

    return true;
}

/* AddSC */

void AddSC_blackrock_depths()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_marshal_windsor";
    pNewScript->GetAI = &GetAI_npc_marshal_windsor;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_marshal_windsor;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_commander_gorshak";
    pNewScript->GetAI = &GetAI_npc_commander_gorshak;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_commander_gorshak;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_commander_gorshak";
    pNewScript->GetAI = &GetAI_npc_commander_gorshak;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_commander_gorshak;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_shadowforge_lock";
    pNewScript->pGOUse = &GOUse_go_shadowforge_lock;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_shadowforge_brazier";
    pNewScript->pGOUse = &GOUse_go_shadowforge_brazier;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_dark_keeper_portrait";
    pNewScript->pGOUse = &GOUse_go_dark_keeper_portrait;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_relic_coffer_door";
    pNewScript->pGOUse = &GOUse_go_relic_coffer_door;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_warbringer_construct";
    pNewScript->GetAI = &GetAI_mob_warbringer_construct;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_watchman_doomgrip";
    pNewScript->GetAI = &GetAI_mob_watchman_doomgrip;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_plugger_spazzring";
    pNewScript->GetAI = &GetAI_mob_plugger_spazzring;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_ribbly_crony";
    pNewScript->GetAI = &GetAI_npc_ribbly_crony;
    pNewScript->pGossipHello =  &GossipHello_npc_ribbly_crony;
    pNewScript->pGossipSelect = &GossipSelect_npc_ribbly_crony;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "the_grim_guzzler_patrons";
    pNewScript->GetAI = &GetAI_the_grim_guzzler_patrons;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_dark_iron_ale_mug_and_grim_guzzler_boar";
    pNewScript->pGOUse = &GOUse_go_dark_iron_ale_mug_and_grim_guzzler_boar;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_thunderbrew_lager_keg";
    pNewScript->pGOUse = &GOUse_go_thunderbrew_lager_keg;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_ring_of_law";
    pNewScript->pAreaTrigger = &AreaTrigger_at_ring_of_law;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_grimstone";
    pNewScript->GetAI = &GetAI_npc_grimstone;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_theldren_trigger";
    pNewScript->GetAI = &GetAI_npc_theldren_trigger;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_phalanx";
    pNewScript->GetAI = &GetAI_mob_phalanx;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_kharan_mighthammer";
    pNewScript->pGossipHello =  &GossipHello_npc_kharan_mighthammer;
    pNewScript->pGossipSelect = &GossipSelect_npc_kharan_mighthammer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_lokhtos_darkbargainer";
    pNewScript->pGossipHello =  &GossipHello_npc_lokhtos_darkbargainer;
    pNewScript->pGossipSelect = &GossipSelect_npc_lokhtos_darkbargainer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_rocknot";
    pNewScript->GetAI = &GetAI_npc_rocknot;
    pNewScript->pQuestRewardedNPC = &QuestRewarded_npc_rocknot;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_mistress_nagmara";
    pNewScript->GetAI = &GetAI_npc_mistress_nagmara;
    pNewScript->pGossipHello =  &GossipHello_npc_mistress_nagmara;
    pNewScript->pGossipSelect = &GossipSelect_npc_mistress_nagmara;
    pNewScript->RegisterSelf();
}

