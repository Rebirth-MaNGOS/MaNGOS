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
SDName: Boss Hydrospawn
SD%Complete:
SDComment:
SDCategory: Dire Maul
EndScriptData */

#include "precompiled.h"
#include "dire_maul.h"

enum Spells
{
    // Hydrospawn
    SPELL_MASSIVE_GEYSER    = 22421,    // not used, manual spawn
    SPELL_RIPTIDE           = 22419,
    SPELL_SUBMERSION        = 22420,
    SPELL_SUMMON_HYDROLING  = 22714,    // not used, manual spawn

    // Massive Geyser
    SPELL_WATER             = 22422,
};

struct MANGOS_DLL_DECL boss_hydrospawnAI : public ScriptedAI
{
    boss_hydrospawnAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiMassiveGeyserTimer;
    uint32 m_uiRiptideTimer;
    uint32 m_uiSubmersionTimer;
    uint32 m_uiSummonHydrolingTimer;

    void Reset()
    {
        m_uiMassiveGeyserTimer = urand(6000,8000);
        m_uiRiptideTimer = urand(0,3000);
        m_uiSubmersionTimer = urand(10000,12000);
        m_uiSummonHydrolingTimer = urand(15000,18000);
    }

    void Aggro(Unit* /*pWho*/)
    {
    }

    void JustSummoned(Creature* pSummoned)
    {
        Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);

        if (!pTarget && !m_creature->getVictim())
            return;

        pSummoned->AI()->AttackStart(pTarget ? pTarget : m_creature->getVictim());
    }

    void UpdateAI(const uint32 uiDiff)
    {       
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Massive Geyser
        if (m_uiMassiveGeyserTimer <= uiDiff)
        {
            DoSpawnCreature(NPC_MASSIVE_GEYSER, irand(-5,5), irand(-5,5), 0, 0, TEMPSUMMON_MANUAL_DESPAWN, 0);
            m_uiMassiveGeyserTimer = urand(10000,15000);
        }
        else
            m_uiMassiveGeyserTimer -= uiDiff;

        // Riptide
        if (m_uiRiptideTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_RIPTIDE);
            m_uiRiptideTimer = urand(4000,8000);
        }
        else
            m_uiRiptideTimer -= uiDiff;

        // Submersion
        if (m_uiSubmersionTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SUBMERSION);
            m_uiSubmersionTimer = urand(20000,40000);
        }
        else
            m_uiSubmersionTimer -= uiDiff;

        // Summon Hydroling
        if (m_uiSummonHydrolingTimer <= uiDiff)
        {
            DoSpawnCreature(NPC_HYDROLING, irand(-5,5), irand(-5,5), 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
            m_uiSummonHydrolingTimer = urand(10000,15000);
        }
        else
            m_uiSummonHydrolingTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_hydrospawn(Creature* pCreature)
{
    return new boss_hydrospawnAI(pCreature);
}

struct MANGOS_DLL_DECL mob_massive_geyserAI : public ScriptedAI
{
    mob_massive_geyserAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    bool m_bWatered;

    void Reset()
    {
        m_bWatered = false;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void UpdateAI(const uint32 /*uiDiff*/)
    {       
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_bWatered && m_creature->IsWithinDistInMap(m_creature->getVictim(), ATTACK_DISTANCE))
        {
            m_creature->CastSpell(m_creature->getVictim(), SPELL_WATER, true);
            m_creature->ForcedDespawn(1000);
            m_bWatered = true;
        }
    }
};

CreatureAI* GetAI_mob_massive_geyser(Creature* pCreature)
{
    return new mob_massive_geyserAI(pCreature);
}

void AddSC_boss_hydrospawn()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_hydrospawn";
    pNewscript->GetAI = &GetAI_boss_hydrospawn;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_massive_geyser";
    pNewscript->GetAI = &GetAI_mob_massive_geyser;
    pNewscript->RegisterSelf();
}
