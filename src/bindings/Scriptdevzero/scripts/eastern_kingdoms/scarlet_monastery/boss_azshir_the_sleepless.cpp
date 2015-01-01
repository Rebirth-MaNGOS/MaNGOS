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
SDName: Boss_Azshir_the_Sleepless
SD%Complete: 80
SDComment:
SDCategory: Scarlet Monastery
EndScriptData */

#include "precompiled.h"

enum eAzshirTheSleepless
{
    SPELL_CALL_OF_THE_GRAVE         = 17831,
    SPELL_TERRIFY                   = 7399,
    SPELL_SOUL_SIPHON               = 7290,
};

struct MANGOS_DLL_DECL boss_azshir_the_sleeplessAI : public ScriptedAI
{
    boss_azshir_the_sleeplessAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiSoulSiphonTimer;
    uint32 m_uiCallOftheGraveTimer;
    uint32 m_uiTerrifyTimer;

    void Reset()
    {
        m_uiSoulSiphonTimer = 1;
        m_uiCallOftheGraveTimer = 30000;
        m_uiTerrifyTimer = 20000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // If we are <50% hp cast Soul Siphon rank 1
        if (HealthBelowPct(50) && !m_creature->IsNonMeleeSpellCasted(false))
        {
            // Soul Siphon
            if (m_uiSoulSiphonTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_SOUL_SIPHON);
                m_uiSoulSiphonTimer = 20000;
                return;
            }
            else 
                m_uiSoulSiphonTimer -= uiDiff;
        }

        // Call of the Grave
        if (m_uiCallOftheGraveTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CALL_OF_THE_GRAVE);
            m_uiCallOftheGraveTimer = 30000;
        }
        else 
            m_uiCallOftheGraveTimer -= uiDiff;

        // Terrify
        if (m_uiTerrifyTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_TERRIFY);
            m_uiTerrifyTimer = 20000;
        }
        else 
            m_uiTerrifyTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_azshir_the_sleepless(Creature* pCreature)
{
    return new boss_azshir_the_sleeplessAI(pCreature);
}

void AddSC_boss_azshir_the_sleepless()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "boss_azshir_the_sleepless";
    pNewScript->GetAI = &GetAI_boss_azshir_the_sleepless;
    pNewScript->RegisterSelf();
}
