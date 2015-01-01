/*
 * Copyright (C) 2006-2011 ScriptDev2 <http://www.scriptdev2.com/>
 * Copyright (C) 2010-2011 ScriptDev0 <http://github.com/mangos-zero/scriptdev0>
 *
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
SDName: Boss_Broodlord_Lashlayer
SD%Complete: 100
SDComment:
SDCategory: Blackwing Lair
EndScriptData */

#include "precompiled.h"
#include "blackwing_lair.h"

enum
{
    SAY_AGGRO                   = -1469000,
    SAY_LEASH                   = -1469001,

    SPELL_CLEAVE                = 26350,
    SPELL_BLAST_WAVE            = 23331,
    SPELL_MORTAL_STRIKE         = 24573,
    SPELL_KNOCK_AWAY            = 25778
};

struct MANGOS_DLL_DECL boss_broodlordAI : public ScriptedAI
{
    boss_broodlordAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = dynamic_cast<instance_blackwing_lair*>(pCreature->GetInstanceData());
        Reset();
    }

    instance_blackwing_lair* m_pInstance;

    uint32 m_uiCleaveTimer;
    uint32 m_uiBlastWaveTimer;
    uint32 m_uiMortalStrikeTimer;
    uint32 m_uiKnockAwayTimer;

    void Reset()
    {
        m_uiCleaveTimer         = GetRndCleaveTimer();                     // These times are probably wrong
        m_uiBlastWaveTimer      = GetRndBlastWaveTimer();
        m_uiMortalStrikeTimer   = GetRndMortalStrikeTimer();
        m_uiKnockAwayTimer      = GetRndKnockAwayTimer();
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LASHLAYER, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, m_creature);
    }

	uint32 GetRndBlastWaveTimer()
	{
		uint32 time_group = urand(0, 99);
		uint32 cast_time;

		if (time_group < 35)
			cast_time = urand(17000, 22000);
		else if (time_group < 70)
			cast_time = urand(23000, 26000);
		else if (time_group < 83)
			cast_time = urand(25000, 27000);
		else if (time_group < 93)
			cast_time = urand(35000, 60000);
		else
			cast_time = urand(13000, 16000);

		return cast_time;
	}

	uint32 GetRndCleaveTimer()
	{
		uint32 time_group = urand(0, 99);
		uint32 cast_time;

		if (time_group < 30)
			cast_time = urand(10000, 13300);
		else if (time_group < 60)
			cast_time = urand(13700, 20500);
		else if (time_group < 89)
			cast_time = urand(20900, 43700);
		else if (time_group < 98)
			cast_time = urand(7700, 9200);
		else if (time_group < 99)
			cast_time = urand(42000, 59000);
		else
			cast_time = 6400;

		return cast_time;
	}


	uint32 GetRndKnockAwayTimer()
	{
		uint32 time_group = urand(0, 99);
		uint32 cast_time;

		if (time_group < 83)
			cast_time = urand(12000, 28900);
		else if (time_group < 90)
			cast_time = urand(29000, 34900);
		else if (time_group < 97)
			cast_time = urand(36100, 69900);
		else
			cast_time = 10800;

		return cast_time;
	}

	uint32 GetRndMortalStrikeTimer()
	{
		uint32 time_group = urand(0, 99);
		uint32 cast_time;

		if (time_group < 63)
			cast_time = urand(7200, 14500);
		else if (time_group < 90)
			cast_time = urand(15600, 25300);
		else if (time_group < 97)
			cast_time = urand(26500, 30100);
		else
			cast_time = 6000;

		return cast_time;
	}

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
		{
            m_pInstance->SetData(TYPE_LASHLAYER, DONE);
			m_pInstance->SuspendOrResumeSuppressionRooms(true);
		}
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LASHLAYER, FAIL);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Cleave Timer
        if (m_uiCleaveTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE) == CAST_OK)
                m_uiCleaveTimer = GetRndCleaveTimer();
        }
        else
            m_uiCleaveTimer -= uiDiff;

        // Blast Wave
        if (m_uiBlastWaveTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BLAST_WAVE) == CAST_OK)
                m_uiBlastWaveTimer = GetRndBlastWaveTimer();
        }
        else
            m_uiBlastWaveTimer -= uiDiff;

        // Mortal Strike Timer
        if (m_uiMortalStrikeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_MORTAL_STRIKE) == CAST_OK)
                m_uiMortalStrikeTimer = GetRndMortalStrikeTimer();
        }
        else
            m_uiMortalStrikeTimer -= uiDiff;

        if (m_uiKnockAwayTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_KNOCK_AWAY);
            // Drop 50% aggro - TODO should be scriptedEffect?
            if (m_creature->getThreatManager().getThreat(m_creature->getVictim()))
                m_creature->getThreatManager().modifyThreatPercent(m_creature->getVictim(), -50);

            m_uiKnockAwayTimer = GetRndKnockAwayTimer();
        }
        else
            m_uiKnockAwayTimer -= uiDiff;

        DoMeleeAttackIfReady();

        if (EnterEvadeIfOutOfCombatArea(uiDiff))
            DoScriptText(SAY_LEASH, m_creature);
    }
};
CreatureAI* GetAI_boss_broodlord(Creature* pCreature)
{
    return new boss_broodlordAI(pCreature);
}

void AddSC_boss_broodlord()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_broodlord";
    pNewScript->GetAI = &GetAI_boss_broodlord;
    pNewScript->RegisterSelf();
}
