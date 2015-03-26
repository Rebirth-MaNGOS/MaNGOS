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
SDName: Thousand_Needles
SD%Complete: 90
SDComment: Quest support: 1950, 4770, 4904, 4966, 5151
SDCategory: Thousand Needles
EndScriptData
*/

/* ContentData
npc_kanati
npc_lakota_windsong
npc_paoka_swiftmountain
npc_plucky_johnson
npc_enraged_panther
go_panther_cage
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"

/*######
# npc_kanati
######*/

enum
{
    SAY_KAN_START              = -1000410,

    QUEST_PROTECT_KANATI        = 4966,
    NPC_GALAK_ASS               = 10720
};

const float m_afGalakLoc[]= {-4867.387695f, -1357.353760f, -48.226f};

struct MANGOS_DLL_DECL npc_kanatiAI : public npc_escortAI
{
    npc_kanatiAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    void Reset() { }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 0:
                DoScriptText(SAY_KAN_START, m_creature);
                DoSpawnGalak();
                break;
            case 1:
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_PROTECT_KANATI, m_creature);
                break;
        }
    }

    void DoSpawnGalak()
    {
        for(int i = 0; i < 3; ++i)
            m_creature->SummonCreature(NPC_GALAK_ASS,
            m_afGalakLoc[0], m_afGalakLoc[1], m_afGalakLoc[2], 0.0f,
            TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
    }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->AI()->AttackStart(m_creature);
    }
};

CreatureAI* GetAI_npc_kanati(Creature* pCreature)
{
    return new npc_kanatiAI(pCreature);
}

bool QuestAccept_npc_kanati(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_PROTECT_KANATI)
    {
        if (npc_kanatiAI* pEscortAI = dynamic_cast<npc_kanatiAI*>(pCreature->AI()))
            pEscortAI->Start(false, pPlayer, pQuest, true);
    }
    return true;
}

/*######
# npc_lakota_windsong
######*/

enum
{
    SAY_LAKO_START              = -1000365,
    SAY_LAKO_LOOK_OUT           = -1000366,
    SAY_LAKO_HERE_COME          = -1000367,
    SAY_LAKO_MORE               = -1000368,
    SAY_LAKO_END                = -1000369,

    QUEST_FREE_AT_LAST          = 4904,
    NPC_GRIM_BANDIT             = 10758,

    ID_AMBUSH_1                 = 0,
    ID_AMBUSH_2                 = 2,
    ID_AMBUSH_3                 = 4
};

float m_afBanditLoc[6][6]=
{
    {-4905.479492f, -2062.732666f, 84.352f},
    {-4915.201172f, -2073.528320f, 84.733f},
    {-4878.883301f, -1986.947876f, 91.966f},
    {-4877.503906f, -1966.113403f, 91.859f},
    {-4767.985352f, -1873.169189f, 90.192f},
    {-4788.861328f, -1888.007813f, 89.888f}
};

struct MANGOS_DLL_DECL npc_lakota_windsongAI : public npc_escortAI
{
    npc_lakota_windsongAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    void Reset() { }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 8:
                DoScriptText(SAY_LAKO_LOOK_OUT, m_creature);
                DoSpawnBandits(ID_AMBUSH_1);
                break;
            case 14:
                DoScriptText(SAY_LAKO_HERE_COME, m_creature);
                DoSpawnBandits(ID_AMBUSH_2);
                break;
            case 21:
                DoScriptText(SAY_LAKO_MORE, m_creature);
                DoSpawnBandits(ID_AMBUSH_3);
                break;
            case 45:
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_FREE_AT_LAST, m_creature);
                break;
        }
    }

    void DoSpawnBandits(int uiAmbushId)
    {
        for(int i = 0; i < 2; ++i)
            m_creature->SummonCreature(NPC_GRIM_BANDIT,
            m_afBanditLoc[i+uiAmbushId][0], m_afBanditLoc[i+uiAmbushId][1], m_afBanditLoc[i+uiAmbushId][2], 0.0f,
            TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000);
    }
};

CreatureAI* GetAI_npc_lakota_windsong(Creature* pCreature)
{
    return new npc_lakota_windsongAI(pCreature);
}

bool QuestAccept_npc_lakota_windsong(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_FREE_AT_LAST)
    {
        DoScriptText(SAY_LAKO_START, pCreature, pPlayer);
        pCreature->setFaction(FACTION_ESCORT_H_NEUTRAL_ACTIVE);

        if (npc_lakota_windsongAI* pEscortAI = dynamic_cast<npc_lakota_windsongAI*>(pCreature->AI()))
            pEscortAI->Start(false, pPlayer, pQuest);
    }
    return true;
}

/*######
# npc_paoka_swiftmountain
######*/

enum
{
    SAY_START           = -1000362,
    SAY_WYVERN          = -1000363,
    SAY_COMPLETE        = -1000364,

