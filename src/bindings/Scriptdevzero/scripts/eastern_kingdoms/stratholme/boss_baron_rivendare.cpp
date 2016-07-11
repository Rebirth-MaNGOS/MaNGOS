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
SDName: Boss_Baron_Rivendare
SD%Complete: 90
SDComment: aura applied/defined in database
SDCategory: Stratholme
EndScriptData */

#include "precompiled.h"
#include "stratholme.h"

static Locations Skeleton[]=
{
    {4017.40f,-3339.70f,115.05f, 0, 0},
    {4013.18f,-3351.80f,115.05f, 0, 0},
    {4017.73f,-3363.47f,115.05f, 0, 0},
    {4048.87f,-3363.22f,115.05f, 0, 0},
    {4051.77f,-3350.89f,115.05f, 0, 0},
    {4048.37f,-3339.96f,115.05f, 0, 0}
};

enum Spells
{
    SPELL_CLEAVE            = 15284,
    SPELL_DEATH_PACT        = 17471,
    SPELL_SHADOW_BOLT       = 17393,
    SPELL_MORTAL_STRIKE     = 15708,
    SPELL_UNHOLY_AURA       = 17467,

    SPELL_RAISE_DEAD        = 17473,
    SPELL_RAISE_DEAD1       = 17475,
    SPELL_RAISE_DEAD2       = 17476,
    SPELL_RAISE_DEAD3       = 17477,
    SPELL_RAISE_DEAD4       = 17478,
    SPELL_RAISE_DEAD5       = 17479,
    SPELL_RAISE_DEAD6       = 17480,
};

struct MANGOS_DLL_DECL boss_baron_rivendareAI : public ScriptedAI
{
    boss_baron_rivendareAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_stratholme*)pCreature->GetInstanceData();
        Reset();
    }

    instance_stratholme* m_pInstance;

    uint32 m_uiCleaveTimer;
    uint32 m_uiDeathPactTimer;
    uint32 m_uiMortalStrikeTimer;
    uint32 m_uiShadowBoltTimer;
    uint32 m_uiSummonSkeletonsTimer;
    
    uint32 m_uiSumAuriusTimer;

    GUIDList m_uiSkeletonGUID;

    void Reset()
    {
        m_uiCleaveTimer = urand(6000,8000);
        m_uiDeathPactTimer = 0;
        m_uiMortalStrikeTimer = urand(10000,12000);
        m_uiShadowBoltTimer = urand(2000,4000);
        m_uiSummonSkeletonsTimer = urand(15000,20000);
        
        m_uiSumAuriusTimer = 5000;

        m_uiSkeletonGUID.clear();
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BARON_RIVENDARE, FAIL);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BARON_RIVENDARE, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BARON_RIVENDARE, DONE);
    }

    void JustSummoned(Creature* pSummoned)
    {
		m_uiSkeletonGUID.push_back(pSummoned->GetObjectGuid());

        Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
        if (!pTarget && !m_creature->getVictim())
            return;

        pSummoned->AI()->AttackStart(pTarget ? pTarget : m_creature->getVictim());
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_pInstance->m_bSummonAurius == true)
        {
            
            if (m_uiSumAuriusTimer <= uiDiff)
            {
                // Spawn Aurius in the Slaughter House to engage Baron
                if (Creature* pAurius = m_creature->SummonCreature(NPC_AURIUS, 4032.63f, -3362.61f, 115.05f, 1.64f, TEMPSUMMON_DEAD_DESPAWN, 0))
                {                        
                    pAurius->MonsterYell("Rivendare! I come for you!", LANG_UNIVERSAL, nullptr);
                    pAurius->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                    pAurius->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    pAurius->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                    pAurius->AI()->AttackStart(m_creature);
                }
                    
                m_uiSumAuriusTimer = 0;            
                m_pInstance->m_bSummonAurius = false;
            }
            else
                m_uiSumAuriusTimer -= uiDiff;            
        }
        
        DoCastSpellIfCan(m_creature, SPELL_UNHOLY_AURA, CAST_AURA_NOT_PRESENT);

        // Cleave
        if (m_uiCleaveTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE);
            m_uiCleaveTimer = urand(6000,9000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        // Death Pact
        if (m_uiDeathPactTimer)
	{
            if (m_uiDeathPactTimer <= uiDiff)
            {
                if (!m_uiSkeletonGUID.empty())
                    for(GUIDList::iterator itr = m_uiSkeletonGUID.begin(); itr != m_uiSkeletonGUID.end(); ++itr)
                    {
                        if (Creature* pSkeleton = m_creature->GetMap()->GetCreature(*itr))
                            if (pSkeleton->isAlive() && m_creature->GetHealth() < m_creature->GetMaxHealth())
                                m_creature->SetHealth(m_creature->GetHealth()+2000);
                    }

                m_uiSkeletonGUID.clear();
                m_uiDeathPactTimer = 0;
            }
            else
                m_uiDeathPactTimer -= uiDiff;
	}

        // Mortal Strike
        if (m_uiMortalStrikeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MORTAL_STRIKE);
            m_uiMortalStrikeTimer = urand(5000,10000);
        }
        else
            m_uiMortalStrikeTimer -= uiDiff;

        // Shadow Bolt
        if (m_uiShadowBoltTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_SHADOW_BOLT);
            m_uiShadowBoltTimer = urand(7000,12000);
        }
        else
            m_uiShadowBoltTimer -= uiDiff;

        // Summon Skeletons
        if (m_uiSummonSkeletonsTimer <= uiDiff)
        {
            for(uint32 i = 0; i < sizeof(Skeleton)/sizeof(Locations); ++i)
                m_creature->SummonCreature(NPC_MINDLESS_SKELETON, Skeleton[i].x , Skeleton[i].y, Skeleton[i].z, 0, TEMPSUMMON_TIMED_DESPAWN, 14000);

            m_uiDeathPactTimer = 12000;
            m_uiSummonSkeletonsTimer = urand(25000,35000);
        }
        else
            m_uiSummonSkeletonsTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_baron_rivendare(Creature* pCreature)
{
    return new boss_baron_rivendareAI(pCreature);
}

void AddSC_boss_baron_rivendare()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_baron_rivendare";
    pNewscript->GetAI = &GetAI_boss_baron_rivendare;
    pNewscript->RegisterSelf();
}
