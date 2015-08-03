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
SDName: Felwood
SD%Complete: 95
SDComment: Quest support: related to 4101/4102 (To obtain Cenarion Beacon), 4506, 7603 (Summon Pollo Grande)
SDCategory: Felwood
EndScriptData */

/* ContentData
npc_vartrus_the_ancient
at_irontree_woods
go_felwood_corrupted_flowers
go_cleansed_felwood_flowers
npc_kitten
npc_kroshius
npc_niby_the_almighty
npcs_riverbreeze_and_silversky
EndContentData */

#include "precompiled.h"
#include "follower_ai.h"
#include "escort_ai.h"
#include "ObjectMgr.h"

/*######
## npc_vartrus_the_ancient
######*/

struct MANGOS_DLL_DECL npc_vartrus_the_ancientAI : public ScriptedAI
{
    npc_vartrus_the_ancientAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_creature->SetVisibility(VISIBILITY_OFF);
        m_uiHideTimer = 0;
        Reset();
    }

    uint32 m_uiHideTimer;

    void Reset() {}

    void MakeVartrusVisible()
    {
        debug_log("SD0: Felwood: NPC Vartrus the Ancient (entry 14524) is now visible for 2 minutes.");
        m_creature->SetVisibility(VISIBILITY_ON);
        m_uiHideTimer = 2*MINUTE*IN_MILLISECONDS;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiHideTimer)
        {
            if (m_uiHideTimer <= uiDiff)
            {
                m_uiHideTimer = 0;
                m_creature->SetVisibility(VISIBILITY_OFF);
            }
            else
                m_uiHideTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_vartrus_the_ancient(Creature* pCreature)
{
    return new npc_vartrus_the_ancientAI(pCreature);
}

/*######
## at_irontree_woods
######*/

enum eAtIrontreeWoods
{
    QUEST_THE_ANCIENT_LEAF      = 7632,
    ITEM_ANCIENT_PETRIFIED_LEAF = 18703,
    NPC_VARTRUS_THE_ANCIENT     = 14524,
};

bool AreaTrigger_at_irontree_woods(Player* pPlayer, const AreaTriggerEntry* /*atEntry*/)
{
    Creature* pVartrus = NULL;

    if (pPlayer->GetQuestStatus(QUEST_THE_ANCIENT_LEAF) == QUEST_STATUS_COMPLETE)
        if ((pVartrus = GetClosestCreatureWithEntry(pPlayer, NPC_VARTRUS_THE_ANCIENT, 30.0f)))
            if (npc_vartrus_the_ancientAI* pVartrusAI = dynamic_cast<npc_vartrus_the_ancientAI*>(pVartrus->AI()))
                pVartrusAI->MakeVartrusVisible();

    return false;
}

/*####
# go_felwood_corrupted_flowers
####*/

enum eFelwoodCorruptedAndCleansedFlowers
{
    GO_CORRUPTED_NIGHT_DRAGON         = 164885,
    GO_CORRUPTED_SONGFLOWER           = 164886,
    GO_CORRUPTED_WINDBLOSSOM          = 164887,
    GO_CORRUPTED_WHIPPER_ROOT         = 164888,

    GO_CLEANSED_NIGHT_DRAGON          = 164881,
    GO_CLEANSED_SONGFLOWER            = 164882,
    GO_CLEANSED_WHIPPER_ROOT          = 164883,
    GO_CLEANSED_WINDBLOSSOM           = 164884,

    SPELL_CREATE_WINDBLOSSOM_BERRIES  = 15342,
    SPELL_CREATE_WHIPPER_ROOT_TUBERS  = 15343,
    SPELL_CREATE_NIGHT_DRAGONS_BREATH = 15344,
    SPELL_SONGFLOWER_SERENADE         = 15366,
};

bool QuestRewarded_go_felwood_corrupted_flowers(Player* /*pPlayer*/, GameObject* pGo, const Quest* pQuest)
{
    if (pGo->GetGoType() != GAMEOBJECT_TYPE_QUESTGIVER)
        return false;

    // We want only corrupted flowers in felwood
    if (strcmp(pQuest->GetTitle().data(), "Corrupted Songflower") == 0 ||
        strcmp(pQuest->GetTitle().data(), "Corrupted Night Dragon") == 0 ||
        strcmp(pQuest->GetTitle().data(), "Corrupted Whipper Root") == 0 ||
        strcmp(pQuest->GetTitle().data(), "Corrupted Windblossom") == 0)
    {
        debug_log("SD0: Felwood: despawning gameobject %s (entry %u) for 175 seconds", pQuest->GetTitle().data(), pGo->GetEntry());
		pGo->SetLootState(GO_JUST_DEACTIVATED);
		pGo->SetRespawnTime(175);
		pGo->SaveRespawnTime();
		pGo->Refresh();
    }
    return false;
}

/*######
## go_cleansed_felwood_flowers
######*/

bool GOUse_go_cleansed_felwood_flowers(Player* pPlayer, GameObject* pGo)
{
    if (pGo->GetGoType() != GAMEOBJECT_TYPE_GOOBER)
        return true;

	
    //switch(pGo->GetEntry())
    //{
    //    case GO_CLEANSED_NIGHT_DRAGON:
    //        pPlayer->CastSpell(pPlayer, SPELL_CREATE_NIGHT_DRAGONS_BREATH, true);
    //        break;
    //    case GO_CLEANSED_SONGFLOWER:
    //        pPlayer->CastSpell(pPlayer, SPELL_SONGFLOWER_SERENADE, true);
    //        break;
    //    case GO_CLEANSED_WHIPPER_ROOT:
    //        pPlayer->CastSpell(pPlayer, SPELL_CREATE_WHIPPER_ROOT_TUBERS, true);
    //        break;
    //    case GO_CLEANSED_WINDBLOSSOM:
    //        pPlayer->CastSpell(pPlayer, SPELL_CREATE_WINDBLOSSOM_BERRIES, true);
    //        break;
    //}

	//if (strcmp(pGo->GetName(), "Cleansed Night Dragon") == 0)
	//	pPlayer->CastSpell(pPlayer, SPELL_CREATE_NIGHT_DRAGONS_BREATH, true);
	//else if (strcmp(pGo->GetName(), "Cleansed Songflower") == 0)
	//	pPlayer->CastSpell(pPlayer, SPELL_SONGFLOWER_SERENADE, true);
	//else if (strcmp(pGo->GetName(), "Cleansed Whipper Root") == 0)
	//	pPlayer->CastSpell(pPlayer, SPELL_CREATE_WHIPPER_ROOT_TUBERS, true);	
	//else if (strcmp(pGo->GetName(), "Cleansed Windblossom") == 0)
	//	pPlayer->CastSpell(pPlayer, SPELL_CREATE_WINDBLOSSOM_BERRIES, true);

	switch (pGo->GetEntry())
	{
		case 164881:
		case 174609:
		case 174685:
		case 173325:
			pPlayer->CastSpell(pPlayer, SPELL_CREATE_NIGHT_DRAGONS_BREATH, true);
			break;
		case 174613:
		case 174614:
		case 174615:
		case 174715:
		case 174714:
		case 174612:
		case 174610:
		case 164882:
		case 171943:
		case 171940:
			pPlayer->CastSpell(pPlayer, SPELL_SONGFLOWER_SERENADE, true);
			break;
		case 174623:
		case 174624:
		case 174625:
		case 164883:
		case 174687:
		case 174622:
			pPlayer->CastSpell(pPlayer, SPELL_CREATE_WHIPPER_ROOT_TUBERS, true);	
			break;
		case 174621:
		case 173326:
		case 174711:
		case 174710:
		case 174616:
		case 164884:
		case 174618:
		case 174619:
		case 174620:
		case 174617:
			pPlayer->CastSpell(pPlayer, SPELL_CREATE_WINDBLOSSOM_BERRIES, true);
			break;
	}

    return false;
}

/*####
# npc_kitten
####*/

enum
{
    EMOTE_SAB_JUMP              = -1000541,
    EMOTE_SAB_FOLLOW            = -1000542,

    SPELL_CORRUPT_SABER_VISUAL  = 16510,

    QUEST_CORRUPT_SABER         = 4506,
    NPC_WINNA                   = 9996,
    NPC_CORRUPT_SABER           = 10042
};

#define GOSSIP_ITEM_RELEASE     "I want to release the corrupted saber to Winna."

struct MANGOS_DLL_DECL npc_kittenAI : public FollowerAI
{
    npc_kittenAI(Creature* pCreature) : FollowerAI(pCreature)
    {
        if (pCreature->GetOwner() && pCreature->GetOwner()->GetTypeId() == TYPEID_PLAYER)
        {
            StartFollow((Player*)pCreature->GetOwner());
            SetFollowPaused(true);
            DoScriptText(EMOTE_SAB_JUMP, m_creature);

            pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            //find a decent way to move to center of moonwell
        }

        m_uiMoonwellCooldown = 7500;
        Reset();
    }

    uint32 m_uiMoonwellCooldown;

    void Reset() { }

    void MoveInLineOfSight(Unit* pWho)
    {
        //should not have npcflag by default, so set when expected
        if (!m_creature->getVictim() && !m_creature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP) && HasFollowState(STATE_FOLLOW_INPROGRESS) && pWho->GetEntry() == NPC_WINNA)
        {
            if (m_creature->IsWithinDistInMap(pWho, INTERACTION_DISTANCE))
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }
    }

    void UpdateFollowerAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (HasFollowState(STATE_FOLLOW_PAUSED))
            {
                if (m_uiMoonwellCooldown < uiDiff)
                {
                    m_creature->CastSpell(m_creature, SPELL_CORRUPT_SABER_VISUAL, false);
                    SetFollowPaused(false);
                }
                else
                    m_uiMoonwellCooldown -= uiDiff;
            }

            return;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_kitten(Creature* pCreature)
{
    return new npc_kittenAI(pCreature);
}

bool EffectDummyCreature_npc_kitten(Unit* /*pCaster*/, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget)
{
    //always check spellid and effectindex
    if (uiSpellId == SPELL_CORRUPT_SABER_VISUAL && uiEffIndex == EFFECT_INDEX_0)
    {
        // Not nice way, however using UpdateEntry will not be correct.
        if (const CreatureInfo* pTemp = GetCreatureTemplateStore(NPC_CORRUPT_SABER))
        {
            pCreatureTarget->SetEntry(pTemp->Entry);
            pCreatureTarget->SetDisplayId(Creature::ChooseDisplayId(pTemp));
            pCreatureTarget->SetName(pTemp->Name);
            pCreatureTarget->SetFloatValue(OBJECT_FIELD_SCALE_X, pTemp->scale);
        }

        if (Unit* pOwner = pCreatureTarget->GetOwner())
            DoScriptText(EMOTE_SAB_FOLLOW, pCreatureTarget, pOwner);

        //always return true when we are handling this spell and effect
        return true;
    }
    return false;
}

bool GossipHello_npc_corrupt_saber(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_CORRUPT_SABER) == QUEST_STATUS_INCOMPLETE)
    {
        if (GetClosestCreatureWithEntry(pCreature, NPC_WINNA, INTERACTION_DISTANCE))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_RELEASE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    }

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_corrupt_saber(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        if (npc_kittenAI* pKittenAI = dynamic_cast<npc_kittenAI*>(pCreature->AI()))
            pKittenAI->SetFollowComplete();

        pPlayer->AreaExploredOrEventHappens(QUEST_CORRUPT_SABER);
    }

    return true;
}