    QUEST_HOMEWARD      = 4770,
    NPC_WYVERN          = 4107
};

float m_afWyvernLoc[3][3]=
{
    {-4990.606f, -906.057f, -5.343f},
    {-4970.241f, -927.378f, -4.951f},
    {-4985.364f, -952.528f, -5.199f}
};

struct MANGOS_DLL_DECL npc_paoka_swiftmountainAI : public npc_escortAI
{
    npc_paoka_swiftmountainAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    void Reset() { }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 15:
                DoScriptText(SAY_WYVERN, m_creature);
                DoSpawnWyvern();
                break;
            case 26:
                DoScriptText(SAY_COMPLETE, m_creature);
                break;
            case 27:
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_HOMEWARD, m_creature);
                break;
        }
    }

    void DoSpawnWyvern()
    {
        for(int i = 0; i < 3; ++i)
            m_creature->SummonCreature(NPC_WYVERN,
            m_afWyvernLoc[i][0], m_afWyvernLoc[i][1], m_afWyvernLoc[i][2], 0.0f,
            TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000);
    }
};

CreatureAI* GetAI_npc_paoka_swiftmountain(Creature* pCreature)
{
    return new npc_paoka_swiftmountainAI(pCreature);
}

bool QuestAccept_npc_paoka_swiftmountain(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_HOMEWARD)
    {
        DoScriptText(SAY_START, pCreature, pPlayer);
        pCreature->setFaction(FACTION_ESCORT_H_NEUTRAL_ACTIVE);

        if (npc_paoka_swiftmountainAI* pEscortAI = dynamic_cast<npc_paoka_swiftmountainAI*>(pCreature->AI()))
            pEscortAI->Start(false, pPlayer, pQuest);
    }
    return true;
}

/*######
# "Plucky" Johnson
######*/

enum
{
    FACTION_FRIENDLY        = 35,
    QUEST_SCOOP             = 1950,
    SPELL_PLUCKY_HUMAN      = 9192,
    SPELL_PLUCKY_CHICKEN    = 9220
};

#define GOSSIP_ITEM_QUEST   "Please tell me the Phrase.."

