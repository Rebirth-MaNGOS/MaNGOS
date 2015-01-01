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
    SPELL_FRENZY            = 8269,
    SPELL_KNOCKDOWN         = 19128,
    SPELL_WHIRLWIND         = 15589,
    SPELL_WHIRLWIND2        = 13736,
    SPELL_SWEEPING_STRIKES  = 18765, // not sure
    SPELL_CLEAVE            = 20677, // not sure
    SPELL_WINDFURY          = 35886, // not sure
    SPELL_STORMPIKE         = 51876, // not sure
};

enum Yells
{
    YELL_AGGRO              = -1030000,
    YELL_EVADE              = -1030001,
    YELL_RESPAWN            = -1030002,
    YELL_RANDOM1            = -1030003,
    YELL_RANDOM2            = -1030004,
    YELL_RANDOM3            = -1030005,
    YELL_RANDOM4            = -1030006,
    YELL_RANDOM5            = -1030007,
};

struct MANGOS_DLL_DECL boss_drektharAI : public ScriptedAI
{
    boss_drektharAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiFrenzyTimer;
    uint32 m_uiKnockdownTimer;
    uint32 m_uiWhirlwindTimer;
    uint32 m_uiWhirlwind2Timer;
    uint32 m_uiYellTimer;
    uint32 m_uiEvadeTimer;

    void Reset()
    {
        m_uiWhirlwindTimer = urand(1*IN_MILLISECONDS,20*IN_MILLISECONDS);
        m_uiWhirlwind2Timer = urand(1*IN_MILLISECONDS,20*IN_MILLISECONDS);
        m_uiKnockdownTimer = 12*IN_MILLISECONDS;
        m_uiFrenzyTimer = 6*IN_MILLISECONDS;
        m_uiYellTimer = urand(20*IN_MILLISECONDS,30*IN_MILLISECONDS);
        m_uiEvadeTimer = 5*IN_MILLISECONDS;
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(YELL_AGGRO, m_creature);
    }

    void JustReachedHome()
    {
        DoScriptText(YELL_RESPAWN, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Frenzy
        if (m_uiFrenzyTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FRENZY);
            m_uiFrenzyTimer = urand(20*IN_MILLISECONDS,30*IN_MILLISECONDS);
        }
        else
            m_uiFrenzyTimer -= uiDiff;

        // Knockdown
        if (m_uiKnockdownTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_KNOCKDOWN);
            m_uiKnockdownTimer = urand(10*IN_MILLISECONDS,15*IN_MILLISECONDS);
        }
        else
            m_uiKnockdownTimer -= uiDiff;

        // Whirlwind 1
        if (m_uiWhirlwindTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_WHIRLWIND);
            m_uiWhirlwindTimer =  urand(8*IN_MILLISECONDS,18*IN_MILLISECONDS);
        }
        else
            m_uiWhirlwindTimer -= uiDiff;

        // Whirlwind 2
        if (m_uiWhirlwind2Timer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_WHIRLWIND2);
            m_uiWhirlwind2Timer = urand(7*IN_MILLISECONDS,25*IN_MILLISECONDS);
        }
        else
            m_uiWhirlwind2Timer -= uiDiff;

        // Yell
        if (m_uiYellTimer <= uiDiff)
        {
            switch(urand(0,5))
            {
                case 0:
                    DoScriptText(YELL_RANDOM1, m_creature);
                    break;
                case 1:
                    DoScriptText(YELL_RANDOM2, m_creature);
                    break;
                case 2:
                    DoScriptText(YELL_RANDOM3, m_creature);
                    break;
                case 3:
                    DoScriptText(YELL_RANDOM4, m_creature);
                    break;
                case 4:
                    DoScriptText(YELL_RANDOM5, m_creature);
                    break;
            }
            m_uiYellTimer = urand(20*IN_MILLISECONDS,30*IN_MILLISECONDS);
        }
        else
            m_uiYellTimer -= uiDiff;

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


CreatureAI* GetAI_boss_drekthar(Creature* pCreature)
{
    return new boss_drektharAI(pCreature);
}

void AddSC_boss_drekthar()
{
    Script* pNewscript;
    pNewscript = new Script;
    pNewscript->Name = "boss_drekthar";
    pNewscript->GetAI = &GetAI_boss_drekthar;
    pNewscript->RegisterSelf();
}
