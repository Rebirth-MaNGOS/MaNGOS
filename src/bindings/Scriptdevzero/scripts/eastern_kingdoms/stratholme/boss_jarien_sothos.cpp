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
SDName: Boss Jarien and Sothos
SD%Complete: 95
SDComment:
SDCategory: Stratholme
EndScriptData */

#include "precompiled.h"
#include "stratholme.h"

enum Spells
{
    SPELL_CLEAVE                    = 15284,
    SPELL_MORTAL_STRIKE             = 16856,
    SPELL_CRIPPLE                   = 20812,
    SPELL_SHADOW_SHOCK              = 22575,
    SPELL_VENGEANCE                 = 27650,
    SPELL_FEAR                      = 27641,
    SPELL_SHADOW_BOLT_VOLLEY        = 27646,
    SPELL_SHIELD_BLOCK              = 12169,
    SPELL_SHIELD_CHARGE             = 15749,
    SPELL_SHIELD_SLAM               = 15655
};

struct MANGOS_DLL_DECL boss_jarienAI : public ScriptedAI
{
    boss_jarienAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_stratholme*)pCreature->GetInstanceData();
        Reset();
    }

    instance_stratholme* m_pInstance;

    uint32 m_uiCleaveTimer;
    uint32 m_uiMortalStrikeTimer;
    uint32 m_uiCrippleTimer;
    uint32 m_uiShadowShockTimer;

    void Reset()
    {
        m_uiCleaveTimer = urand(7000,12000);
        m_uiMortalStrikeTimer = urand(10000,15000);
        m_uiCrippleTimer = urand(15000,20000);
        m_uiShadowShockTimer = urand(10000,20000);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
        {
			if (Creature* pSothos = m_pInstance->GetSingleCreatureFromStorage(NPC_SOTHOS))
                pSothos->RemoveFromWorld();

            m_pInstance->SetData(TYPE_JARIEN, NOT_STARTED);
            m_pInstance->SetData(TYPE_SOTHOS, NOT_STARTED);
        }

        m_creature->RemoveFromWorld();
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_JARIEN, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_JARIEN, DONE);

            // If Jarien dies, summons her spirit and attack Sothos
            if (m_pInstance->GetData(TYPE_SOTHOS) == IN_PROGRESS)
                if (Creature* pSpirit = DoSpawnCreature(NPC_SPIRIT_OF_JARIEN, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000))
                    if (Creature* pSothos = GetClosestCreatureWithEntry(pSpirit, NPC_SOTHOS, DEFAULT_VISIBILITY_INSTANCE))
                        pSpirit->AI()->AttackStart(pSothos);

            // Handle looting until second mate is dead
            if (m_pInstance->GetData(TYPE_SOTHOS) == DONE)
            {
                if (Creature* pSothos = m_pInstance->GetSingleCreatureFromStorage(NPC_SOTHOS))
                    pSothos->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
            }
            else
                m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // If Sothos died, cast Vengeance
        if (!m_creature->HasAura(SPELL_VENGEANCE, EFFECT_INDEX_0) && m_pInstance->GetData(TYPE_SOTHOS) == DONE)
        {
            DoCastSpellIfCan(m_creature, SPELL_VENGEANCE);
            //m_creature->SetHealth(m_creature->GetHealth()+10000);
            //m_creature->SetMaxHealth(m_creature->GetMaxHealth()+10000);
        }

        // Cleave
        if (m_uiCleaveTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE);
            m_uiCleaveTimer = urand(7000,12000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        // Mortal Strike
        if (m_uiMortalStrikeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MORTAL_STRIKE);
            m_uiMortalStrikeTimer = urand(10000,15000);
        }
        else
            m_uiMortalStrikeTimer -= uiDiff;

        // Cripple
        if (m_uiCrippleTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CRIPPLE);
            m_uiCrippleTimer = urand(15000,20000);
        }
        else
            m_uiCrippleTimer -= uiDiff;

        // Shadow Shock
        if (m_uiShadowShockTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_SHADOW_SHOCK);
            m_uiShadowShockTimer = urand(10000,20000);
        }
        else
            m_uiShadowShockTimer -= uiDiff;        

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_jarien(Creature* pCreature)
{
    return new boss_jarienAI(pCreature);
}

