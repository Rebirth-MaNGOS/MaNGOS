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
SDName: Boss_Jin'do the Hexxer
SD%Complete: 85
SDComment: Mind Control not working because of core bug.
SDCategory: Zul'Gurub
EndScriptData */

#include "precompiled.h"
#include "zulgurub.h"

enum eJindo
{
    SAY_AGGRO                       = -1309021,

    SPELL_BANISH                    = 24466,
    SPELL_BRAINWASH_TOTEM           = 24262,
    SPELL_HEX                       = 17172,
    SPELL_DELUSIONS_OF_JINDO        = 24306,
    SPELL_SHADE_OF_JINDO_PASSIVE    = 24307,
    SPELL_SHADE_OF_JINDO            = 24308,    // not work by creature, but as player yes :?
    SPELL_POWERFULL_HEALING_WARD    = 24309,

    // Healing Ward
    SPELL_HEAL                      = 24311,

    // Brain Wash Totem
    SPELL_BRAIN_WASH                = 24261,

    // Shade of Jindo
    SPELL_SHADOW_SHOCK              = 24458
};

struct MANGOS_DLL_DECL boss_jindoAI : public ScriptedAI
{
    boss_jindoAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_zulgurub*)pCreature->GetInstanceData();
        Reset();
    }

    instance_zulgurub* m_pInstance;

    uint32 m_uiBrainWashTotemTimer;
    uint32 m_uiHealingWardTimer;
    uint32 m_uiHexTimer;
    uint32 m_uiDelusionsTimer;
    uint32 m_uiTeleportTimer;
    uint8 m_uiCurseCounter;
    Unit* m_hexedTarget;
    uint32 m_uiHexSaveTimer;

    void Reset()
    {
        m_uiCurseCounter = 0;
        m_uiBrainWashTotemTimer = 20000;
        m_uiHealingWardTimer = 16000;
        m_uiHexTimer = 8000;
        m_uiDelusionsTimer = 10000;
        m_uiTeleportTimer = 5000;
        m_hexedTarget = NULL;
        m_uiHexSaveTimer = 1000;
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Brain Wash Totem
        if (m_uiBrainWashTotemTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_BRAINWASH_TOTEM);
            m_uiBrainWashTotemTimer = urand(20000,24000);
        }
        else
            m_uiBrainWashTotemTimer -= uiDiff;

        // Healing Ward
        if (m_uiHealingWardTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_POWERFULL_HEALING_WARD);
            //DoSpawnCreature(NPC_POWERFUL_HEALING_WARD, irand(-3,3), irand(-3,3), 0, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 30000);
            m_uiHealingWardTimer = urand(15000,19000);
        }
        else
            m_uiHealingWardTimer -= uiDiff;

        // If the player has the curse dispelled within one second all aggro is restored.
        if (m_hexedTarget)
        {
            if (m_uiHexSaveTimer <= uiDiff)
            {
                bool hex_found = false;
                for (Aura* current_aura : m_hexedTarget->GetAurasByType(AuraType::SPELL_AURA_TRANSFORM))
                    if (current_aura->GetSpellProto()->Id == SPELL_HEX)
                    {
                        hex_found = true;
                        break;
                    }

                if (!hex_found && m_creature->getThreatManager().getThreat(m_hexedTarget))
                    m_creature->getThreatManager().modifyThreatPercent(m_hexedTarget, 500);

                m_hexedTarget = NULL;
                m_uiHexSaveTimer = 1000;
            }
            else
                m_uiHexSaveTimer -= uiDiff;

        }

        // Hex
        if (m_uiHexTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
            {
                m_creature->CastSpell(pTarget, SPELL_HEX, true);

                if (m_creature->getThreatManager().getThreat(pTarget))
                    m_creature->getThreatManager().modifyThreatPercent(pTarget,-80);

                m_hexedTarget = pTarget;
            }

            m_uiHexTimer = urand(12000,16000); // 12000, 20000
        }
        else
            m_uiHexTimer -= uiDiff;

        // Casting the delusion curse
        if (m_uiDelusionsTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            if (pTarget)
            {
                DoCastSpellIfCan(pTarget, SPELL_DELUSIONS_OF_JINDO);
                //DoCastSpellIfCan(m_creature, SPELL_SHADE_OF_JINDO);     // not work
            }

            //Spawn Shade
            pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            if (pTarget)
                if (Creature* pShade = m_creature->SummonCreature(NPC_SHADE_OF_JINDO, pTarget->GetPositionX()-5, pTarget->GetPositionY()+5, pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000))
                    pShade->AI()->AttackStart(pTarget);

            m_uiCurseCounter++;
            if (m_uiCurseCounter == 6)
            {
                m_uiCurseCounter = 0;
                m_uiDelusionsTimer = 14000;
            }
            else
                m_uiDelusionsTimer = urand(3000,8000);
        }
        else
            m_uiDelusionsTimer -= uiDiff;

        // Teleporting a random gamer and spawning 9 skeletons that will attack this player
        if (m_uiTeleportTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            if (pTarget && pTarget->GetTypeId() == TYPEID_PLAYER)
            {
                //DoCastSpellIfCan(pTarget, SPELL_BANISH);
                DoTeleportPlayer(pTarget, -11583.7783f, -1249.4278f, 77.5471f, 4.745f);

                HostileReference const* top_aggro_reference = *(m_creature->getThreatManager().getThreatList().begin());
                if(top_aggro_reference)
                    if (m_creature->getThreatManager().getThreat(pTarget) && top_aggro_reference->getTarget() != pTarget)
                        m_creature->getThreatManager().modifyThreatPercent(pTarget,-100);

                //float fX, fY, fZ;
                //pTarget->GetPosition(fX, fY, fZ);
                for(uint32 i = 0; i < 9; ++i)
                {
                    if (Creature* Skeleton = m_creature->SummonCreature(NPC_SACRIFICED_TROLL, -11583.7783f + irand(-4,4), -1249.4278f + irand(-4,4), 77.5471f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000))
                        Skeleton->AI()->AttackStart(pTarget);
                }
            }
            m_uiTeleportTimer = urand(22000,26000);
        }
        else
            m_uiTeleportTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_jindo(Creature* pCreature)
{
    return new boss_jindoAI(pCreature);
}

