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
SDName: Blasted_Lands
SD%Complete: 90
SDComment: Quest support: 2784, 2801, 3628. Missing some texts for Fallen Hero. Teleporter to Rise of the Defiler missing group support.
SDCategory: Blasted Lands
EndScriptData */

/* ContentData
npc_deathly_usher
npc_fallen_hero_of_horde
EndContentData */

#include "precompiled.h"

/*######
## npc_deathly_usher
######*/

#define GOSSIP_ITEM_USHER "I wish to to visit the Rise of the Defiler."

#define SPELL_TELEPORT_SINGLE           12885
#define SPELL_TELEPORT_SINGLE_IN_GROUP  13142
#define SPELL_TELEPORT_GROUP            27686

bool GossipHello_npc_deathly_usher(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(3628) == QUEST_STATUS_INCOMPLETE && pPlayer->HasItemCount(10757, 1))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_USHER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

     pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_deathly_usher(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->CastSpell(pPlayer, SPELL_TELEPORT_SINGLE, true);
    }

    return true;
}

/*######
## npc_fallen_hero_of_horde
######*/

#define GOSSIP_ITEM_FALLEN "Continue..."

#define GOSSIP_ITEM_FALLEN1 "What could be worse than death?"
#define GOSSIP_ITEM_FALLEN2 "Subordinates?"
#define GOSSIP_ITEM_FALLEN3 "What are the stones of binding?"
#define GOSSIP_ITEM_FALLEN4 "You can count on me, Hero"
#define GOSSIP_ITEM_FALLEN5 "I shall"

bool GossipHello_npc_fallen_hero_of_horde(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(2784) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Why are you here?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    if (pPlayer->GetQuestStatus(2801) == QUEST_STATUS_INCOMPLETE && pPlayer->GetTeam() == HORDE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Continue story...", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);

    if (pPlayer->GetQuestStatus(2801) == QUEST_STATUS_INCOMPLETE && pPlayer->GetTeam() == ALLIANCE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Why are you here?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_fallen_hero_of_horde(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FALLEN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
            pPlayer->SEND_GOSSIP_MENU(1392, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+11:
            pPlayer->SEND_GOSSIP_MENU(1411, pCreature->GetObjectGuid());
            if (pPlayer->GetQuestStatus(2784) == QUEST_STATUS_INCOMPLETE)
                pPlayer->AreaExploredOrEventHappens(2784);
            if (pPlayer->GetTeam() == ALLIANCE)
            {
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FALLEN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                pPlayer->SEND_GOSSIP_MENU(1411, pCreature->GetObjectGuid());
            }
            break;

        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FALLEN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 21);
            pPlayer->SEND_GOSSIP_MENU(1451, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+21:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FALLEN1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 22);
            pPlayer->SEND_GOSSIP_MENU(1452, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+22:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FALLEN2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 23);
            pPlayer->SEND_GOSSIP_MENU(1453, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+23:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FALLEN3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 24);
            pPlayer->SEND_GOSSIP_MENU(1454, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+24:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FALLEN4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 25);
            pPlayer->SEND_GOSSIP_MENU(1455, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+25:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FALLEN5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 26);
            pPlayer->SEND_GOSSIP_MENU(1456, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+26:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->AreaExploredOrEventHappens(2801);
            break;
    }
    return true;
}

/*######
## mob_lady_sevine
######*/

enum eSevine
{
    SPELL_IMMOLATE               = 12742,
    SPELL_FROST_ARMOR                = 12544,
    SPELL_CURSE_OF_WEAKNESS           = 12741,
    SPELL_SHADOW_BOLT         = 12739,
    SPELL_SUMMON_INFERNAL        = 12740,
    SPELL_FEL_CURSE = 12938
};

struct MANGOS_DLL_DECL mob_lady_sevineAI : public ScriptedAI
{
    mob_lady_sevineAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        DoCastSpellIfCan(m_creature, SPELL_FROST_ARMOR, CAST_AURA_NOT_PRESENT);
        Reset();
    }

    uint32 m_uiSummonCount;

    uint32 m_uiImmolateTimer;
    uint32 m_uiCurseTimer;
    uint32 m_uiShadowBoltTimer;
    uint32 m_uiSummonInfernal;
    
    bool m_bSalved;

    void Reset()
    {
        m_bSalved = false;
        m_uiSummonCount = 0;

        m_uiImmolateTimer = urand(4000, 6000);
        m_uiCurseTimer = urand(1000, 3000);
        m_uiShadowBoltTimer = urand(7000, 8000);
        m_uiSummonInfernal = urand(10000, 15000);
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoCastSpellIfCan(m_creature, SPELL_FROST_ARMOR, CAST_AURA_NOT_PRESENT);
    }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->AI()->AttackStart(m_creature->getVictim());
        ++m_uiSummonCount;
    }
    void SummonedCreatureJustDied(Creature* /*pSummoned*/)
    {
        --m_uiSummonCount;
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32& uiDamage)
    {
        if (!m_bSalved && (uiDamage >= m_creature->GetHealth()))   
            uiDamage = 0;
    }
    
    void SpellHit(Unit* /*pCaster*/, SpellEntry const* pSpell) // emote if he enrages
    {
        if (pSpell->Id == SPELL_FEL_CURSE)
        {
            m_creature->GenericTextEmote("Lady Sevine is substantially more susceptible to your attacks. The Felcurse is working!", NULL, false);
            m_creature->InterruptNonMeleeSpells(false);
            m_bSalved = true;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiShadowBoltTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_BOLT);
            m_uiShadowBoltTimer = urand(3000, 5000);
        }
        else
            m_uiShadowBoltTimer -= uiDiff;
        
        if (m_uiImmolateTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_IMMOLATE, CAST_INTERRUPT_PREVIOUS);
            m_uiImmolateTimer = urand(15000, 24000);
        }
        else
            m_uiImmolateTimer -= uiDiff;

        if (m_uiCurseTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CURSE_OF_WEAKNESS, CAST_AURA_NOT_PRESENT);
            m_uiCurseTimer = urand(30000, 45000);
        }
        else
            m_uiCurseTimer -= uiDiff;

        if (m_uiSummonInfernal <= uiDiff)
        {
            // change for summon spell
            if (m_uiSummonCount < 4)
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_INFERNAL, CAST_INTERRUPT_PREVIOUS);

            m_uiSummonInfernal = urand(15000, 25000);
        }
        else
            m_uiSummonInfernal -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_lady_sevine(Creature* pCreature)
{
    return new mob_lady_sevineAI(pCreature);
}

void AddSC_blasted_lands()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "npc_deathly_usher";
    pNewscript->pGossipHello =  &GossipHello_npc_deathly_usher;
    pNewscript->pGossipSelect = &GossipSelect_npc_deathly_usher;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_fallen_hero_of_horde";
    pNewscript->pGossipHello =  &GossipHello_npc_fallen_hero_of_horde;
    pNewscript->pGossipSelect = &GossipSelect_npc_fallen_hero_of_horde;
    pNewscript->RegisterSelf();
    
    pNewscript = new Script;
    pNewscript->Name = "mob_lady_sevine";
    pNewscript->GetAI = &GetAI_mob_lady_sevine;
    pNewscript->RegisterSelf();
}
