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
SDName: Eastern_Plaguelands
SD%Complete: 100
SDComment: Quest support: 5211, 5742. Argent Medic, Special vendor Augustus the Touched, Wind Master William Kielar
SDCategory: Eastern Plaguelands
EndScriptData */

/* ContentData
mobs_ghoul_flayer
npc_argent_medic
npc_augustus_the_touched
npc_betina_bigglezink
npc_darrowshire_spirit
npc_tirion_fordring
npc_highlord_taelan_fordring
npc_william_kielar
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"
#include "ObjectMgr.h"

//id8530 - cannibal ghoul
//id8531 - gibbering ghoul
//id8532 - diseased flayer

struct MANGOS_DLL_DECL mobs_ghoul_flayerAI : public ScriptedAI
{
    mobs_ghoul_flayerAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset() { }

    void JustDied(Unit* pKiller)
    {
        if (pKiller->GetTypeId() == TYPEID_PLAYER)
            m_creature->SummonCreature(11064, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 40000);
    }

};

CreatureAI* GetAI_mobs_ghoul_flayer(Creature* pCreature)
{
    return new mobs_ghoul_flayerAI(pCreature);
}

/*######
## npc_argent_medic
######*/

enum eArgentMedic
{
    FACTION_ARGENT_DAWN     = 529,

    GOSSIP_BASE             = 8454,
    GOSSIP_HONORED          = 8455,

    SPELL_GHOUL_ROT         = 12541,
    SPELL_MAGGOT_SLIME      = 16449
};

bool GossipHello_npc_argent_medic(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetReputationRank(FACTION_ARGENT_DAWN) >= REP_HONORED)
    {
        if (pPlayer->HasAura(SPELL_GHOUL_ROT) || pPlayer->HasAura(SPELL_MAGGOT_SLIME))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I'm infected, help me please!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        pPlayer->SEND_GOSSIP_MENU(GOSSIP_HONORED, pCreature->GetObjectGuid());
    }
    else
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_BASE, pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_argent_medic(Player* pPlayer, Creature* /*pCreature*/, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        if (pPlayer->HasAura(SPELL_GHOUL_ROT))
            pPlayer->RemoveAurasDueToSpell(SPELL_GHOUL_ROT);

        if (pPlayer->HasAura(SPELL_MAGGOT_SLIME))
            pPlayer->RemoveAurasDueToSpell(SPELL_MAGGOT_SLIME);
    }
    return true;
}

/*######
## npc_augustus_the_touched
######*/

bool GossipHello_npc_augustus_the_touched(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pCreature->isVendor() && pPlayer->GetQuestRewardStatus(6164))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_augustus_the_touched(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_TRADE)
        pPlayer->SEND_VENDORLIST(pCreature->GetObjectGuid());
    return true;
}

/*######
## npc_betina_bigglezink
######*/

#define GOSSIP_GAMBIT     "Could you give me one more Dawn's Gambit?"

enum eBetina
{
    ITEM_DAWNS_GAMBIT                 = 12368,
    SPELL_CREATE_ITEM_DAWNS_GAMBIT    = 18367,
    QUEST_DAWNS_GAMBIT                = 4771
};

bool GossipHello_npc_betina_bigglezink(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (!pPlayer->HasItemCount(ITEM_DAWNS_GAMBIT, 1, true) && pPlayer->GetQuestRewardStatus(QUEST_DAWNS_GAMBIT))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_GAMBIT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_betina_bigglezink(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->CastSpell(pPlayer, SPELL_CREATE_ITEM_DAWNS_GAMBIT, false);
    }
    return true;
}

/*######
## npc_darrowshire_spirit
######*/

enum eSpirit
{
    SPELL_SPIRIT_SPAWNIN      = 17321,
    GOSSIP_SPIRIT             = 3873
};

struct MANGOS_DLL_DECL npc_darrowshire_spiritAI : public ScriptedAI
{
    npc_darrowshire_spiritAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset()
    {
        DoCastSpellIfCan(m_creature, SPELL_SPIRIT_SPAWNIN);
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }
};

CreatureAI* GetAI_npc_darrowshire_spirit(Creature* pCreature)
{
    return new npc_darrowshire_spiritAI(pCreature);
}

