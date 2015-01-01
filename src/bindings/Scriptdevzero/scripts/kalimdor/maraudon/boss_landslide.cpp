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
SDName: Boss_Landslide
SD%Complete: 100
SDComment:
SDCategory: Maraudon
EndScriptData */

#include "precompiled.h"

enum eSpells
{
	SPELL_KNOCKAWAY			= 18670,
	SPELL_TRAMPLE			= 5568,
	SPELL_LANDSLIDE			= 21808
};

struct MANGOS_DLL_DECL boss_landslideAI : public ScriptedAI
{
    boss_landslideAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 KnockAwayTimer;
    uint32 TrampleTimer;
    uint32 LandslideTimer;

    void Reset()
    {
        KnockAwayTimer = 8000;
        TrampleTimer = 2000;
        LandslideTimer = 0;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // KnockAwayTimer
        if (KnockAwayTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_KNOCKAWAY);
            KnockAwayTimer = 15000;
        }
		else
			KnockAwayTimer -= uiDiff;

        // TrampleTimer
        if (TrampleTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_TRAMPLE);
            TrampleTimer = 8000;
        }
		else
			TrampleTimer -= uiDiff;

        // Landslide
        if (HealthBelowPct(50))
        {
            if (LandslideTimer < uiDiff)
            {
                m_creature->InterruptNonMeleeSpells(false);
                DoCastSpellIfCan(m_creature, SPELL_LANDSLIDE);
                LandslideTimer = 60000;
            }
			else
				LandslideTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_landslide(Creature* pCreature)
{
    return new boss_landslideAI(pCreature);
}

void AddSC_boss_landslide()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_landslide";
    pNewscript->GetAI = &GetAI_boss_landslide;
    pNewscript->RegisterSelf();
}