/*######
## npc_kroshius
######*/

enum
{
    NPC_KROSHIUS            = 14467,
    SPELL_KNOCKBACK         = 10101,
    SAY_KROSHIUS_REVIVE     = -1000167,
    EVENT_KROSHIUS_REVIVE   = 8328,
    FACTION_HOSTILE         = 16,
};

struct MANGOS_DLL_DECL npc_kroshiusAI : public ScriptedAI
{
    npc_kroshiusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    ObjectGuid m_uiPlayerGUID;

    uint32 m_uiKnockBackTimer;
    uint32 m_uiPhaseTimer;

    uint8 m_uiPhase;

    void Reset()
    {
        m_uiKnockBackTimer = urand(5000, 8000);
        m_uiPhase = 0;
		m_uiPlayerGUID.Clear();

        if (!m_uiPhase)
        {
            m_creature->setFaction(m_creature->GetCreatureInfo()->faction_A);
            // TODO: Workaround? till better solution
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
            m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
        }
    }

    void DoRevive(Player* pSource)
    {
        if (m_uiPhase)
            return;

        m_uiPhase = 1;
        m_uiPhaseTimer = 2500;
		m_uiPlayerGUID = pSource->GetObjectGuid();

        // TODO: A visual Flame Circle around the mob still missing
    }

