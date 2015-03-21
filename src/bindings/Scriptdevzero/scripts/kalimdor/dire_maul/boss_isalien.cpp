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
SDName: Boss Isalien
SD%Complete:
SDComment: Boss AI and summon trigger
SDCategory: Dire Maul
EndScriptData */

#include "precompiled.h"
#include "dire_maul.h"

enum Spells
{
    SPELL_MULTI_SHOT                = 14443,
    SPELL_NET                       = 12024,
    SPELL_REGROWTH                  = 27637,
    SPELL_SHOOT                     = 22907,
    SPELL_STARSHARDS                = 27636,
	SAY_AGGRO						= -1720039,
	SAY_DIE							= -1720040,
};

struct MANGOS_DLL_DECL boss_isalienAI : public ScriptedAI
{
    boss_isalienAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_dire_maul*)pCreature->GetInstanceData();
        Reset();
    }

    instance_dire_maul* m_pInstance;

    bool m_bIsPetSpawned;
    uint32 m_uiMultiShotTimer;
    uint32 m_uiNetTimer;
    uint32 m_uiRegrowthTimer;
    uint32 m_uiShootTimer;
    uint32 m_uiStarshardsTimer;
    uint32 m_uiPetTimer;

    void Reset()
    {
        m_bIsPetSpawned = false;
        m_uiPetTimer = 10000;
        m_uiShootTimer = urand(5000,8000);
        m_uiMultiShotTimer = urand(8000,12000);
        m_uiNetTimer = urand(7000,12000);
        m_uiRegrowthTimer = urand(8000,12000);
        m_uiStarshardsTimer = urand(15000,20000);
    }

    void JustReachedHome()
    {
        m_creature->RemoveFromWorld();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ISALIEN, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ISALIEN, IN_PROGRESS);
		DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ISALIEN, DONE);
		DoScriptText(SAY_DIE, m_creature);
    }

    void JustSummoned(Creature* pSummoned)
    {
        Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1);
        if (!m_creature->getVictim() && !pTarget)
            return;

        pSummoned->AI()->AttackStart(pTarget ? pTarget : m_creature->getVictim());
    }

    void UpdateAI(const uint32 uiDiff)
    {       
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Call Isalien's pet
        if (!m_bIsPetSpawned)
	{
            if (m_uiPetTimer <= uiDiff)
            {
                DoSpawnCreature(NPC_EMPYREAN, irand(-5,5), irand(-5,5), 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                m_bIsPetSpawned = true;
                m_uiPetTimer = 10000;
            }
            else
                m_uiPetTimer -= uiDiff;
	}

        // Shoot
        if (m_uiShootTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHOOT);
            m_uiShootTimer = urand(5000,8000);
        }
        else
            m_uiShootTimer -= uiDiff;

        // Multi-Shot
        if (m_uiMultiShotTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MULTI_SHOT);
            m_uiMultiShotTimer = urand(8000,12000);
        }
        else
            m_uiMultiShotTimer -= uiDiff;

        // Net
        if (m_uiNetTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_NET);
            m_uiNetTimer = urand(7000,12000);
        }
        else
            m_uiNetTimer -= uiDiff;

        // Starshards
        if (m_uiStarshardsTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_STARSHARDS);
            m_uiStarshardsTimer = urand(15000,20000);
        }
        else
            m_uiStarshardsTimer -= uiDiff;

        // Regrowth
        if (m_uiRegrowthTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_REGROWTH, CAST_INTERRUPT_PREVIOUS);
            m_uiRegrowthTimer = urand(8000,12000);
        }
        else
            m_uiRegrowthTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_isalien(Creature* pCreature)
{
    return new boss_isalienAI(pCreature);
}

struct MANGOS_DLL_DECL npc_isalien_triggerAI : public Scripted_NoMovementAI
{
    npc_isalien_triggerAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (instance_dire_maul*)pCreature->GetInstanceData();
        Reset();
    }

    instance_dire_maul* m_pInstance;

    void Reset()
    {
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!pWho || pWho->GetTypeId() != TYPEID_PLAYER)
            return;

        if (!m_pInstance || m_pInstance->GetData(TYPE_ISALIEN) != NOT_STARTED)
            return;

        GameObject* pBeckoning = GetClosestGameObjectWithEntry(m_creature, GO_BRAZIER_OF_BECKONING, 40.0f);
        GameObject* pInvocation = GetClosestGameObjectWithEntry(m_creature, GO_BRAZIER_OF_INVOCATION, 40.0f);

        if (!pBeckoning && !pInvocation)
            return;
        
        if (pBeckoning)
            pBeckoning->RemoveFromWorld();

        if (pInvocation)
            pInvocation->RemoveFromWorld();

        m_pInstance->SetData(TYPE_ISALIEN, IN_PROGRESS);
        
        if (Creature* pIsalien = DoSpawnCreature(NPC_ISALIEN, 0, 0, 0, 0, TEMPSUMMON_DEAD_DESPAWN, 30000))
            pIsalien->AI()->AttackStart(pWho);
    }
};

CreatureAI* GetAI_npc_isalien_trigger(Creature* pCreature)
{
    return new npc_isalien_triggerAI(pCreature);
}

void AddSC_boss_isalien()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_isalien";
    pNewscript->GetAI = &GetAI_boss_isalien;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_isalien_trigger";
    pNewscript->GetAI = &GetAI_npc_isalien_trigger;
    pNewscript->RegisterSelf();
}
