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
SDName: Boss_Sulfuron_Harbringer
SD%Complete: 80
SDComment:
SDCategory: Molten Core
EndScriptData */

#include "precompiled.h"
#include "molten_core.h"

enum eSulfuron
{
    // Sulfuron Harbinger
    SPELL_DEMORALIZING_SHOUT    = 19778,
    SPELL_INSPIRE               = 19779,
    SPELL_HAND_OF_RAGNAROS      = 19780,
    SPELL_FLAME_SPEAR           = 19781,

    // Adds Spells
    SPELL_DARKSTRIKE            = 19777,
    SPELL_HEAL                  = 19775,
    SPELL_SHADOW_WORD_PAIN      = 19776,
    SPELL_IMMOLATE              = 20294
};

struct MANGOS_DLL_DECL boss_sulfuronAI : public ScriptedAI
{
    boss_sulfuronAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_molten_core*)pCreature->GetInstanceData();
        Reset();
    }

    instance_molten_core* m_pInstance;

    uint32 m_uiDemoralizingShoutTimer;
    uint32 m_uiInspireTimer;
    uint32 m_uiHandOfRagnarosTimer;
    uint32 m_uiFlameSpearTimer;

    void Reset()
    {
        m_uiDemoralizingShoutTimer = 15000;                 // These times are probably wrong
        m_uiInspireTimer = 13000;
        m_uiHandOfRagnarosTimer = 6000;
        m_uiFlameSpearTimer = 2000;
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SULFURON, FAIL);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SULFURON, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Demoralizing Shout
        if (m_uiDemoralizingShoutTimer <= uiDiff)
        {
			m_uiDemoralizingShoutTimer = 0;
            
			if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_DEMORALIZING_SHOUT) == CAST_OK)
				m_uiDemoralizingShoutTimer = urand(40000, 45000);
        }
        else
            m_uiDemoralizingShoutTimer -= uiDiff;

        // Inspire
        if (m_uiInspireTimer <= uiDiff)
        {
			m_uiInspireTimer = 0;

			if (DoCastSpellIfCan(m_creature, SPELL_INSPIRE) == CAST_OK)
				m_uiInspireTimer = urand(26000, 30000);
        }
        else
            m_uiInspireTimer -= uiDiff;

        // Hand of Ragnaros
        if (m_uiHandOfRagnarosTimer <= uiDiff)
        {
			m_uiHandOfRagnarosTimer = 0;

            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_HAND_OF_RAGNAROS) == CAST_OK)
				m_uiHandOfRagnarosTimer = urand(20000, 25000);
        }
        else
            m_uiHandOfRagnarosTimer -= uiDiff;

        // Flame Spear
        if (m_uiFlameSpearTimer <= uiDiff)
        {
			m_uiFlameSpearTimer = 0;

            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
			{
				//Only use on ranged targets
                if (!m_creature->IsWithinDistInMap(pTarget,5,true) && DoCastSpellIfCan(pTarget, SPELL_FLAME_SPEAR) == CAST_OK)
					m_uiFlameSpearTimer = urand(12000, 16000);
			}
        }
        else
            m_uiFlameSpearTimer -= uiDiff;

		// For making sure that the adds get pulled back into combat should they evade.
		if (m_pInstance)
		{
			for (const ObjectGuid current_add : m_pInstance->GetSulfuronAdds())
			{
				Creature* pAdd = m_pInstance->instance->GetCreature(current_add);

				if (pAdd && pAdd->isAlive() && !pAdd->isInCombat())
				{
					pAdd->SetInCombatWithZone();
					pAdd->Attack(m_creature->getVictim(), true);
					pAdd->GetMotionMaster()->MoveChase(m_creature->getVictim());
				}
			}
		}


        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_sulfuron(Creature* pCreature)
{
    return new boss_sulfuronAI(pCreature);
}

struct MANGOS_DLL_DECL mob_flamewaker_priestAI : public ScriptedAI
{
    mob_flamewaker_priestAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = dynamic_cast<instance_molten_core*>(m_creature->GetInstanceData());
      
        Reset();
    }

    uint32 m_uiDarkStrikeTimer;
    uint32 m_uiHealTimer;
    uint32 m_uiShadowWordPainTimer;
    uint32 m_uiImmolateTimer;
    
    instance_molten_core* m_pInstance;

    void Reset()
    {
        m_uiDarkStrikeTimer = urand(5000,8000);
        m_uiHealTimer = 15000;
        m_uiShadowWordPainTimer = 2000;
        m_uiImmolateTimer = 8000;

        m_creature->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK, true);
	m_creature->ApplySpellImmune(0, IMMUNITY_DISPEL, DISPEL_POISON, true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
	// If Sulfuron's adds get evaded after combat we put them back into combat.
	if (m_pInstance && m_pInstance->GetData(TYPE_SULFURON) == DONE && m_creature->isAlive() && !m_creature->isInCombat())
	    m_creature->SetInCombatWithZone();
      
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Dark Strike
        if (m_uiDarkStrikeTimer <= uiDiff)
        {
			m_uiDarkStrikeTimer = 0;

			if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_DARKSTRIKE) == CAST_OK)
				m_uiDarkStrikeTimer = urand(5000,8000);
        }
        else
            m_uiDarkStrikeTimer -= uiDiff;

        // Casting Heal to Sulfuron or other Guards.
        if (m_uiHealTimer <= uiDiff)
        {
			m_uiHealTimer = 0;

            if (Unit* pUnit = DoSelectLowestHpFriendly(60.0f, 1))
			{
                if (DoCastSpellIfCan(pUnit, SPELL_HEAL) == CAST_OK)
					m_uiHealTimer = urand(7000, 10000);
			}
        }
        else
            m_uiHealTimer -= uiDiff;

        // Shadow Word: Pain
        if (m_uiShadowWordPainTimer <= uiDiff)
        {
			m_uiShadowWordPainTimer = 0;

            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
			{
				if (!pTarget->HasAura(SPELL_SHADOW_WORD_PAIN) && DoCastSpellIfCan(pTarget, SPELL_SHADOW_WORD_PAIN) == CAST_OK)
					m_uiShadowWordPainTimer = 5000;
			}
        }
        else
            m_uiShadowWordPainTimer -= uiDiff;

        // Immolate
        if (m_uiImmolateTimer <= uiDiff)
        {
			m_uiImmolateTimer = 0;

            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
			{
				if (!pTarget->HasAura(SPELL_IMMOLATE) && DoCastSpellIfCan(pTarget, SPELL_IMMOLATE) == CAST_OK)
					m_uiImmolateTimer = 10000;
			}
        }
        else
            m_uiImmolateTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_flamewaker_priest(Creature* pCreature)
{
    return new mob_flamewaker_priestAI(pCreature);
}

void AddSC_boss_sulfuron()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_sulfuron";
    pNewscript->GetAI = &GetAI_boss_sulfuron;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_flamewaker_priest";
    pNewscript->GetAI = &GetAI_mob_flamewaker_priest;
    pNewscript->RegisterSelf();
}
