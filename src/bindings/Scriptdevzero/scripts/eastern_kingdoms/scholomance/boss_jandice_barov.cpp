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
SDName: Boss_jandicebarov
SD%Complete: 100
SDComment:
SDCategory: Scholomance
EndScriptData */

#include "precompiled.h"
#include "scholomance.h"

enum Spells
{
    SPELL_BANISH                  = 8994,
    SPELL_CURSE_OF_BLOOD          = 16098,
    SPELL_ILLUSION                = 17773,
    SPELL_CLEAVE                  = 15584
};

struct MANGOS_DLL_DECL boss_jandice_barovAI : public ScriptedAI
{
    boss_jandice_barovAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_scholomance*)pCreature->GetInstanceData();
        Reset();
    }

    instance_scholomance* m_pInstance;

    bool m_bInvisible;
    uint32 m_uiBanishTimer;
    uint32 m_uiCurseOfBloodTimer;
    uint32 m_uiIllusionTimer;
    uint32 m_uiInvisibleTimer;

    void Reset()
    {
        m_bInvisible = false;
        m_uiBanishTimer = urand(16000,20000);
        m_uiCurseOfBloodTimer = urand(10000,15000);
        m_uiIllusionTimer = 25000;
        m_uiInvisibleTimer = 3000;
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_JANDICE_BAROV, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_JANDICE_BAROV, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_bInvisible)
        {
            m_bInvisible = false;
            m_creature->setFaction(FACTION_HOSTILE);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->SetDisplayId(11073);
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_JANDICE_BAROV, DONE);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
            pSummoned->AI()->AttackStart(pTarget);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bInvisible && m_uiInvisibleTimer < uiDiff)
        {
            m_creature->setFaction(FACTION_HOSTILE);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->SetDisplayId(11073);
            m_bInvisible = false;
        }
        else if (m_bInvisible)
        {
            m_uiInvisibleTimer -= uiDiff;
            return;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Banish
        if (m_uiBanishTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                DoCastSpellIfCan(pTarget, SPELL_BANISH);
            m_uiBanishTimer = urand(25000,35000);
        }
        else
            m_uiBanishTimer -= uiDiff;

        // Curse Of Blood
        if (m_uiCurseOfBloodTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CURSE_OF_BLOOD);
            m_uiCurseOfBloodTimer = 30000;
        }
        else
            m_uiCurseOfBloodTimer -= uiDiff;

        // Illusion summon
        if (!m_bInvisible && m_uiIllusionTimer < uiDiff)
        {
            m_creature->InterruptNonMeleeSpells(false);
            m_creature->setFaction(FACTION_FRIENDLY);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->SetDisplayId(11686);
            m_creature->getThreatManager().modifyThreatPercent(m_creature->getVictim(),-99);

            for(uint8 i = 0; i < 10; ++i)
                DoSpawnCreature(NPC_ILLUSION_OF_JANDICE_BAROV, irand(-10,10), irand(-10,10), 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 25000);

            m_bInvisible = true;
            m_uiInvisibleTimer = 3000;

            m_uiIllusionTimer = 25000;
        }
        else
            m_uiIllusionTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_jandice_barov(Creature* pCreature)
{
    return new boss_jandice_barovAI(pCreature);
}

struct MANGOS_DLL_DECL mob_illusion_of_jandice_barovAI : public ScriptedAI
{
    mob_illusion_of_jandice_barovAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiCleaveTimer;

    void Reset()
    {
        m_uiCleaveTimer = urand(2000,8000);
        m_creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_MAGIC, true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Cleave
        if (m_uiCleaveTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE);
            m_uiCleaveTimer = urand(5000,8000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_illusion_of_jandice_barov(Creature* pCreature)
{
    return new mob_illusion_of_jandice_barovAI(pCreature);
}

void AddSC_boss_jandice_barov()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_jandice_barov";
    pNewscript->GetAI = &GetAI_boss_jandice_barov;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_illusion_of_jandice_barov";
    pNewscript->GetAI = &GetAI_mob_illusion_of_jandice_barov;
    pNewscript->RegisterSelf();
}
