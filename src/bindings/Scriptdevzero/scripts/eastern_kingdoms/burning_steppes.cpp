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
SDName: Burning_Steppes
SD%Complete: 100
SDComment: Quest support: 4121, 4122(disabled, needs work but most is there), 4224, 4866
SDCategory: Burning Steppes
EndScriptData */

/* ContentData
npc_ragged_john
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"

/*######
## npc_ragged_john
######*/

struct MANGOS_DLL_DECL npc_ragged_johnAI : public ScriptedAI
{
    npc_ragged_johnAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset() {}

    void MoveInLineOfSight(Unit *who)
    {
        if (who->HasAura(16468, EFFECT_INDEX_0))
        {
            if (who->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(who, 15) && who->isInAccessablePlaceFor(m_creature))
            {
                DoCastSpellIfCan(who,16472);
                ((Player*)who)->AreaExploredOrEventHappens(4866);
            }
        }

        if (!m_creature->getVictim() && who->isTargetableForAttack() && (m_creature->IsHostileTo(who)) && who->isInAccessablePlaceFor(m_creature))
        {
            //if (!m_creature->canFly() && m_creature->GetDistanceZ(who) > CREATURE_Z_ATTACK_RANGE)
              //  return;

            float attackRadius = m_creature->GetAttackDistance(who);
            if (m_creature->IsWithinDistInMap(who, attackRadius) && m_creature->IsWithinLOSInMap(who))
            {
                AttackStart(who);
            }
        }
    }
};

CreatureAI* GetAI_npc_ragged_john(Creature* pCreature)
{
    return new npc_ragged_johnAI(pCreature);
}

