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
SDName: Boss_Mekgineer_Thermaplug
SD%Complete: 70
SDComment:
SDCategory: Gnomeregan
EndScriptData */

#include "precompiled.h"
#include "gnomeregan.h"

enum eMekgineerThermaplug
{
    // Mekgineer Thermaplug spells
    SPELL_KNOCK_AWAY          = 10101,
    SPELL_KNOCK_AWAY_         = 11130,

    // Walking Bomb spells
	SPELL_WALKING_BOMB_EFFECT_DMG = 25099,		// Does damage to players around
    SPELL_WALKING_BOMB_EFFECT = 11504,			// Kills the bomb
};

struct MANGOS_DLL_DECL boss_mekgineer_thermaplugAI : public ScriptedAI
{
    boss_mekgineer_thermaplugAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_gnomeregan*)pCreature->GetInstanceData();
        Reset();
    }

    instance_gnomeregan* m_pInstance;
    uint32 m_uiKnockAwayTimer;
    uint32 m_uiKnockAway_Timer;
    uint32 m_uiYellTimer;

  	void Reset()
  	{
        m_uiKnockAwayTimer = 15000;
        m_uiKnockAway_Timer = 40000;
        m_uiYellTimer = 20000;
  	}

    void Aggro(Unit* /*pAttacker*/)
    {
        DoScriptText(YELL_AGGRO, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_THERMAPLUG, IN_PROGRESS);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_THERMAPLUG, NOT_STARTED);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_THERMAPLUG, DONE);
    }

    void KilledUnit(Unit* /*pVictim*/)
    {
        DoScriptText(YELL_KILL, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Knock Away
        if (m_uiKnockAwayTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_KNOCK_AWAY);
            m_uiKnockAwayTimer = urand(15000, 35000);
        }
        else
            m_uiKnockAwayTimer -= uiDiff;

        // Knock Away (2)
        if (m_uiKnockAway_Timer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_KNOCK_AWAY_);
            m_uiKnockAway_Timer = urand(40000, 60000);
        }
        else
            m_uiKnockAway_Timer -= uiDiff;

        // "My machines are the future.." yell timer
        if (m_uiYellTimer <= uiDiff)
        {
            DoScriptText(YELL_COMBAT, m_creature);
            m_uiYellTimer = urand(40000, 65000);
        }
        else
            m_uiYellTimer -= uiDiff;

		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_mekgineer_thermaplug(Creature* pCreature)
{
    return new boss_mekgineer_thermaplugAI(pCreature);
}

/*######
## mob_walking_bomb
######*/

struct MANGOS_DLL_DECL mob_walking_bombAI : public ScriptedAI
{
    mob_walking_bombAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiWalkingBombEffectTimer;
	
    void Reset()
    {
        m_uiWalkingBombEffectTimer = 3000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

			if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
			{
				if (m_creature->IsWithinDistInMap(pTarget, 10.0f))		// If player is within 10yrds blow up
				{
					if (m_uiWalkingBombEffectTimer <= uiDiff)
					{
					// Walking Bomb Effect (200 dmg in 10 yard radius + caster instant kill)	- spell not working well
					DoCastSpellIfCan(m_creature, SPELL_WALKING_BOMB_EFFECT_DMG);			// does about 50 dmg
					DoCastSpellIfCan(m_creature, SPELL_WALKING_BOMB_EFFECT);
					}
					else
						m_uiWalkingBombEffectTimer -= uiDiff;
				}
			}
		   
        // No melee
    }
};

CreatureAI* GetAI_mob_walking_bomb(Creature* pCreature)
{
    return new mob_walking_bombAI(pCreature);
}

void AddSC_boss_mekgineer_thermaplug()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_mekgineer_thermaplug";
    pNewScript->GetAI = &GetAI_boss_mekgineer_thermaplug;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_walking_bomb";
    pNewScript->GetAI = &GetAI_mob_walking_bomb;
    pNewScript->RegisterSelf();
}
