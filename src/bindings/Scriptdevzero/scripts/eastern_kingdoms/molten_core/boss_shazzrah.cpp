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
SDName: Boss_Shazzrah
SD%Complete: 90
SDComment:
SDCategory: Molten Core
EndScriptData */

#include "precompiled.h"
#include "molten_core.h"

enum Spells
{
    SPELL_ARCANE_EXPLOSION          = 19712,
    SPELL_COUNTERSPELL              = 19715,
    SPELL_DEADEN_MAGIC              = 19714,
    SPELL_SHAZZRAH_CURSE            = 19713,
    SPELL_SHAZZRAH_GATE             = 23138,
    SPELL_SHAZZRAH_GATE_EFFECT      = 23139
};

struct MANGOS_DLL_DECL boss_shazzrahAI : public ScriptedAI
{
    boss_shazzrahAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_molten_core*)pCreature->GetInstanceData();
        Reset();
    }

    instance_molten_core* m_pInstance;

    uint32 m_uiArcaneExplosionTimer;
    uint32 m_uiShazzrahCurseTimer;
    uint32 m_uiCounterspellTimer;
    uint32 m_uiDeadenMagicTimer;
    uint32 m_uiGateOfShazzrahTimer;

    void Reset()
    {
		m_creature->SetBoundingValue(0, 5);
		m_creature->SetBoundingValue(1, 3);

        m_uiArcaneExplosionTimer = 5000;                       // These times are probably wrong
        m_uiShazzrahCurseTimer = 10000;
        m_uiCounterspellTimer = 15000;
        m_uiDeadenMagicTimer = 7000;
        m_uiGateOfShazzrahTimer = 40000;
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SHAZZRAH, FAIL);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SHAZZRAH, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Arcane Explosion
        if (m_uiArcaneExplosionTimer <= uiDiff)
        {
			if (this->DoCastSpellIfCan(m_creature->getVictim(), SPELL_ARCANE_EXPLOSION) == CAST_OK)
				m_uiArcaneExplosionTimer = 4000;
			else
				m_uiArcaneExplosionTimer = 0;
        }
        else
            m_uiArcaneExplosionTimer -= uiDiff;

        // Shazzrah Curse
        if (m_uiShazzrahCurseTimer <= uiDiff)
        {
			if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
			{
				if (this->DoCastSpellIfCan(pTarget, SPELL_SHAZZRAH_CURSE) == CAST_OK)
					m_uiShazzrahCurseTimer = urand(15000, 20000);
				else
					m_uiShazzrahCurseTimer = 0;
			} else
				m_uiShazzrahCurseTimer = 0;
        }
        else
            m_uiShazzrahCurseTimer -= uiDiff;

        // Counterspell
        if (m_uiCounterspellTimer <= uiDiff)
        {
			if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_COUNTERSPELL) == CAST_OK)
				m_uiCounterspellTimer = urand(16000, 20000);
			else
				m_uiCounterspellTimer = 0;
        }
        else
            m_uiCounterspellTimer -= uiDiff;

        // Deaden Magic
        if (m_uiDeadenMagicTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_DEADEN_MAGIC) == CAST_OK)
				m_uiDeadenMagicTimer = 7000;
			else
				m_uiDeadenMagicTimer = 0;
        }
        else
            m_uiDeadenMagicTimer -= uiDiff;

        // Gate of Shazzrah
        if (m_uiGateOfShazzrahTimer <= uiDiff)
        {
			if (!m_creature->IsNonMeleeSpellCasted(true))
			{
				Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1);
				if (pTarget && pTarget->GetDistance(m_creature) >= 15.0f)
				{
					DoResetThreat();
					DoCastSpellIfCan(pTarget, SPELL_SHAZZRAH_GATE_EFFECT, CAST_TRIGGERED);
					m_creature->AddThreat(pTarget, 50.0f);
					m_creature->AI()->AttackStart(pTarget);
					m_uiGateOfShazzrahTimer = 40000;
				}
			} else
				m_uiGateOfShazzrahTimer = 0;
        }
        else
            m_uiGateOfShazzrahTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_shazzrah(Creature* pCreature)
{
    return new boss_shazzrahAI(pCreature);
}

void AddSC_boss_shazzrah()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_shazzrah";
    pNewscript->GetAI = &GetAI_boss_shazzrah;
    pNewscript->RegisterSelf();
}
