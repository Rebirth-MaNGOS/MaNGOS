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
SDName: Boss_High_Inquisitor_Fairbanks
SD%Complete: 100
SDComment:
SDCategory: Scarlet Monastery
EndScriptData */

#include "precompiled.h"
#include "scarlet_monastery.h"

enum
{
    SPELL_CURSEOFBLOOD              = 8282,
    SPELL_DISPELMAGIC               = 15090,
    SPELL_FEAR                      = 12096,
    SPELL_HEAL                      = 12039,
    SPELL_POWER_WORD_SHIELD         = 11647,
    SPELL_SLEEP                     = 8399
};

#define GOSSIP_ITEM_FAIRBANKS_1  "Curse? What's going on here, Fairbanks?"
#define GOSSIP_ITEM_FAIRBANKS_2  "Mograine?"
#define GOSSIP_ITEM_FAIRBANKS_3  "What do you mean?"
#define GOSSIP_ITEM_FAIRBANKS_4  "I still do not fully understand."
#define GOSSIP_ITEM_FAIRBANKS_5  "Incredible story. So how did he die?"
#define GOSSIP_ITEM_FAIRBANKS_6  "You mean..."
#define GOSSIP_ITEM_FAIRBANKS_7  "How do you know all of this?"
#define GOSSIP_ITEM_FAIRBANKS_8  "A thousand? For one man?"
#define GOSSIP_ITEM_FAIRBANKS_9  "Yet? Yet what??"
#define GOSSIP_ITEM_FAIRBANKS_10 "And did he?"
#define GOSSIP_ITEM_FAIRBANKS_11 "Continue please, Fairbanks."
#define GOSSIP_ITEM_FAIRBANKS_12 "You mean..."
#define GOSSIP_ITEM_FAIRBANKS_13 "You were right, Fairbanks. That is tragic."
#define GOSSIP_ITEM_FAIRBANKS_14 "And you did..."
#define GOSSIP_ITEM_FAIRBANKS_15 "You tell an incredible tale, Fairbanks. What of the blade? Is it beyond redemption?"
#define GOSSIP_ITEM_FAIRBANKS_16 "But his son is dead."

struct MANGOS_DLL_DECL boss_high_inquisitor_fairbanksAI : public ScriptedAI
{
    boss_high_inquisitor_fairbanksAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiCurseOfBloodTimer;
    uint32 m_uiDispelMagicTimer;
    uint32 m_uiFearTimer;
    uint32 m_uiHealTimer;
    uint32 m_uiSleepTimer;
    uint32 m_uiDispelTimer;
    bool bPowerWordShield;

    void Reset()
    {
        m_uiCurseOfBloodTimer = 10000;
        m_uiDispelMagicTimer = 30000;
        m_uiFearTimer = 40000;
        m_uiHealTimer = 30000;
        m_uiSleepTimer = 30000;
        m_uiDispelTimer = 20000;
        bPowerWordShield = false;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // If we are <25% hp cast Heal
        if (HealthBelowPct(25) && !m_creature->IsNonMeleeSpellCasted(false) && m_uiHealTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature,SPELL_HEAL);
            m_uiHealTimer = 30000;
        }else m_uiHealTimer -= uiDiff;

        // Fear
        if (m_uiFearTimer <= uiDiff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                DoCastSpellIfCan(target, SPELL_FEAR);

            m_uiFearTimer = 40000;
        }
        else 
            m_uiFearTimer -= uiDiff;

        // Sleep
        if (m_uiSleepTimer <= uiDiff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
                DoCastSpellIfCan(target, SPELL_SLEEP);

            m_uiSleepTimer = 30000;
        }
        else 
            m_uiSleepTimer -= uiDiff;

        // Power Word: Shield
        if (!bPowerWordShield && HealthBelowPct(25))
        {
            DoCastSpellIfCan(m_creature, SPELL_POWER_WORD_SHIELD);
            bPowerWordShield = true;
        }

        //m_uiDispelTimer
        if (m_uiDispelTimer <= uiDiff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(target, SPELL_DISPELMAGIC);

            m_uiDispelMagicTimer = 30000;
        }
        else 
            m_uiDispelMagicTimer -= uiDiff;

        //m_uiCurseOfBloodTimer
        if (m_uiCurseOfBloodTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CURSEOFBLOOD);
            m_uiCurseOfBloodTimer = 25000;
        }
        else 
            m_uiCurseOfBloodTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

