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
SDName: Ungoro_Crater
SD%Complete: 100
SDComment: Quest support: 4245, 4291, 4491
SDCategory: Un'Goro Crater
EndScriptData */

/* ContentData
mob_simone_the_inconspicuous		// moved 
npc_ame01
npc_ringo
npc_larkorwi_mate
at_larkorwi_eggs
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"
#include "follower_ai.h"

///*######
//## mob_simone_the_inconspicuous
//######*/
//
//enum eSimoneTheInconspocuous
//{
//    SPELL_PRECIOUS_TRANSFORM_TO_FELHOUND_DND    = 23200,
//    SPELL_TEMPTRESS_KISS                        = 23205,    // Reduces ranged attack power by 1,500 for 45 seconds.
//    SPELL_LIGHTNING_BOLT                        = 15801,    // probably wrong id (~750 damage yet)
//    SPELL_FOOLS_PLIGHT                          = 23504,
//
//    NPC_SIMONE_THE_SEDUCTRESS                   = 14533,
//    NPC_THE_CLEANER                             = 14503,
//    NPC_PRECIOUS_THE_DEVOURER                   = 14538,
//
//    MODELID_SUCCUBUS                            = 2834,
//    QUEST_STAVE_OF_THE_ANCIENTS                 = 7636,
//    NPC_PRECIOUS                                = 14528,
//
//    SAY_FOOL_EFFECT                             = -1000666,
//};
//
//#define GOSSIP_ITEM_SIMONE_CHALLENGE "I came for your head demon. I challenge you!"
//
//struct MANGOS_DLL_DECL mob_simone_the_inconspicuousAI : public ScriptedAI
//{
//    mob_simone_the_inconspicuousAI(Creature* pCreature) : ScriptedAI(pCreature)
//    {
//        m_uiDefaultEntry    = pCreature->GetEntry();
//        m_uiResetOOCTimer   = 0;
//        m_uiDemonTransTimer = 0;
//		m_uiChallengerGUID.Clear();
//		m_uiPreciousPetGUID.Clear();
//        Reset();
//    }
//
//    uint32 m_uiDefaultEntry;
//    uint32 m_uiResetOOCTimer;
//    uint32 m_uiDemonTransTimer;
//    uint32 m_uiLightningBoltTimer;
//    uint32 m_uiTemptressKissTimer;
//
//	ObjectGuid m_uiChallengerGUID;
//    ObjectGuid m_uiPreciousPetGUID;
//
//    void Reset()
//    {
//        m_uiLightningBoltTimer = urand(2500, 4000);
//        m_uiTemptressKissTimer = 0;
//    }
//
//    void JustReachedHome()
//    {
//        CompleteSimoneChallenge();
//    }
//
//    void KilledUnit(Unit* /*pWho*/)
//    {
//        // Only one target can attacks Simone, so ..
//        CompleteSimoneChallenge();
//    }
//
//    void Aggro(Unit* pAttacker)
//    {
//        // The pAttacker must to be a player and must to be a hunter
//        // The hunter's pet is not allowed
//        if ((!pAttacker->IsCharmerOrOwnerPlayerOrPlayerItself()) || (pAttacker->getClass() != CLASS_HUNTER))
//        {
//            SummonTheCleaner();
//            return;
//        }
//        ScriptedAI::Aggro(pAttacker);
//    }
//
//    void JustDied(Unit* pKiller)
//    {
//        if (pKiller->GetObjectGuid() != m_uiChallengerGUID)
//            SummonTheCleaner();
//        m_creature->SetLootRecipient(NULL);
//    }
//
//    void DamageTaken(Unit* pDoneBy, uint32& /*uiDamage*/)
//    {
//        // Never trigger because not_attackable due to neutral faction??
//        if (m_creature->GetEntry() == m_uiDefaultEntry)
//        {
//            DoCastSpellIfCan(pDoneBy, SPELL_FOOLS_PLIGHT, CAST_FORCE_CAST);
//        }
//        else if (m_creature->GetEntry() == NPC_SIMONE_THE_SEDUCTRESS)
//        {
//            if (pDoneBy->GetObjectGuid() != m_uiChallengerGUID)
//            {
//                DoScriptText(SAY_FOOL_EFFECT, m_creature);
//                SummonTheCleaner();
//            }
//        }
//    }
//
//    void StartSimoneChallenge(ObjectGuid pChallengerGUID)
//    {
//        if (!pChallengerGUID)
//            return;
//
//        m_uiChallengerGUID = pChallengerGUID;
//
//        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
//        
//        m_uiDemonTransTimer = 3000;
//        m_uiResetOOCTimer   = 60000 + m_uiDemonTransTimer;
//    }
//
//    void CompleteSimoneChallenge(bool despawn = false)
//    {
//        if (despawn)
//        {
//            if (Creature* pPrecious = GetPreciousPet())
//            {
//                /*pPrecious->RemoveAllAuras();
//                pPrecious->SetObjectScale(pPrecious->GetCreatureInfo()->scale);
//                pPrecious->setFaction(pPrecious->GetCreatureInfo()->faction_A);*/
//                pPrecious->UpdateEntry(NPC_PRECIOUS);   // restore default entry
//                pPrecious->ForcedDespawn();
//            }
//			m_uiChallengerGUID.Clear();
//			m_uiPreciousPetGUID.Clear();
//            m_uiDemonTransTimer = 0;
//            m_uiResetOOCTimer   = 0;
//            m_creature->UpdateEntry(m_uiDefaultEntry);
//            m_creature->ForcedDespawn();
//        }
//        else
//        {
//            if (Creature* pPrecious = GetPreciousPet())
//            {
//                if (pPrecious->isAlive())
//                {
//                    if (pPrecious->isInCombat())
//                        pPrecious->AI()->ResetToHome();
//                    pPrecious->SetDeathState(JUST_DIED);
//                    pPrecious->SetHealth(0);
//                }
//                pPrecious->UpdateEntry(NPC_PRECIOUS);
//                pPrecious->Respawn();
//            }
//
//			m_uiChallengerGUID.Clear();
//			m_uiPreciousPetGUID.Clear();
//            m_uiDemonTransTimer = 0;
//            m_uiResetOOCTimer   = 0;
//
//            m_creature->UpdateEntry(m_uiDefaultEntry);
//            /*m_creature->setFaction(m_creature->GetCreatureInfo()->faction_A);
//            m_creature->SetDisplayId(m_creature->GetCreatureInfo()->ModelId[0]);
//            m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);*/
//            m_creature->SetHealth(0);
//            m_creature->SetDeathState(JUST_DIED);
//            m_creature->Respawn();
//        }
//    }
//
//    Creature* GetPreciousPet()
//    {
//        Creature* pPrecious = NULL;
//
//        if (m_uiPreciousPetGUID)
//            pPrecious = m_creature->GetMap()->GetCreature(m_uiPreciousPetGUID);
//        
//        if (!pPrecious)
//            pPrecious = GetClosestCreatureWithEntry(m_creature, NPC_PRECIOUS, DEFAULT_VISIBILITY_DISTANCE);
//
//        if (!pPrecious)
//            pPrecious = GetClosestCreatureWithEntry(m_creature, NPC_PRECIOUS_THE_DEVOURER, DEFAULT_VISIBILITY_DISTANCE);
//
//        return pPrecious;
//    }
//
//    void SummonTheCleaner()
//    {
//        m_creature->SummonCreature(NPC_THE_CLEANER, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), m_creature->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 2000);
//        CompleteSimoneChallenge(true);
//    }
//
//    void UpdateAI(const uint32 uiDiff)
//    {
//        if (m_uiDemonTransTimer)
//        {
//            // Transformation into Demon (event start)
//            if (m_uiDemonTransTimer <= uiDiff)
//            {
//                m_uiDemonTransTimer = 0;
//
//                if (Player* pChallenger = m_creature->GetMap()->GetPlayer(m_uiChallengerGUID))
//                {
//                    if (!pChallenger)
//                    {
//                        CompleteSimoneChallenge();
//                        return;
//                    }
//                }
//
//                //m_creature->SetEntry(NPC_SIMONE_THE_SEDUCTRESS);
//                //m_creature->SetDisplayId(MODELID_SUCCUBUS);
//                //m_creature->setFaction(54);
//                m_creature->UpdateEntry(NPC_SIMONE_THE_SEDUCTRESS);
//                
//                if (Creature* pPrecious = GetPreciousPet())
//                {
//					m_uiPreciousPetGUID = pPrecious->GetObjectGuid();
//                    pPrecious->UpdateEntry(NPC_PRECIOUS_THE_DEVOURER);
//                    pPrecious->CastSpell(pPrecious, SPELL_PRECIOUS_TRANSFORM_TO_FELHOUND_DND, false);
//                    //pPrecious->SetObjectScale(pPrecious->GetCreatureInfo()->scale * 3.0f);
//                    //pPrecious->setFaction(54);
//                }
//                else
//                    error_log("SD0: UnGoro: Hunter epic quest challenge event started, but Simone's pet Precious has been not handled.");
//            }
//            else
//                m_uiDemonTransTimer -= uiDiff;
//        }
//
//        if (m_uiResetOOCTimer)
//        {
//            // Reset "Out of Combat"
//            if (m_uiResetOOCTimer <= uiDiff)
//            {
//                if (!m_creature->isInCombat())
//                {
//                    m_uiResetOOCTimer = 0;
//                    CompleteSimoneChallenge();
//                }
//                else
//                    m_uiResetOOCTimer = 60000;
//            }
//            else
//                m_uiResetOOCTimer -= uiDiff;
//        }
//
//        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
//            return;
//
//        // Despawn Simone and spawn The Cleaner,
//        // when Simone or her pet is attacked by another attacker
//        if (m_creature->getThreatManager().getThreatList().size() > 1)
//        {
//            SummonTheCleaner();
//            return;
//        }
//        else if (Creature* pPrecious = GetPreciousPet())
//        {
//            if (pPrecious->getThreatManager().getThreatList().size() > 1)
//            {
//                SummonTheCleaner();
//                return;
//            }
//        }
//
//        // Temptress Kiss (reduce range attack power)
//        if (m_uiTemptressKissTimer <= uiDiff)
//        {
//            DoCastSpellIfCan(m_creature->getVictim(), SPELL_TEMPTRESS_KISS);
//            m_uiTemptressKissTimer = 45000;
//        }
//        else
//            m_uiTemptressKissTimer -= uiDiff;
//
//        // Lightning Bolt
//        if (m_uiLightningBoltTimer <= uiDiff)
//        {
//            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_LIGHTNING_BOLT) == CAST_OK)
//                m_uiLightningBoltTimer = 4000;
//        }
//        else
//            m_uiLightningBoltTimer -= uiDiff;
//        
//        DoMeleeAttackIfReady();
//    }
//};
//
//CreatureAI* GetAI_mob_simone_the_inconspicuous(Creature* pCreature)
//{
//    return new mob_simone_the_inconspicuousAI(pCreature);
//}
//
//bool GossipHello_mob_simone_the_inconspicuous(Player* pPlayer, Creature* pCreature)
//{
//    if (pCreature->isQuestGiver())
//        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());
//
//    if (pPlayer->GetQuestStatus(QUEST_STAVE_OF_THE_ANCIENTS) == QUEST_STATUS_INCOMPLETE)
//        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_SIMONE_CHALLENGE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
//
//    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
//    return true;
//}
//
//bool GossipSelect_mob_simone_the_inconspicuous(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
//{
//    if (uiSender == GOSSIP_SENDER_MAIN && uiAction == (GOSSIP_ACTION_INFO_DEF + 1))
//        if (mob_simone_the_inconspicuousAI* pSimoneAI = dynamic_cast<mob_simone_the_inconspicuousAI*>(pCreature->AI()))
//			pSimoneAI->StartSimoneChallenge(pPlayer->GetObjectGuid());
//
//    pPlayer->CLOSE_GOSSIP_MENU();
//    return true;
//}
//
///*######
//## mob_precious
//######*/
//
//struct MANGOS_DLL_DECL mob_preciousAI : public ScriptedAI
//{
//    mob_preciousAI(Creature* pCreature) : ScriptedAI(pCreature)
//    {
//        m_uiDefaultEntry = pCreature->GetEntry();
//        Reset();
//    }
//
//    uint32 m_uiDefaultEntry;
//
//    void Reset()
//    {
//        m_creature->RemoveAllAuras();
//        m_creature->UpdateEntry(m_uiDefaultEntry);
//        /*m_creature->SetObjectScale(m_creature->GetCreatureInfo()->scale);
//        m_creature->setFaction(m_creature->GetCreatureInfo()->faction_A);*/
//    }
//
//    void Aggro(Unit* pAttacker)
//    {
//        if (Creature* pSimone = GetClosestCreatureWithEntry(m_creature, NPC_SIMONE_THE_SEDUCTRESS, DEFAULT_VISIBILITY_DISTANCE))
//        {
//            if (mob_simone_the_inconspicuousAI* pSimoneAI = dynamic_cast<mob_simone_the_inconspicuousAI*>(pSimone->AI()))
//            {
//                if (!pSimone->SelectHostileTarget() && !m_creature->getVictim())
//                    pSimoneAI->Aggro(pAttacker);
//            }
//        }
//        ScriptedAI::Aggro(pAttacker);
//    }
//
//    void JustDied(Unit* pKiller)
//    {
//        Reset();
//        ScriptedAI::JustDied(pKiller);
//    }
//};
//
//CreatureAI* GetAI_mob_precious(Creature* pCreature)
//{
//    return new mob_preciousAI(pCreature);
//}

