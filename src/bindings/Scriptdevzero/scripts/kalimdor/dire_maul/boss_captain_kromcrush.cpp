/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

/* ScriptData
SDName: Boss Captain Kromcrush
SD%Complete:
SDComment:
SDCategory: Dire Maul
EndScriptData */

#include "precompiled.h"
#include "dire_maul.h"

#define GOSSIP_PLAYER_START "So, now that I'm the king... what have you got for me?!"
#define GOSSIP_PLAYER_END "This sounds like a task worthy of the new king!"

enum eCaptainKromCrush
{
    GOSSIP_CAPTAIN_START      = 6914,
    GOSSIP_CAPTAIN_END        = 6920,

    SAY_SUMMON                = -1429008,
    EMOTE_ENRAGE              = -1429009,
    SAY_FENGUS_1              = -1429010,
    SAY_FENGUS_2              = -1429011,

    SPELL_CALL_REAVERS        = 22860,
    SPELL_ENRAGE              = 8599,
    SPELL_FRIGHTENING_SHOUT   = 19134,
    SPELL_MORTAL_CLEAVE       = 22859,
    SPELL_RETALIATION         = 22857,
    SPELL_OGRE_SUIT_BUFF      = 22736,
};

struct MANGOS_DLL_DECL boss_captain_kromcrushAI : public ScriptedAI
{
    boss_captain_kromcrushAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_dire_maul*)pCreature->GetInstanceData();
        Reset();
    }

    instance_dire_maul* m_pInstance;

    bool m_bIsSpawned;
    bool m_bIsEnraged;
    uint32 m_uiFrighteningShoutTimer;
    uint32 m_uiMortalCleaveTimer;
    uint32 m_uiRetaliationTimer;

    void Reset()
    {
        m_bIsSpawned = false;
        m_bIsEnraged = false;
        m_uiFrighteningShoutTimer = urand(6000,10000);
        m_uiMortalCleaveTimer = urand(1000,3000);
        m_uiRetaliationTimer = 15000;
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CAPTAIN_KROMCRUSH, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CAPTAIN_KROMCRUSH, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CAPTAIN_KROMCRUSH, DONE);
    }

    void JustSummoned(Creature* pSummoned)
    {
        Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
        if (!m_creature->getVictim() && !pTarget)
            return;

        pSummoned->AI()->AttackStart(pTarget ? pTarget : m_creature->getVictim());
    }

    void MovementInform(uint32 /*uiMotionType*/, uint32 uiPointId)
    {
        if (uiPointId == 0)
        {
            m_creature->SetVisibility(VISIBILITY_OFF);
            m_creature->setFaction(FACTION_FRIENDLY);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Spawn adds at 75% health
        if (!m_bIsSpawned && HealthBelowPct(75))
        {
            DoScriptText(SAY_SUMMON, m_creature);

            for(uint8 i = 0; i < 2; ++i)
                DoSpawnCreature(NPC_GORDOK_REAVER, irand(-2,2), irand(-2,2), 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);

            m_bIsSpawned = true;
        }

        // Enrage at 33% health
        if (!m_bIsEnraged && HealthBelowPct(33))
        {
            DoScriptText(EMOTE_ENRAGE, m_creature);
            DoCastSpellIfCan(m_creature, SPELL_ENRAGE);
            m_bIsEnraged = true;
        }

        // Frightening Shout
        if (m_uiFrighteningShoutTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FRIGHTENING_SHOUT);
            m_uiFrighteningShoutTimer = 22000;
        }
        else
            m_uiFrighteningShoutTimer -= uiDiff;

        // Mortal Cleave
        if (m_uiMortalCleaveTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MORTAL_CLEAVE);
            m_uiMortalCleaveTimer = urand(14000,18000);
        }
        else
            m_uiMortalCleaveTimer -= uiDiff;

        // Retaliation
        if (m_uiRetaliationTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_RETALIATION);
            m_uiRetaliationTimer = 30000;
        }
        else
            m_uiRetaliationTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_captain_kromcrush(Creature* pCreature)
{
    return new boss_captain_kromcrushAI(pCreature);
}

bool GossipHello_boss_captain_kromcrush(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->HasAura(SPELL_OGRE_SUIT_BUFF, EFFECT_INDEX_0))
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Hey, Kromcrush, Fengus and his fellas are bitching out you!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
		pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    }
    else
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_PLAYER_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_CAPTAIN_START, pCreature->GetObjectGuid());
    }
    return true;
}

bool GossipSelect_boss_captain_kromcrush(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "...continue...", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            DoScriptText(SAY_FENGUS_1, pCreature);
            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_PLAYER_END, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_CAPTAIN_END, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 3:
            DoScriptText(SAY_FENGUS_2, pCreature);
            pCreature->GetMotionMaster()->MovePoint(0, 665.77f, 502.78f, 29.46f);
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
        case GOSSIP_ACTION_INFO_DEF + 4:
            if (pCreature->isQuestGiver())
                pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());
            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
            break;
    }
    return true;
}

void AddSC_boss_captain_kromcrush()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_captain_kromcrush";
    pNewscript->GetAI = &GetAI_boss_captain_kromcrush;
    pNewscript->pGossipHello =  &GossipHello_boss_captain_kromcrush;
    pNewscript->pGossipSelect = &GossipSelect_boss_captain_kromcrush;
    pNewscript->RegisterSelf();
}