bool GossipHello_npc_darrowshire_spirit(Player* pPlayer, Creature* pCreature)
{
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Farewell, my friend.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    pPlayer->SEND_GOSSIP_MENU(GOSSIP_SPIRIT, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_darrowshire_spirit(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->TalkedToCreature(pCreature->GetEntry(), pCreature->GetObjectGuid());
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }
    return true;
}

/*######
## npc_tirion_fordring
######*/

enum eTirionFordring
{
    SPELL_DEVOTION_AURA     = 8285,
    SPELL_EXORCISM          = 17149,
    SPELL_HAMMER_OF_JUSTICE = 13005,

    NPC_TAELAN              = 1842
};

struct MANGOS_DLL_DECL npc_tirion_fordringAI : public ScriptedAI
{
    npc_tirion_fordringAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiExorcismTimer;
    uint32 m_uiHammerOfJusticeTimer;

    void Reset()
    {
        m_uiExorcismTimer = 7000;
        m_uiHammerOfJusticeTimer = 4000;
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoCastSpellIfCan(m_creature, SPELL_DEVOTION_AURA);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (GetClosestCreatureWithEntry(m_creature, NPC_TAELAN, 40.0f))
            if (pDoneBy->GetTypeId() == TYPEID_UNIT)
                uiDamage = (uiDamage >= m_creature->GetHealth() ? 0 : urand(8,12));
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Exorcism
        if (m_uiHammerOfJusticeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_HAMMER_OF_JUSTICE);
            m_uiHammerOfJusticeTimer = 10000;
        }
        else
            m_uiHammerOfJusticeTimer -= uiDiff;

        // Hammer of Justice
        if (m_uiHammerOfJusticeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_HAMMER_OF_JUSTICE);
            m_uiHammerOfJusticeTimer = 18000;
        }
        else
            m_uiHammerOfJusticeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_npc_tirion_fordring(Creature* pCreature)
{
    return new npc_tirion_fordringAI(pCreature);
}

bool GossipHello_npc_tirion_fordring(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(5742) == QUEST_STATUS_INCOMPLETE && pPlayer->getStandState() == UNIT_STAND_STATE_SIT)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I am ready to hear your tale, Tirion.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_tirion_fordring(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Thank you, Tirion.  What of your identity?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(4493, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "That is terrible.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            pPlayer->SEND_GOSSIP_MENU(4494, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I will, Tirion.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            pPlayer->SEND_GOSSIP_MENU(4495, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->AreaExploredOrEventHappens(5742);
            break;
    }
    return true;
}

/*######
## npc_william_kielar
######*/

bool GossipHello_npc_william_kielar(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Take me to Northpass Tower!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Take me to Eastwall Tower!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Take me to Crown Guard Tower!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_william_kielar(Player* pPlayer, Creature* /*pCreature*/, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->ActivateTaxiPathTo(494);
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->ActivateTaxiPathTo(495);
            break;
        case GOSSIP_ACTION_INFO_DEF + 3:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->ActivateTaxiPathTo(496);
            break;
    }    
    return true;
}

/*######
## mob_eris_havenfire				// don't use old script
######*/

/*enum eErisHavenfire
{
    QUEST_THE_BALANCE_OF_LIGHT_AND_SHADOW   = 7622,

    NPC_SCOURGE_ARCHER                      = 14489,
    NPC_INJURED_PEASANT                     = 14484,
    NPC_PLAGUED_PEASANT                     = 14485,
    NPC_SCOURGE_FOOTSOLDIER                 = 14486,
	NPC_THE_CLEANER							= 14503,            // summoned if the priest gets help
	GO_PEASNT_LIGHT_TRAP					= 179693,

    SPELL_SHOT_VISUAL                       = 7105,
    SPELL_DEATHS_DOOR                       = 23127,
    SPELL_BLESSING_OF_NORDRASSIL            = 23108,            // yell: be healed!

    MAX_TIME_FOR_BENEDICTION_EVENT          = 15 * 60 * 1000,   // minutes * minute * ms

    // says and yells
    SAY_YELL_PEASANT_SPAWN1                 = -1000682,
    SAY_YELL_PEASANT_SPAWN2                 = -1000683,
    SAY_YELL_PEASANT_SPAWN3                 = -1000684,
    SAY_PEASANT_WALK1                       = -1000685,
    SAY_PEASANT_WALK2                       = -1000686,
    SAY_PEASANT_WALK3                       = -1000687,
    SAY_PEASANT_DONE1                       = -1000688,
    SAY_PEASANT_DONE2                       = -1000689,
    SAY_PEASANT_DONE3                       = -1000690,
    SAY_PEASANT_DONE4                       = -1000691,
    SAY_YELL_ERIS_FAILED                    = -1000692,
    SAY_ERIS_FAILED                         = -1000693,
    SAY_YELL_ERIS_DONE                      = -1000694,
    SAY_YELL_ERIS_BE_HEALED                 = -1000695,
};

float archers_pos[9][3]=
{
    {3333.51f, -3051.17f, 174.18f},
    {3356.81f, -3063.30f, 172.62f},
    {3350.63f, -3065.70f, 173.64f},
    {3358.04f, -3075.34f, 174.18f},
    {3368.06f, -3075.25f, 175.99f},
    {3371.06f, -3068.99f, 175.07f},
    {3377.81f, -3059.43f, 180.50f},
    {3376.75f, -3041.97f, 172.64f},
    {3369.26f, -3023.43f, 171.83f},
};

float peasants_pos[11][3]=
{
    {3368.17f, -3057.53f, 166.06f},
    {3366.28f, -3058.45f, 165.90f},
    {3364.48f, -3054.75f, 165.36f},
    {3362.51f, -3055.58f, 165.27f},
    {3360.68f, -3056.20f, 165.32f},
    {3359.55f, -3052.88f, 165.38f},
    {3357.58f, -3053.55f, 165.56f},
    {3356.17f, -3056.18f, 166.04f},
    {3364.07f, -3049.74f, 165.23f},
    {3359.53f, -3049.58f, 165.25f},
    {3355.05f, -3050.96f, 165.29f},
};

static float aPeasantMoveLoc[3]= {3332.89f, -2982.04f, 161.27f};

struct MANGOS_DLL_DECL mob_eris_havenfireAI : public ScriptedAI							// SHOULD BE 5 WAVES!
{
    mob_eris_havenfireAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiPlayerGUID = ObjectGuid();
        Reset();
    }

    uint32 m_uiEventPhase;
    uint32 m_uiEventTimer;
    uint32 m_uiEventResetTimer;
    uint32 m_uiBeHealedTimer;
    ObjectGuid m_uiPlayerGUID;
    uint8 m_uiPeasantsDead;
    GUIDList m_lPeasants;
    GUIDList m_lScourgeArchers;
    GUIDList m_lScourgeFootsoldiers;
    bool m_bFirstWaveSay;
    bool m_bSecondWaveSay;
    bool m_bThirdaveSay;

    void Reset()
    {
        if (m_uiPlayerGUID)
            return;

        m_uiEventPhase = 0;
        m_uiEventTimer = 0;
        m_uiEventResetTimer = 0;
        m_uiBeHealedTimer = 0;
		m_uiPlayerGUID.Clear();
        m_uiPeasantsDead = 0;
        m_lPeasants.clear();
        m_lScourgeArchers.clear();
        m_lScourgeFootsoldiers.clear();
        m_bFirstWaveSay = false;
        m_bSecondWaveSay = false;
        m_bThirdaveSay = false;
        m_creature->setFaction(m_creature->GetCreatureInfo()->faction_A);
    }

    void JustSummoned(Creature* pSummoned)
    {
        switch(pSummoned->GetEntry())
        {
        case NPC_INJURED_PEASANT:
			float fX, fY, fZ;
            pSummoned->GetRandomPoint(aPeasantMoveLoc[0], aPeasantMoveLoc[1], aPeasantMoveLoc[2], 10.0f, fX, fY, fZ);
            pSummoned->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
			m_lPeasants.push_back(pSummoned->GetObjectGuid());
            break;
        case NPC_SCOURGE_ARCHER:
            m_lScourgeArchers.push_back(pSummoned->GetObjectGuid());
            break;
        case NPC_SCOURGE_FOOTSOLDIER:
            m_lScourgeFootsoldiers.push_back(pSummoned->GetObjectGuid());
            if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID))
                pSummoned->AI()->AttackStart(pPlayer);
            break;
		case NPC_THE_CLEANER:
			if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID))
			{
				pSummoned->AI()->AttackStart(pPlayer);
            }
            break;
        }
    }

    void SummonedCreatureJustDied(Creature* pCreature)
    {
        if (pCreature->GetEntry() == NPC_INJURED_PEASANT || pCreature->GetEntry() == NPC_PLAGUED_PEASANT)
        {
            if (++m_uiPeasantsDead == 15)
            {
                m_uiEventPhase = 100;
                m_uiEventTimer = 100;   // ms
            }
        }
    }

    ObjectGuid StartBenedictionEvent(ObjectGuid player_guid)
    {
        if (m_uiPlayerGUID)
            return ObjectGuid();

		if (GameObject* pLight = GetClosestGameObjectWithEntry(m_creature, GO_PEASNT_LIGHT_TRAP, 40.0f))
        {
        pLight->SetRespawnTime(-10);			// not gonna use timer for now
        pLight->Refresh();
		pLight->UpdateVisibilityAndView();
        }

        m_uiPlayerGUID = player_guid;
        m_uiEventResetTimer = MAX_TIME_FOR_BENEDICTION_EVENT;
        m_creature->setFaction(250);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_uiBeHealedTimer = 20000;
        m_uiEventPhase = 10;
        m_uiEventTimer = 10000;
        return m_uiPlayerGUID;
    }

    void CancelBenedictionEvent()
    {
		if (GameObject* pLight = GetClosestGameObjectWithEntry(m_creature, GO_PEASNT_LIGHT_TRAP, 40.0f))
        {
			if (pLight->isSpawned())
			{
				pLight->SetRespawnTime(5);			// despawn the light
			}
		}

        if (!m_lScourgeArchers.empty())
            for(GUIDList::iterator itr = m_lScourgeArchers.begin(); itr != m_lScourgeArchers.end(); ++itr)
                if (Creature* target = m_creature->GetMap()->GetCreature(*itr))
                    target->ForcedDespawn();
        if (!m_lPeasants.empty())
            for(GUIDList::iterator itr = m_lPeasants.begin(); itr != m_lPeasants.end(); ++itr)
                if (Creature* target = m_creature->GetMap()->GetCreature(*itr))
                    target->ForcedDespawn();
        if (!m_lScourgeFootsoldiers.empty())
            for(GUIDList::iterator itr = m_lScourgeFootsoldiers.begin(); itr != m_lScourgeFootsoldiers.end(); ++itr)
                if (Creature* target = m_creature->GetMap()->GetCreature(*itr))
                    target->ForcedDespawn();
		m_uiPlayerGUID.Clear();
        Reset();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID))
        {
            if (m_uiEventResetTimer)
            {
                if (m_uiEventResetTimer <= uiDiff)
                {
                    CancelBenedictionEvent();
                    return;
                }
                else
                    m_uiEventResetTimer -= uiDiff;
            }

            // Blessing Of Nordrassil
            if (m_uiEventPhase < 19)
            {
                if (m_uiBeHealedTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(pPlayer, SPELL_BLESSING_OF_NORDRASSIL) == CAST_OK)
                    {
                        DoScriptText(SAY_YELL_ERIS_BE_HEALED, m_creature);
                        m_uiBeHealedTimer = 20000;
                    }
                }
                else
                    m_uiBeHealedTimer -= uiDiff;
            }

            if (m_uiEventTimer)
            {
                if (m_uiEventTimer <= uiDiff)
                {
                    m_uiEventTimer = 0;
                    switch(++m_uiEventPhase)
                    {
                        case 11:
                            // first wave
                            for(int itr = 0; itr < 11; ++itr)
                                if (itr % 2 == 0)
                                    if (Creature* pCreature = m_creature->SummonCreature(NPC_INJURED_PEASANT, peasants_pos[itr][0], peasants_pos[itr][1], peasants_pos[itr][2], 0.f, TEMPSUMMON_DEAD_DESPAWN, 0))
                                        if (!m_bFirstWaveSay)
                                        {
                                            m_bFirstWaveSay = true;
                                            DoScriptText(SAY_YELL_PEASANT_SPAWN1, pCreature);
                                        }
                            m_uiEventTimer = 3000;
                            break;
                        case 12:
                            for(int itr = 6; itr < 11; ++itr)
                                m_creature->SummonCreature(NPC_INJURED_PEASANT, peasants_pos[itr][0], peasants_pos[itr][1], peasants_pos[itr][2], 0.f, TEMPSUMMON_DEAD_DESPAWN, 0);
                            m_uiEventTimer = 33000;
                            break;
                        case 13:
                            // second wave
                            for(int itr = 0; itr < 11; ++itr)
                                if (itr % 2 == 0)
                                    if (Creature* pCreature = m_creature->SummonCreature(NPC_INJURED_PEASANT, peasants_pos[itr][0], peasants_pos[itr][1], peasants_pos[itr][2], 0.f, TEMPSUMMON_DEAD_DESPAWN, 0))
                                        if (!m_bSecondWaveSay)
                                        {
                                            m_bSecondWaveSay = true;
                                            DoScriptText(SAY_YELL_PEASANT_SPAWN2, pCreature);
                                        }
                            m_uiEventTimer = 3000;
                            break;
                        case 14:
                            for(int itr = 6; itr < 11; ++itr)
                                m_creature->SummonCreature(NPC_INJURED_PEASANT, peasants_pos[itr][0], peasants_pos[itr][1], peasants_pos[itr][2], 0.f, TEMPSUMMON_DEAD_DESPAWN, 0);
                            m_uiEventTimer = 5000;
                            break;
                        case 15:
                            m_creature->SummonCreature(NPC_SCOURGE_FOOTSOLDIER, peasants_pos[0][0], peasants_pos[0][1], peasants_pos[0][2], 0, TEMPSUMMON_DEAD_DESPAWN, 0);
                            m_creature->SummonCreature(NPC_SCOURGE_FOOTSOLDIER, peasants_pos[7][0], peasants_pos[7][1], peasants_pos[7][2], 0, TEMPSUMMON_DEAD_DESPAWN, 0);
                            m_uiEventTimer = 28000;
                            break;
                        case 16:
                            // third wave (last wave)
                            for(int itr = 0; itr < 11; ++itr)
                                if (itr % 2 == 0)
                                    if (Creature* pCreature = m_creature->SummonCreature(NPC_INJURED_PEASANT, peasants_pos[itr][0], peasants_pos[itr][1], peasants_pos[itr][2], 0.f, TEMPSUMMON_DEAD_DESPAWN, 0))
                                        if (!m_bThirdaveSay)
                                        {
                                            m_bThirdaveSay = true;
                                            DoScriptText(SAY_YELL_PEASANT_SPAWN3, pCreature);
                                        }
                            m_uiEventTimer = 3000;
                            break;
                        case 17:
                            for(int itr = 6; itr < 11; ++itr)
                                m_creature->SummonCreature(NPC_INJURED_PEASANT, peasants_pos[itr][0], peasants_pos[itr][1], peasants_pos[itr][2], 0.f, TEMPSUMMON_DEAD_DESPAWN, 0);
                            m_uiEventTimer = 5000;
                        case 18:
                            m_creature->SummonCreature(NPC_SCOURGE_FOOTSOLDIER, peasants_pos[0][0], peasants_pos[0][1], peasants_pos[0][2], 0, TEMPSUMMON_DEAD_DESPAWN, 0);
                            m_creature->SummonCreature(NPC_SCOURGE_FOOTSOLDIER, peasants_pos[7][0], peasants_pos[7][1], peasants_pos[7][2], 0, TEMPSUMMON_DEAD_DESPAWN, 0);
                            m_uiEventTimer = 30000;
                            break;
                        case 19:
                            DoScriptText(SAY_YELL_ERIS_DONE, m_creature);
                            pPlayer->CompleteQuest(QUEST_THE_BALANCE_OF_LIGHT_AND_SHADOW);
                            m_uiEventTimer = 5000;
                            break;
                        case 20:
                            CancelBenedictionEvent();
                            break;

                        case 101:   // quest failed (15+ peasants are dead)
                            pPlayer->FailQuest(QUEST_THE_BALANCE_OF_LIGHT_AND_SHADOW);
                            DoScriptText(SAY_YELL_ERIS_FAILED, m_creature);
                            m_uiEventTimer = 4000;
                            break;
                        case 102:
                            DoScriptText(SAY_ERIS_FAILED, m_creature);
                            m_uiEventTimer = 10000;
                            break;
                        case 103:
                            CancelBenedictionEvent();
                            m_uiEventPhase = 0;
                            m_creature->ForcedDespawn();
                            break;
                    }
                }
                else
                    m_uiEventTimer -= uiDiff;
            }
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI * GetAI_mob_eris_havenfire(Creature* pCreature)
{
    return new mob_eris_havenfireAI(pCreature);
}

bool QuestAccept_mob_eris_havenfire(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_THE_BALANCE_OF_LIGHT_AND_SHADOW)
    {
        mob_eris_havenfireAI* pErisAI = dynamic_cast<mob_eris_havenfireAI*>(pCreature->AI());
        if (!pErisAI)
            return true;

		if (!pErisAI->StartBenedictionEvent(pPlayer->GetObjectGuid()))
        {
            pCreature->MonsterSay("Not now, $N..", LANG_UNIVERSAL, pPlayer);
            return true;
        }

        for(int itr = 0; itr < 9; ++itr)
            pCreature->SummonCreature(NPC_SCOURGE_ARCHER, archers_pos[itr][0], archers_pos[itr][1], archers_pos[itr][2], 0.f, TEMPSUMMON_DEAD_DESPAWN, 0);
    }
    return true;
}*/

enum eErisHavenfire
{
    QUEST_THE_BALANCE_OF_LIGHT_AND_SHADOW   = 7622,

    NPC_SCOURGE_ARCHER                      = 14489,
    NPC_INJURED_PEASANT                     = 14484,
    NPC_PLAGUED_PEASANT                     = 14485,
    NPC_SCOURGE_FOOTSOLDIER                 = 14486,
	NPC_THE_CLEANER							= 14503,            // summoned if the priest gets help
	GO_PEASNT_LIGHT_TRAP					= 179693,

    SPELL_SHOT_VISUAL                       = 7105,
    SPELL_DEATHS_DOOR                       = 23127,
	SPELL_SEETHING_PLAGUE				    = 23072,
    SPELL_BLESSING_OF_NORDRASSIL            = 23108,            // yell: be healed!
	SPELL_ENTER_THE_LIGHT_DND		        = 23107,

    MAX_TIME_FOR_BENEDICTION_EVENT          = 15 * 60 * 1000,   // minutes * minute * ms
	MAX_PEASANTS							= 12,
    MAX_ARCHERS								= 8,

    // says and yells
    SAY_YELL_PEASANT_SPAWN1                 = -1000682,
    SAY_YELL_PEASANT_SPAWN2                 = -1000683,
    SAY_YELL_PEASANT_SPAWN3                 = -1000684,
    SAY_PEASANT_WALK1                       = -1000685,
    SAY_PEASANT_WALK2                       = -1000686,
    SAY_PEASANT_WALK3                       = -1000687,
    SAY_PEASANT_DONE1                       = -1000688,
    SAY_PEASANT_DONE2                       = -1000689,
    SAY_PEASANT_DONE3                       = -1000690,
    SAY_PEASANT_DONE4                       = -1000691,
    SAY_YELL_ERIS_FAILED                    = -1000692,
    SAY_ERIS_FAILED                         = -1000693,
    SAY_YELL_ERIS_DONE                      = -1000694,
    SAY_YELL_ERIS_BE_HEALED                 = -1000695,
};
struct Loc
{
    float x, y, z;
};

float archers_pos[8][4] =
{
    {3327.42f, -3021.11f, 170.57f, 6.01f},
    {3335.4f,  -3054.3f,  173.63f, 0.49f},
    {3351.3f,  -3079.08f, 178.67f, 1.15f},
    {3358.93f, -3076.1f,  174.87f, 1.57f},
    {3371.58f, -3069.24f, 175.20f, 1.99f},
    {3369.46f, -3023.11f, 171.83f, 3.69f},
    {3383.25f, -3057.01f, 181.53f, 2.21f},
    {3380.03f, -3062.73f, 181.90f, 2.31f},
};

float peasants_pos[11][3] =
{
    {3368.17f, -3057.53f, 166.06f},
    {3366.28f, -3058.45f, 165.90f},
    {3364.48f, -3054.75f, 165.36f},
    {3362.51f, -3055.58f, 165.27f},
    {3360.68f, -3056.20f, 165.32f},
    {3359.55f, -3052.88f, 165.38f},
    {3357.58f, -3053.55f, 165.56f},
    {3356.17f, -3056.18f, 166.04f},
    {3364.07f, -3049.74f, 165.23f},
    {3359.53f, -3049.58f, 165.25f},
    {3355.05f, -3050.96f, 165.29f},
};

static Loc aPeasantMoveLoc[]= 
{
	{3344.94f, -3023.39f, 161.18f},
	{3332.89f, -2982.04f, 161.27f},
};

static const int32 aPeasantSpawnYells[3] = {SAY_YELL_PEASANT_SPAWN1, SAY_YELL_PEASANT_SPAWN2, SAY_YELL_PEASANT_SPAWN3};

struct MANGOS_DLL_DECL mob_eris_havenfireAI : public ScriptedAI
{
    mob_eris_havenfireAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiPlayerGUID = ObjectGuid();
        Reset();
    }

    uint32 m_uiEventTimer;
	uint32 m_uiEventResetTimer;
	uint32 m_uiSoldierTimer;
    uint32 m_uiSadEndTimer;
    uint8 m_uiPhase;
    uint8 m_uiCurrentWave;
    uint8 m_uiKillCounter;
    uint8 m_uiSaveCounter;

    ObjectGuid m_uiPlayerGUID;
    GUIDList m_lPeasants;
    GUIDList m_lScourgeArchers;
    GUIDList m_lScourgeFootsoldiers;

    void Reset()
    {
        m_uiEventTimer      = 0;
		m_uiEventResetTimer = 0;
		m_uiSoldierTimer	= 0;
        m_uiSadEndTimer     = 0;
        m_uiPhase           = 0;
        m_uiCurrentWave     = 0;
        m_uiKillCounter     = 0;
        m_uiSaveCounter     = 0;

        m_uiPlayerGUID.Clear();
        m_lPeasants.clear();
        m_lScourgeArchers.clear();
        m_lScourgeFootsoldiers.clear();
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
		m_creature->setFaction(m_creature->GetCreatureInfo()->faction_A);
	}

    void JustSummoned(Creature* pSummoned)
    {
        switch(pSummoned->GetEntry())
        {
        case NPC_INJURED_PEASANT:
            pSummoned->GetMotionMaster()->MovePoint(1, aPeasantMoveLoc[0].x+irand(-3,3), aPeasantMoveLoc[0].y+irand(-3,3), aPeasantMoveLoc[0].z);
			m_lPeasants.push_back(pSummoned->GetObjectGuid());
            break;
        case NPC_SCOURGE_ARCHER:
            m_lScourgeArchers.push_back(pSummoned->GetObjectGuid());
            break;
        case NPC_SCOURGE_FOOTSOLDIER:
            m_lScourgeFootsoldiers.push_back(pSummoned->GetObjectGuid());
            if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID))
                pSummoned->AI()->AttackStart(pPlayer);
            break;
		case NPC_THE_CLEANER:
			if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID))
			{
				pSummoned->AI()->AttackStart(pPlayer);
            }
            break;
        }
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiMotionType, uint32 uiPointId)
    {
        if (uiMotionType != POINT_MOTION_TYPE || !uiPointId)
        {
            return;
        }

		switch (uiPointId)
		{
		case 1:
			pSummoned->GetMotionMaster()->MovePoint(2, aPeasantMoveLoc[1].x+irand(-3,3), aPeasantMoveLoc[1].y+irand(-3,3), aPeasantMoveLoc[1].z);
			break;
		case 2:
            ++m_uiSaveCounter;
            pSummoned->GetMotionMaster()->Clear();

            pSummoned->RemoveAllAuras();
            pSummoned->CastSpell(pSummoned, SPELL_ENTER_THE_LIGHT_DND, false);
            pSummoned->ForcedDespawn(urand(4000, 7000));

			if (rand()%3 == 0)
            {
                switch(urand(1,4))
                {
                    case 1: DoScriptText(SAY_PEASANT_DONE1, pSummoned); break;
                    case 2: DoScriptText(SAY_PEASANT_DONE2, pSummoned); break;
                    case 3: DoScriptText(SAY_PEASANT_DONE3, pSummoned); break;
                    case 4: DoScriptText(SAY_PEASANT_DONE4, pSummoned); break;
                }
            }

            // Event ended
            if (m_uiSaveCounter >= 50 && m_uiCurrentWave == 5)
            {
                DoBalanceEventEnd();
            }
            // Phase ended
            else if (m_uiSaveCounter + m_uiKillCounter == m_uiCurrentWave * MAX_PEASANTS)
            {
                DoHandlePhaseEnd();
            }
			break;
        }
	}

    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_INJURED_PEASANT || pSummoned->GetEntry() == NPC_PLAGUED_PEASANT)
        {
            ++m_uiKillCounter;

            // If more than 15 peasants have died, then fail the quest
            if (m_uiKillCounter == MAX_PEASANTS)
            {
                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID))
                {
                    pPlayer->FailQuest(QUEST_THE_BALANCE_OF_LIGHT_AND_SHADOW);
                }

                DoScriptText(SAY_YELL_ERIS_FAILED, m_creature);
                m_uiSadEndTimer = 4000;
            }
            else if (m_uiSaveCounter + m_uiKillCounter == m_uiCurrentWave * MAX_PEASANTS)
            {
                DoHandlePhaseEnd();
            }
        }
    }

    void DoSummonWave(uint32 uiSummonId = 0)
    {
        float fX, fY, fZ;

        if (!uiSummonId)
        {
            for (uint8 i = 0; i < MAX_PEASANTS; ++i)
            {
                m_creature->GetRandomPoint(peasants_pos[i][0], peasants_pos[i][1], peasants_pos[i][2], 3.0f, fX, fY, fZ);
                if (Creature* pTemp = m_creature->SummonCreature(NPC_INJURED_PEASANT, fX, fY, fZ, 0, TEMPSUMMON_DEAD_DESPAWN, 0))
                {
                    // Only the first mob needs to yell
                    if (!i)
                    {
                        DoScriptText(aPeasantSpawnYells[urand(0, 2)], pTemp);
                    }
                }
            }

            ++m_uiCurrentWave;
        }
        else if (uiSummonId == NPC_SCOURGE_FOOTSOLDIER)
        {
            uint8 uiRand = urand(2, 6);
            for (uint8 i = 0; i < uiRand; ++i)
            {
                m_creature->GetRandomPoint(peasants_pos[i][0], peasants_pos[i][1], peasants_pos[i][2], 10.0f, fX, fY, fZ);
                m_creature->SummonCreature(NPC_SCOURGE_FOOTSOLDIER, fX, fY, fZ, 0, TEMPSUMMON_DEAD_DESPAWN, 0);
            }
        }
        else if (uiSummonId == NPC_SCOURGE_ARCHER)
        {
            for (uint8 i = 0; i < MAX_ARCHERS; ++i)
            {
                m_creature->SummonCreature(NPC_SCOURGE_ARCHER, archers_pos[i][0], archers_pos[i][1], archers_pos[i][2], archers_pos[i][3], TEMPSUMMON_DEAD_DESPAWN, 0);
            }
        }
    }

    void DoHandlePhaseEnd()
    {
        // Send next wave
        if (m_uiCurrentWave < 5)
        {
            DoSummonWave();
        }
    }

	void DoHeal()						// put the heal here since npc stopped using heal on player after each wave
	{
		if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID))
		{
			// Blessing Of Nordrassil
			pPlayer->CastSpell(pPlayer, SPELL_BLESSING_OF_NORDRASSIL, true);			// heal after each wave
			DoScriptText(SAY_YELL_ERIS_BE_HEALED, m_creature);
		}
	}

	ObjectGuid DoStartBalanceEvent(ObjectGuid player_guid)
	{
		if (m_uiPlayerGUID)
            return ObjectGuid();

		m_uiEventResetTimer = MAX_TIME_FOR_BENEDICTION_EVENT;
		m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->setFaction(250);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_uiEventTimer = 3000;
		if (GameObject* pLight = GetClosestGameObjectWithEntry(m_creature, GO_PEASNT_LIGHT_TRAP, 40.0f))
        {
			pLight->SetRespawnTime(600000);			// despawn after 10 min or when event is done
			pLight->Refresh();
			pLight->UpdateVisibilityAndView();
        }
		
        m_uiPlayerGUID = player_guid;
        
        return m_uiPlayerGUID;
	}

    void DoBalanceEventEnd()
    {
        if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID))
        {
            pPlayer->AreaExploredOrEventHappens(QUEST_THE_BALANCE_OF_LIGHT_AND_SHADOW);
        }

        DoScriptText(SAY_YELL_ERIS_DONE, m_creature);
        DoDespawnSummons(true);
        Reset();
    }

    void DoDespawnSummons(bool bIsEventEnd = false)
    {
		if (!m_lScourgeArchers.empty())
            for(GUIDList::iterator itr = m_lScourgeArchers.begin(); itr != m_lScourgeArchers.end(); ++itr)
                if (Creature* target = m_creature->GetMap()->GetCreature(*itr))
                    target->ForcedDespawn();
        if (!m_lPeasants.empty())
            for(GUIDList::iterator itr = m_lPeasants.begin(); itr != m_lPeasants.end(); ++itr)
                if (Creature* target = m_creature->GetMap()->GetCreature(*itr))
                    target->ForcedDespawn();
        if (!m_lScourgeFootsoldiers.empty())
            for(GUIDList::iterator itr = m_lScourgeFootsoldiers.begin(); itr != m_lScourgeFootsoldiers.end(); ++itr)
                if (Creature* target = m_creature->GetMap()->GetCreature(*itr))
                    target->ForcedDespawn();
		m_uiPlayerGUID.Clear();

		if (GameObject* pLight = GetClosestGameObjectWithEntry(m_creature, GO_PEASNT_LIGHT_TRAP, 40.0f))
        {
			if (pLight->isSpawned())
			{
				pLight->Respawn();			// despawn the light, needs rework
				pLight->UpdateVisibilityAndView();
			}
		}
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID))
        {
            if (m_uiEventResetTimer)
            {
                if (m_uiEventResetTimer <= uiDiff)
                {
                    m_uiSadEndTimer = 1000;
                    return;
                }
                else
                    m_uiEventResetTimer -= uiDiff;
            }

        if (m_uiEventTimer)
        {
            if (m_uiEventTimer <= uiDiff)
            {
                switch (m_uiPhase)
                {
                    case 0:
                        DoSummonWave(NPC_SCOURGE_ARCHER);
                        m_uiEventTimer = 8000;
						m_uiSoldierTimer = urand(40000, 75000);
                        break;
                    case 1:
					case 2:
					case 3:
					case 4:
					case 5:
                        DoSummonWave();				
						if (m_uiPhase >= 2)
							DoHeal();
                        m_uiEventTimer = 65000;
                        break;
                }
                ++m_uiPhase;
            }
            else
            {
                m_uiEventTimer -= uiDiff;
            }
        }

        // Handle event end in case of fail
        if (m_uiSadEndTimer)
        {
            if (m_uiSadEndTimer <= uiDiff)
            {
                DoScriptText(SAY_ERIS_FAILED, m_creature);
                m_creature->ForcedDespawn(5000);
                DoDespawnSummons();
                m_uiSadEndTimer = 0;
            }
            else
            {
                m_uiSadEndTimer -= uiDiff;
            }
        }

		if (m_uiSoldierTimer)
            {
                if (m_uiSoldierTimer <= uiDiff)
                {
					DoSummonWave(NPC_SCOURGE_FOOTSOLDIER);
                    m_uiSoldierTimer = urand(35000, 65000);
                }
                else
                    m_uiSoldierTimer -= uiDiff;
            }
    }
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI * GetAI_mob_eris_havenfire(Creature* pCreature)
{
    return new mob_eris_havenfireAI(pCreature);
}