/*######
## npc_ame01
######*/

enum
{
    SAY_AME_START           = -1000446,
    SAY_AME_PROGRESS        = -1000447,
    SAY_AME_END             = -1000448,
    SAY_AME_AGGRO1          = -1000449,
    SAY_AME_AGGRO2          = -1000450,
    SAY_AME_AGGRO3          = -1000451,

    QUEST_CHASING_AME       = 4245
};

struct MANGOS_DLL_DECL npc_ame01AI : public npc_escortAI
{
    npc_ame01AI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    void Reset() { }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 0:
                DoScriptText(SAY_AME_START, m_creature);
                break;
            case 19:
                DoScriptText(SAY_AME_PROGRESS, m_creature);
                break;
            case 37:
                DoScriptText(SAY_AME_END, m_creature);
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_CHASING_AME, m_creature);
                break;
        }
    }

    void Aggro(Unit* pWho)
    {
        if (pWho->GetTypeId() == TYPEID_PLAYER)
            return;

        if (Player* pPlayer = GetPlayerForEscort())
        {
            if (pPlayer->getVictim() && pPlayer->getVictim() == pWho)
                return;

            switch(urand(0, 2))
            {
                case 0: DoScriptText(SAY_AME_AGGRO1, m_creature); break;
                case 1: DoScriptText(SAY_AME_AGGRO2, m_creature); break;
                case 2: DoScriptText(SAY_AME_AGGRO3, m_creature); break;
            }
        }
    }
};

