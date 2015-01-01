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
SDName: Boss_Vectus
SD%Complete: 90
SDComment:
SDCategory: Scholomance
EndScriptData */

#include "precompiled.h"
#include "scholomance.h"

enum Spells
{
    SAY_MARDUK                  = -1289000,

    SPELL_FLAMESTRIKE           = 18399,
    SPELL_BLAST_WAVE            = 16046    
};

struct MANGOS_DLL_DECL boss_vectusAI : public ScriptedAI
{
    boss_vectusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiFlameStrikeTimer;
    uint32 m_uiBlastWaveTimer;

    void Reset()
    {
        m_uiFlameStrikeTimer = urand(2000,4000);
        m_uiBlastWaveTimer = urand(7000,10000);
    }

    void Aggro(Unit* pWho)
    {
        if (Creature* pMarduk = GetClosestCreatureWithEntry(m_creature, NPC_MARDUK_BLACKPOOL, 30.0f))
        {
            DoScriptText(SAY_MARDUK, pMarduk);
            pMarduk->SetInCombatWithZone();
            pMarduk->AI()->AttackStart(pWho);
        }

        std::list<Creature*> m_lStudent;
        GetCreatureListWithEntryInGrid(m_lStudent, m_creature, NPC_SCHOLOMANCE_STUDENT, DEFAULT_VISIBILITY_INSTANCE);
        if (!m_lStudent.empty())
        {
            for(std::list<Creature*>::iterator itr = m_lStudent.begin(); itr != m_lStudent.end(); ++itr)
            {
                if ((*itr) && (*itr)->isAlive())
                {
                    (*itr)->setFaction(FACTION_HOSTILE);
                    (*itr)->SetInCombatWithZone();
                    (*itr)->AI()->AttackStart(pWho);
                }
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Flame Strike
        if (m_uiFlameStrikeTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_FLAMESTRIKE);
            m_uiFlameStrikeTimer = urand(5000,8000);
        }
        else
            m_uiFlameStrikeTimer -= uiDiff;

        // Blast Wave
        if (m_uiBlastWaveTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BLAST_WAVE);
            m_uiBlastWaveTimer = urand(10000,12000);
        }
        else
            m_uiBlastWaveTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_vectus(Creature* pCreature)
{
    return new boss_vectusAI(pCreature);
}

void AddSC_boss_vectus()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_vectus";
    pNewscript->GetAI = &GetAI_boss_vectus;
    pNewscript->RegisterSelf();
}