bool QuestAccept_mob_eris_havenfire(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_THE_BALANCE_OF_LIGHT_AND_SHADOW)
    {
        mob_eris_havenfireAI* pErisAI = dynamic_cast<mob_eris_havenfireAI*>(pCreature->AI());
        if (!pErisAI)
            return true;

		if (!pErisAI->DoStartBalanceEvent(pPlayer->GetGUID()))
        {
            pCreature->MonsterSay("Not now, $N..", LANG_UNIVERSAL, pPlayer);
            return true;
        }
	}
    return true;
}


/*######
## mob_scourge_archer
######*/

struct MANGOS_DLL_DECL mob_scourge_archerAI : public ScriptedAI
{
    mob_scourge_archerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        Reset();
    }

    uint32 m_uiShotTimer;
    bool m_bSkippedFirstShot;

    void Reset()
    {
        m_uiShotTimer = 0;
        m_bSkippedFirstShot = false;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!pWho)
            return;

        switch(pWho->GetEntry())
        {
        case NPC_INJURED_PEASANT:
        case NPC_PLAGUED_PEASANT:
            if (!m_bSkippedFirstShot)
            {
                m_bSkippedFirstShot = true;
                m_uiShotTimer = urand(3000, 7000);
                break;
            }
            if (!m_uiShotTimer && pWho->IsWithinDist(m_creature, 35.0f))
            {
                m_creature->SetFacingToObject(pWho);
                if (DoCastSpellIfCan(pWho, SPELL_SHOT_VISUAL) == CAST_OK)
                    m_uiShotTimer = urand(2000, 8000);
            }
            break;
        }
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (pDoneBy->IsCharmerOrOwnerPlayerOrPlayerItself())
            if (!((Player*)pDoneBy)->isGameMaster())
                uiDamage = 0;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiShotTimer)
        {
            if (m_uiShotTimer <= uiDiff)
                m_uiShotTimer = 0;
            else
                m_uiShotTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        ResetToHome();

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_scourge_archer(Creature* pCreature)
{
    return new mob_scourge_archerAI(pCreature);
}

/*######
## npc_injured_peasant
######*/

struct MANGOS_DLL_DECL npc_injured_peasantAI : public npc_escortAI
{
    npc_injured_peasantAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        pCreature->SetSpeedRate(MOVE_WALK, frand(0.5f, 0.6f));
		m_bDoneCleaner = false;
		m_uiThreatListSize = 0;
        Reset();
        Start();
    }

    uint32 m_uiWalkRandomSay;
	uint32 m_uiSummonCount;
	uint32 m_uiThreatListSize;

		bool m_bDoneCleaner;

    void Reset()
    {
        m_uiWalkRandomSay = urand(5000,15000);
    }

    void WaypointReached(uint32 uiPointId)	
	{}

    void MoveInLineOfSight(Unit *){/*ignore aggro*/}

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pCaster->GetEntry() == NPC_SCOURGE_ARCHER && pSpell->Id == SPELL_SHOT_VISUAL)
        {
            uint32 m_uiDamageAmout = urand(150,350);
            if (m_uiDamageAmout < m_creature->GetHealth())
            {
				if (rand()%4 == 0 && !m_creature->HasAura(SPELL_DEATHS_DOOR) && !m_creature->HasAura(SPELL_SEETHING_PLAGUE))				// not sure why but they stop when update entry
				{
					// Not nice way, however using UpdateEntry will not be correct.
					if (const CreatureInfo* pTemp = GetCreatureTemplateStore(NPC_PLAGUED_PEASANT))
					{
						m_creature->SetEntry(pTemp->Entry);
						m_creature->SetDisplayId(Creature::ChooseDisplayId(pTemp));
						m_creature->SetName(pTemp->Name);
					}
					//m_creature->UpdateEntry(NPC_PLAGUED_PEASANT);
					int r;
					r = urand(0,1);
					switch (r)
					{
						case 0:
							DoCastSpellIfCan(m_creature, SPELL_DEATHS_DOOR, CAST_AURA_NOT_PRESENT);
							break;
						case 1:
							DoCastSpellIfCan(m_creature, SPELL_SEETHING_PLAGUE, CAST_AURA_NOT_PRESENT);
							break;
					}
                }
            }
            m_creature->DealDamage(m_creature, m_uiDamageAmout, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
    }

	void SummonTheCleaner()
    {
		if (m_uiSummonCount < 1)	// max 1 Cleaner
        {
			float fX, fY, fZ;
			m_creature->GetPosition(fX, fY, fZ);
			for(uint8 i = 0; i < 1; ++i)
				if (Creature* pCleaner = m_creature->SummonCreature(NPC_THE_CLEANER, fX, fY, fZ, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1000))
				{ 
					pCleaner->SetRespawnDelay(-10);				// to stop him from randomly respawning
				}
		}
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
		/*if (m_creature->getThreatManager().getThreatList().size() > 1)			// not working
        {
			if (!m_bDoneCleaner)
				SummonTheCleaner();
        }*/

		// random say during escort
        if (m_uiWalkRandomSay)
        {
            if (m_uiWalkRandomSay <= uiDiff)			// they never do this
            {
                if (rand()%4 == 0)
				{
                    switch(urand(1,3))
                    {
                        case 1: DoScriptText(SAY_PEASANT_WALK1, m_creature); break;
                        case 2: DoScriptText(SAY_PEASANT_WALK2, m_creature); break;
                        case 3: DoScriptText(SAY_PEASANT_WALK3, m_creature); break;
                    }
                }
				m_uiWalkRandomSay = 0;
            }
            else
                m_uiWalkRandomSay -= uiDiff;
        }         
    }
};

