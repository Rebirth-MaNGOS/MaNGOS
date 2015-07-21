/* Special Thanks the Mangos-Zero, and Scriptdev2, and ScriptDev-Zero team!
 * Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Boss_Lord_Kazzak
SD%Complete: 70
SDComment: Core Support needed for spells: 28599 (range), 21056 (explode)
SDCategory: Dark-Portal
EndScriptData */

#include "precompiled.h"

#define SAY_INTRO                       -1000147
#define SAY_AGGRO1                      -1000148
#define SAY_AGGRO2                      -1000149
#define SAY_SURPREME1                   -1000150
#define SAY_SURPREME2                   -1000151
#define SAY_KILL1                       -1000152
#define SAY_KILL2                       -1000153
#define SAY_KILL3                       -1000154
#define SAY_DEATH                       -1000155
#define EMOTE_FRENZY                    -1000156
#define SAY_RAND1                       -1000157
#define SAY_RAND2                       -1000158

#define SPELL_SHADOWVOLLEY              28599
#define SPELL_CLEAVE                    27794
// #define SPELL_THUNDERCLAP               23931
#define SPELL_THUNDERCLAP				8078
#define SPELL_VOIDBOLT                  21066
#define SPELL_MARKOFKAZZAK              21056
#define SPELL_ENRAGE                    28747
#define SPELL_CAPTURESOUL               21054
#define SPELL_TWISTEDREFLECTION         21063
#define SPELL_EXPLODE                   21058

/* 
Melee
Shadowbolt Volley
Cleave
Thunderclap
Twisted Reflection
Mark of Kazzak
Void Bolt
Capture Soul
*/
struct MANGOS_DLL_DECL boss_lordkazzakAI : public ScriptedAI
{
    boss_lordkazzakAI(Creature *c) : ScriptedAI(c) {Reset();}

    uint32 ShadowVolley_Timer;
    uint32 Cleave_Timer;
    uint32 ThunderClap_Timer;
    uint32 VoidBolt_Timer;
    uint32 MarkOfKazzak_Timer;
    uint32 Enrage_Timer;
    uint32 Twisted_Reflection_Timer;

	bool m_bEnraged;

    void Reset()
    {
		m_creature->SetBoundingValue(0, 3);
		m_creature->SetBoundingValue(1, 2);

		m_bEnraged = false;
        ShadowVolley_Timer = urand(7000,11000);
        Cleave_Timer = 7000;
        ThunderClap_Timer = urand(14000,17000);
        VoidBolt_Timer = 20000;
        MarkOfKazzak_Timer = 25000;
        Enrage_Timer = 180000;
        Twisted_Reflection_Timer = 15000;  
    }

    void JustRespawned()
    {
        DoScriptText(SAY_INTRO, m_creature);
    }

    void Aggro(Unit* /*who*/)
    {
        switch(rand()%2)
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
        }
    }

    void KilledUnit(Unit* victim)
    {
        
        if (victim->GetTypeId() != TYPEID_PLAYER)
            return;

        //DoCastSpellIfCan(m_creature,SPELL_CAPTURESOUL);
		m_creature->SetHealth(m_creature->GetHealth()+70000);

        switch(urand(0,2))
        {
            case 0: DoScriptText(SAY_KILL1, m_creature); break;
            case 1: DoScriptText(SAY_KILL2, m_creature); break;
            case 2: DoScriptText(SAY_KILL3, m_creature); break;
        }
    }

    void JustDied(Unit* /*victim*/)
    {
        DoScriptText(SAY_DEATH, m_creature);
		uint32 respawn_time = urand(86400, 604800); // A random respawn time between 1 day and 7 days.
	
		m_creature->SetRespawnDelay(respawn_time);
		m_creature->SetRespawnTime(respawn_time);
		m_creature->SaveRespawnTime();
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() )
            return;

        //ShadowVolley_Timer
        if (ShadowVolley_Timer < diff)
        {
			int32 bp = 1100;
			m_creature->CastCustomSpell(m_creature->getVictim(), SPELL_SHADOWVOLLEY, &bp,NULL,NULL,false);
			if (!m_bEnraged)
				ShadowVolley_Timer = urand(8000,11000);
			else 
				ShadowVolley_Timer = 1000;
        }else ShadowVolley_Timer -= diff;

        //Cleave_Timer
        if (Cleave_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(),SPELL_CLEAVE);
            Cleave_Timer = urand(6000,8000);
        }else Cleave_Timer -= diff;

        //ThunderClap_Timer
        if (ThunderClap_Timer < diff)
        {
            DoCastSpellIfCan(m_creature,SPELL_THUNDERCLAP);
            ThunderClap_Timer = urand(13000,16000);
        }else ThunderClap_Timer -= diff;

        //VoidBolt_Timer
        if (VoidBolt_Timer < diff)
        {
			int32 bp = 2500;
			m_creature->CastCustomSpell(m_creature->getVictim(), SPELL_VOIDBOLT, &bp,NULL,NULL,false);
            VoidBolt_Timer = urand(16000,20000);
        }else VoidBolt_Timer -= diff;

        //MarkOfKazzak_Timer
        if (MarkOfKazzak_Timer < diff)
        {
            Unit* victim = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1);
            if(victim && victim->GetTypeId() == TYPEID_PLAYER && victim->GetPower(POWER_MANA))
            {
                DoCastSpellIfCan(victim, SPELL_MARKOFKAZZAK);
                MarkOfKazzak_Timer = 15000;
            }
        }else MarkOfKazzak_Timer -= diff;

        //Enrage_Timer
		if (!m_bEnraged)
		{
			if (Enrage_Timer < diff)
			{
				DoScriptText(EMOTE_FRENZY, m_creature);
				m_bEnraged = true;
				ShadowVolley_Timer = 1000;
				//DoCastSpellIfCan(m_creature,SPELL_ENRAGE);
				//Enrage_Timer = 30000;
			}else Enrage_Timer -= diff;
		}

		//Twisted_Reflection_Timer
        if(Twisted_Reflection_Timer < diff)
        {
			Unit* u = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1);
			if (u && u->GetTypeId() == TYPEID_PLAYER)
			{
				DoCastSpellIfCan(u, SPELL_TWISTEDREFLECTION);
				Twisted_Reflection_Timer = 15000;
			}
        }else Twisted_Reflection_Timer -= diff;

        DoMeleeAttackIfReady();
    }

};

CreatureAI* GetAI_boss_lordkazzak(Creature *_Creature)
{
    return new boss_lordkazzakAI (_Creature);
}

void AddSC_boss_lord_kazzak()
{
    Script* pNewscript;
    pNewscript = new Script;
    pNewscript->Name = "boss_lord_kazzak";
    pNewscript->GetAI = &GetAI_boss_lordkazzak;
    pNewscript->RegisterSelf();
}

