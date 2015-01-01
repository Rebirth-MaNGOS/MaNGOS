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
SDName: Boss Cho'Rush The Observer
SD%Complete: 95
SDComment:
SDCategory: Dire Maul
EndScriptData */

#include "precompiled.h"
#include "dire_maul.h"

#define GOSSIP_PLAYER "Ok, I'll let you live, so now, how about my tribute!"

enum eChorush
{
    // Gossips
    GOSSIP_CHORUSH_1        = 6918,
    GOSSIP_CHORUSH_2        = 6923,
    SAY_GORDOK_DEAD         = -1429012,

    // Mage spells
    SPELL_ARCANE_EXPLOSION  = 13745,
    SPELL_FIREBALL          = 17290,
    SPELL_FROST_NOVA        = 15531,

    // Priest spells
    SPELL_HEAL              = 22883,
    SPELL_MIND_BLAST        = 17194,
    SPELL_POWER_WORD_SHIELD = 17139,
    SPELL_PSYCHIC_SCREAM    = 22884,

    // Shaman spells
    SPELL_BLOODLUST         = 16170,
    SPELL_EARTHGRAB_TOTEM   = 8376,
    SPELL_HEALING_WAVE      = 15982,
    SPELL_CHAIN_LIGHTNING   = 15305,
    SPELL_LIGHTING_BOLT     = 15234,
};

static uint32 Spells[3][5]=
{
    {SPELL_FIREBALL, SPELL_FROST_NOVA, SPELL_ARCANE_EXPLOSION, 0, 0},
    {SPELL_MIND_BLAST, SPELL_PSYCHIC_SCREAM, SPELL_POWER_WORD_SHIELD, SPELL_HEAL, 0},
    {SPELL_LIGHTING_BOLT, SPELL_CHAIN_LIGHTNING, SPELL_BLOODLUST, SPELL_HEALING_WAVE, SPELL_EARTHGRAB_TOTEM}
};

struct MANGOS_DLL_DECL boss_chorush_the_observerAI : public ScriptedAI
{
    boss_chorush_the_observerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_dire_maul*)pCreature->GetInstanceData();
        Reset();

        // Set a phase on CreatureCreate
        uint8 uiPhase = urand(0,2);

        for(uint32 i = 0; i < 5; ++i)
            m_uiSpellEntry[i] = Spells[uiPhase][i];
    }

    instance_dire_maul* m_pInstance;

    uint32 m_uiSpellEntry[5];
    uint32 m_uiSpellTimer[5];

    void Reset()
    {
        m_uiSpellTimer[0] = urand(0,1000);
        m_uiSpellTimer[1] = urand(5000,7000);
        m_uiSpellTimer[2] = urand(12000,15000);
        m_uiSpellTimer[3] = urand(20000,25000);
        m_uiSpellTimer[4] = urand(25000,30000);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_CHORUSH_THE_OBSERVER, NOT_STARTED);
            if (m_pInstance->GetData(TYPE_KING_GORDOK) == DONE)
            {
                DoScriptText(SAY_GORDOK_DEAD, m_creature);
                m_creature->SetStandState(UNIT_STAND_STATE_SIT);
            }
        }
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHORUSH_THE_OBSERVER, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHORUSH_THE_OBSERVER, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // First spell
        if (m_uiSpellTimer[0] <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), m_uiSpellEntry[0]);
            m_uiSpellTimer[0] = urand(7000,12000);
        }
        else
            m_uiSpellTimer[0] -= uiDiff;

        // Second spell
        if (m_uiSpellTimer[1] <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), m_uiSpellEntry[1]);
            m_uiSpellTimer[1] = urand(12000,18000);
        }
        else
            m_uiSpellTimer[1] -= uiDiff;

        // Third spell
        if (m_uiSpellTimer[2] <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, m_uiSpellEntry[2]);
            m_uiSpellTimer[2] = urand(15000,20000);
        }
        else
            m_uiSpellTimer[2] -= uiDiff;

        // Fourth spell
        if (m_uiSpellEntry[3])
        {
            if (m_uiSpellTimer[3] <= uiDiff)
            {
                if (m_pInstance)
                {
					Creature* pGordok = m_pInstance->GetSingleCreatureFromStorage(NPC_KING_GORDOK);
                    if (pGordok && pGordok->isAlive() && pGordok->GetHealthPercent() < 75.0f)
                        DoCastSpellIfCan(pGordok, m_uiSpellEntry[3]);
                    else
                        DoCastSpellIfCan(m_creature, m_uiSpellEntry[3]);
                }
                m_uiSpellTimer[3] = urand(10000,20000);
            }
            else
                m_uiSpellTimer[3] -= uiDiff;
        }

        // Fifth spell
        if (m_uiSpellEntry[4])
        {
            if (m_uiSpellTimer[4] <= uiDiff)
            {
                DoCastSpellIfCan(m_creature, m_uiSpellEntry[4]);
                m_uiSpellTimer[4] = urand(20000,30000);
            }
            else
                m_uiSpellTimer[4] -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_chorush_the_observer(Creature* pCreature)
{
    return new boss_chorush_the_observerAI(pCreature);
}

bool GossipHello_boss_chorush_the_observer(Player* pPlayer, Creature* pCreature)
{
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_PLAYER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
	pPlayer->SEND_GOSSIP_MENU(GOSSIP_CHORUSH_1, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_boss_chorush_the_observer(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_CHORUSH_2, pCreature->GetObjectGuid());

    return true;
}

void AddSC_boss_chorush_the_observer()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_chorush_the_observer";
    pNewscript->GetAI = &GetAI_boss_chorush_the_observer;
    pNewscript->pGossipHello = &GossipHello_boss_chorush_the_observer;
    pNewscript->pGossipSelect = &GossipSelect_boss_chorush_the_observer;
    pNewscript->RegisterSelf();
}