CreatureAI* GetAI_npc_injured_peasant(Creature* pCreature)
{
    return new npc_injured_peasantAI(pCreature);
}

/*######
## npc_scourge_building
######*/

struct MANGOS_DLL_DECL mob_scourge_buildingAI : public ScriptedAI
{
    mob_scourge_buildingAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
		Reset();
    }

	uint32 explosion_timer;
	uint32 update_timer;
	GameObject* Mark;
	Unit* pPlayer;

	void Reset()
	{
		Mark = GetClosestGameObjectWithEntry(m_creature,177668,10.0f);
		pPlayer = 0;
		explosion_timer = 6000;
		update_timer = 500;
	}

    void UpdateAI(const uint32 uiDiff)
    {
		if (explosion_timer < 6000)
		{
			if (explosion_timer <= uiDiff)
			{
				explosion_timer = 6000;
				GameObject* trap = GetClosestGameObjectWithEntry(m_creature,177672,10.0f);
				if (trap && trap->isSpawned())
					trap->SetLootState(GO_JUST_DEACTIVATED);
				if (pPlayer)
					pPlayer->DealDamage(m_creature,10000,NULL,DIRECT_DAMAGE,SPELL_SCHOOL_MASK_NATURE,NULL,false);
			}
			else
				explosion_timer -= uiDiff;
		}
		else
		{
			if (update_timer <= uiDiff)
			{
				update_timer = 500;
				if (!Mark)
					Mark = GetClosestGameObjectWithEntry(m_creature,177668,10.0f);
				else if (Mark->GetOwner())
				{
					pPlayer = Mark->GetOwner();
					Mark->SetOwnerGuid(ObjectGuid());
					Mark->SetLootState(GO_JUST_DEACTIVATED);
					explosion_timer = 3500;
				}
			}
			else
				update_timer -= uiDiff;
		}
    }
};