bool QuestAccept_npc_ame01(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_CHASING_AME)
    {
        if (npc_ame01AI* pAmeAI = dynamic_cast<npc_ame01AI*>(pCreature->AI()))
        {
            pCreature->SetStandState(UNIT_STAND_STATE_STAND);

            if (pPlayer->GetTeam() == ALLIANCE)
                pCreature->setFaction(FACTION_ESCORT_A_PASSIVE);
            else if (pPlayer->GetTeam() == HORDE)
                pCreature->setFaction(FACTION_ESCORT_H_PASSIVE);

            pAmeAI->Start(false, pPlayer, pQuest);
        }
    }
    return true;
}

CreatureAI* GetAI_npc_ame01(Creature* pCreature)
{
    return new npc_ame01AI(pCreature);
}

/*####
# npc_ringo
####*/

enum
{
    SAY_RIN_START_1             = -1000416,
    SAY_RIN_START_2             = -1000417,

    SAY_FAINT_1                 = -1000418,
    SAY_FAINT_2                 = -1000419,
    SAY_FAINT_3                 = -1000420,
    SAY_FAINT_4                 = -1000421,

    SAY_WAKE_1                  = -1000422,
    SAY_WAKE_2                  = -1000423,
    SAY_WAKE_3                  = -1000424,
    SAY_WAKE_4                  = -1000425,