    void JustDied(Unit* /*pKiller*/)
    {
        m_uiPhase = 0;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_uiPhase)
            return;

        if (m_uiPhase < 4)
        {
            if (m_uiPhaseTimer <= uiDiff)
            {
                switch (m_uiPhase)
                {
                    case 1:                                         // Revived
                        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                        m_uiPhaseTimer = 1000;
                        break;
                    case 2:
                        DoScriptText(SAY_KROSHIUS_REVIVE, m_creature);
                        m_uiPhaseTimer = 3500;
                        break;
                    case 3:                                         // Attack
                        m_creature->setFaction(FACTION_HOSTILE);
                        // TODO workaround will better idea
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                        if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID))
                        {
                            if (m_creature->IsWithinDistInMap(pPlayer, 30.0f))
                                AttackStart(pPlayer);
                        }
                        break;
                }
                ++m_uiPhase;
            }
            else
                m_uiPhaseTimer -= uiDiff;
        }
        else
        {
            if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                return;

            if (m_uiKnockBackTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_KNOCKBACK);
                m_uiKnockBackTimer = urand(9000, 12000);
            }
            else
                m_uiKnockBackTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }
    }
};

CreatureAI* GetAI_npc_kroshius(Creature* pCreature)
{
    return new npc_kroshiusAI(pCreature);
}

