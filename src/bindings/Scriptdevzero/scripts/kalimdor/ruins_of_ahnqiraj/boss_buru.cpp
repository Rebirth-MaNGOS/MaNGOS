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
SDName: Boss_Buru
SD%Complete: 80
SDComment:
SDCategory: Ruins of Ahn'Qiraj
EndScriptData */

#include "precompiled.h"
#include "ruins_of_ahnqiraj.h"

enum Spells
{
    EMOTE_TARGET            = -1509002,

    // Buru
    SPELL_CREEPING_PLAGUE   = 20512,
    SPELL_DISMEMBER         = 96,
    SPELL_FULL_SPEED        = 1557,
    SPELL_THORNS            = 25640,
    SPELL_GATHERING_SPEED   = 1834,
    SPELL_BURU_TRANSFORM    = 24721,

    // Buru Egg
    SPELL_SUMMON_HATCHLING  = 1881,
    SPELL_EXPLODE           = 19593,
};

struct MANGOS_DLL_DECL boss_buruAI : public ScriptedAI
{
    boss_buruAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ruins_of_ahnqiraj*)pCreature->GetInstanceData();
        Reset();
    }

    instance_ruins_of_ahnqiraj* m_pInstance;

    bool m_bIsEnraged;

    uint32 m_uiDismemberTimer;
    uint32 m_uiSpeedTimer;
    uint32 m_uiCreepingPlagueTimer;

    void Reset() 
    {
        m_bIsEnraged = false;
        m_creature->RemoveAllAuras();        
        m_creature->SetSpeedRate(MOVE_RUN, 0.5f);
        m_uiDismemberTimer = 5000;
        m_uiSpeedTimer = 5000;
        m_uiCreepingPlagueTimer = 6000;
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BURU, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BURU, IN_PROGRESS);

        DoCastSpellIfCan(m_creature, SPELL_THORNS);
        m_creature->SetArmor(20000);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BURU, DONE);
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_EXPLODE)
        {
            m_creature->SetSpeedRate(MOVE_RUN, 0.5f);
            m_creature->SetHealth(m_creature->GetHealth() - 45000);
        }
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32& /*uiDamage*/)
    {
        if (!m_bIsEnraged && HealthBelowPct(20))
        {
            DoCastSpellIfCan(m_creature, SPELL_BURU_TRANSFORM);
            m_creature->SetArmor(0);
            m_bIsEnraged = true;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiDismemberTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_DISMEMBER);
            m_uiDismemberTimer = 20000;
        }
        else
            m_uiDismemberTimer -= uiDiff;

        if (m_bIsEnraged)
	{
            if (m_uiCreepingPlagueTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_CREEPING_PLAGUE);
                m_uiCreepingPlagueTimer = 6000;
            }
            else
                m_uiCreepingPlagueTimer -= uiDiff;
	}

        if (m_uiSpeedTimer <= uiDiff)
        {
            m_creature->SetSpeedRate(MOVE_RUN, m_creature->GetSpeedRate(MOVE_RUN) + 0.5f);
            m_uiSpeedTimer = 5000;
        }
        else
            m_uiSpeedTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_buru(Creature* pCreature)
{
    return new boss_buruAI(pCreature);
}

struct MANGOS_DLL_DECL mob_buru_eggAI : public ScriptedAI
{
    mob_buru_eggAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ruins_of_ahnqiraj*)pCreature->GetInstanceData();
        Reset();
    }
    
    instance_ruins_of_ahnqiraj* m_pInstance;

    void Reset()
    {
        SetCombatMovement(false);
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;

		Creature* pBuru = m_pInstance->GetSingleCreatureFromStorage(NPC_BURU);
        if (pBuru && pBuru->isAlive())
        {
            pBuru->GetMotionMaster()->MoveChase(pWho);
            pBuru->AI()->AttackStart(pWho);
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_BURU_EGG_TRIGGER)
        {
            if (!m_pInstance)
                return;

            if (Creature* pBuru = m_pInstance->GetSingleCreatureFromStorage(NPC_BURU))
            {
                pSummoned->CastSpell(pBuru, SPELL_EXPLODE, true);
                pSummoned->CastSpell(pBuru, SPELL_SUMMON_HATCHLING, true);
            }
        }
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
            return;

        Creature* pBuru = m_pInstance->GetSingleCreatureFromStorage(NPC_BURU);
        if (pBuru && pBuru->isAlive())
        {
            pBuru->GetMotionMaster()->MoveChase(pKiller);
            pBuru->Attack(pKiller, true);
            DoScriptText(EMOTE_TARGET, pBuru, pKiller);
            pBuru->RemoveAurasDueToSpell(SPELL_FULL_SPEED);
            m_creature->SummonCreature(NPC_BURU_EGG_TRIGGER, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 1000);                
            pBuru->SetHealth(pBuru->GetHealth() - 45000);
            //pBuru->SetSpeedRate(MOVE_RUN, 0.5f);
        }
    }
};

CreatureAI* GetAI_mob_buru_egg(Creature* pCreature)
{
    return new mob_buru_eggAI(pCreature);
}

void AddSC_boss_buru()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_buru";
    pNewscript->GetAI = &GetAI_boss_buru;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_buru_egg";
    pNewscript->GetAI = &GetAI_mob_buru_egg;
    pNewscript->RegisterSelf();
}
