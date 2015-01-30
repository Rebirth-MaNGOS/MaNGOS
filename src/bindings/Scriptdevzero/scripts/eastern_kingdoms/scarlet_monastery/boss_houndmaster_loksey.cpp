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
SDName: Boss_Houndmaster_Loksey
SD%Complete: 100
SDComment:
SDCategory: Scarlet Monastery
EndScriptData */

#include "precompiled.h"

enum
{
    SAY_AGGRO                       = -1189021,
    SPELL_SUMMON_SCARLET_HOUND      = 17164,			// Can't be cast in combat.
	SPELL_BATTLE_SHOUT				= 6192,
    SPELL_BLOODLUST                 = 6742
};

struct MANGOS_DLL_DECL boss_houndmaster_lokseyAI : public ScriptedAI
{
    boss_houndmaster_lokseyAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiBattleShoutTimer;
	bool bEnrage;

    void Reset()
    {
		bEnrage = false;
		m_uiBattleShoutTimer = 1000;
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_SUMMON_SCARLET_HOUND);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (!bEnrage && HealthBelowPct(25))
        {
            DoCastSpellIfCan(m_creature, SPELL_BLOODLUST);
            bEnrage = true;
        }

        if (m_uiBattleShoutTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_BATTLE_SHOUT);
            m_uiBattleShoutTimer = 120000;
        }
        else 
            m_uiBattleShoutTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_houndmaster_loksey(Creature* pCreature)
{
    return new boss_houndmaster_lokseyAI(pCreature);
}

void AddSC_boss_houndmaster_loksey()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "boss_houndmaster_loksey";
    pNewScript->GetAI = &GetAI_boss_houndmaster_loksey;
    pNewScript->RegisterSelf();
}
