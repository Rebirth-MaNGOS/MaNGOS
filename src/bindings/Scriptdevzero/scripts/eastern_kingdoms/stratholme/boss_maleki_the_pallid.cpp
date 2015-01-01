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
SDName: Boss_Maleki_the_Pallid
SD%Complete: 90
SDComment:
SDCategory: Stratholme
EndScriptData */

#include "precompiled.h"
#include "stratholme.h"

enum Spells
{
    SPELL_DRAIN_LIFE          = 17238,
    SPELL_DRAIN_MANA          = 17243,
    SPELL_FROSTBOLT           = 17503,
    SPELL_ICE_TOMB            = 16869
};

struct MANGOS_DLL_DECL boss_maleki_the_pallidAI : public ScriptedAI
{
    boss_maleki_the_pallidAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_stratholme*)pCreature->GetInstanceData();
        Reset();
    }

    instance_stratholme* m_pInstance;

    uint32 m_uiDrainManaTimer;
    uint32 m_uiDrainLifeTimer;
    uint32 m_uiFrostboltTimer;
    uint32 m_uiIceTombTimer;

    void Reset()
    {
        m_uiDrainManaTimer = urand(16000,20000);
        m_uiDrainLifeTimer = urand(12000,14000);
        m_uiFrostboltTimer = urand(2000,4000);
        m_uiIceTombTimer = urand(6000,8000);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MALEKI_THE_PALLID, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MALEKI_THE_PALLID, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MALEKI_THE_PALLID, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Drain Mana spell
        if (m_uiDrainManaTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_DRAIN_LIFE);
            m_uiDrainManaTimer = urand(15000,20000);
        }
        else
            m_uiDrainManaTimer -= uiDiff;

        // Drain Life spell
        if (m_uiDrainLifeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_DRAIN_LIFE);
            m_uiDrainLifeTimer = urand(15000,20000);
        }
        else
            m_uiDrainLifeTimer -= uiDiff;

        // Frostbolt spell
        if (m_uiFrostboltTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FROSTBOLT);
            m_uiFrostboltTimer = urand(3000,6000);
        }
        else
            m_uiFrostboltTimer -= uiDiff;

        // Ice Tomb spell
        if (m_uiIceTombTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_ICE_TOMB);
            m_uiIceTombTimer = urand(15000,20000);
        }
        else
            m_uiIceTombTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_maleki_the_pallid(Creature* pCreature)
{
    return new boss_maleki_the_pallidAI(pCreature);
}

void AddSC_boss_maleki_the_pallid()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_maleki_the_pallid";
    pNewscript->GetAI = &GetAI_boss_maleki_the_pallid;
    pNewscript->RegisterSelf();
}