    SAY_RIN_END_1               = -1000426,
    SAY_SPR_END_2               = -1000427,
    SAY_RIN_END_3               = -1000428,
    EMOTE_RIN_END_4             = -1000429,
    EMOTE_RIN_END_5             = -1000430,
    SAY_RIN_END_6               = -1000431,
    SAY_SPR_END_7               = -1000432,
    EMOTE_RIN_END_8             = -1000433,

    SPELL_REVIVE_RINGO          = 15591,
    QUEST_A_LITTLE_HELP         = 4491,
    NPC_SPRAGGLE                = 9997
};

struct MANGOS_DLL_DECL npc_ringoAI : public FollowerAI
{
    npc_ringoAI(Creature* pCreature) : FollowerAI(pCreature) { Reset(); }

    uint32 m_uiFaintTimer;
    uint32 m_uiEndEventProgress;
    uint32 m_uiEndEventTimer;

    Unit* pSpraggle;

    void Reset()
    {
        m_uiFaintTimer = urand(30000, 60000);
        m_uiEndEventProgress = 0;
        m_uiEndEventTimer = 1000;
        pSpraggle = NULL;
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        FollowerAI::MoveInLineOfSight(pWho);

        if (!m_creature->getVictim() && !HasFollowState(STATE_FOLLOW_COMPLETE) && pWho->GetEntry() == NPC_SPRAGGLE)
        {
            if (m_creature->IsWithinDistInMap(pWho, INTERACTION_DISTANCE))
            {
                if (Player* pPlayer = GetLeaderForFollower())
                {
                    if (pPlayer->GetQuestStatus(QUEST_A_LITTLE_HELP) == QUEST_STATUS_INCOMPLETE)
                        pPlayer->GroupEventHappens(QUEST_A_LITTLE_HELP, m_creature);
                }

                pSpraggle = pWho;
                SetFollowComplete(true);
            }
        }
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell)
    {
        if (HasFollowState(STATE_FOLLOW_INPROGRESS | STATE_FOLLOW_PAUSED) && pSpell->Id == SPELL_REVIVE_RINGO)
            ClearFaint();
    }

