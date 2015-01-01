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
SDName: Boss_Illucia_Barov
SD%Complete: 100
SDComment:
SDCategory: Scholomance
EndScriptData */

#include "precompiled.h"
#include "scholomance.h"

enum Spells
{
    SPELL_CURSE_OF_AGONY      = 18671,
    SPELL_DOMINATE_MIND       = 14515,
    SPELL_FEAR                = 12542,
    SPELL_SHADOW_SHOCK        = 17289,
    SPELL_SILENCE             = 12528
};

struct MANGOS_DLL_DECL boss_lady_illucia_barovAI : public ScriptedAI
{
    boss_lady_illucia_barovAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_scholomance*)pCreature->GetInstanceData();
        Reset();
    }

    instance_scholomance* m_pInstance;

    uint32 m_uiCurseOfAgonyTimer;
    uint32 m_uiDominateMindTimer;
    uint32 m_uiShadowShockTimer;
    uint32 m_uiSilenceTimer;
    uint32 m_uiFearTimer;

    void Reset()
    {
        m_uiCurseOfAgonyTimer = urand(12000,14000);
        m_uiDominateMindTimer = urand(17000,20000);
        m_uiShadowShockTimer = urand(2000,4000);
        m_uiSilenceTimer = urand(6000,9000);
        m_uiFearTimer = urand(23000,25000);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LADY_ILLUCIA_BAROV, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LADY_ILLUCIA_BAROV, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LADY_ILLUCIA_BAROV, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Curse Of Agony
        if (m_uiCurseOfAgonyTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CURSE_OF_AGONY);
            m_uiCurseOfAgonyTimer = urand(35000,45000);
        }
        else
            m_uiCurseOfAgonyTimer -= uiDiff;

        // Dominate Mind
        if (m_uiDominateMindTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, SPELL_DOMINATE_MIND);
            m_uiDominateMindTimer = urand(25000,35000);
        }
        else
            m_uiDominateMindTimer -= uiDiff;

        // Fear
        if (m_uiFearTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 1))
                DoCastSpellIfCan(pTarget, SPELL_FEAR);
            m_uiFearTimer = urand(10000,15000);
        }
        else
            m_uiFearTimer -= uiDiff;

        // Shadow Shock
        if (m_uiShadowShockTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_SHOCK);
            m_uiShadowShockTimer = urand(6000,9000);
        }
        else
            m_uiShadowShockTimer -= uiDiff;

        // Silence
        if (m_uiSilenceTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, SPELL_SILENCE);
            m_uiSilenceTimer = urand (10000,15000);
        }
        else
            m_uiSilenceTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_lady_illucia_barov(Creature* pCreature)
{
    return new boss_lady_illucia_barovAI(pCreature);
}

void AddSC_boss_lady_illucia_barov()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_lady_illucia_barov";
    pNewscript->GetAI = &GetAI_boss_lady_illucia_barov;
    pNewscript->RegisterSelf();
}
