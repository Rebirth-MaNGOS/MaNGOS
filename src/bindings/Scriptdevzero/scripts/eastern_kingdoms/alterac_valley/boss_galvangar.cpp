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

#include "precompiled.h"

enum Spells
{
    SPELL_CLEAVE              = 15284,
    SPELL_FRIGHTENING_SHOUT   = 19134,
    SPELL_FURY_OF_FROSTWOLF   = 22751,
    SPELL_WHIRLWIND1          = 15589,
    SPELL_WHIRLWIND2          = 13736,
    SPELL_MORTAL_STRIKE       = 16856,
};

enum Yells
{
    YELL_AGGRO                = -1030019,
    YELL_BUFF                 = -1030020,
    YELL_EVADE                = -1030021,
};

struct MANGOS_DLL_DECL boss_galvangarAI : public ScriptedAI
{
    boss_galvangarAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiCleaveTimer;
    uint32 m_uiFuryOfFrostwolfTimer;
    uint32 m_uiFrighteningShoutTimer;
    uint32 m_uiMortalStrikeTimer;
    uint32 m_uiWhirlwind1Timer;
    uint32 m_uiWhirlwind2Timer;
    uint32 m_uiEvadeTimer;

    void Reset()
    {
        m_uiCleaveTimer = urand(1*IN_MILLISECONDS,9*IN_MILLISECONDS);
        m_uiFuryOfFrostwolfTimer = 0;
        m_uiFrighteningShoutTimer = urand(10*IN_MILLISECONDS,15*IN_MILLISECONDS);
        m_uiMortalStrikeTimer = urand(5*IN_MILLISECONDS,20*IN_MILLISECONDS);
        m_uiWhirlwind1Timer = urand(1*IN_MILLISECONDS,13*IN_MILLISECONDS);
        m_uiWhirlwind2Timer = urand(5*IN_MILLISECONDS,20*IN_MILLISECONDS);
        m_uiEvadeTimer = 5*IN_MILLISECONDS;
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(YELL_AGGRO, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Fury of Frostwolf
        if (m_creature->isAlive())
	{
            if (m_uiFuryOfFrostwolfTimer <= uiDiff)
            {
                DoScriptText(YELL_BUFF, m_creature);
                DoCastSpellIfCan(m_creature, SPELL_FURY_OF_FROSTWOLF);
                m_uiFuryOfFrostwolfTimer = 2*MINUTE*IN_MILLISECONDS;
            }
            else
                m_uiFuryOfFrostwolfTimer -= uiDiff;
	}

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Cleave
        if (m_uiCleaveTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE);
            m_uiCleaveTimer =  urand(10*IN_MILLISECONDS,16*IN_MILLISECONDS);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        // Frightening Shout
        if (m_uiFrighteningShoutTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FRIGHTENING_SHOUT);
            m_uiFrighteningShoutTimer = urand(20*IN_MILLISECONDS,30*IN_MILLISECONDS);
        }
        else
            m_uiFrighteningShoutTimer -= uiDiff;

        // Mortal Strike
        if (m_uiMortalStrikeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MORTAL_STRIKE);
            m_uiMortalStrikeTimer = urand(10*IN_MILLISECONDS,30*IN_MILLISECONDS);
        }
        else
            m_uiMortalStrikeTimer -= uiDiff;

        // Whirlwind 1
        if (m_uiWhirlwind1Timer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_WHIRLWIND1);
            m_uiWhirlwind1Timer = urand(6*IN_MILLISECONDS,10*IN_MILLISECONDS);
        }
        else
            m_uiWhirlwind1Timer -= uiDiff;

        // Whirlwind 2
        if (m_uiWhirlwind2Timer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_WHIRLWIND2);
            m_uiWhirlwind2Timer = urand(10*IN_MILLISECONDS,25*IN_MILLISECONDS);
        }
        else
            m_uiWhirlwind2Timer -= uiDiff;

        // Check if creature is not outside of building
        if (m_uiEvadeTimer <= uiDiff)
        {
            float fX, fY, fZ;
            m_creature->GetRespawnCoord(fX,fY,fZ);
            if (m_creature->GetDistance2d(fX, fY) > 50.0f)
            {
                ResetToHome();
                DoScriptText(YELL_EVADE, m_creature);
            }
            m_uiEvadeTimer = 5*IN_MILLISECONDS;
        }
        else
            m_uiEvadeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_galvangar(Creature* pCreature)
{
    return new boss_galvangarAI(pCreature);
}

void AddSC_boss_galvangar()
{
    Script* pNewscript;
    pNewscript = new Script;
    pNewscript->Name = "boss_galvangar";
    pNewscript->GetAI = &GetAI_boss_galvangar;
    pNewscript->RegisterSelf();
}
