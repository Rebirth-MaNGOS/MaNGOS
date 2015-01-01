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
SDName: Boss_Overlord_Wyrmthalak
SD%Complete: 100
SDComment:
SDCategory: Blackrock Spire
EndScriptData */

#include "precompiled.h"
#include "blackrock_spire.h"

const uint32 aSummonEntry[3] = {NPC_SPIRESTONE_WARLORD, NPC_SMOLDERTHORN_BERSERKER, NPC_BLOODAXE_VETERAN};

enum Spells
{
    SPELL_CLEAVE              = 15284,
    SPELL_DEMORALIZING_SHOUT  = 16244,
    SPELL_SWEEPING_SLAM       = 12887
};

static Loc Spawn[]=
{
    {-39.35f,-513.45f,88.47f},
    {-49.87f,-511.89f,88.19f}
};

struct MANGOS_DLL_DECL boss_overlord_wyrmthalakAI : public ScriptedAI
{
    boss_overlord_wyrmthalakAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    bool m_bSummoned;
    uint32 m_uiCleaveTimer;
    uint32 m_uiDemoralizingShoutTimer;
    uint32 m_uiSweepingSlamTimer;

    void Reset()
    {
        m_bSummoned = false;
        m_uiCleaveTimer = urand(5000,6000);
        m_uiDemoralizingShoutTimer = urand(1000,3000);
        m_uiSweepingSlamTimer = urand(8000,10000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Cleave
        if (m_uiCleaveTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE);
            m_uiCleaveTimer = urand(3000,6000);
        }
        else
            m_uiCleaveTimer -= uiDiff;
        
        // Demoralizing Shout
        if (m_uiDemoralizingShoutTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_DEMORALIZING_SHOUT);
            m_uiDemoralizingShoutTimer = urand(15000,20000);
        }
        else
            m_uiDemoralizingShoutTimer -= uiDiff;

        // Sweeping Slam
        if (m_uiSweepingSlamTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SWEEPING_SLAM);
            m_uiSweepingSlamTimer = urand(6000,9000);
        }
        else
            m_uiSweepingSlamTimer -= uiDiff;

        // Summons
        if (!m_bSummoned && HealthBelowPct(50))
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
            for(uint32 i = 0; i < sizeof(Spawn)/sizeof(Loc); ++i)
            {
                if (Creature* pSpawn = m_creature->SummonCreature(aSummonEntry[urand(0,2)], Spawn[i].x, Spawn[i].y, Spawn[i].z, 4.61f, TEMPSUMMON_DEAD_DESPAWN, 30000))
                    pSpawn->AI()->AttackStart(pTarget ? pTarget : m_creature->getVictim());
            }
            m_bSummoned = true;
        }

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_overlord_wyrmthalak(Creature* pCreature)
{
    return new boss_overlord_wyrmthalakAI(pCreature);
}

void AddSC_boss_overlord_wyrmthalak()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_overlord_wyrmthalak";
    pNewscript->GetAI = &GetAI_boss_overlord_wyrmthalak;
    pNewscript->RegisterSelf();
}
