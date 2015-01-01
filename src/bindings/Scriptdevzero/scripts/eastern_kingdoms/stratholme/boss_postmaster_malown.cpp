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
SDName: boss_postmaster_malown
SD%Complete: 90
SDComment:
SDCategory: Stratholme
EndScriptData */

#include "precompiled.h"
#include "stratholme.h" 

enum
{
    SAY_AGGRO                   = -1329010,
    SAY_MALOWNED                = -1329011,

    SPELL_WAILING_DEAD          = 7713,
    SPELL_BACKHAND              = 6253,
    SPELL_CURSE_OF_WEAKNESS     = 8552,
    SPELL_CURSE_OF_TONGUES      = 12889,
    SPELL_CALL_OF_THE_GRAVE     = 17831
};

struct MANGOS_DLL_DECL boss_postmaster_malownAI : public ScriptedAI
{
    boss_postmaster_malownAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    bool m_bHasYelled;
    uint32 m_uiBackhandTimer;
    uint32 m_uiCallOfTheGraveTimer;
    uint32 m_uiCurseOfWeaknessTimer;
    uint32 m_uiCurseOfTonguesTimer;
    uint32 m_uiWailingDeadTimer;

    void Reset()
    {
        m_bHasYelled = false;
        m_uiBackhandTimer = urand(4000,6000);
        m_uiCallOfTheGraveTimer = urand(25000,28000);
        m_uiCurseOfWeaknessTimer = urand(15000,18000);
        m_uiCurseOfTonguesTimer = urand(21000,23000);
        m_uiWailingDeadTimer = urand(10000,12000);
    }

    void JustReachedHome()
    {
        m_creature->RemoveFromWorld();
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void KilledUnit(Unit* /*pVictim*/)
    {
        DoScriptText(SAY_MALOWNED, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Backhand spell
        if (m_uiBackhandTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BACKHAND);
            m_uiBackhandTimer = urand(4000,8000);
        }
        else
            m_uiBackhandTimer -= uiDiff;

        // Call Of The Grave spell
        if (m_uiCallOfTheGraveTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, SPELL_CALL_OF_THE_GRAVE);
            m_uiCallOfTheGraveTimer = urand(20000,25000);
        }
        else
            m_uiCallOfTheGraveTimer -= uiDiff;

        // Curse Of Weakness spell
        if (m_uiCurseOfWeaknessTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, SPELL_CURSE_OF_WEAKNESS);
            m_uiCurseOfWeaknessTimer = urand(20000,25000);
        }
        else
            m_uiCurseOfWeaknessTimer -= uiDiff;

        // Curse Of Tongues spell
        if (m_uiCurseOfTonguesTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, SPELL_CURSE_OF_TONGUES);
            m_uiCurseOfTonguesTimer = urand(20000,25000);
        }
        else
            m_uiCurseOfTonguesTimer -= uiDiff;

        // Wailing Dead spell
        if (m_uiWailingDeadTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_WAILING_DEAD);
            m_uiWailingDeadTimer = urand(15000,20000);
        }
        else
            m_uiWailingDeadTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_postmaster_malown(Creature* pCreature)
{
    return new boss_postmaster_malownAI(pCreature);
}

void AddSC_boss_postmaster_malown()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_postmaster_malown";
    pNewscript->GetAI = &GetAI_boss_postmaster_malown;
    pNewscript->RegisterSelf();
}