struct MANGOS_DLL_DECL boss_sothosAI : public ScriptedAI
{
    boss_sothosAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_stratholme*)pCreature->GetInstanceData();
        Reset();
    }

    instance_stratholme* m_pInstance;

    uint32 m_uiFearTimer;
    uint32 m_uiShadowBoltVolleyTimer;
    uint32 m_uiShieldTimer;

    void Reset()
    {
        m_uiFearTimer = urand(10000,15000);
        m_uiShadowBoltVolleyTimer = urand(10000,15000);
        m_uiShieldTimer = urand(4000,6000);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
        {
            if (Creature* pJarien = m_pInstance->GetSingleCreatureFromStorage(NPC_JARIEN))
                pJarien->RemoveFromWorld();

            m_pInstance->SetData(TYPE_SOTHOS, NOT_STARTED);
            m_pInstance->SetData(TYPE_JARIEN, NOT_STARTED);
        }

        m_creature->RemoveFromWorld();
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SOTHOS, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_SOTHOS, DONE);

            // Handle looting until second mate is dead
            if (m_pInstance->GetData(TYPE_JARIEN) == DONE)
            {
                if (Creature* pJarien = m_pInstance->GetSingleCreatureFromStorage(NPC_JARIEN))
                    pJarien->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
            }
            else
                m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);            
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // If Jarien died, cast Vengeance
        if (!m_creature->HasAura(SPELL_VENGEANCE, EFFECT_INDEX_0) && m_pInstance->GetData(TYPE_JARIEN) == DONE)
        {
            DoCastSpellIfCan(m_creature, SPELL_VENGEANCE);
            //m_creature->SetHealth(m_creature->GetHealth()+10000);
            //m_creature->SetMaxHealth(m_creature->GetMaxHealth()+10000);
        }

        // Fear
        if (m_uiFearTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_FEAR);
            m_uiFearTimer = urand(10000,15000);
        }
        else
            m_uiFearTimer -= uiDiff;

        // Shadow Bolt Volley
        if (m_uiShadowBoltVolleyTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_SHADOW_BOLT_VOLLEY);
            m_uiShadowBoltVolleyTimer = urand(10000,15000);
        }
        else
            m_uiShadowBoltVolleyTimer -= uiDiff;

        // Shield's spells
        if (m_uiShieldTimer <= uiDiff)
        {
            switch (urand(0,2))
            {
                case 0:
                    DoCastSpellIfCan(m_creature, SPELL_SHIELD_BLOCK);
                    break;
                case 1:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCastSpellIfCan(pTarget, SPELL_SHIELD_CHARGE);
                    break;
                case 2:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHIELD_SLAM);
                    break;
            }
            m_uiShieldTimer = urand(4000,6000);
        }
        else
            m_uiShieldTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_sothos(Creature* pCreature)
{
    return new boss_sothosAI(pCreature);
}

/*######
## npc_jarien_sothos_trigger
######*/

struct MANGOS_DLL_DECL npc_jarien_sothos_triggerAI : public Scripted_NoMovementAI
{
    npc_jarien_sothos_triggerAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (instance_stratholme*)pCreature->GetInstanceData();
        Reset();
    }

    instance_stratholme* m_pInstance;

    void Reset()
    {
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!pWho || pWho->GetTypeId() != TYPEID_PLAYER || !m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_JARIEN) != NOT_STARTED || m_pInstance->GetData(TYPE_SOTHOS) != NOT_STARTED)
            return;

        GameObject* pBeckoning = GetClosestGameObjectWithEntry(m_creature, GO_BRAZIER_OF_BECKONING, 40.0f);
        GameObject* pInvocation = GetClosestGameObjectWithEntry(m_creature, GO_BRAZIER_OF_INVOCATION, 40.0f);

        if (!pBeckoning && !pInvocation)
            return;
        
        if (pBeckoning)
            pBeckoning->RemoveFromWorld();

        if (pInvocation)
            pInvocation->RemoveFromWorld();

        m_pInstance->SetData(TYPE_JARIEN, IN_PROGRESS);
        m_pInstance->SetData(TYPE_SOTHOS, IN_PROGRESS);

        if (Creature* pJarien = DoSpawnCreature(NPC_JARIEN, 0, 0, 0, 0, TEMPSUMMON_DEAD_DESPAWN, 30000))
            pJarien->AI()->AttackStart(pWho);
        if (Creature* pSothos = DoSpawnCreature(NPC_SOTHOS, 0, 0, 0, 0, TEMPSUMMON_DEAD_DESPAWN, 30000))
            pSothos->AI()->AttackStart(pWho);
    }
};

CreatureAI* GetAI_npc_jarien_sothos_trigger(Creature* pCreature)
{
    return new npc_jarien_sothos_triggerAI(pCreature);
}

void AddSC_boss_jarien_sothos()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_jarien";
    pNewscript->GetAI = &GetAI_boss_jarien;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "boss_sothos";
    pNewscript->GetAI = &GetAI_boss_sothos;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_jarien_sothos_trigger";
    pNewscript->GetAI = &GetAI_npc_jarien_sothos_trigger;
    pNewscript->RegisterSelf();
}