struct MANGOS_DLL_DECL npc_plucky_johnsonAI : public ScriptedAI
{
    npc_plucky_johnsonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiNormFaction = pCreature->getFaction();
        Reset();
    }

    uint32 m_uiNormFaction;
    uint32 m_uiResetTimer;

    void Reset()
    {
        m_uiResetTimer = 120000;

        if (m_creature->getFaction() != m_uiNormFaction)
            m_creature->setFaction(m_uiNormFaction);

        if (m_creature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
            m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        m_creature->CastSpell(m_creature, SPELL_PLUCKY_CHICKEN, false);
    }

    void ReceiveEmote(Player* pPlayer, uint32 uiTextEmote)
    {
        if (pPlayer->GetQuestStatus(QUEST_SCOOP) == QUEST_STATUS_INCOMPLETE)
        {
            if (uiTextEmote == TEXTEMOTE_BECKON)
            {
                m_creature->setFaction(FACTION_FRIENDLY);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                m_creature->CastSpell(m_creature, SPELL_PLUCKY_HUMAN, false);
            }
        }

        if (uiTextEmote == TEXTEMOTE_CHICKEN)
        {
            if (m_creature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
                return;
            else
            {
                m_creature->setFaction(FACTION_FRIENDLY);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                m_creature->CastSpell(m_creature, SPELL_PLUCKY_HUMAN, false);
                m_creature->HandleEmote(EMOTE_ONESHOT_WAVE);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_creature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
        {
            if (m_uiResetTimer < uiDiff)
            {
                if (!m_creature->getVictim())
                    ResetToHome();
                else
                    m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                return;
            }
            else
                m_uiResetTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_plucky_johnson(Creature* pCreature)
{
    return new npc_plucky_johnsonAI(pCreature);
}

bool GossipHello_npc_plucky_johnson(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_SCOOP) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_QUEST, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    pPlayer->SEND_GOSSIP_MENU(720, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_plucky_johnson(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF)
    {
        pPlayer->SEND_GOSSIP_MENU(738, pCreature->GetObjectGuid());
        pPlayer->AreaExploredOrEventHappens(QUEST_SCOOP);
    }

    return true;
}

/*######
# Panther Cage
######*/

enum ePantherCage
{
    QUEST_HYPERCAPACITOR_GIZMO  = 5151,
    NPC_ENRAGED_PANTHER    = 10992,
    //FACTION_FRIENDLY            = 35, - defined in Plucky Johnson
    FACTION_HOSTILE             = 14
};

bool go_panther_cage(Player* pPlayer, GameObject* /*pGo*/)
{
    if (pPlayer->GetQuestStatus(QUEST_HYPERCAPACITOR_GIZMO) == QUEST_STATUS_INCOMPLETE)
    {
        if (Creature* Panther = GetClosestCreatureWithEntry(pPlayer, NPC_ENRAGED_PANTHER, 5)) 
        {
            Panther->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
            Panther->setFaction(FACTION_HOSTILE);
            Panther->AI()->AttackStart(pPlayer);
        }
    }
    return true;
}

struct MANGOS_DLL_DECL npc_enraged_pantherAI : public ScriptedAI
{
    npc_enraged_pantherAI(Creature *pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->setFaction(FACTION_FRIENDLY);
    }

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return; 

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_enraged_panther(Creature* pCreature)
{
    return new npc_enraged_pantherAI(pCreature);
}

/*######
# Harpy Foodstuffs
######*/

bool go_harpy_foodstuffs(Player* pPlayer, GameObject* /*pGo*/)
{
	std::list<GameObject*> L;
	GetGameObjectListWithEntryInGrid(L, pPlayer, 20447, 15.0f);
	if (!L.empty())
	{
		for (std::list<GameObject*>::const_iterator itr = L.begin(); itr != L.end(); itr++)
		{
			if ((*itr)->isSpawned())
			{
				(*itr)->SetGoState(GO_STATE_ACTIVE);
				(*itr)->SetLootState(GO_JUST_DEACTIVATED);
			}
		}
	}

    return false;
}

/*####
# npc_kravel_koalbeard
####*/

enum
{
	KRAVEL_SAY_1					 = -1720047,

	GO_PARTS_CRATE					 = 175168,

    QUEST_ID_PARTS_FOR_KRAVEL		 = 1112,
};

struct MANGOS_DLL_DECL npc_kravel_koalbeardAI : public npc_escortAI
{
    npc_kravel_koalbeardAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }
	
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
		m_uiSpeechTimer = 3000;
		m_uiSpeechStep = 1;
		m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
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
                        m_creature->GenericTextEmote("Kravel Koalbeard places the crate of parts on the ground.", NULL, false);
						
						m_uiSpeechTimer = 3000;
                        break;
					case 2:
						m_creature->SummonGameObject(GO_PARTS_CRATE, 13000, -6232.27f,-3854.748f,-58.74f,4.10f);
                        m_uiSpeechTimer = 4000;
                        break;
					case 3:
						m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
						m_uiSpeechTimer = 1000;
						break;
					case 4:
						m_creature->GenericTextEmote("Kravel Koalbeard grabs a part and puts it in his pocket...", NULL, false);
						m_uiSpeechTimer = 2000;
						break;
					case 5:
						m_creature->SetStandState(UNIT_STAND_STATE_STAND);
						m_uiSpeechTimer = 3000;
						break;
					case 6:
						DoScriptText(KRAVEL_SAY_1, m_creature);
						m_uiSpeechTimer = 3000;
						break;
					case 7:
						m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
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

CreatureAI* GetAI_npc_kravel_koalbeard(Creature* pCreature)
{
    return new npc_kravel_koalbeardAI(pCreature);
}

bool OnQuestRewarded_npc_kravel_koalbeard(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
	if (pQuest->GetQuestId() == QUEST_ID_PARTS_FOR_KRAVEL)
    {
		if (npc_kravel_koalbeardAI* pEscortAI = dynamic_cast<npc_kravel_koalbeardAI*>(pCreature->AI()))
			pEscortAI->StartOutro();
	}
	return true;
}

void AddSC_thousand_needles()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "npc_kanati";
    pNewscript->GetAI = &GetAI_npc_kanati;
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_kanati;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_lakota_windsong";
    pNewscript->GetAI = &GetAI_npc_lakota_windsong;
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_lakota_windsong;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_paoka_swiftmountain";
    pNewscript->GetAI = &GetAI_npc_paoka_swiftmountain;
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_paoka_swiftmountain;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_plucky_johnson";
    pNewscript->GetAI = &GetAI_npc_plucky_johnson;
    pNewscript->pGossipHello = &GossipHello_npc_plucky_johnson;
    pNewscript->pGossipSelect = &GossipSelect_npc_plucky_johnson;
    pNewscript->RegisterSelf();
    
    pNewscript = new Script;
    pNewscript->Name="npc_enraged_panther";
    pNewscript->GetAI = &GetAI_npc_enraged_panther;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name="go_panther_cage";
    pNewscript->pGOUse = &go_panther_cage;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name="go_harpy_foodstuffs";
    pNewscript->pGOUse = &go_harpy_foodstuffs;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_kravel_koalbeard";
    pNewscript->GetAI = &GetAI_npc_kravel_koalbeard;
    pNewscript->pQuestRewardedNPC = &OnQuestRewarded_npc_kravel_koalbeard;
    pNewscript->RegisterSelf();
}