struct MANGOS_DLL_DECL mob_healing_wardAI : public Scripted_NoMovementAI
{
    mob_healing_wardAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (instance_zulgurub*)pCreature->GetInstanceData();
        Reset();
    }

    instance_zulgurub* m_pInstance;

    uint32 m_uiHealTimer;

    void Reset()
    {
        m_uiHealTimer = 2000;
        //m_creature->addUnitState(UNIT_STAT_CAN_NOT_MOVE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Heal spell
        if (m_uiHealTimer < uiDiff)
        {
            if (m_pInstance)
            {
                Unit* pJindo = m_pInstance->GetSingleCreatureFromStorage(NPC_JINDO);
                if (pJindo && pJindo->isAlive())
                    DoCastSpellIfCan(pJindo, SPELL_HEAL);
            }

            m_uiHealTimer = 3000;
        }
        else
            m_uiHealTimer -= uiDiff;
		m_creature->SetTargetGuid(ObjectGuid());
    }
};

CreatureAI* GetAI_mob_healing_ward(Creature* pCreature)
{
    return new mob_healing_wardAI(pCreature);
}

struct MANGOS_DLL_DECL mob_brain_wash_totemAI : public Scripted_NoMovementAI
{
    mob_brain_wash_totemAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (instance_zulgurub*)pCreature->GetInstanceData();
        //SetCombatMovement(false);
        bSpellCasted = false;
        Reset();
    }

    instance_zulgurub* m_pInstance;
    ObjectGuid targetGuid;
    bool bSpellCasted;

    void Reset()
    {
        //m_creature->addUnitState(UNIT_STAT_CAN_NOT_MOVE);
    }

    void DamageTaken(Unit *, uint32&)
    {
        // Totem will always looking at charmed target
        m_creature->SetTargetGuid(targetGuid);
    }

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        
        if (m_pInstance)
        {
            Unit* pJindo = m_pInstance->GetSingleCreatureFromStorage(NPC_JINDO);
            if (pJindo && !pJindo->isInCombat())
            {
                Player* player = m_creature->GetMap()->GetPlayer(targetGuid);
                if (player && pJindo->isAlive())
                    m_creature->DealDamage(player, player->GetHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, true);
                
                m_creature->DealDamage(m_creature, m_creature->GetHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, true);
            }
        }
        
        // Brain Wash spell
        if (!bSpellCasted)
        {
            Unit* player =  GetRandomPlayerInCurrentMap(20.f);
            if (player && !player->isDead())
            {
                m_creature->CastSpell(player, SPELL_BRAIN_WASH, true);
                targetGuid = player->GetObjectGuid();
                bSpellCasted = true;
            }
        }
    }
};

CreatureAI* GetAI_mob_brain_wash_totem(Creature* pCreature)
{
    return new mob_brain_wash_totemAI(pCreature);
}

struct MANGOS_DLL_DECL mob_shade_of_jindoAI : public ScriptedAI
{
    mob_shade_of_jindoAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_zulgurub*)pCreature->GetInstanceData();
        Reset();
    }

    instance_zulgurub* m_pInstance;

    uint32 m_uiShadowShockTimer;
    uint32 m_uiUpdateTimer;

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (!pDoneBy->HasAura(SPELL_DELUSIONS_OF_JINDO))
            uiDamage = 0;
    }

    void Reset()
    {
        m_uiShadowShockTimer = 2000;
        m_uiUpdateTimer = 500;
        DoCastSpellIfCan(m_creature, SPELL_SHADE_OF_JINDO_PASSIVE, CAST_FORCE_CAST + CAST_FORCE_TARGET_SELF + CAST_TRIGGERED);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Check if Jin'do is alive
        if (m_uiUpdateTimer <= uiDiff)
        {
            m_uiUpdateTimer = 500;
            if (m_pInstance)
            {
                Creature* pJindo = m_pInstance->GetSingleCreatureFromStorage(NPC_JINDO);
                if (pJindo && !pJindo->isAlive())
                {
                    m_creature->ForcedDespawn();
                    return;
                }
            }
        }
        else
            m_uiUpdateTimer -= uiDiff;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Shadow Shock
        if (m_uiShadowShockTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_SHOCK);
            m_uiShadowShockTimer = 3000;
        }
        else
            m_uiShadowShockTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_shade_of_jindo(Creature* pCreature)
{
    return new mob_shade_of_jindoAI(pCreature);
}

void AddSC_boss_jindo()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_jindo";
    pNewScript->GetAI = &GetAI_boss_jindo;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_healing_ward";
    pNewScript->GetAI = &GetAI_mob_healing_ward;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_brain_wash_totem";
    pNewScript->GetAI = &GetAI_mob_brain_wash_totem;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_shade_of_jindo";
    pNewScript->GetAI = &GetAI_mob_shade_of_jindo;
    pNewScript->RegisterSelf();
}
