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

#define GOSSIP_KREEG_PLAYER "What do you mean, buy?! I'm the king now... and it's good to be the king!"

enum eStomperKreeg
{
    GOSSIP_KREEG_START              = 6894,
    GOSSIP_KREEG_END                = 6922,

    SPELL_BOOZE_SPIT                = 22833,
    SPELL_DRUNKEN_RAGE              = 22835,
    SPELL_ENRAGE                    = 8269,
    SPELL_WAR_STOMP                 = 16740,
    SPELL_WHIRLWIND                 = 15577,
};

struct MANGOS_DLL_DECL boss_stomper_kreegAI : public ScriptedAI
{
    boss_stomper_kreegAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_dire_maul*)pCreature->GetInstanceData();
        Reset();
    }

    instance_dire_maul* m_pInstance;

    uint32 m_uiBoozeSpitTimer;
    uint32 m_uiWarStompTimer;
    uint32 m_uiWhirlwindTimer;

    void Reset()
    {
        m_uiBoozeSpitTimer = urand(2000,4000);
        m_uiWarStompTimer = urand(7000,9000);
        m_uiWhirlwindTimer = urand(10000,13000);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_STOMPER_KREEG, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_STOMPER_KREEG, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_STOMPER_KREEG, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Enrage at 50% health
        if (HealthBelowPct(50))
            DoCastSpellIfCan(m_creature, SPELL_ENRAGE, CAST_AURA_NOT_PRESENT);

        // Booze Spit
        if (m_uiBoozeSpitTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BOOZE_SPIT);
            m_uiBoozeSpitTimer = urand(5000,7000);
        }
        else
            m_uiBoozeSpitTimer -= uiDiff;

        // War Stomp
        if (m_uiWarStompTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_WAR_STOMP);
            m_uiWarStompTimer = urand(8000,10000);
        }
        else
            m_uiWarStompTimer -= uiDiff;

        // Whirlwind
        if (m_uiWhirlwindTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_WHIRLWIND);
            m_uiWhirlwindTimer = urand(5000,6000);
        }
        else
            m_uiWhirlwindTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_stomper_kreeg(Creature* pCreature)
{
    return new boss_stomper_kreegAI(pCreature);
}

bool GossipHello_boss_stomper_kreeg(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
		pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, GOSSIP_KREEG_PLAYER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    pPlayer->SEND_GOSSIP_MENU(GOSSIP_KREEG_START, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_boss_stomper_kreeg(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
            if (pCreature->isVendor())
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_KREEG_END, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_TRADE:
            pPlayer->SEND_VENDORLIST(pCreature->GetObjectGuid());
            break;
    }
    return true;
}

void AddSC_boss_stomper_kreeg()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_stomper_kreeg";
    pNewscript->GetAI = &GetAI_boss_stomper_kreeg;
    pNewscript->pGossipHello = &GossipHello_boss_stomper_kreeg;
    pNewscript->pGossipSelect = &GossipSelect_boss_stomper_kreeg;
    pNewscript->RegisterSelf();
}