bool ProcessEventId_npc_kroshius(uint32 uiEventId, Object* pSource, Object* /*pTarget*/, bool /*bIsStart*/)
{
    if (uiEventId == EVENT_KROSHIUS_REVIVE)
    {
        if (pSource->GetTypeId() == TYPEID_PLAYER)
        {
            if (Creature* pKroshius = GetClosestCreatureWithEntry((Player*)pSource, NPC_KROSHIUS, 20.0f))
            {
                if (npc_kroshiusAI* pKroshiusAI = dynamic_cast<npc_kroshiusAI*>(pKroshius->AI()))
                    pKroshiusAI->DoRevive((Player*)pSource);
            }
        }
        return true;
    }
    return false;
}

/*######
## npc_niby_the_almighty (summons el pollo grande)
######*/

enum
{
    QUEST_KROSHIUS     = 7603,

    NPC_IMPSY          = 14470,

    SPELL_SUMMON_POLLO = 23056,

    SAY_NIBY_1         = -1000280,
    SAY_NIBY_2         = -1000281,
    EMOTE_IMPSY_1      = -1000282,
    SAY_IMPSY_1        = -1000283,
    SAY_NIBY_3         = -1000284
};

struct MANGOS_DLL_DECL npc_niby_the_almightyAI : public ScriptedAI
{
    npc_niby_the_almightyAI(Creature* pCreature) : ScriptedAI(pCreature){ Reset(); }

    uint32 m_uiSummonTimer;
    uint8  m_uiSpeech;

    bool m_bEventStarted;

    void Reset()
    {
        m_uiSummonTimer = 500;
        m_uiSpeech = 0;

        m_bEventStarted = false;
    }