    void SetFaint()
    {
        if (!HasFollowState(STATE_FOLLOW_POSTEVENT))
        {
            SetFollowPaused(true);

            switch(urand(0, 3))
            {
                case 0: DoScriptText(SAY_FAINT_1, m_creature); break;
                case 1: DoScriptText(SAY_FAINT_2, m_creature); break;
                case 2: DoScriptText(SAY_FAINT_3, m_creature); break;
                case 3: DoScriptText(SAY_FAINT_4, m_creature); break;
            }
        }

        //what does actually happen here? Emote? Aura?
        m_creature->SetStandState(UNIT_STAND_STATE_SLEEP);
    }

    void ClearFaint()
    {
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);

        if (HasFollowState(STATE_FOLLOW_POSTEVENT))
            return;

        switch(urand(0, 3))
        {
            case 0: DoScriptText(SAY_WAKE_1, m_creature); break;
            case 1: DoScriptText(SAY_WAKE_2, m_creature); break;
            case 2: DoScriptText(SAY_WAKE_3, m_creature); break;
            case 3: DoScriptText(SAY_WAKE_4, m_creature); break;
        }

        SetFollowPaused(false);
    }

    void UpdateFollowerAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (HasFollowState(STATE_FOLLOW_POSTEVENT))
            {
                if (m_uiEndEventTimer < uiDiff)
                {
                    if (!pSpraggle || !pSpraggle->isAlive())
                    {
                        SetFollowComplete();
                        return;
                    }

                    switch(m_uiEndEventProgress)
                    {
                        case 1:
                            DoScriptText(SAY_RIN_END_1, m_creature);
                            m_uiEndEventTimer = 3000;
                            break;
                        case 2:
                            DoScriptText(SAY_SPR_END_2, pSpraggle);
                            m_uiEndEventTimer = 5000;
                            break;
                        case 3:
                            DoScriptText(SAY_RIN_END_3, m_creature);
                            m_uiEndEventTimer = 1000;
                            break;
                        case 4:
                            DoScriptText(EMOTE_RIN_END_4, m_creature);
                            SetFaint();
                            m_uiEndEventTimer = 9000;
                            break;
                        case 5:
                            DoScriptText(EMOTE_RIN_END_5, m_creature);
                            ClearFaint();
                            m_uiEndEventTimer = 1000;
                            break;
                        case 6:
                            DoScriptText(SAY_RIN_END_6, m_creature);
                            m_uiEndEventTimer = 3000;
                            break;
                        case 7:
                            DoScriptText(SAY_SPR_END_7, pSpraggle);
                            m_uiEndEventTimer = 10000;
                            break;
                        case 8:
                            DoScriptText(EMOTE_RIN_END_8, m_creature);
                            m_uiEndEventTimer = 5000;
                            break;
                        case 9:
                            SetFollowComplete();
                            break;
                    }

                    ++m_uiEndEventProgress;
                }
                else
                    m_uiEndEventTimer -= uiDiff;
            }
            else if (HasFollowState(STATE_FOLLOW_INPROGRESS))
            {
                if (!HasFollowState(STATE_FOLLOW_PAUSED))
                {
                    if (m_uiFaintTimer < uiDiff)
                    {
                        SetFaint();
                        m_uiFaintTimer = urand(60000, 120000);
                    }
                    else
                        m_uiFaintTimer -= uiDiff;
                }
            }

            return;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_ringo(Creature* pCreature)
{
    return new npc_ringoAI(pCreature);
}