bool GossipHello_npc_ragged_john(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(4224) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Official buisness, John. I need some information about Marsha Windsor. Tell me about the last time you saw him.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    pPlayer->SEND_GOSSIP_MENU(2713, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_ragged_john(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "So what did you do?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SEND_GOSSIP_MENU(2714, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Start making sense, dwarf. I don't want to have anything to do with your cracker, your pappy, or any sort of 'discreditin'.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(2715, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Ironfoe?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            pPlayer->SEND_GOSSIP_MENU(2716, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Interesting... continue John.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            pPlayer->SEND_GOSSIP_MENU(2717, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "So that's how Windsor died...", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            pPlayer->SEND_GOSSIP_MENU(2718, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "So how did he die?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
            pPlayer->SEND_GOSSIP_MENU(2719, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+6:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Ok so where the hell is he? Wait a minute! Are you drunk?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
            pPlayer->SEND_GOSSIP_MENU(2720, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+7:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "WHY is he in Blackrock Depths?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
            pPlayer->SEND_GOSSIP_MENU(2721, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+8:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "300? So the Dark Irons killed him and dragged him into the Depths?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
            pPlayer->SEND_GOSSIP_MENU(2722, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+9:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Ahh... Ironfoe", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
            pPlayer->SEND_GOSSIP_MENU(2723, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+10:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Thanks, Ragged John. Your story was very uplifting and informative", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
            pPlayer->SEND_GOSSIP_MENU(2725, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+11:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->AreaExploredOrEventHappens(4224);
            break;
    }
    return true;
}

/*######
## npc_grark_lorkrub
######*/

enum
{
    SAY_START                   = -1000873,
    SAY_PAY                     = -1000874,
    SAY_FIRST_AMBUSH_START      = -1000875,
    SAY_FIRST_AMBUSH_END        = -1000876,
    SAY_SEC_AMBUSH_START        = -1000877,
    SAY_SEC_AMBUSH_END          = -1000878,
    SAY_THIRD_AMBUSH_START      = -1000879,
    SAY_THIRD_AMBUSH_END        = -1000880,
    EMOTE_LAUGH                 = -1000881,
    SAY_LAST_STAND              = -1000882,
    SAY_LEXLORT_1               = -1000883,
    SAY_LEXLORT_2               = -1000884,
    EMOTE_RAISE_AXE             = -1000885,
    EMOTE_LOWER_HAND            = -1000886,
    SAY_LEXLORT_3               = -1000887,
    SAY_LEXLORT_4               = -1000888,

    EMOTE_SUBMIT                = -1000889,
    SAY_AGGRO                   = -1000890,

    SPELL_CAPTURE_GRARK             = 14250,

    NPC_BLACKROCK_AMBUSHER          = 9522,
    NPC_BLACKROCK_RAIDER            = 9605,
    NPC_FLAMESCALE_DRAGONSPAWN      = 7042,
    NPC_SEARSCALE_DRAKE             = 7046,

    NPC_GRARK_LORKRUB               = 9520,
    NPC_HIGH_EXECUTIONER_NUZARK     = 9538,
    NPC_SHADOW_OF_LEXLORT           = 9539,

    FACTION_FRIENDLY                = 35,
	FACTION_HOSTILE					= 103,

	QUEST_ID_GRARK_LOKRUB			= 4122,
    QUEST_ID_PRECARIOUS_PREDICAMENT = 4121
};

static const DialogueEntry aOutroDialogue[] =
{
    {SAY_LAST_STAND,    NPC_GRARK_LORKRUB,              5000},
    {SAY_LEXLORT_2,     NPC_SHADOW_OF_LEXLORT,          5000},
    {EMOTE_RAISE_AXE,   NPC_HIGH_EXECUTIONER_NUZARK,    4000},
    {EMOTE_LOWER_HAND,  NPC_SHADOW_OF_LEXLORT,          3000},
    {SAY_LEXLORT_3,     NPC_SHADOW_OF_LEXLORT,          3000},
    {NPC_GRARK_LORKRUB, 0,                              5000},
    {SAY_LEXLORT_4,     NPC_SHADOW_OF_LEXLORT,          0},
    {0, 0, 0},
};

struct MANGOS_DLL_DECL npc_grark_lorkrubAI : public npc_escortAI, private DialogueHelper
{
    npc_grark_lorkrubAI(Creature* pCreature) : npc_escortAI(pCreature),
        DialogueHelper(aOutroDialogue)
    {
        Reset();
    }

    ObjectGuid m_nuzarkGuid;
    ObjectGuid m_lexlortGuid;

    GUIDList m_lSearscaleGuidList;

    uint8 m_uiKilledCreatures;
    uint32 m_startTimer;
	uint32 m_despawnTimer;
    bool m_bIsFirstSearScale;
	/*bool m_bIsShackle;
	bool m_bHealth;*/

    void Reset()
    {
		m_creature->SetStandState(UNIT_STAND_STATE_STAND);			// make sure he isn't playing dead after quest is done and he respawns

        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            m_uiKilledCreatures = 0;
            m_startTimer = 0;
			m_despawnTimer = 0;
            m_bIsFirstSearScale = true;

            m_lSearscaleGuidList.clear();

            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			/*m_bIsShackle = false;
			m_bHealth = false;
			m_creature->ClearTemporaryFaction();*/
        }
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            DoScriptText(SAY_AGGRO, m_creature);
        }
    }

	//void SpellHit(Unit* pCaster, SpellEntry const* pSpell)									// all these are for quest 4122 (the pre-q)
 //   {
 //       if (pSpell->Id == SPELL_CAPTURE_GRARK && !HasEscortState(STATE_ESCORT_ESCORTING))
 //       {
 //           m_creature->SetFactionTemporary(FACTION_HOSTILE, TEMPFACTION_RESTORE_RESPAWN);
	//		m_creature->AI()->AttackStart(pCaster);
	//		m_bIsShackle = true;
	//	}
	//}

	//void UpdateAI(const uint32 uiDiff)							// For quest 4122
 //   {
 //       // Return since we have no target
 //       if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
 //           return;
	//}
	//	if (HealthBelowPct(25) && !HasEscortState(STATE_ESCORT_ESCORTING))
	//	{
	//		// The faction is guesswork - needs more research
	//		DoScriptText(EMOTE_SUBMIT, m_creature);
	//		m_creature->SetFactionTemporary(FACTION_FRIENDLY, TEMPFACTION_RESTORE_RESPAWN);
	//		m_creature->RemoveAllAuras();
 //           m_creature->AttackStop();
	//		ResetToHome();
	//		m_creature->SetEvadeMode(true);
	//		m_bHealth = true;
	//	}

	//	if (m_bIsShackle && m_bHealth && !HasEscortState(STATE_ESCORT_ESCORTING))									// needs a better way to do this
	//	{
	//		Player* pPlayer = GetPlayerAtMinimumRange(20);
	//		if (pPlayer->HasQuest(QUEST_ID_GRARK_LOKRUB) && QuestStatus(IN_PROGRESS));
	//			pPlayer->GroupEventHappens(QUEST_ID_GRARK_LOKRUB, m_creature);				// give quest credit if player has used item to get him to aggro AND got him down to below 25% hp
	//	}

	//	DoMeleeAttackIfReady();
	//}

    void MoveInLineOfSight(Unit* pWho)
    {
        // No combat during escort
        if (HasEscortState(STATE_ESCORT_ESCORTING))
        {
            return;
        }

        npc_escortAI::MoveInLineOfSight(pWho);
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch (uiPointId)
        {
            case 1:
                DoScriptText(SAY_START, m_creature);
                break;
            case 7:
                DoScriptText(SAY_PAY, m_creature);
                break;
            case 12:
                DoScriptText(SAY_FIRST_AMBUSH_START, m_creature);
                SetEscortPaused(true);

                m_creature->SummonCreature(NPC_BLACKROCK_AMBUSHER, -7844.3f, -1521.6f, 139.2f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                m_creature->SummonCreature(NPC_BLACKROCK_AMBUSHER, -7860.4f, -1507.8f, 141.0f, 6.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);	
                m_creature->SummonCreature(NPC_BLACKROCK_RAIDER,   -7845.6f, -1508.1f, 138.8f, 6.1f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                m_creature->SummonCreature(NPC_BLACKROCK_RAIDER,   -7859.8f, -1521.8f, 139.2f, 6.2f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                break;
            case 24:
                DoScriptText(SAY_SEC_AMBUSH_START, m_creature);
                SetEscortPaused(true);

                m_creature->SummonCreature(NPC_BLACKROCK_AMBUSHER,     -8035.3f, -1222.2f, 135.5f, 5.1f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                m_creature->SummonCreature(NPC_FLAMESCALE_DRAGONSPAWN, -8037.5f, -1216.9f, 135.8f, 5.1f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                m_creature->SummonCreature(NPC_BLACKROCK_AMBUSHER,     -8009.5f, -1222.1f, 139.2f, 3.9f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                m_creature->SummonCreature(NPC_FLAMESCALE_DRAGONSPAWN, -8007.1f, -1219.4f, 140.1f, 3.9f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                break;
            case 30:
            {
				m_creature->SummonCreature(NPC_SEARSCALE_DRAKE, -7887.8f, -1129.1f, 196.71f, 2.8f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000);
				m_creature->SummonCreature(NPC_SEARSCALE_DRAKE, -7870.3f, -1142.1f, 202.4f, 2.6f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000);
                m_creature->SummonCreature(NPC_SEARSCALE_DRAKE, -7868.0f, -1125.5f, 201.8f, 3.1f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000);

                SetEscortPaused(true);
                DoScriptText(SAY_THIRD_AMBUSH_START, m_creature);

                Player* pPlayer = GetPlayerForEscort();
                if (!pPlayer)
                {
                    return;
                }

                break;
            }
            case 36:
                DoScriptText(EMOTE_LAUGH, m_creature);
                break;
			case 44:
				m_despawnTimer = 0;				// 0 so Dialogue text works
				break;
            case 45:
                StartNextDialogueText(SAY_LAST_STAND);
                SetEscortPaused(true);
				
                m_creature->SummonCreature(NPC_HIGH_EXECUTIONER_NUZARK, -7532.3f, -1029.4f, 258.0f, 2.7f, TEMPSUMMON_TIMED_DESPAWN, 40000);
                m_creature->SummonCreature(NPC_SHADOW_OF_LEXLORT,       -7532.8f, -1032.9f, 258.2f, 2.5f, TEMPSUMMON_TIMED_DESPAWN, 40000);
                break;
        }
    }

    void JustDidDialogueStep(int32 iEntry)
    {
        switch (iEntry)
        {
            case SAY_LEXLORT_2:
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                break;
            case SAY_LEXLORT_3:
                // Note: this part isn't very clear. Should he just simply attack him, or charge him?
                if (Creature* pNuzark = m_creature->GetMap()->GetCreature(m_nuzarkGuid))
                {
                    pNuzark->HandleEmote(EMOTE_ONESHOT_ATTACK2HTIGHT);
                }
                break;
            case NPC_GRARK_LORKRUB:
                // Fake death creature when the axe is lowered. This will allow us to finish the event
                m_creature->InterruptNonMeleeSpells(true);
                m_creature->SetHealth(1);
                m_creature->StopMoving();
                m_creature->ClearComboPointHolders();
                m_creature->RemoveAllAurasOnDeath();
                m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->ClearAllReactives();
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveIdle();
                m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
                break;
            case SAY_LEXLORT_4:
                // Finish the quest
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    pPlayer->GroupEventHappens(QUEST_ID_PRECARIOUS_PREDICAMENT, m_creature);
                }
                // Kill self
                m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
                break;
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        switch (pSummoned->GetEntry())
        {
			case NPC_BLACKROCK_AMBUSHER:
			case NPC_BLACKROCK_RAIDER:
				pSummoned->SetFactionTemporary(FACTION_HOSTILE);
				if (Player* pPlayer = GetPlayerForEscort())
                {
                   pSummoned->AI()->AttackStart(pPlayer);
                }
				break;
            case NPC_HIGH_EXECUTIONER_NUZARK:
                m_nuzarkGuid  = pSummoned->GetObjectGuid();
				pSummoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                break;
            case NPC_SHADOW_OF_LEXLORT:
                m_lexlortGuid = pSummoned->GetObjectGuid();
				pSummoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                break;
            case NPC_SEARSCALE_DRAKE:
                // If it's the flying drake allow him to move in circles
                if (m_bIsFirstSearScale)
                {
                    m_bIsFirstSearScale = false;
					if (Player* pPlayer = GetPlayerForEscort())
					{
						pSummoned->AI()->AttackStart(pPlayer);
					}
				}

				m_lSearscaleGuidList.push_back(pSummoned->GetObjectGuid());
                break;

            default:
                // The hostile mobs should attack the player only
                if (Player* pPlayer = GetPlayerForEscort())
                {
                   pSummoned->AI()->AttackStart(pPlayer);
                }
                break;
        }
    }

    void SummonedCreatureJustDied(Creature* /*pSummoned*/)
    {
        ++m_uiKilledCreatures;

        switch (m_uiKilledCreatures)
        {
            case 4:
                DoScriptText(SAY_FIRST_AMBUSH_END, m_creature);
                SetEscortPaused(false);
                break;
            case 8:
                DoScriptText(SAY_SEC_AMBUSH_END, m_creature);
                SetEscortPaused(false);
                break;
            case 11:
                DoScriptText(SAY_THIRD_AMBUSH_END, m_creature);
                SetEscortPaused(false);
                break;
        }
    }

    Creature* GetSpeakerByEntry(uint32 uiEntry)
    {
        switch (uiEntry)
        {
            case NPC_GRARK_LORKRUB:
                return m_creature;
            case NPC_HIGH_EXECUTIONER_NUZARK:
                return m_creature->GetMap()->GetCreature(m_nuzarkGuid);
            case NPC_SHADOW_OF_LEXLORT:
                return m_creature->GetMap()->GetCreature(m_lexlortGuid);

            default:
                return NULL;
        }
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if(m_startTimer)
        {
            if(m_startTimer <= uiDiff)
            {
                SetEscortPaused(false);
                m_startTimer = 0;
            }
            else
            {
                m_startTimer -= uiDiff;
                return;
            }
        }

		if (m_despawnTimer)
		{
			if(m_despawnTimer <= uiDiff)
            {
                m_creature->ForcedDespawn();
                m_despawnTimer = 0;
            }
			else
			{
                m_despawnTimer -= uiDiff;
                return;
			}
		}
		
        DialogueUpdate(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            return;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_grark_lorkrub(Creature* pCreature)
{
    return new npc_grark_lorkrubAI(pCreature);
}

bool QuestAccept_npc_grark_lorkrub(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ID_PRECARIOUS_PREDICAMENT)
    {
        if (npc_grark_lorkrubAI* pEscortAI = dynamic_cast<npc_grark_lorkrubAI*>(pCreature->AI()))
        {
            pEscortAI->Start(false, pPlayer, pQuest);
            pEscortAI->SetEscortPaused(true);
            pEscortAI->m_startTimer = urand(60000, 180000); // Start timer, 1 min - 3 min.
			pEscortAI->m_despawnTimer = 1800000; // despawn after 30min if he didn't reach the outro dialogue
        }

        return true;
    }

    return false;
}

//bool EffectDummyCreature_spell_capture_grark(Unit* /*pCaster*/, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget) //, ObjectGuid /*originalCasterGuid*/)		// unused D:
//{
//    // always check spellid and effectindex
//    if (uiSpellId == SPELL_CAPTURE_GRARK && uiEffIndex == EFFECT_INDEX_0)
//    {
//        // Note: this implementation needs additional research! There is a lot of guesswork involved in this!
//        if (pCreatureTarget->GetHealthPercent() > 25.0f)
//        {
//            return false;
//        }
//
//        // The faction is guesswork - needs more research
//        DoScriptText(EMOTE_SUBMIT, pCreatureTarget);
//        pCreatureTarget->SetFactionTemporary(FACTION_FRIENDLY, TEMPFACTION_RESTORE_RESPAWN);
//        pCreatureTarget->SetEvadeMode(true);
//																						// Group event should happen here so they can turn in quest and start the escort
//        // always return true when we are handling this spell and effect
//        return true;
//    }
//
//    //return false;								// this is for quest 4122(the pre-q)
//}

void AddSC_burning_steppes()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "npc_ragged_john";
    pNewscript->GetAI = &GetAI_npc_ragged_john;
    pNewscript->pGossipHello =  &GossipHello_npc_ragged_john;
    pNewscript->pGossipSelect = &GossipSelect_npc_ragged_john;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_grark_lorkrub";
    pNewscript->GetAI = &GetAI_npc_grark_lorkrub;
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_grark_lorkrub;
    //pNewscript->pEffectDummyNPC = &EffectDummyCreature_spell_capture_grark;
    pNewscript->RegisterSelf();
}