    void StartEvent()
    {
        Reset();
        m_bEventStarted = true;
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bEventStarted)
        {
            if (m_uiSummonTimer <= uiDiff)
            {
                switch (m_uiSpeech)
                {
                    case 1:
                        m_creature->GetMotionMaster()->Clear();
                        m_creature->GetMotionMaster()->MovePoint(0, 5407.19f, -753.00f, 350.82f);
                        m_uiSummonTimer = 6200;
                        break;
                    case 2:
                        m_creature->SetFacingTo(1.2f);
                        DoScriptText(SAY_NIBY_1, m_creature);
                        m_uiSummonTimer = 3000;
                        break;
                    case 3:
                        DoScriptText(SAY_NIBY_2, m_creature);
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_POLLO);
                        m_uiSummonTimer = 2000;
                        break;
                    case 4:
                        if (Creature* pImpsy = GetClosestCreatureWithEntry(m_creature, NPC_IMPSY, 20.0))
                        {
                            DoScriptText(EMOTE_IMPSY_1, pImpsy);
                            DoScriptText(SAY_IMPSY_1, pImpsy);
                            m_uiSummonTimer = 2500;
                        }
                        else
                        {
                            //Skip Speech 5
                            m_uiSummonTimer = 40000;
                            ++m_uiSpeech;
                        }
                        break;
                    case 5:
                        DoScriptText(SAY_NIBY_3, m_creature);
                        m_uiSummonTimer = 40000;
                        break;
                    case 6:
                        m_creature->GetMotionMaster()->MoveTargetedHome();
                        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                        m_bEventStarted = false;
                        break;
                }
                ++m_uiSpeech;
            }
            else
                m_uiSummonTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_niby_the_almighty(Creature* pCreature)
{
    return new npc_niby_the_almightyAI(pCreature);
}

bool QuestRewarded_npc_niby_the_almighty(Player* /*pPlayer*/, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_KROSHIUS)
    {
        if (npc_niby_the_almightyAI* pNibyAI = dynamic_cast<npc_niby_the_almightyAI*>(pCreature->AI()))
        {
            pNibyAI->StartEvent();
        }
    }
    return true;
}

/*######
## npcs_riverbreeze_and_silversky
######*/

enum
{
    QUEST_CLEANSING_FELWOOD_A = 4101,
    QUEST_CLEANSING_FELWOOD_H = 4102,

    NPC_ARATHANDIS_SILVERSKY  = 9528,
    NPC_MAYBESS_RIVERBREEZE   = 9529,

    SPELL_CENARION_BEACON     = 15120
};

#define GOSSIP_ITEM_BEACON  "Please make me a Cenarion Beacon"

bool GossipHello_npcs_riverbreeze_and_silversky(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    switch (pCreature->GetEntry())
    {
        case NPC_ARATHANDIS_SILVERSKY:
            if (pPlayer->GetQuestRewardStatus(QUEST_CLEANSING_FELWOOD_A))
            {
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BEACON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                pPlayer->SEND_GOSSIP_MENU(2848, pCreature->GetObjectGuid());
            }
            else if (pPlayer->GetTeam() == HORDE)
                pPlayer->SEND_GOSSIP_MENU(2845, pCreature->GetObjectGuid());
            else
                pPlayer->SEND_GOSSIP_MENU(2844, pCreature->GetObjectGuid());
            break;
        case NPC_MAYBESS_RIVERBREEZE:
            if (pPlayer->GetQuestRewardStatus(QUEST_CLEANSING_FELWOOD_H))
            {
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BEACON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                pPlayer->SEND_GOSSIP_MENU(2849, pCreature->GetObjectGuid());
            }
            else if (pPlayer->GetTeam() == ALLIANCE)
                pPlayer->SEND_GOSSIP_MENU(2843, pCreature->GetObjectGuid());
            else
                pPlayer->SEND_GOSSIP_MENU(2842, pCreature->GetObjectGuid());
            break;
    }

    return true;
}

bool GossipSelect_npcs_riverbreeze_and_silversky(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->CastSpell(pPlayer, SPELL_CENARION_BEACON, false);
    }
    return true;
}

/*####
# npc_ooze_multi
####*/

enum eOoze
{
	NPC_GLUTINUS_OOZE		= 6559,
	NPC_PRIMAL_OOZE			= 6557,
	NPC_FOREST_OOZE			= 8766,
	NPC_JADE_OOZE			= 2656,
	NPC_CURSED_OOZE			= 7086,
	NPC_TAINTED_OOZE		= 7092,

