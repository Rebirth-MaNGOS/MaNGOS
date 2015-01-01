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
SDName: Boss King Gordok
SD%Complete:
SDComment:
SDCategory: Dire Maul
EndScriptData */

#include "precompiled.h"
#include "dire_maul.h"

enum eKingGordok
{
    SPELL_BERSERKER_CHARGE  = 22886,
    SPELL_MORTAL_STRIKE     = 15708,
    SPELL_SUNDER_ARMOR      = 15572,
    SPELL_WAR_STOMP         = 16727,

    SAY_AGGRO               = -1429013,
};

struct MANGOS_DLL_DECL boss_king_gordokAI : public ScriptedAI
{
    boss_king_gordokAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_dire_maul*)pCreature->GetInstanceData();
        Reset();
    }

    instance_dire_maul* m_pInstance;

    uint32 m_uiBerserkerChargeTimer;
    uint32 m_uiMortalStrikeTimer;
    uint32 m_uiSunderArmorTimer;
    uint32 m_uiWarStompTimer;

    void Reset()
    {
        m_uiBerserkerChargeTimer = 1000;
        m_uiMortalStrikeTimer = urand(3000,5000);
        m_uiSunderArmorTimer = urand(4000,6000);
        m_uiWarStompTimer = urand(6000,7000);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KING_GORDOK, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KING_GORDOK, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KING_GORDOK, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {            
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Berserker Charge
        if (m_uiBerserkerChargeTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_BERSERKER_CHARGE);
            m_uiBerserkerChargeTimer = urand(7000,9000);
        }
        else
            m_uiBerserkerChargeTimer -= uiDiff;

        // Mortal Strike
        if (m_uiMortalStrikeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MORTAL_STRIKE);
            m_uiMortalStrikeTimer = urand(6000,8000);
        }
        else
            m_uiMortalStrikeTimer -= uiDiff;

        // Sunder Armor
        if (m_uiSunderArmorTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SUNDER_ARMOR);
            m_uiSunderArmorTimer = urand(5000,6000);
        }
        else
            m_uiSunderArmorTimer -= uiDiff;

        // War Stomp
        if (m_uiWarStompTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_WAR_STOMP);
            m_uiWarStompTimer = urand(8000,10000);
        }
        else
            m_uiWarStompTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_king_gordok(Creature* pCreature)
{
    return new boss_king_gordokAI(pCreature);
}

void AddSC_boss_king_gordok()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_king_gordok";
    pNewscript->GetAI = &GetAI_boss_king_gordok;
    pNewscript->RegisterSelf();
}
