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
    SPELL_ARCANE_EXPLOSION      = 46608,
    SPELL_CONE_OF_COLD          = 38384,
    SPELL_FIREBALL              = 46988,
    SPELL_FROSTBOLT             = 46987,
    SPELL_STORMPIKES_SALVATION  = 23693,
};

enum Yells
{
    YELL_AGGRO                  = -1030022,
    YELL_BUFF                   = -1030023,
    YELL_EVADE                  = -1030024,
};

struct MANGOS_DLL_DECL boss_balinda_stonehearthAI : public ScriptedAI
{
    boss_balinda_stonehearthAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiArcaneExplosionTimer;
    uint32 m_uiConeOfColdTimer;
    uint32 m_uiFireBoltTimer;
    uint32 m_uiFrostboltTimer;
    uint32 m_uiEvadeTimer;

    void Reset()
    {
        m_uiArcaneExplosionTimer      = urand(5*IN_MILLISECONDS,15*IN_MILLISECONDS);
        m_uiConeOfColdTimer           = 8*IN_MILLISECONDS;
        m_uiFireBoltTimer             = 1*IN_MILLISECONDS;
        m_uiFrostboltTimer            = 4*IN_MILLISECONDS;
        m_uiEvadeTimer                = 5*IN_MILLISECONDS;
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(YELL_AGGRO, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Arcane Explosion
        if (m_uiArcaneExplosionTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_ARCANE_EXPLOSION);
            m_uiArcaneExplosionTimer =  urand(5*IN_MILLISECONDS,15*IN_MILLISECONDS);
        }
        else
            m_uiArcaneExplosionTimer -= uiDiff;

        // Cone Of Cold
        if (m_uiConeOfColdTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CONE_OF_COLD);
            m_uiConeOfColdTimer = urand(10*IN_MILLISECONDS,20*IN_MILLISECONDS);
        }
        else
            m_uiConeOfColdTimer -= uiDiff;

        // Firebolt
        if (m_uiFireBoltTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FIREBALL);
            m_uiFireBoltTimer = urand(5*IN_MILLISECONDS,9*IN_MILLISECONDS);
        }
        else
            m_uiFireBoltTimer -= uiDiff;

        // Frostbolt
        if (m_uiFrostboltTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FROSTBOLT);
            m_uiFrostboltTimer = urand(4*IN_MILLISECONDS,12*IN_MILLISECONDS);
        }
        else
            m_uiFrostboltTimer -= uiDiff;

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

CreatureAI* GetAI_boss_balinda_stonehearth(Creature* pCreature)
{
    return new boss_balinda_stonehearthAI(pCreature);
}

void AddSC_boss_balinda_stonehearth()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_balinda_stonehearth";
    pNewscript->GetAI = &GetAI_boss_balinda_stonehearth;
    pNewscript->RegisterSelf();
}
