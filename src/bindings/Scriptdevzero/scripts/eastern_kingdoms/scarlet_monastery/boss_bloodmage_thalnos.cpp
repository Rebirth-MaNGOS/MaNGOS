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
SDName: Boss_Bloodmage_Thalnos
SD%Complete: 100
SDComment:
SDCategory: Scarlet Monastery
EndScriptData */

#include "precompiled.h"

enum
{
    SAY_AGGRO               = -1189016,
    SAY_HEALTH              = -1189017,
    SAY_KILL                = -1189018,

    SPELL_FLAMESHOCK        = 8053,
    SPELL_SHADOWBOLT        = 1106,
    SPELL_FLAMESPIKE        = 8814,
    SPELL_FIRENOVA          = 16079,
};

struct MANGOS_DLL_DECL boss_bloodmage_thalnosAI : public ScriptedAI
{
    boss_bloodmage_thalnosAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    bool bHpYell;
    uint32 m_uiFlameShockTimer;
    uint32 m_uiShadowBoltTimer;
    uint32 m_uiFlameSpikeTimer;
    uint32 m_uiFireNovaTimer;

    void Reset()
    {
        bHpYell = false;
        m_uiFlameShockTimer = 10000;
        m_uiShadowBoltTimer = 2000;
        m_uiFlameSpikeTimer = 8000;
        m_uiFireNovaTimer = 40000;
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void KilledUnit(Unit* /*pVictim*/)
    {
        DoScriptText(SAY_KILL, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // If we are <35% hp
        if (!bHpYell && HealthBelowPct(35))
        {
            DoScriptText(SAY_HEALTH, m_creature);
            bHpYell = true;
        }

        // Flame Shock
        if (m_uiFlameShockTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FLAMESHOCK);
            m_uiFlameShockTimer = urand(10000, 15000);
        }
        else 
            m_uiFlameShockTimer -= uiDiff;

        // Flame Spike
        if (m_uiFlameSpikeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FLAMESPIKE);
            m_uiFlameSpikeTimer = 30000;
        }
        else 
            m_uiFlameSpikeTimer -= uiDiff;

        // Fire Nova
        if (m_uiFireNovaTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FIRENOVA);
            m_uiFireNovaTimer = 40000;
        }
        else 
            m_uiFireNovaTimer -= uiDiff;

        // Shadow Bolt
        if (m_uiShadowBoltTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOWBOLT);
            m_uiShadowBoltTimer = 2000;
        }
        else 
            m_uiShadowBoltTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_bloodmage_thalnos(Creature* pCreature)
{
    return new boss_bloodmage_thalnosAI(pCreature);
}

void AddSC_boss_bloodmage_thalnos()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "boss_bloodmage_thalnos";
    pNewScript->GetAI = &GetAI_boss_bloodmage_thalnos;
    pNewScript->RegisterSelf();
}
