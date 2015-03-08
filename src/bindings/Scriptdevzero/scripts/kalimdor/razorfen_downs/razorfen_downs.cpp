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
SDName: Razorfen_Downs
SD%Complete: 100
SDComment: Support for Henry Stern(2 recipes), Gong of Razorfen Downs (Tuten'Kash Event)
SDCategory: Razorfen Downs
EndScriptData */

/* ContentData
go_gong_of_downs
npc_belnistrasz
npc_henry_stern
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"
#include "razorfen_downs.h"

/*###
# go_gong_of_downs
####*/

bool GOUse_go_gong_of_downs(Player* /*pPlayer*/, GameObject* pGo)
{
    instance_razorfen_downs* m_pInstance = (instance_razorfen_downs*)pGo->GetInstanceData();
    if (m_pInstance)
    {
        if (m_pInstance->GetData(DATA_GONG_USE) != DONE)
        {
            m_pInstance->SetData(DATA_GONG_USE, IN_PROGRESS);
            return true;
        }
    }
    return false;
}

/*###
# npc_belnistrasz
####*/

static float EventSpawns[2][3] =
{
	{2598, 972, 54},
	{2545, 979, 50}
};

enum eBelistrasz
{
    QUEST_EXTINGUISHING_THE_IDOL    = 3525,

    SAY_BELNISTRASZ_READY           = -1000700,
    SAY_BELNISTRASZ_START_RIT       = -1000701,
    SAY_BELNISTRASZ_AGGRO_1         = -1000702,
    SAY_BELNISTRASZ_AGGRO_2         = -1000703,
    SAY_BELNISTRASZ_3_MIN           = -1000704,
    SAY_BELNISTRASZ_2_MIN           = -1000705,
    SAY_BELNISTRASZ_1_MIN           = -1000706,
    SAY_BELNISTRASZ_FINISH          = -1000707,

    NPC_IDOL_ROOM_SPAWNER           = 8611,

    NPC_WITHERED_BATTLE_BOAR        = 7333,
    NPC_WITHERED_QUILGUARD          = 7329,
    NPC_DEATHS_HEAD_GEOMANCER       = 7335,
    NPC_PLAGUEMAW_THE_ROTTING       = 7356,

    GO_BELNISTRASZ_BRAZIER          = 152097,

    SPELL_ARCANE_INTELLECT          = 13326,                // use this somewhere (he has it as default)
    SPELL_FIREBALL                  = 9053,
    SPELL_FROST_NOVA                = 11831,
    SPELL_IDOL_SHUTDOWN             = 12774,

    // summon spells only exist in 1.x
    //SPELL_SUMMON_1                  = 12694,              // NPC_WITHERED_BATTLE_BOAR
    //SPELL_SUMMON_2                  = 14802,              // NPC_DEATHS_HEAD_GEOMANCER
    //SPELL_SUMMON_3                  = 14801,              // NPC_WITHERED_QUILGUARD
};

static float m_fSpawnerCoord[3][4]=
{
    {2582.79f, 954.392f, 52.4821f, 3.78736f},
    {2569.42f, 956.380f, 52.2732f, 5.42797f},
    {2570.62f, 942.393f, 53.7433f, 0.71558f}
};

struct MANGOS_DLL_DECL npc_belnistraszAI : public npc_escortAI
{
    npc_belnistraszAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_uiRitualPhase = 0;
        m_uiRitualTimer = 1000;
        Reset();
    }

    uint8 m_uiRitualPhase;
    uint32 m_uiRitualTimer;

    uint32 m_uiFireballTimer;
    uint32 m_uiFrostNovaTimer;

    void Reset()
    {
        m_uiFireballTimer  = 1000;
        m_uiFrostNovaTimer = 6000;
    }

	void Aggro(Unit* pAttacker)			// not working
    {
		DoScriptText(urand(0,1) ? SAY_BELNISTRASZ_AGGRO_1 : SAY_BELNISTRASZ_AGGRO_2, m_creature, pAttacker);
	}
 
    void SpawnerSummon(Creature* pSummoner)
    {
		Creature* pTemp;
        if (m_uiRitualPhase > 7)
        {
			int random = urand(0,1);
            pTemp = pSummoner->SummonCreature(NPC_PLAGUEMAW_THE_ROTTING,EventSpawns[random][0], EventSpawns[random][1], EventSpawns[random][2], pSummoner->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
			pTemp->AI()->AttackStart(m_creature);
            return;
        }

		int random = urand(0,1);
        for(int i = 0; i < 4; ++i)
        {
            uint32 uiEntry = 0;

            // ref TARGET_RANDOM_CIRCUMFERENCE_POINT
            float angle = 2.0f * M_PI_F * rand_norm_f();
            float fX, fZ, fY;
            pSummoner->GetClosePoint(fX, fZ, fY, 0.0f, 2.0f, angle);

            switch(i)
            {
                case 0:
                case 1:
                    uiEntry = NPC_WITHERED_BATTLE_BOAR;
                    break;
                case 2:
                    uiEntry = NPC_WITHERED_QUILGUARD;
                    break;
                case 3:
                    uiEntry = NPC_DEATHS_HEAD_GEOMANCER;
                    break;
            }
			int randsp = urand(-5,5);
			pTemp = pSummoner->SummonCreature(uiEntry, EventSpawns[random][0]+randsp, EventSpawns[random][1]+randsp, EventSpawns[random][2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
			pTemp->AI()->AttackStart(m_creature);
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        SpawnerSummon(pSummoned);
		pSummoned->SetRespawnDelay(-10);		// don't randomly respawn ever, better safe than sorry
    }

    void DoSummonSpawner(int32 iType)
    {
        m_creature->SummonCreature(NPC_IDOL_ROOM_SPAWNER, m_fSpawnerCoord[iType][0], m_fSpawnerCoord[iType][1], m_fSpawnerCoord[iType][2], m_fSpawnerCoord[iType][3], TEMPSUMMON_TIMED_DESPAWN, 10000);
    }

    void WaypointReached(uint32 uiPointId)
    {
        if (uiPointId == 24)
        {
            DoScriptText(SAY_BELNISTRASZ_START_RIT, m_creature);
            SetEscortPaused(true);
        }
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if (HasEscortState(STATE_ESCORT_PAUSED))
        {
            if (m_uiRitualTimer < uiDiff)
            {
                switch(m_uiRitualPhase)
                {
                    case 0:
                        DoCastSpellIfCan(m_creature, SPELL_IDOL_SHUTDOWN);
                        m_uiRitualTimer = 1000;
                        break;
                    case 1:
                        DoSummonSpawner(irand(1, 3));
                        m_uiRitualTimer = 39000;
                        break;
                    case 2:
                        DoSummonSpawner(irand(1, 3));
                        m_uiRitualTimer = 20000;
                        break;
                    case 3:
                        DoScriptText(SAY_BELNISTRASZ_3_MIN, m_creature);
                        m_uiRitualTimer = 20000;
                        break;
                    case 4:
                        DoSummonSpawner(irand(1, 3));
                        m_uiRitualTimer = 40000;
                        break;
                    case 5:
                        DoSummonSpawner(irand(1, 3));
                        DoScriptText(SAY_BELNISTRASZ_2_MIN, m_creature);
                        m_uiRitualTimer = 40000;
                        break;
                    case 6:
                        DoSummonSpawner(irand(1, 3));
                        m_uiRitualTimer = 20000;
                        break;
                    case 7:
                        DoScriptText(SAY_BELNISTRASZ_1_MIN, m_creature);
                        m_uiRitualTimer = 40000;
                        break;
                    case 8:
                        DoSummonSpawner(irand(1, 3));
                        m_uiRitualTimer = 20000;
                        break;
                    case 9:
                        DoScriptText(SAY_BELNISTRASZ_FINISH, m_creature);
                        m_uiRitualTimer = 3000;
                        break;
                    case 10:
                    {
                        if (Player* pPlayer = GetPlayerForEscort())
                        {
                            pPlayer->GroupEventHappens(QUEST_EXTINGUISHING_THE_IDOL, m_creature);

                            if (GameObject* pGo = GetClosestGameObjectWithEntry(m_creature, GO_BELNISTRASZ_BRAZIER, 10.0f))
                            {
                                if (!pGo->isSpawned())
                                {
                                    pGo->SetRespawnTime(HOUR*IN_MILLISECONDS);
                                    pGo->Refresh();
                                }
                            }
                        }
                        m_creature->RemoveAurasDueToSpell(SPELL_IDOL_SHUTDOWN);
                        SetEscortPaused(false);
                        break;
                    }
                }

                ++m_uiRitualPhase;
            }
            else
                m_uiRitualTimer -= uiDiff;

            return;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiFireballTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FIREBALL);
            m_uiFireballTimer  = urand(2000,3000);
        }
        else
            m_uiFireballTimer -= uiDiff;

        if (m_uiFrostNovaTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FROST_NOVA);
            m_uiFrostNovaTimer = urand(10000,15000);
        }
        else
            m_uiFrostNovaTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_belnistrasz(Creature* pCreature)
{
    return new npc_belnistraszAI(pCreature);
}

bool QuestAccept_npc_belnistrasz(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_EXTINGUISHING_THE_IDOL)
    {
        if (npc_belnistraszAI* pEscortAI = dynamic_cast<npc_belnistraszAI*>(pCreature->AI()))
        {
            pEscortAI->Start(true, pPlayer, pQuest);
            DoScriptText(SAY_BELNISTRASZ_READY, pCreature, pPlayer);
            pCreature->setFaction(FACTION_ESCORT_N_NEUTRAL_ACTIVE);
        }
    }

    return true;
}

/*###
# npc_henry_stern
####*/

enum eHenryStern
{
    SPELL_GOLDTHORN_TEA                         = 13028,
    SPELL_TEACHING_GOLDTHORN_TEA                = 13029,
    SPELL_MIGHT_TROLLS_BLOOD_POTION             = 3451,
    SPELL_TEACHING_MIGHTY_TROLLS_BLOOD_POTION   = 13030,
    GOSSIP_TEXT_TEA_ANSWER                      = 2114,
    GOSSIP_TEXT_POTION_ANSWER                   = 2115,
};

#define GOSSIP_ITEM_TEA     "Teach me the cooking recipe"
#define GOSSIP_ITEM_POTION  "Teach me the alchemy recipe"

bool GossipHello_npc_henry_stern(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetBaseSkillValue(SKILL_COOKING) >= 175 && !pPlayer->HasSpell(SPELL_GOLDTHORN_TEA))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TEA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    if (pPlayer->GetBaseSkillValue(SKILL_ALCHEMY) >= 180 && !pPlayer->HasSpell(SPELL_MIGHT_TROLLS_BLOOD_POTION))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_POTION, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

	pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_henry_stern(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
	{
		case GOSSIP_ACTION_INFO_DEF + 1:
			pCreature->CastSpell(pPlayer, SPELL_TEACHING_GOLDTHORN_TEA, true);
			pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_TEA_ANSWER, pCreature->GetObjectGuid());
			break;
		case GOSSIP_ACTION_INFO_DEF + 2:
			pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_POTION_ANSWER, pCreature->GetObjectGuid());
			pCreature->CastSpell(pPlayer, SPELL_TEACHING_MIGHTY_TROLLS_BLOOD_POTION, true);
			break;
	}
    return true;
}

void AddSC_razorfen_downs()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "go_gong_of_downs";
    pNewScript->pGOUse = &GOUse_go_gong_of_downs;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_belnistrasz";
    pNewScript->GetAI = &GetAI_npc_belnistrasz;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_belnistrasz;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_henry_stern";
    pNewScript->pGossipHello = &GossipHello_npc_henry_stern;
    pNewScript->pGossipSelect = &GossipSelect_npc_henry_stern;
    pNewScript->RegisterSelf();
}
