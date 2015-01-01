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
SDName: Boss_Kormok
SD%Complete: 100
SDComment:
SDCategory: Scholomance
EndScriptData */

#include "precompiled.h"
#include "scholomance.h"

enum Spells
{
    SPELL_BLOODLUST                 = 27689,
    SPELL_BONE_SHIELD               = 27688,
    SPELL_FRENZY                    = 8269,
    SPELL_SHADOW_BOLT_VOLLEY        = 17228,
};

struct MANGOS_DLL_DECL boss_kormokAI : public ScriptedAI
{
    boss_kormokAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_scholomance*)pCreature->GetInstanceData();
        Reset();
    }

    instance_scholomance* m_pInstance;

    bool m_bMages;
    uint32 m_uiBloodLustTimer;
    uint32 m_uiBoneShieldTimer;
    uint32 m_uiFrenzyTimer;
    uint32 m_uiMinionTimer;
    uint32 m_uiShadowVolleyTimer;

    void Reset()
    {
        m_bMages = false;
        m_uiBloodLustTimer = urand(15000,20000);
        m_uiBoneShieldTimer = urand(4000,6000);
        m_uiFrenzyTimer = 0;
        m_uiMinionTimer = 15000;
        m_uiShadowVolleyTimer = urand(8000,12000);
    }

    void JustReachedHome()
    {
        m_creature->RemoveFromWorld();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_KORMOK, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KORMOK, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KORMOK, DONE);
    }

    void JustSummoned(Creature* pSummoned)
    {
        switch(pSummoned->GetEntry())
        {
            case NPC_BONE_MINION:
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    pSummoned->AI()->AttackStart(pTarget);
                break;
            case NPC_BONE_MAGE:
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_BOTTOMAGGRO, 0))
                    pSummoned->AI()->AttackStart(pTarget);
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {        
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Bloodlust
        if (m_uiBloodLustTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_BLOODLUST);
            m_uiBloodLustTimer = urand(10000,15000);
        }
        else
            m_uiBloodLustTimer -= uiDiff;

        // Bone Shield
        if (m_uiBoneShieldTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_BONE_SHIELD);
            m_uiBoneShieldTimer = urand(30000,45000);
        }
        else
            m_uiBoneShieldTimer -= uiDiff;

        // Shadow Bolt Volley
        if (m_uiShadowVolleyTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_BOLT_VOLLEY);
            m_uiShadowVolleyTimer = urand(8000,15000);
        }
        else
            m_uiShadowVolleyTimer -= uiDiff;

        // Minions summon
        if (m_uiMinionTimer <= uiDiff)
        {
            for(uint8 i = 0; i < 4; ++i)
                DoSpawnCreature(NPC_BONE_MINION, irand(-10,10), irand(-10,10), 0, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000);

            m_uiMinionTimer = urand(10000,15000);
        }
        else
            m_uiMinionTimer -= uiDiff;

        // Summon 3 Bone Mages
        if (!m_bMages && HealthBelowPct(50))
        {
            for(uint8 i = 0; i < 3; ++i)
                DoSpawnCreature(NPC_BONE_MAGE, irand(-10,10), irand(-10,10), 0, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000);

            m_bMages = true;
        }

        if (HealthBelowPct(25))
        {
            if (m_uiFrenzyTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_FRENZY);
                m_uiFrenzyTimer = 120000;
            }
            else
                m_uiFrenzyTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_kormok(Creature* pCreature)
{
    return new boss_kormokAI(pCreature);
}

struct MANGOS_DLL_DECL npc_kormok_triggerAI : public Scripted_NoMovementAI
{
    npc_kormok_triggerAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (instance_scholomance*)pCreature->GetInstanceData();
        Reset();
    }

    instance_scholomance* m_pInstance;

    void Reset()
    {
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!pWho || pWho->GetTypeId() != TYPEID_PLAYER)
            return;

        if (!m_pInstance || m_pInstance->GetData(TYPE_KORMOK) != NOT_STARTED)
            return;

        GameObject* pBeckoning = GetClosestGameObjectWithEntry(m_creature, GO_BRAZIER_OF_BECKONING, 40.0f);
        GameObject* pInvocation = GetClosestGameObjectWithEntry(m_creature, GO_BRAZIER_OF_INVOCATION, 40.0f);

        if (!pBeckoning && !pInvocation)
            return;
        
        if (pBeckoning)
            pBeckoning->RemoveFromWorld();

        if (pInvocation)
            pInvocation->RemoveFromWorld();

        m_pInstance->SetData(TYPE_KORMOK, IN_PROGRESS);

        if (Creature* pKormok = DoSpawnCreature(NPC_KORMOK, 0, 0, 0, 0, TEMPSUMMON_DEAD_DESPAWN, 30000))
            pKormok->AI()->AttackStart(pWho);
    }
};

CreatureAI* GetAI_npc_kormok_trigger(Creature* pCreature)
{
    return new npc_kormok_triggerAI(pCreature);
}

void AddSC_boss_kormok()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_kormok";
    pNewscript->GetAI = &GetAI_boss_kormok;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_kormok_trigger";
    pNewscript->GetAI = &GetAI_npc_kormok_trigger;
    pNewscript->RegisterSelf();
}
