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
SDName: Boss_Ambassador_Flamelash
SD%Complete: 100
SDComment: With script for Burning Spirits
SDCategory: Blackrock Depths
EndScriptData */

#include "precompiled.h"
#include "blackrock_depths.h"

enum eFlamelash
{
    SPELL_BURNING_SPIRIT  = 13489,
    SPELL_BURNING_SPIRIT_ = 14744,
};

static float SpiritsSpawnLocations[7][3]=
{
	{978.343f, -240.966f, -61.946f},
	{995.262f, -257.966f, -61.946f},
	{1012.280f, -274.911f, -61.946f},
	{1029.219f, -257.987f, -61.946f},
	{1046.229f, -241.007f, -61.946f},
	{1029.290f, -224.057f, -61.946f},
	{1012.309f, -207.095f, -61.946f}
};

struct MANGOS_DLL_DECL boss_ambassador_flamelashAI : public ScriptedAI
{
    boss_ambassador_flamelashAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_blackrock_depths*)pCreature->GetInstanceData();
        Reset();
    }

    instance_blackrock_depths* m_pInstance;

    uint32 m_uiSummonSpiritsTimer;

    uint8 m_uiRand;
    uint8 m_uiRandCache[4];
    uint8 m_uiSpiritsSummonedCount;

    void Reset()
    {
        m_uiSummonSpiritsTimer = 4000;

        DoResetSpiritsCache();

        m_creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_FIRE, true);
    }

    void SummonSpirits()
    {
        bool bCanSummon = true;
        m_uiRand = urand(0,6);

        for(uint8 i = 0; i < 4; ++i)
        {
            if (m_uiRand == m_uiRandCache[i])
                bCanSummon = false;
        }

        if (bCanSummon)
        {
            m_uiRandCache[m_uiSpiritsSummonedCount++] = m_uiRand;

            if (m_uiSpiritsSummonedCount == 4)
                DoResetSpiritsCache();

            if (Creature* pSummoned = m_creature->SummonCreature(NPC_BURNING_SPIRIT, SpiritsSpawnLocations[m_uiRand][0], SpiritsSpawnLocations[m_uiRand][1], SpiritsSpawnLocations[m_uiRand][2], 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 60000))
                pSummoned->GetMotionMaster()->MoveFollow(m_creature, 0.0f, 2.0f);
        }
        else
            SummonSpirits();
    }

    void DoResetSpiritsCache()
    {
        m_uiSpiritsSummonedCount = 0;
        m_uiRand = 0;
        for(uint8 itr = 0; itr < 4; ++itr)
            m_uiRandCache[itr] = 0;
    }
    
    void Aggro(Unit *)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMBER_OF_ENCHANTMENT, IN_PROGRESS);
    }
    
    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMBER_OF_ENCHANTMENT, FAIL);
    }
    
    void JustDied(Unit* /*pVictim*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMBER_OF_ENCHANTMENT, DONE);
    }
    
    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell)
    {
        // Increases model scale and damage done (up to 50 stacks = +1500 damage)
        if (pSpell->Id == SPELL_BURNING_SPIRIT)
            DoCastSpellIfCan(m_creature, SPELL_BURNING_SPIRIT_, CAST_FORCE_CAST);
    }

    void MoveInLineOfSight(Unit* pUnit)
    {
        if (pUnit->GetEntry() == NPC_BURNING_SPIRIT && pUnit->IsWithinDistInMap(m_creature, 3.0f) && !pUnit->isInCombat() && m_creature->isInCombat())
        {
            pUnit->CastSpell(m_creature, SPELL_BURNING_SPIRIT, true);
            return;
        }

        ScriptedAI::MoveInLineOfSight(pUnit);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Summon Burning Spirits
        if (m_uiSummonSpiritsTimer <= uiDiff)
        {
            SummonSpirits();
            SummonSpirits();
            SummonSpirits();
            SummonSpirits();

            m_uiSummonSpiritsTimer = 10000;
        }
        else
            m_uiSummonSpiritsTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_ambassador_flamelash(Creature* pCreature)
{
    return new boss_ambassador_flamelashAI(pCreature);
}

/*######
## AddSC
######*/

void AddSC_boss_ambassador_flamelash()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_ambassador_flamelash";
    pNewScript->GetAI = &GetAI_boss_ambassador_flamelash;
    pNewScript->RegisterSelf();
}
