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
SDName: Boss_Nerubenkan
SD%Complete: 90
SDComment:
SDCategory: Stratholme
EndScriptData */

#include "precompiled.h"
#include "stratholme.h"

enum Spells
{
    SPELL_ENCASING_WEBS             = 4962,
    SPELL_PIERCE_ARMOR              = 6016,
    SPELL_RAISE_UNDEAD_SCARAB       = 17235
};

struct MANGOS_DLL_DECL boss_nerubenkanAI : public ScriptedAI
{
    boss_nerubenkanAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_stratholme*)pCreature->GetInstanceData();
        Reset();
    }

    instance_stratholme* m_pInstance;

    uint32 m_uiEncasingWebsTimer;
    uint32 m_uiPierceArmorTimer;
    uint32 m_uiRaiseUndeadScarabTimer;

    void Reset()
    {
        m_uiEncasingWebsTimer = urand(6000,8000);
        m_uiPierceArmorTimer = urand(12000,13000);
        m_uiRaiseUndeadScarabTimer = urand(2000,4000);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NERUBENKAN, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NERUBENKAN, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NERUBENKAN, DONE);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
            pSummoned->AI()->AttackStart(pTarget);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Encasing Webs spell
        if (m_uiEncasingWebsTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_ENCASING_WEBS);
            m_uiEncasingWebsTimer = urand(10000,15000);
        }
        else
            m_uiEncasingWebsTimer -= uiDiff;

        // Pierce Armor spell
        if (m_uiPierceArmorTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_PIERCE_ARMOR);
            m_uiPierceArmorTimer = urand(15000,25000);
        }
        else
            m_uiPierceArmorTimer -= uiDiff;

        // Raise Undead Scarab
        if (m_uiRaiseUndeadScarabTimer <= uiDiff)
        {
            //m_uiSummonCount = m_bInitialSpawn ? 5 : 3;
            //for(uint32 i = 0; i < m_uiSummonCount; ++i)

            DoSpawnCreature(NPC_UNDEAD_SCARAB, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);

            //if (m_bInitialSpawn)
            //    m_bInitialSpawn = false;

            m_uiRaiseUndeadScarabTimer = urand(5000,8000);
        }
        else
            m_uiRaiseUndeadScarabTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_nerubenkan(Creature* pCreature)
{
    return new boss_nerubenkanAI(pCreature);
}

void AddSC_boss_nerubenkan()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_nerubenkan";
    pNewscript->GetAI = &GetAI_boss_nerubenkan;
    pNewscript->RegisterSelf();
}
