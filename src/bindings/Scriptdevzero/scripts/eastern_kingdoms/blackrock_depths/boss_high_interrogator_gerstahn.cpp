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
SDName: Boss_High_Interrogator_Gerstahn
SD%Complete: 100
SDComment:
SDCategory: Blackrock Depths
EndScriptData */

#include "precompiled.h"

#define SPELL_SHADOWWORDPAIN        14032 //10894
#define SPELL_MANABURN              14033 //10876
#define SPELL_PSYCHICSCREAM         13704 //8122
#define SPELL_SHADOWSHIELD          12040 //22417

struct MANGOS_DLL_DECL boss_high_interrogator_gerstahnAI : public ScriptedAI
{
    boss_high_interrogator_gerstahnAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 ShadowWordPain_Timer;
    uint32 ManaBurn_Timer;
    uint32 PsychicScream_Timer;
    uint32 ShadowShield_Timer;

    void Reset()
    {
        ShadowWordPain_Timer = 4000;
        ManaBurn_Timer = 12000;		 // old value: 14000
        PsychicScream_Timer = 16000; // old value: 32000
        ShadowShield_Timer = 8000;
    }

	void Aggro(Unit*)
	{
		m_creature->CallAssistance(3);
	}

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //ShadowWordPain_Timer
        if (ShadowWordPain_Timer < diff)
        {
            Unit* target = NULL;
            target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
            if (target)DoCastSpellIfCan(target,SPELL_SHADOWWORDPAIN);
            ShadowWordPain_Timer = 7000;
        }else ShadowWordPain_Timer -= diff;

        //ManaBurn_Timer
        if (ManaBurn_Timer < diff)
        {
            Unit* target = NULL;
            target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
            if (target)DoCastSpellIfCan(target,SPELL_MANABURN);
            ManaBurn_Timer = 10000;
        }else ManaBurn_Timer -= diff;

        //PsychicScream_Timer
        if (PsychicScream_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(),SPELL_PSYCHICSCREAM);
            PsychicScream_Timer = 18000; // old value: 30000
        }else PsychicScream_Timer -= diff;

        //ShadowShield_Timer
        if (ShadowShield_Timer < diff)
        {
            DoCastSpellIfCan(m_creature,SPELL_SHADOWSHIELD);
            ShadowShield_Timer = 25000;
        }else ShadowShield_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_high_interrogator_gerstahn(Creature* pCreature)
{
    return new boss_high_interrogator_gerstahnAI(pCreature);
}

void AddSC_boss_high_interrogator_gerstahn()
{
    Script* pNewscript;
    pNewscript = new Script;
    pNewscript->Name = "boss_high_interrogator_gerstahn";
    pNewscript->GetAI = &GetAI_boss_high_interrogator_gerstahn;
    pNewscript->RegisterSelf();
}