bool QuestAccept_npc_ringo(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_A_LITTLE_HELP)
    {
        if (npc_ringoAI* pRingoAI = dynamic_cast<npc_ringoAI*>(pCreature->AI()))
        {
            pCreature->SetStandState(UNIT_STAND_STATE_STAND);
            pRingoAI->StartFollow(pPlayer, FACTION_ESCORT_N_FRIEND_PASSIVE, pQuest);
        }
    }

    return true;
}

/*######
## npc_larkorwi_mate
## at_larkorwi_eggs
######*/

enum Larkorwi
{
    SPELL_REND                    = 13443,
    SPELL_TENDON_RIP              = 3604,
    NPC_LARKORWI_MATE             = 9683,
    QUEST_THE_SCENT_OF_LARKORWI   = 4291,
    FACTION_LARKORWI_MATE         = 48,
    FACTION_FRIENDLY              = 35
};

struct MANGOS_DLL_DECL npc_larkorwi_mateAI : public ScriptedAI
{
    npc_larkorwi_mateAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{ 
		protectingNest = false;
		Reset();
	}
    
    uint32 m_uiRendTimer;
    uint32 m_uiTendonRipTimer;
	uint32 m_uiNestTimer;
	uint32 m_uiResetTimer;
	float nestLocation[3];
	bool protectingNest;
    
    void Reset()
    {
		if(protectingNest)
		{
			m_creature->GetMotionMaster()->MovePoint(0, nestLocation[0], nestLocation[1],nestLocation[2]);
		}
		else
		{
	        m_creature->setFaction(FACTION_FRIENDLY);
			m_creature->SetVisibility(VISIBILITY_OFF);
			m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		}

        m_uiRendTimer = 0;
        m_uiTendonRipTimer = 0;
		m_uiNestTimer = 0;
    }