	SPELL_ACID_SLIME		= 14147,
	SPELL_CLONE				= 14146,
	SPELL_DISEASE_SLIME		= 6907,

	SPELL_DARK_SLUDGE		= 3335,
};

struct MANGOS_DLL_DECL npc_ooze_multiAI : public ScriptedAI
{
    npc_ooze_multiAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{ 
		Reset(); 
	}

	uint32 m_uiSpell1Entry;
    uint32 m_uiSpell2Entry;

    uint32 m_uiSpell1Timer;
    uint32 m_uiSpell2Timer;

	bool m_bCloned;

    void Reset()
	{
		m_bCloned = false;
		m_uiSpell1Timer = urand(4000,12000);
		m_uiSpell1Timer = urand(12000,22000);

		switch(m_creature->GetEntry())
        {
			case NPC_GLUTINUS_OOZE:
				m_uiSpell1Entry = 0;
                m_uiSpell2Entry = 0;
				break;
            case NPC_PRIMAL_OOZE:
                m_uiSpell1Entry = 0;
				m_uiSpell2Entry = SPELL_CLONE;
                break;
			case NPC_FOREST_OOZE:
                m_uiSpell1Entry = SPELL_DISEASE_SLIME;
                m_uiSpell2Entry = 0;
                break;
			case NPC_JADE_OOZE:
                m_uiSpell1Entry = SPELL_DISEASE_SLIME;
                m_uiSpell2Entry = 0;
                break;
			case NPC_CURSED_OOZE:
                m_uiSpell1Entry = 0;
                m_uiSpell2Entry = 0;
                break;
			case NPC_TAINTED_OOZE:
                m_uiSpell1Entry = SPELL_DARK_SLUDGE;
                m_uiSpell2Entry = 0;
                break;
		}
	}

	void JustDied(Unit* /*pKiller*/)
    {
		if(m_creature->GetEntry() == NPC_GLUTINUS_OOZE)
			m_creature->CastSpell(m_creature, SPELL_ACID_SLIME, true);
    }

	void SpellHit(Unit* pCaster, SpellEntry const* pSpell)
    {
        if (pSpell->Id == 15698 || pSpell->Id == 15699 || pSpell->Id == 15702)			// no need to separate the two kinds of Oozes since the spell has target req in DB
			m_creature->ForcedDespawn();
	}

	void Execute(uint32 uiSpellEntry)
    {
        Unit* pTarget = NULL;
        switch(uiSpellEntry)
        {
            case SPELL_ACID_SLIME:
			case SPELL_CLONE:
			case SPELL_DARK_SLUDGE:
                pTarget = m_creature;
                break;
			case SPELL_DISEASE_SLIME:
                pTarget = m_creature->getVictim();
                break;
        }
        if (pTarget && uiSpellEntry != 0)
            DoCastSpellIfCan(pTarget, uiSpellEntry, CAST_AURA_NOT_PRESENT);
    }