CreatureAI* GetAI_mob_scourge_building(Creature* pCreature)
{
    return new mob_scourge_buildingAI(pCreature);
}

void AddSC_eastern_plaguelands()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "mobs_ghoul_flayer";
    pNewscript->GetAI = &GetAI_mobs_ghoul_flayer;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_argent_medic";
    pNewscript->pGossipHello = &GossipHello_npc_argent_medic;
    pNewscript->pGossipSelect = &GossipSelect_npc_argent_medic;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_augustus_the_touched";
    pNewscript->pGossipHello = &GossipHello_npc_augustus_the_touched;
    pNewscript->pGossipSelect = &GossipSelect_npc_augustus_the_touched;
    pNewscript->RegisterSelf();
    
    pNewscript = new Script;
    pNewscript->Name = "npc_betina_bigglezink";
    pNewscript->pGossipHello = &GossipHello_npc_betina_bigglezink;
    pNewscript->pGossipSelect = &GossipSelect_npc_betina_bigglezink;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_darrowshire_spirit";
    pNewscript->pGossipHello = &GossipHello_npc_darrowshire_spirit;
    pNewscript->pGossipSelect = &GossipSelect_npc_darrowshire_spirit;
    pNewscript->GetAI = &GetAI_npc_darrowshire_spirit;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_tirion_fordring";
    pNewscript->GetAI = &GetAI_npc_tirion_fordring;
    pNewscript->pGossipHello = &GossipHello_npc_tirion_fordring;
    pNewscript->pGossipSelect = &GossipSelect_npc_tirion_fordring;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_william_kielar";
    pNewscript->pGossipHello = &GossipHello_npc_william_kielar;
    pNewscript->pGossipSelect = &GossipSelect_npc_william_kielar;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_eris_havenfire";
    pNewscript->GetAI = &GetAI_mob_eris_havenfire;
    pNewscript->pQuestAcceptNPC = &QuestAccept_mob_eris_havenfire;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_scourge_archer";
    pNewscript->GetAI = &GetAI_mob_scourge_archer;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_injured_peasant";
    pNewscript->GetAI = &GetAI_npc_injured_peasant;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_scourge_building";
    pNewscript->GetAI = &GetAI_mob_scourge_building;
    pNewscript->RegisterSelf();
}