    void Aggro(Unit* /*who*/)
    {
    }

	void JustDied(Unit* /*pKiller*/)
	{
		protectingNest = false;
	}

	void startDinoEvent(float x, float y, float z)
	{
		if(!protectingNest)
		{
			nestLocation[0] = x;
			nestLocation[1] = y;
			nestLocation[2] = z;
			m_uiNestTimer = 3000;
		}
	}

	void moveToNest()
	{
		m_creature->SetVisibility(VISIBILITY_ON);
		m_creature->setFaction(FACTION_LARKORWI_MATE);
		m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		m_creature->GetMotionMaster()->MovePoint(0, nestLocation[0], nestLocation[1], nestLocation[2]);
		protectingNest = true;
	}

    void UpdateAI(const uint32 uiDiff)
    {
		// Move dino to nest
		if(m_uiNestTimer)
		{
			if(m_uiNestTimer <= uiDiff)
			{
				moveToNest();
				m_uiNestTimer = 0;
			}
			else
			{
				m_uiNestTimer -= uiDiff;
			}
		}

		if(m_creature->isInCombat() && !m_uiRendTimer)
			m_uiRendTimer = urand(1000,2000);

		if(m_creature->isInCombat() && !m_uiTendonRipTimer)
			m_uiTendonRipTimer = urand(1000,2000);
        
        //Rend spell
		if(m_uiRendTimer)
		{
			if (m_uiRendTimer < uiDiff)
			{
				if(m_creature->getVictim())
				{
					DoCastSpellIfCan(m_creature->getVictim(), SPELL_REND);
				}
				m_uiRendTimer = urand(6000,7000);
			} else m_uiRendTimer -= uiDiff;
		}

        
        //Rend spell
		if(m_uiTendonRipTimer)
		{
			if (m_uiTendonRipTimer < uiDiff)
			{
				if(m_creature->getVictim())
				{
					DoCastSpellIfCan(m_creature->getVictim(), SPELL_TENDON_RIP);
				}
				m_uiTendonRipTimer = urand(20000,21000);
			} else m_uiTendonRipTimer -= uiDiff;
		}

		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_larkorwi_mate(Creature *pCreature)
{
    return new npc_larkorwi_mateAI (pCreature);
}

bool AreaTrigger_at_larkorwi_eggs(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (pPlayer->GetQuestStatus(QUEST_THE_SCENT_OF_LARKORWI) == QUEST_STATUS_INCOMPLETE)
    {
        if (Creature* Mate = GetClosestCreatureWithEntry(pPlayer, NPC_LARKORWI_MATE, 30))
        {
			if(npc_larkorwi_mateAI* mateAI = dynamic_cast<npc_larkorwi_mateAI*>(Mate->AI()))
			{
				if(pPlayer && mateAI && Mate && !Mate->isInCombat())
				{
					Mate->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
					mateAI->startDinoEvent(pAt->x + 3.0f, pAt->y, pPlayer->GetPositionZ());
				}
			}
            return false;
        }
    }
    return false;
}

/*######
## npc_captured_ooze
######*/

struct MANGOS_DLL_DECL npc_captured_ooze : public ScriptedAI
{
    npc_captured_ooze(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}
    
    uint32 m_uiUpdateTimer;
    
    void Reset()
    {
		m_uiUpdateTimer = 500;
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
		if (m_uiUpdateTimer)
		{
			if (m_uiUpdateTimer <= uiDiff)
			{
				m_uiUpdateTimer = 500;
				if (Creature* C = GetClosestCreatureWithEntry(m_creature,6557,5.0f))
				{
					m_uiUpdateTimer = 0;
					m_creature->SummonCreature(9621,m_creature->GetPositionX(),m_creature->GetPositionY(),m_creature->GetPositionZ(),0,TEMPSUMMON_CORPSE_TIMED_DESPAWN,60000);
					C->ForcedDespawn();
					m_creature->ForcedDespawn();
				}
			}
			else
				m_uiUpdateTimer -= uiDiff;
		}
    }
};

CreatureAI* GetAI_npc_captured_ooze(Creature *pCreature)
{
    return new npc_captured_ooze (pCreature);
}

/*####
# npc_j_d_collie
####*/

enum
{
	COLLIE_SAY_1					 = -1720069,
	COLLIE_SAY_2					 = -1720070,
	COLLIE_SAY_3					 = -1720071,

    QUEST_ID_MAKING_SENSE_OF_IT		 = 4321,
};

struct MANGOS_DLL_DECL npc_j_d_collieAI : public npc_escortAI
{
    npc_j_d_collieAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }
	
	uint8 m_uiSpeechStep;
	uint32 m_uiSpeechTimer;
	bool m_bOutro;

	ObjectGuid m_uiPlayerGUID;

    void Reset()
	{
		m_bOutro = false;
		m_uiSpeechStep = 1;
		m_uiSpeechTimer = 0;
		m_uiPlayerGUID.Clear();
	}

	void WaypointReached(uint32 uiPointId)
    {
	}

	void JustStartedEscort()
    {
    }

	void StartOutro(ObjectGuid pPlayerGUID)
	{
		if (!pPlayerGUID)
            return;

        m_uiPlayerGUID = pPlayerGUID;

		m_bOutro = true; 
		m_uiSpeechTimer = 1000;
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
				Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID);
                switch(m_uiSpeechStep)
                {
                    case 1:
						DoScriptText(COLLIE_SAY_1, m_creature);                        
						m_uiSpeechTimer = 7000;
                        break;
					case 2:
						DoScriptText(COLLIE_SAY_2, m_creature);
                        m_uiSpeechTimer = 5000;
                        break;
					case 3:
						DoScriptText(COLLIE_SAY_3, m_creature, pPlayer);
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

CreatureAI* GetAI_npc_j_d_collie(Creature* pCreature)
{
    return new npc_j_d_collieAI(pCreature);
}

bool OnQuestRewarded_npc_j_d_collie(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
	if (pQuest->GetQuestId() == QUEST_ID_MAKING_SENSE_OF_IT)
    {
		if (npc_j_d_collieAI* pEscortAI = dynamic_cast<npc_j_d_collieAI*>(pCreature->AI()))
			pEscortAI->StartOutro(pPlayer->GetObjectGuid());
	}
	return true;
}

void AddSC_ungoro_crater()
{
    Script* pNewscript;

    pNewscript = new Script;
  /*  pNewscript->Name = "mob_simone_the_inconspicuous";
    pNewscript->GetAI = &GetAI_mob_simone_the_inconspicuous;
    pNewscript->pGossipHello = &GossipHello_mob_simone_the_inconspicuous;
    pNewscript->pGossipSelect = &GossipSelect_mob_simone_the_inconspicuous;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_precious";
    pNewscript->GetAI = &GetAI_mob_precious;
    pNewscript->RegisterSelf();*/
        
    pNewscript = new Script;
    pNewscript->Name = "npc_ame01";
    pNewscript->GetAI = &GetAI_npc_ame01;
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_ame01;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_ringo";
    pNewscript->GetAI = &GetAI_npc_ringo;
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_ringo;
    pNewscript->RegisterSelf();
    
    pNewscript = new Script;
    pNewscript->Name="npc_larkorwi_mate";
    pNewscript->GetAI = &GetAI_npc_larkorwi_mate;
    pNewscript->RegisterSelf();
    
    pNewscript = new Script;
    pNewscript->Name = "at_larkorwi_eggs";
    pNewscript->pAreaTrigger = &AreaTrigger_at_larkorwi_eggs;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name="npc_captured_ooze";
    pNewscript->GetAI = &GetAI_npc_captured_ooze;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_j_d_collie";
    pNewscript->GetAI = &GetAI_npc_j_d_collie;
    pNewscript->pQuestRewardedNPC = &OnQuestRewarded_npc_j_d_collie;
    pNewscript->RegisterSelf();
}
