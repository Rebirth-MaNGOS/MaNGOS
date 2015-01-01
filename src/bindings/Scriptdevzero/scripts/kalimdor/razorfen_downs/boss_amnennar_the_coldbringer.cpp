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
SDName: Boss Amnennar The Coldbringer
SD%Complete: 100
SDComment:
SDCategory: Razorfen Downs
EndScriptData */

#include "precompiled.h"
#include "razorfen_downs.h"

enum Yells
{
    SAY_AGGRO               = -1129000,
    SAY_SUMMON_1            = -1129001,
    SAY_SUMMON_2            = -1129002,
    SAY_HP                  = -1129003,
    SAY_KILL                = -1129004
};

enum Spells
{
    SPELL_AMNENNARS_WRATH   = 13009,
    SPELL_FROSTBOLT         = 15530,
    SPELL_FROST_NOVA        = 15531,
    SPELL_FROST_SPECTRES    = 12642
};


struct MANGOS_DLL_DECL boss_amnennar_the_coldbringerAI : public ScriptedAI
{
    boss_amnennar_the_coldbringerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_razorfen_downs*)pCreature->GetInstanceData(); 
        Reset();
    }
    
    instance_razorfen_downs* m_pInstance;

    bool Spectrals25;
    bool Spectrals45;
    bool Spectrals65;
    bool Hp;

	uint32 m_uiAmnenarsWrathTimer;
    uint32 m_uiFrostBoltTimer;
    uint32 m_uiFrostNovaTimer;

    void Reset()
    {
        Spectrals25 = false;
        Spectrals45 = false;
        Spectrals65 = false;
        Hp = false;

		m_uiAmnenarsWrathTimer = 8000;
        m_uiFrostBoltTimer = 1000;
        m_uiFrostNovaTimer = urand(10000, 15000);
    }

    void Aggro(Unit* /*who*/)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AMNENNAR, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*pKilled*/)
    {
        DoScriptText(SAY_KILL, m_creature);
    }

    void JustDied(Unit* /*killer*/)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AMNENNAR, DONE);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AMNENNAR, NOT_STARTED);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Amnenars Wrath spell
        if (m_uiAmnenarsWrathTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_AMNENNARS_WRATH);
            m_uiAmnenarsWrathTimer = 12000;
        }
		else
			m_uiAmnenarsWrathTimer -= uiDiff;

        // Frost Bolt spell
        if (m_uiFrostBoltTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FROSTBOLT);
            m_uiFrostBoltTimer = 8000;
        }
		else
			m_uiFrostBoltTimer -= uiDiff;

        // Frost Nova spell
        if (m_uiFrostNovaTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_FROST_NOVA);
            m_uiFrostNovaTimer = 15000;
        }
		else
			m_uiFrostNovaTimer -= uiDiff;

        if (!Spectrals65 && HealthBelowPct(65))
        {
            DoScriptText(SAY_SUMMON_1, m_creature);
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FROST_SPECTRES);
            Spectrals65 = true;
        }

        if(!Hp && HealthBelowPct(50))
        {
            DoScriptText(SAY_HP, m_creature);
            Hp = true;
        }

        if (!Spectrals45 && HealthBelowPct(45))
        {
            DoScriptText(SAY_SUMMON_2, m_creature);
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FROST_SPECTRES);
            Spectrals45 = true;
        }

        if (!Spectrals25 && HealthBelowPct(25))
        {
            DoScriptText(SAY_SUMMON_1, m_creature);
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FROST_SPECTRES);
            Spectrals25 = true;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_amnennar_the_coldbringer(Creature* pCreature)
{
    return new boss_amnennar_the_coldbringerAI(pCreature);
}

void AddSC_boss_amnennar_the_coldbringer()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_amnennar_the_coldbringer";
    pNewscript->GetAI = &GetAI_boss_amnennar_the_coldbringer;
    pNewscript->RegisterSelf();
}