	void UpdateAI(const uint32 uiDiff)
    {
		if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (m_uiSpell1Timer < uiDiff)
        {
            Execute(m_uiSpell1Entry);
            m_uiSpell1Timer = urand(15000,30000);
        }
        else
            m_uiSpell1Timer -= uiDiff;

		if(HealthBelowPct(20) && !m_bCloned)
		{
			Execute(m_uiSpell2Entry);
			m_bCloned = true;
		}

        DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_ooze_multi(Creature* pCreature)
{
    return new npc_ooze_multiAI(pCreature);
}

enum anaorkin
{
    QUEST_RESCUE_FROM_JAEDENAR     = 5203,
    MOB_LEGIONAIRE                 = 9862,
    MOB_SPIRIT_OF_TREY             = 11141,
    SPELL_STRENGHT_OF_ARKO         = 18163,
};

struct DemonLoc
{
    float m_fX, m_fY, m_fZ;
};

static const DemonLoc aDemonSpawns[7] =
{
    {5085.63f, -508.54f, 296.86f},                       // Shade spawn location 1
    {5093.48f, -503.63f, 296.67f},                       // Shade spawn location 2
    {5101.33f, -500.30f, 296.67f},                    // Shade spawn location 3
    {5036.02f, -537.74f, 297.80f},                      // Shade spawn location 4
    {5042.32f, -544.05f, 297.80f},                     // Shade spawn location 5
    {5054.05f, -536.73f, 297.80f},                     // Hover spot
    {4853.24f, -390.89f, 350.75f},                      // Eranikus landing spot                  
};

struct MANGOS_DLL_DECL npc_captured_anoarkin : public npc_escortAI
{
    npc_captured_anoarkin(Creature* pCreature) : npc_escortAI(pCreature)
    { 
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        Reset(); 
    }

	uint32 m_uiEventPhase;
    uint32 m_uiEventTimer;

    void Reset() 
    {
        if(HasEscortState(STATE_ESCORT_ESCORTING) || HasEscortState(STATE_ESCORT_PAUSED))
            return;

        m_uiEventPhase = 0;
        m_uiEventTimer = 0;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->LoadEquipment(m_creature->GetCreatureInfo()->equipmentId, true);
        m_creature->UpdateVisibilityAndView();
    }

    void WaypointReached(uint32 i)
    {
      switch(i)
      {
      case 1:
          m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
          break;
      case 59:
          m_creature->MonsterSay("There it is!", LANG_UNIVERSAL);
          m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
          break;
      case 62:
          {
          SetEscortPaused(true);
          m_creature->SetSplineFlags(SPLINEFLAG_WALKMODE);
          GameObject *pChest = GetClosestGameObjectWithEntry(m_creature, 176225, 50.0f);
          
          if(pChest)
              m_creature->SetFacingToObject(pChest);

          m_uiEventPhase = 0;
          m_uiEventTimer = 3000;
          m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
          break;
          }
      case 63:
          for(int i = 0; i < 3; ++i)
          {
              Creature *pDemon = m_creature->SummonCreature(MOB_LEGIONAIRE, aDemonSpawns[i].m_fX, aDemonSpawns[i].m_fY, aDemonSpawns[i].m_fZ, 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 60000);

              if(pDemon)
              {
                  pDemon->SetFacingToObject(m_creature);
                  pDemon->AI()->AttackStart(m_creature);
              }
          }
          break;
      case 77:
          for(int i = 3; i < 6; ++i)
          {
              Creature *pDemon = m_creature->SummonCreature(MOB_LEGIONAIRE, aDemonSpawns[i].m_fX, aDemonSpawns[i].m_fY, aDemonSpawns[i].m_fZ, 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 60000);

              if(pDemon)
              {
                  pDemon->SetFacingToObject(m_creature);
                  pDemon->AI()->AttackStart(m_creature);
              }
          }
          break;
      case 155:
          SetEscortPaused(true);
          m_uiEventTimer = 2000;
          break;
      }
    }

    void SummonedCreatureJustDied(Creature *pSummoned)
    {
        if(pSummoned && pSummoned->GetEntry() == MOB_SPIRIT_OF_TREY)
        {
            Player *pPlayer = GetPlayerForEscort();

            if(pPlayer)
                pPlayer->GroupEventHappens(QUEST_RESCUE_FROM_JAEDENAR, m_creature);

            SetEscortPaused(false);
        }
    }

    void UpdateEscortAI(uint32 uiDiff)
    {
        if(m_uiEventTimer)
        {
            if(m_uiEventTimer <= uiDiff)
            {
                switch(m_uiEventPhase)
                {
                case 0:
                    m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                    DoCast(m_creature, SPELL_STRENGHT_OF_ARKO);
                    m_uiEventTimer = 6000;
                    break;
                case 1:
                    {
                    m_creature->SetDisplayId(10402);
                    m_creature->SetEntry(11018);
                    m_creature->UpdateVisibilityAndView();
                    m_creature->LoadEquipment(20504);

                    Unit *pPlayer = GetPlayerForEscort();

                    if(pPlayer)
                        m_creature->SetFacingToObject(pPlayer);

                    m_creature->MonsterSay("All I need now is a golden lasso!", LANG_UNIVERSAL);
                    m_uiEventTimer = 3000;
                    break;
                    }
                case 2:
                    SetEscortPaused(false);
                    m_uiEventTimer = 0;
                    break;
                case 3:
                    {
                        Creature *pDemon = m_creature->SummonCreature(MOB_SPIRIT_OF_TREY, aDemonSpawns[7].m_fX, aDemonSpawns[7].m_fX, aDemonSpawns[7].m_fX, 6.05f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 60000);

                    if(pDemon)
                    {
                        pDemon->MonsterYell("BETRAYER!", LANG_UNIVERSAL);
                        pDemon->SetFacingToObject(m_creature);
                        pDemon->AI()->AttackStart(m_creature);
                    }
                    SetEscortPaused(true);
                    m_uiEventTimer = 0;
                    break;
                    }
                case 4:
                case 5:
                    break;
                }
                ++m_uiEventPhase;
            }
            else
                m_uiEventTimer -= uiDiff;
        }

        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }



};

CreatureAI* GetAI_npc_captured_anoarkin(Creature* pCreature)
{
    return new npc_captured_anoarkin(pCreature);
}

bool QuestAccept_npc_captured_anoarkin(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pPlayer && pCreature && pQuest && pQuest->GetQuestId() == QUEST_RESCUE_FROM_JAEDENAR)
    {
        npc_captured_anoarkin *pAnarkinAI = dynamic_cast<npc_captured_anoarkin*>(pCreature->AI());

        if(pAnarkinAI)
            pAnarkinAI->Start(false, pPlayer, pQuest, true);
    }
    return true;
}

bool GossipHello_npc_captured_anoarkin(Player* pPlayer, Creature* pCreature)
{
    if(pPlayer && pCreature)
    {
        if (pCreature->isQuestGiver())
            pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

        if (pPlayer->GetQuestRewardStatus(QUEST_RESCUE_FROM_JAEDENAR))
            pPlayer->SEND_GOSSIP_MENU(100, pCreature->GetObjectGuid());
        else if(pPlayer->GetQuestStatus(QUEST_RESCUE_FROM_JAEDENAR) == QUEST_STATUS_COMPLETE)
            pPlayer->SEND_GOSSIP_MENU(99, pCreature->GetObjectGuid());
        else
            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    }

    return true;
}

void AddSC_felwood()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "npc_vartrus_the_ancient";
    pNewscript->GetAI = &GetAI_npc_vartrus_the_ancient;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "at_irontree_woods";
    pNewscript->pAreaTrigger = &AreaTrigger_at_irontree_woods;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_felwood_corrupted_flowers";
    pNewscript->pQuestRewardedGO = &QuestRewarded_go_felwood_corrupted_flowers;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_cleansed_felwood_flowers";
    pNewscript->pGOUse = &GOUse_go_cleansed_felwood_flowers;
    pNewscript->RegisterSelf();
        
