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
SDName: Boss_Baroness_Anastari
SD%Complete: 90
SDComment: If the player who has aggro on the MC'd player dies the MC'd player just stand in place and you have to wait 2min for the force remove buff.
SDCategory: Stratholme
EndScriptData */

#include "precompiled.h"
#include "stratholme.h"

enum Spells
{
    SPELL_BANSHEE_CURSE       = 16867,
    SPELL_BANSHEE_WAIL        = 16565,
    SPELL_SILENCE             = 18327,
	SPELL_POSSESS             = 17244,			 // the actual MC
	SPELL_POSSESSED           = 17246,			// damage buff
    SPELL_POSSESS_INV         = 17250,         // baroness becomes invisible while possessing a target
};

struct MANGOS_DLL_DECL boss_baroness_anastariAI : public ScriptedAI
{
    boss_baroness_anastariAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_stratholme*)pCreature->GetInstanceData();
        Reset();
    }

    instance_stratholme* m_pInstance;

    uint32 m_uiBansheeCurseTimer;
    uint32 m_uiBansheeWailTimer;
    uint32 m_uiSilenceTimer;
	uint32 m_uiPossessTimer;
	uint32 m_uiPossessEndTimer;
	uint32 m_uiPossessRemoveTimer;

    ObjectGuid m_possessedPlayer;

    void Reset()
    {
		m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		EndTimersRemoveInvis();
		RemovePossesAgain();
        m_uiBansheeCurseTimer = urand(5000,7000);
        m_uiBansheeWailTimer = urand(0,1000);
		m_uiSilenceTimer = urand(9000,11000);
        m_uiPossessTimer = urand(30000,35000);        
		m_possessedPlayer.Clear();
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BARONESS_ANASTARI, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BARONESS_ANASTARI, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BARONESS_ANASTARI, DONE);
		EndTimersRemoveInvis();
		RemovePossesAgain();
    }

	void RemovePossess(Player* pPlayer)
	{
		pPlayer->RemoveAurasDueToSpell(SPELL_POSSESS);
		pPlayer->RemoveAurasDueToSpell(SPELL_POSSESSED);
		pPlayer->SetHealthPercent(100);
		m_possessedPlayer.Clear();
	}

	void RemovePossesAgain()
	{
		if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_possessedPlayer))
		{
			pPlayer->RemoveAurasDueToSpell(SPELL_POSSESS);
			pPlayer->RemoveAurasDueToSpell(SPELL_POSSESSED);
			pPlayer->SetHealthPercent(100);
			m_possessedPlayer.Clear();
		}
	}

	void EndTimersRemoveInvis()
	{
		m_uiPossessEndTimer = 0;
		m_uiPossessRemoveTimer = 0;
		if (m_creature->HasAura(SPELL_POSSESS_INV))
			m_creature->RemoveAurasDueToSpell(SPELL_POSSESS_INV);
		if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
			m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
	}

	// cast buff and give full hp to the possessed player
	void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if (pTarget->GetTypeId() == TYPEID_PLAYER && pSpell->Id == SPELL_POSSESS)
        {
			DoCastSpellIfCan(pTarget, SPELL_POSSESSED, CAST_TRIGGERED);
			DoCastSpellIfCan(m_creature, SPELL_POSSESS_INV, CAST_TRIGGERED);		// turn invisible
			pTarget->SetHealthPercent(100);
			m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);		// make sure no pet can dps while invisible
		}
	}

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (m_uiPossessEndTimer)
        {
			// Check if the possessed player has been damaged
            if (m_uiPossessEndTimer <= uiDiff)
            {
				// Check for possessed player
				Player* pPlayer = m_creature->GetMap()->GetPlayer(m_possessedPlayer);
				if (!pPlayer || !pPlayer->isAlive())
					EndTimersRemoveInvis();

                // If aura has expired, return to fight
                if (!m_creature->HasAura(SPELL_POSSESS_INV))
				{
					RemovePossess(pPlayer);
					EndTimersRemoveInvis();
				}

				// If aura has expired, return to fight
                if (!pPlayer->HasAura(SPELL_POSSESSED))
				{
					RemovePossess(pPlayer);
					EndTimersRemoveInvis();
				}

                // If possessed player has less than 50% health
                if (pPlayer->GetHealth() <= pPlayer->GetMaxHealth() * .5f)
                {
					RemovePossess(pPlayer);
					EndTimersRemoveInvis();

                }
                m_uiPossessEndTimer = 1000;
            }
            else
            {
                m_uiPossessEndTimer -= uiDiff;
            }
        }

        // Banshee Curse spell
        if (m_uiBansheeCurseTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BANSHEE_CURSE);
            m_uiBansheeCurseTimer = urand(10000,15000);
        }
        else
            m_uiBansheeCurseTimer -= uiDiff;

        // Banshee Wail spell
        if (m_uiBansheeWailTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BANSHEE_WAIL);
            m_uiBansheeWailTimer = urand(3000,5000);
        }
        else
            m_uiBansheeWailTimer -= uiDiff;

		// Force remove possess after 2min
		if (m_uiPossessRemoveTimer <= uiDiff)
			EndTimersRemoveInvis();
		else
			m_uiPossessRemoveTimer -= uiDiff;

        // Possess spell
        if (m_uiPossessTimer <= uiDiff)
        {
			//if (Player* pPlayer = GetRandomPlayerInCurrentMap(40.0f))								// think this one bugged if tank got MC'd
			if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,1))
				if (Player* pPlayer = pTarget->GetCharmerOrOwnerPlayerOrPlayerItself())				// make sure we're not getting a pet
				{  
					DoCastSpellIfCan(pPlayer, SPELL_POSSESS);

					m_possessedPlayer = pPlayer->GetObjectGuid();
					m_uiPossessEndTimer = 1000;
					m_uiPossessRemoveTimer = 120000;
				}
			m_uiPossessTimer = urand(45000,60000);
        }
        else
            m_uiPossessTimer -= uiDiff;
		 
        // Silence spell
        if (m_uiSilenceTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SILENCE);
            m_uiSilenceTimer = urand(10000,14000);
        }
        else
            m_uiSilenceTimer -= uiDiff;
		
        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_baroness_anastari(Creature* pCreature)
{
    return new boss_baroness_anastariAI(pCreature);
}

void AddSC_boss_baroness_anastari()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_baroness_anastari";
    pNewscript->GetAI = &GetAI_boss_baroness_anastari;
    pNewscript->RegisterSelf();
}
