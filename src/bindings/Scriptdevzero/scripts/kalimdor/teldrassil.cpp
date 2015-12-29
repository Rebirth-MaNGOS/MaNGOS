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
SDName: Teldrassil
SD%Complete: 100
SDComment: Quest support: 938
SDCategory: Teldrassil
EndScriptData */

/* ContentData
npc_mist
EndContentData */

#include "precompiled.h"
#include "follower_ai.h"

/*####
# npc_mist
####*/

enum
{
    SAY_AT_HOME             = -1000323,
    EMOTE_AT_HOME           = -1000324,
    QUEST_MIST              = 938,
    NPC_ARYNIA              = 3519,
    FACTION_DARNASSUS       = 79
};

struct MANGOS_DLL_DECL npc_mistAI : public FollowerAI
{
    npc_mistAI(Creature* pCreature) : FollowerAI(pCreature) { Reset(); }

    void Reset() { }

    void MoveInLineOfSight(Unit *pWho)
    {
        FollowerAI::MoveInLineOfSight(pWho);

        if (!m_creature->getVictim() && !HasFollowState(STATE_FOLLOW_COMPLETE) && pWho->GetEntry() == NPC_ARYNIA)
        {
            if (m_creature->IsWithinDistInMap(pWho, 10.0f))
            {
                DoScriptText(SAY_AT_HOME, pWho);
                DoComplete();
            }
        }
    }

    void DoComplete()
    {
        DoScriptText(EMOTE_AT_HOME, m_creature);

        if (Player* pPlayer = GetLeaderForFollower())
        {
            if (pPlayer->GetQuestStatus(QUEST_MIST) == QUEST_STATUS_INCOMPLETE)
                pPlayer->GroupEventHappens(QUEST_MIST, m_creature);
        }

        //The follow is over (and for later development, run off to the woods before really end)
        SetFollowComplete();
    }

    //call not needed here, no known abilities
    /*void UpdateFollowerAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }*/
};

CreatureAI* GetAI_npc_mist(Creature* pCreature)
{
    return new npc_mistAI(pCreature);
}

bool QuestAccept_npc_mist(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_MIST)
    {
        if (npc_mistAI* pMistAI = dynamic_cast<npc_mistAI*>(pCreature->AI()))
            pMistAI->StartFollow(pPlayer, FACTION_DARNASSUS, pQuest);
    }
    return true;
}

/*####
# mob_sethir_the_ancient
####*/

enum eSethir
{
    MOB_MINION_OF_SETHIR = 6911
};

struct MANGOS_DLL_DECL mob_sethir_the_ancientAI : public ScriptedAI
{
    mob_sethir_the_ancientAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiSayTimer;

    void Reset() 
    {       
        m_uiSayTimer = urand(60000, 120000);
    }
    
    void Aggro(Unit* who)
    {
        m_creature->MonsterYell("DIE!", LANG_UNIVERSAL, NULL);
        
        uint8 rand = urand(1,5);
        float fX, fY, fZ;
        m_creature->GetPosition(fX, fY, fZ);
        for(uint8 i = 0; i < rand; ++i)
            m_creature->SummonCreature(MOB_MINION_OF_SETHIR, fX+irand(-3,3), fY+irand(-3,3), fZ, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiSayTimer <= uiDiff)
        {
            if(!m_creature->isInCombat())
            {
                uint8 r = urand(0,1);
                if(r<1)
                    m_creature->MonsterSay("I know you are there, rogue. Leave my hove or join the others at the bottom of the world tree.", LANG_UNIVERSAL, NULL);
                else
                    m_creature->MonsterSay("The end of the days is upon us! May the earth embrace my flesh and bones.", LANG_UNIVERSAL, NULL);
            }
            m_uiSayTimer = urand(60000, 120000);
         } 
        else
            m_uiSayTimer -= uiDiff;
        
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady(); 
    }
};

CreatureAI* GetAI_mob_sethir_the_ancient(Creature* pCreature)
{
    return new mob_sethir_the_ancientAI(pCreature);
}

void AddSC_teldrassil()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "npc_mist";
    pNewscript->GetAI = &GetAI_npc_mist;
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_mist;
    pNewscript->RegisterSelf();
    
    pNewscript = new Script;
    pNewscript->Name = "mob_sethir_the_ancient";
    pNewscript->GetAI = &GetAI_mob_sethir_the_ancient;
    pNewscript->RegisterSelf();
}
