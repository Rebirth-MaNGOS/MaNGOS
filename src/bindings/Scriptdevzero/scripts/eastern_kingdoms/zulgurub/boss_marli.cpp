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
SDName: Boss_Marli
SD%Complete: 100
SDComment:
SDCategory: Zul'Gurub
EndScriptData */

#include "precompiled.h"
#include "zulgurub.h"

enum eMarli
{
    SAY_AGGRO                   = -1309005,
    SAY_TRANSFORM               = -1309006,
    SAY_TRANSFORM_BACK          = -1309007,
    SAY_SPIDER_SPAWN            = -1309008,
    SAY_DEATH                   = -1309009,

    SPELL_CHARGE                = 22911,
    SPELL_CORROSIVE_POISON      = 24111,
    SPELL_DRAIN_LIFE            = 24300,
    SPELL_ENLARGE               = 24109,
    SPELL_ENVELOPING_WEBS       = 24110,
    SPELL_HATCH_EGGS            = 24083,
    SPELL_MARLI_TRANSFORM       = 24084,
    SPELL_POISON_BOLT_VOLLEY    = 24099,
    SPELL_POISON_SHOCK          = 24112,
    SPELL_THRASH                = 3391,

    // The Spider's Spells
    SPELL_LEVEL_UP              = 24312,
};

