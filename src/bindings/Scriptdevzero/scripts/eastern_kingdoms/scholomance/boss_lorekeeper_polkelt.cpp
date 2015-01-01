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
SDName: Boss_Lorekeeper_Polkelt
SD%Complete: 100
SDComment:
SDCategory: Scholomance
EndScriptData */

#include "precompiled.h"
#include "scholomance.h"

enum Spells
{
    SPELL_CORROSIVE_ACID          = 8245,
    SPELL_DARK_PLAGUE             = 18270,
    SPELL_NOXIOUS_CATALYST        = 18151,
    SPELL_VOLATILE_INFECTION      = 18149
};

struct MANGOS_DLL_DECL boss_lorekeeper_polkeltAI : public ScriptedAI
{
    boss_lorekeeper_polkeltAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_scholomance*)pCreature->GetInstanceData();
        Reset();
    }

    instance_scholomance* m_pInstance;

    uint32 m_uiCorrosiveAcidTimer;
    uint32 m_uiDarkPlagueTimer;
    uint32 m_uiNoxiousCatalystTimer;
    uint32 m_uiVolatileInfectionTimer;

    void Reset()
    {
        m_uiCorrosiveAcidTimer = 15000;
        m_uiDarkPlagueTimer = 8000;
        m_uiNoxiousCatalystTimer = 10000;
        m_uiVolatileInfectionTimer = 20000;
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LOREKEEPER_POLKELT, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LOREKEEPER_POLKELT, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LOREKEEPER_POLKELT, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Corrosive Acid
        if (m_uiCorrosiveAcidTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CORROSIVE_ACID);
            m_uiCorrosiveAcidTimer = urand(25000,30000);
        }
        else
            m_uiCorrosiveAcidTimer -= uiDiff;

        // Dark Plague
        if (m_uiDarkPlagueTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_DARK_PLAGUE);
            m_uiDarkPlagueTimer = urand(15000,20000);
        }
        else
            m_uiDarkPlagueTimer -= uiDiff;

        // Noxious Catalyst
        if (m_uiNoxiousCatalystTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_NOXIOUS_CATALYST);
            m_uiNoxiousCatalystTimer = 45000;
        }
        else
            m_uiNoxiousCatalystTimer -= uiDiff;

        // Volatile Infection
        if (m_uiVolatileInfectionTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_VOLATILE_INFECTION);
            m_uiVolatileInfectionTimer = urand(30000,40000);
        }
        else
            m_uiVolatileInfectionTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_lorekeeper_polkelt(Creature* pCreature)
{
    return new boss_lorekeeper_polkeltAI(pCreature);
}

void AddSC_boss_lorekeeper_polkelt()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_lorekeeper_polkelt";
    pNewscript->GetAI = &GetAI_boss_lorekeeper_polkelt;
    pNewscript->RegisterSelf();
}