bool GossipHello_boss_high_inquisitor_fairbanks(Player* pPlayer, Creature* pCreature)
{
	if (instance_scarlet_monastery* m_pInstance = (instance_scarlet_monastery*)pCreature->GetInstanceData())	
	{
		if (m_pInstance->GetData(TYPE_ASHBRINGER_EVENT) == DONE)
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FAIRBANKS_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
		
		pPlayer->SEND_GOSSIP_MENU(30000, pCreature->GetObjectGuid());
	}

	return true;
}

bool GossipSelect_boss_high_inquisitor_fairbanks(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
	switch(uiAction)
	{
		case GOSSIP_ACTION_INFO_DEF:
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FAIRBANKS_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
			pPlayer->SEND_GOSSIP_MENU(30001, pCreature->GetObjectGuid());
			break;
		case GOSSIP_ACTION_INFO_DEF + 1:
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FAIRBANKS_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
			pPlayer->SEND_GOSSIP_MENU(30002, pCreature->GetObjectGuid());
			break;
		case GOSSIP_ACTION_INFO_DEF + 2:
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FAIRBANKS_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
			pPlayer->SEND_GOSSIP_MENU(30003, pCreature->GetObjectGuid());
			break;
		case GOSSIP_ACTION_INFO_DEF + 3:
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FAIRBANKS_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
			pPlayer->SEND_GOSSIP_MENU(30004, pCreature->GetObjectGuid());
			break;
		case GOSSIP_ACTION_INFO_DEF + 4:
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FAIRBANKS_6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
			pPlayer->SEND_GOSSIP_MENU(30005, pCreature->GetObjectGuid());
			break;
		case GOSSIP_ACTION_INFO_DEF + 5:
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FAIRBANKS_7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
			pPlayer->SEND_GOSSIP_MENU(30006, pCreature->GetObjectGuid());
			break;
		case GOSSIP_ACTION_INFO_DEF + 6:
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FAIRBANKS_8, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
			pPlayer->SEND_GOSSIP_MENU(30007, pCreature->GetObjectGuid());
			break;
		case GOSSIP_ACTION_INFO_DEF + 7:
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FAIRBANKS_9, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
			pPlayer->SEND_GOSSIP_MENU(30008, pCreature->GetObjectGuid());
			break;
		case GOSSIP_ACTION_INFO_DEF + 8:
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FAIRBANKS_10, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
			pPlayer->SEND_GOSSIP_MENU(30009, pCreature->GetObjectGuid());
			break;
		case GOSSIP_ACTION_INFO_DEF + 9:
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FAIRBANKS_11, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
			pPlayer->SEND_GOSSIP_MENU(30010, pCreature->GetObjectGuid());
			break;
		case GOSSIP_ACTION_INFO_DEF + 10:
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FAIRBANKS_12, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
			pPlayer->SEND_GOSSIP_MENU(30011, pCreature->GetObjectGuid());
			break;
		case GOSSIP_ACTION_INFO_DEF + 11:
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FAIRBANKS_13, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
			pPlayer->SEND_GOSSIP_MENU(30012, pCreature->GetObjectGuid());
			break;
		case GOSSIP_ACTION_INFO_DEF + 12:
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FAIRBANKS_14, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
			pPlayer->SEND_GOSSIP_MENU(30013, pCreature->GetObjectGuid());
			break;
		case GOSSIP_ACTION_INFO_DEF + 13:
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FAIRBANKS_15, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
			pPlayer->SEND_GOSSIP_MENU(30014, pCreature->GetObjectGuid());
			break;
		case GOSSIP_ACTION_INFO_DEF + 14:
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FAIRBANKS_16, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 15);
			pPlayer->SEND_GOSSIP_MENU(30015, pCreature->GetObjectGuid());
			break;
		case GOSSIP_ACTION_INFO_DEF + 15:
			pPlayer->SEND_GOSSIP_MENU(30016, pCreature->GetObjectGuid());
			break;
	}
	return true;
}

CreatureAI* GetAI_boss_high_inquisitor_fairbanks(Creature* pCreature)
{
    return new boss_high_inquisitor_fairbanksAI(pCreature);
}

void AddSC_boss_high_inquisitor_fairbanks()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "boss_high_inquisitor_fairbanks";
    pNewScript->GetAI = &GetAI_boss_high_inquisitor_fairbanks;
	pNewScript->pGossipHello = &GossipHello_boss_high_inquisitor_fairbanks;
	pNewScript->pGossipSelect = &GossipSelect_boss_high_inquisitor_fairbanks;
    pNewScript->RegisterSelf();
}