struct MANGOS_DLL_DECL boss_marliAI : public ScriptedAI
{
    boss_marliAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_zulgurub*)pCreature->GetInstanceData();
        Reset();
    }

    instance_zulgurub* m_pInstance;

    bool m_bHasWebbed;
    bool m_bSpiderForm;

    uint32 m_uiChargeTimer;
    uint32 m_uiCorrosivePoisonTimer;
    uint32 m_uiDrainLifeTimer;
    uint32 m_uiEnvelopingWebsTimer;
    uint32 m_uiPoisonShockTimer;
    uint32 m_uiPoisonVolleyTimer;
    uint32 m_uiHatchEggTimer;
    uint32 m_uiTransformTimer;
    uint32 m_uiThrashTimer;

    void Reset()
    {
        m_bHasWebbed = false;
        m_bSpiderForm = false;

        m_uiChargeTimer = 1000;
        m_uiCorrosivePoisonTimer = 1000;
        m_uiDrainLifeTimer = 30000;
        m_uiEnvelopingWebsTimer = 15000;
        m_uiPoisonShockTimer = 12000;
        m_uiPoisonVolleyTimer = 15000;
        m_uiHatchEggTimer = 20000;
        m_uiTransformTimer = 60000;
        m_uiThrashTimer = 5000;
    }

    void HatchEgg()
    {
        std::list<GameObject*> lEggs;
        GetGameObjectListWithEntryInGrid(lEggs, m_creature, GO_SPIDER_EGG, DEFAULT_VISIBILITY_INSTANCE);
        if (lEggs.empty())
            debug_log("SD0: ZG: boss_marli, no Eggs with the entry %u were found", GO_SPIDER_EGG);
        else
        {
            lEggs.sort(ObjectDistanceOrder(m_creature));
            for(std::list<GameObject*>::iterator itr = lEggs.begin(); itr != lEggs.end(); ++itr)
            {
                if ((*itr)->GetGoState() == (GO_STATE_READY))
                {
                    (*itr)->SetGoState(GO_STATE_ACTIVE);
                    m_creature->SummonCreature(NPC_SPAWN_OF_MARLI, (*itr)->GetPositionX(), (*itr)->GetPositionY(), (*itr)->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
                    return;
                }
            }
        }
    }

    void JustReachedHome()
    {
        // Reset hatched eggs
        std::list<GameObject*> lSpiderEggs;
        GetGameObjectListWithEntryInGrid(lSpiderEggs, m_creature, GO_SPIDER_EGG, DEFAULT_VISIBILITY_INSTANCE);
        if (lSpiderEggs.empty())
            debug_log("SD0: ZG: boss_marli, no Eggs with the entry %u were found", GO_SPIDER_EGG);
        else
        {
            for(std::list<GameObject*>::iterator itr = lSpiderEggs.begin(); itr != lSpiderEggs.end(); ++itr)
            {
                if ((*itr)->GetGoState() == GO_STATE_ACTIVE)
                    (*itr)->SetGoState(GO_STATE_READY);
            }
        }
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(SAY_AGGRO, m_creature);

        // Hatch four eggs on Aggro
        DoCastSpellIfCan(m_creature, SPELL_HATCH_EGGS);
        for(uint8 i = 0; i < 4 ; ++i)
            HatchEgg();
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_SPAWN_OF_MARLI)
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                pSummoned->AI()->AttackStart(pTarget);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MARLI, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_bSpiderForm)
        {
            // Drain Life
            if (m_uiDrainLifeTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_DRAIN_LIFE);
                m_uiDrainLifeTimer = urand(20000, 50000);
            }
            else
                m_uiDrainLifeTimer -= uiDiff;

            // Hatch Egg
            if (m_uiHatchEggTimer <= uiDiff)
            {
                HatchEgg();
                HatchEgg();     // may delete
                DoScriptText(SAY_SPIDER_SPAWN, m_creature);
                m_uiHatchEggTimer = urand(15000, 30000);
            }
            else
                m_uiHatchEggTimer -= uiDiff;

            // Poison Shock
            if (m_uiPoisonShockTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_POISON_SHOCK);
                m_uiPoisonShockTimer = urand(10000, 20000);
            }
            else
                m_uiPoisonShockTimer -= uiDiff;

            // Poison Volley
            if (m_uiPoisonVolleyTimer <= uiDiff)
            {
                Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_POISON_BOLT_VOLLEY);
                m_uiPoisonVolleyTimer = urand(7000, 14000);        // 10000, 20000
            }
            else
                m_uiPoisonVolleyTimer -= uiDiff;
        }
        else
        {
            // Enveloping Webs
            if (!m_bHasWebbed && m_uiEnvelopingWebsTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_ENVELOPING_WEBS);
                m_uiChargeTimer = 500;
                m_bHasWebbed = true;
            }
            else
                m_uiEnvelopingWebsTimer -= uiDiff;

            // Charge
            if (m_bHasWebbed && m_uiChargeTimer <= uiDiff)
            {   
                GUIDVector vGuids;
                m_creature->FillGuidsListFromThreatList(vGuids);
                for (GUIDVector::const_iterator itr = vGuids.begin(); itr != vGuids.end(); ++itr)
                {
                    if (Unit* pTarget = m_creature->GetMap()->GetUnit(*itr))
                    {
                        if (!pTarget->HasAura(SPELL_ENVELOPING_WEBS, EFFECT_INDEX_0))
                        {
                            DoCastSpellIfCan(pTarget, SPELL_CHARGE);
                            DoResetThreat();
                            AttackStart(pTarget);
                            m_bHasWebbed = false;
                            break;
                        }
                    }
                }
                m_uiEnvelopingWebsTimer = urand(20000, 25000);      // 10000, 20000
            }
            else
                m_uiChargeTimer -= uiDiff;

            // Corrosive Poison
            if (m_uiCorrosivePoisonTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_CORROSIVE_POISON);
                m_uiCorrosivePoisonTimer = urand(6000,8000);         // 25000, 35000
            }
            else
                m_uiCorrosivePoisonTimer -= uiDiff;

            // Thrash
            if (m_uiThrashTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_THRASH);
                m_uiThrashTimer = urand(10000, 20000);
            }
            else
                m_uiThrashTimer -= uiDiff;
        }

        // Transform
        if (m_uiTransformTimer <= uiDiff)
        {
            if (!m_bSpiderForm)
            {
                DoScriptText(SAY_TRANSFORM, m_creature);
                DoCastSpellIfCan(m_creature, SPELL_MARLI_TRANSFORM, CAST_INTERRUPT_PREVIOUS + CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_ENLARGE, CAST_TRIGGERED);

                const CreatureInfo* cinfo = m_creature->GetCreatureInfo();
                m_creature->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, (cinfo->mindmg +((cinfo->mindmg/100) * 35)));
                m_creature->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, (cinfo->maxdmg +((cinfo->maxdmg/100) * 35)));
                m_creature->UpdateDamagePhysical(BASE_ATTACK);

                DoResetThreat();

                m_bSpiderForm = true;
            }
            else
            {
                DoScriptText(SAY_TRANSFORM_BACK, m_creature);
                m_creature->RemoveAurasDueToSpell(SPELL_MARLI_TRANSFORM);
                m_creature->RemoveAurasDueToSpell(SPELL_ENLARGE);

                const CreatureInfo* cinfo = m_creature->GetCreatureInfo();
                m_creature->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, (cinfo->mindmg +((cinfo->mindmg/100) * 1)));
                m_creature->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, (cinfo->maxdmg +((cinfo->maxdmg/100) * 1)));
                m_creature->UpdateDamagePhysical(BASE_ATTACK);

                // Should drain life right after transforming back from spider form
                m_uiDrainLifeTimer = 1000;
                m_bSpiderForm = false;
            }
            m_uiTransformTimer = urand(40000, 60000);
        }
        else
            m_uiTransformTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_marli(Creature* pCreature)
{
    return new boss_marliAI(pCreature);
}

struct MANGOS_DLL_DECL mob_spawn_of_marliAI : public ScriptedAI
{
    mob_spawn_of_marliAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_zulgurub*)m_creature->GetInstanceData();
        Reset();
    }

    instance_zulgurub* m_pInstance;

    uint32 m_uiLevelUpTimer;

    void Reset()
    {
        m_uiLevelUpTimer = 3000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiLevelUpTimer <= uiDiff)
        {
            if (m_pInstance && m_pInstance->GetData(TYPE_MARLI) != DONE)
            {
                DoCastSpellIfCan(m_creature, SPELL_LEVEL_UP);
                m_creature->SetLevel(m_creature->getLevel() + 1);
            }
            m_uiLevelUpTimer = 3000;
        }
        else
            m_uiLevelUpTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_spawn_of_marli(Creature* pCreature)
{
    return new mob_spawn_of_marliAI(pCreature);
}

void AddSC_boss_marli()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_marli";
    pNewScript->GetAI = &GetAI_boss_marli;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_spawn_of_marli";
    pNewScript->GetAI = &GetAI_mob_spawn_of_marli;
    pNewScript->RegisterSelf();
}
