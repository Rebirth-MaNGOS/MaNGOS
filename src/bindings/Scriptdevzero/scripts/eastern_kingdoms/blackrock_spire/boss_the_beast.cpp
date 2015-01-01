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
SDName: Boss_The_Best
SD%Complete: 100
SDComment:
SDCategory: Blackrock Spire
EndScriptData */

#include "precompiled.h"
#include "blackrock_spire.h"

enum eTheBeast
{
    SPELL_BERSERKER_CHARGE = 16636,
    SPELL_FIRE_BLAST       = 16144,
    SPELL_FIREBALL         = 16788,
    SPELL_FLAMEBREAK       = 16785,
    SPELL_IMMOLATE         = 15570,
    SPELL_SUMMON_PLAYER    = 20477,
    SPELL_TERRYIFING_ROAR  = 14100
};

struct MANGOS_DLL_DECL boss_the_beastAI : public ScriptedAI
{
    boss_the_beastAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiBerserkerChargeTimer;
    uint32 m_uiFireBlastTimer;
    uint32 m_uiFireballTimer;
    uint32 m_uiFlamebreakTimer;
    uint32 m_uiImmolateTimer;
    uint32 m_uiSummonPlayerTimer;
    uint32 m_uiTerryifingRoarTimer;

    void Reset()
    {
        m_uiBerserkerChargeTimer = 0;
        m_uiFireBlastTimer = urand(5000, 10000);
        m_uiFireballTimer = 8000;
        m_uiFlamebreakTimer = 12000;
        m_uiImmolateTimer = 3000;
        m_uiSummonPlayerTimer = urand(13000, 18000);
        m_uiTerryifingRoarTimer = urand(16000, 20000);
    }

    void JustDied(Unit* /*pKiller*/)
    {
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pSpell->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_SKINNING || pSpell->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_SKIN_PLAYER_CORPSE)
        {
            float fX, fY, fZ;
            pCaster->GetPosition(fX, fY, fZ);
            pCaster->SummonCreature(NPC_FINKLE_EINHORN, fX, fY, fZ, 0.f, TEMPSUMMON_DEAD_DESPAWN, 0);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Berserker Charge
        if (m_uiBerserkerChargeTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan((pTarget ? pTarget : m_creature->getVictim()), SPELL_BERSERKER_CHARGE);
            m_uiBerserkerChargeTimer = urand(10000,15000);
        }
        else
            m_uiBerserkerChargeTimer -= uiDiff;

        // Fire Blast
        if (m_uiFireBlastTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FIRE_BLAST);
            m_uiFireBlastTimer = urand(7000,10000);
        }
        else
            m_uiFireBlastTimer -= uiDiff;

        // Fire Ball
        if (m_uiFireballTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FIREBALL);
            m_uiFireballTimer = urand(8000,10000);
        }
        else
            m_uiFireballTimer -= uiDiff;

        // Flame Break
        if (m_uiFlamebreakTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FLAMEBREAK);
            m_uiFlamebreakTimer = urand(8000,10000);
        }
        else
            m_uiFlamebreakTimer -= uiDiff;

        // Immolate
        if (m_uiImmolateTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan((pTarget ? pTarget : m_creature->getVictim()), SPELL_IMMOLATE);
            m_uiImmolateTimer = urand(5000,7000);
        }
        else
            m_uiImmolateTimer -= uiDiff;

        // Summon Player
        if (m_uiSummonPlayerTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_SUMMON_PLAYER);
            m_uiSummonPlayerTimer = 10000;
        }
        else
            m_uiSummonPlayerTimer -= uiDiff;

        // Terrifying Roar
        if (m_uiTerryifingRoarTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_TERRYIFING_ROAR);
            m_uiTerryifingRoarTimer = urand(15000,20000);
        }
        else
            m_uiTerryifingRoarTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_the_beast(Creature* pCreature)
{
    return new boss_the_beastAI(pCreature);
}

void AddSC_boss_the_beast()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "boss_the_beast";
    pNewScript->GetAI = &GetAI_boss_the_beast;
    pNewScript->RegisterSelf();
}
