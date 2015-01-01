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
SDName: Boss_Quartmaster_Zigris
SD%Complete: 100
SDComment:
SDCategory: Blackrock Spire
EndScriptData */

#include "precompiled.h"

enum Spells
{
    SPELL_DRINK_HEALING_POTION  = 15504,
    SPELL_HOOKED_NET            = 15609,
    SPELL_SHOOT                 = 16496,
    SPELL_STUN_BOMB             = 16497
};

struct MANGOS_DLL_DECL boss_quatermaster_zigrisAI : public ScriptedAI
{
    boss_quatermaster_zigrisAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiDrinkHealingPotionTimer;
    uint32 m_uiHookedNetTimer;
    uint32 m_uiShootTimer;
    uint32 m_uiStunBombTimer;    

    void Reset()
    {
        m_uiDrinkHealingPotionTimer = urand(20000,30000);
        m_uiHookedNetTimer = urand(8000,12000);
        m_uiShootTimer = urand(0,500);
        m_uiStunBombTimer = urand(3000,5000);        
    }

    void Aggro(Unit* /*pWho*/)
    {
        m_creature->CallForHelp(DEFAULT_VISIBILITY_INSTANCE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Drink Healing Potion
        if (HealthBelowPct(75) && m_uiDrinkHealingPotionTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_DRINK_HEALING_POTION);
            m_uiDrinkHealingPotionTimer = urand(10000,15000);
        }
        else
            m_uiDrinkHealingPotionTimer -= uiDiff;

        // Hooked Net
        if (m_uiHookedNetTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_HOOKED_NET);
            m_uiHookedNetTimer = urand(4000,7000);
        }
        else
            m_uiHookedNetTimer -= uiDiff;

        // Shoot
        if (m_uiShootTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHOOT);
            m_uiShootTimer = urand(4000,8000);
        }
        else
            m_uiShootTimer -= uiDiff;

        // Stun Bomb
        if (m_uiStunBombTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_STUN_BOMB);
            m_uiStunBombTimer = urand(7000,10000);
        }
        else
            m_uiStunBombTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_quatermaster_zigris(Creature* pCreature)
{
    return new boss_quatermaster_zigrisAI(pCreature);
}

void AddSC_boss_quatermaster_zigris()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "quartermaster_zigris";
    pNewscript->GetAI = &GetAI_boss_quatermaster_zigris;
    pNewscript->RegisterSelf();
}
