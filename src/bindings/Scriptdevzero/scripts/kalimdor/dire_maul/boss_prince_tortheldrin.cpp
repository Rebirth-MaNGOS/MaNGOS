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
SDName: Boss Prince Tortheldrin
SD%Complete:
SDComment:
SDCategory: Dire Maul
EndScriptData */

#include "precompiled.h"
#include "dire_maul.h"

enum Spells
{
    SPELL_ARCANE_BLAST          = 22920,
    SPELL_COUNTERSPELL          = 20537,
    SPELL_TRASH                 = 3391,
    SPELL_WHIRLWIND             = 15589,
    GOSSIP_PRINCE_TORTHELDRIN   = 6909,
};

struct MANGOS_DLL_DECL boss_prince_tortheldrinAI : public ScriptedAI
{
    boss_prince_tortheldrinAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_dire_maul*)pCreature->GetInstanceData();
        Reset();
    }

    instance_dire_maul* m_pInstance;

    uint32 m_uiArcaneBlastTimer;
    uint32 m_uiCounterspellTimer;
    uint32 m_uiTrashTimer;
    uint32 m_uiWhirlwindTimer;

    void Reset()
    {
        m_uiArcaneBlastTimer = urand(6000,8000);
        m_uiCounterspellTimer = urand(5000,7000);
        m_uiTrashTimer = urand(3000,5000);
        m_uiWhirlwindTimer = urand(6000,8000);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_PRINCE_TORTHELDRIN, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_PRINCE_TORTHELDRIN, IN_PROGRESS);
            /*if (m_pInstance->GetData(TYPE_IMMOLTHAR) == DONE)
                DoScriptText(SAY_AGGRO, m_creature);*/
        }
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_PRINCE_TORTHELDRIN, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Arcane Blast
        if (m_uiArcaneBlastTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_ARCANE_BLAST);
            m_uiArcaneBlastTimer = urand(8000,10000);
        }
        else
            m_uiArcaneBlastTimer -= uiDiff;

        // Counterspell
        if (m_uiCounterspellTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_COUNTERSPELL);
            m_uiCounterspellTimer = urand(7000,9000);
        }
        else
            m_uiCounterspellTimer -= uiDiff;

        // Trash
        if (m_uiTrashTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_TRASH);
            m_uiTrashTimer = urand(6000,8000);
        }
        else
            m_uiTrashTimer -= uiDiff;

        // Whirlwind
        if (m_uiWhirlwindTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_WHIRLWIND);
            m_uiWhirlwindTimer = urand(6000,8000);
        }
        else
            m_uiWhirlwindTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_prince_tortheldrin(Creature* pCreature)
{
    return new boss_prince_tortheldrinAI(pCreature);
}

bool GossipHello_boss_prince_tortheldrin(Player* pPlayer, Creature* pCreature)
{
	pPlayer->SEND_GOSSIP_MENU(GOSSIP_PRINCE_TORTHELDRIN, pCreature->GetObjectGuid());
    return true;
}

void AddSC_boss_prince_tortheldrin()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_prince_tortheldrin";
    pNewscript->GetAI = &GetAI_boss_prince_tortheldrin;
    pNewscript->pGossipHello = &GossipHello_boss_prince_tortheldrin;
    pNewscript->RegisterSelf();
}