    pNewscript = new Script;
    pNewscript->Name = "npc_kitten";
    pNewscript->GetAI = &GetAI_npc_kitten;
    pNewscript->pEffectDummyNPC = &EffectDummyCreature_npc_kitten;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_corrupt_saber";
    pNewscript->pGossipHello = &GossipHello_npc_corrupt_saber;
    pNewscript->pGossipSelect = &GossipSelect_npc_corrupt_saber;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_kroshius";
    pNewscript->GetAI = &GetAI_npc_kroshius;
    pNewscript->pProcessEventId = &ProcessEventId_npc_kroshius;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_niby_the_almighty";
    pNewscript->GetAI = &GetAI_npc_niby_the_almighty;
    pNewscript->pQuestRewardedNPC = &QuestRewarded_npc_niby_the_almighty;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npcs_riverbreeze_and_silversky";
    pNewscript->pGossipHello = &GossipHello_npcs_riverbreeze_and_silversky;
    pNewscript->pGossipSelect = &GossipSelect_npcs_riverbreeze_and_silversky;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_ooze_multi";
    pNewscript->GetAI = &GetAI_npc_ooze_multi;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_captured_anoarkin";
    pNewscript->GetAI = &GetAI_npc_captured_anoarkin;
    pNewscript->pGossipHello = &GossipHello_npc_captured_anoarkin;
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_captured_anoarkin;
    pNewscript->RegisterSelf();
}
