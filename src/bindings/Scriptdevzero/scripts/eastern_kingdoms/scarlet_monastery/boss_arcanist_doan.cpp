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
SDName: Boss_Arcanist_Doan
SD%Complete: 100
SDComment:
SDCategory: Scarlet Monastery
EndScriptData */

#include "precompiled.h"

enum
{
    SAY_AGGRO                   = -1189019,
    SAY_SPECIALAE               = -1189020,

    SPELL_POLYMORPH             = 13323,
    SPELL_AOESILENCE            = 8988,
    SPELL_ARCANE_EXPLOSION      = 9433,
    SPELL_FIREAOE               = 9435,
    SPELL_ARCANE_BUBBLE         = 9438,
};

struct MANGOS_DLL_DECL boss_arcanist_doanAI : public ScriptedAI
{
    boss_arcanist_doanAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiPolymorphTimer;
    uint32 m_uiAoESilenceTimer;
    uint32 m_uiArcaneExplosionTimer;
    bool bCanDetonate;
    bool bShielded;

    void Reset()
    {
        m_uiPolymorphTimer = 20000;
        m_uiAoESilenceTimer = 15000;
        m_uiArcaneExplosionTimer = 3000;
        bCanDetonate = false;
        bShielded = false;
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (bShielded && bCanDetonate)
        {
            DoCastSpellIfCan(m_creature, SPELL_FIREAOE);
            bCanDetonate = false;
        }

        if (m_creature->HasAura(SPELL_ARCANE_BUBBLE))
            return;

        // If we are <50% hp cast Arcane Bubble
        if (!bShielded && HealthBelowPct(50))
        {
            // wait if we already casting
            if (m_creature->IsNonMeleeSpellCasted(false))
                return;

            DoScriptText(SAY_SPECIALAE, m_creature);
            DoCastSpellIfCan(m_creature, SPELL_ARCANE_BUBBLE);

            bCanDetonate = true;
            bShielded = true;
        }

        if (m_uiPolymorphTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                DoCastSpellIfCan(pTarget, SPELL_POLYMORPH);

            m_uiPolymorphTimer = 20000;
        }
        else 
            m_uiPolymorphTimer -= uiDiff;

        // AoE Silence
        if (m_uiAoESilenceTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_AOESILENCE);
            m_uiAoESilenceTimer = urand(15000, 20000);
        }else m_uiAoESilenceTimer -= uiDiff;

        // Arcane Explosion
        if (m_uiArcaneExplosionTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_ARCANE_EXPLOSION);
            m_uiArcaneExplosionTimer = 8000;
        }
        else
            m_uiArcaneExplosionTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_arcanist_doan(Creature* pCreature)
{
    return new boss_arcanist_doanAI(pCreature);
}

void AddSC_boss_arcanist_doan()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "boss_arcanist_doan";
    pNewScript->GetAI = &GetAI_boss_arcanist_doan;
    pNewScript->RegisterSelf();
}
