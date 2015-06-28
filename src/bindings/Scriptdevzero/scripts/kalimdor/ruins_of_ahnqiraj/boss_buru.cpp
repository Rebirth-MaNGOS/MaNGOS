/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* ScriptData
SDName: Boss_Buru
SD%Complete: 80
SDComment:
SDCategory: Ruins of Ahn'Qiraj
EndScriptData */

#include "precompiled.h"
#include "ruins_of_ahnqiraj.h"
#include "ObjectMgr.h"

enum Spells
{
    EMOTE_TARGET            = -1509002,

	NPC_BURU_TRANSFORM		= 15507,
	
    // Buru
    SPELL_CREEPING_PLAGUE   = 20512,
    SPELL_DISMEMBER         = 96,
    SPELL_FULL_SPEED        = 1557,
    SPELL_THORNS            = 25640,
    SPELL_GATHERING_SPEED   = 1834,
    SPELL_BURU_TRANSFORM    = 24721,
	SPELL_STUN				= 25900,

    // Buru Egg
    SPELL_SUMMON_HATCHLING  = 1881,
    SPELL_EXPLODE           = 19593,
};

struct MANGOS_DLL_DECL boss_buruAI : public ScriptedAI			// Should hit for 750~~ on cloth
{
    boss_buruAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ruins_of_ahnqiraj*)pCreature->GetInstanceData();
		m_creature->RemoveAurasAtMechanicImmunity(MECHANIC_STUN, 25900);
		m_creature->CastSpell(m_creature, SPELL_THORNS, true);
        Reset();
    }

    instance_ruins_of_ahnqiraj* m_pInstance;

    bool m_bIsEnraged;
	bool m_bStun;
	bool m_bTransform;
	bool m_bStop;

	uint32 m_uiTransformTimer;
	uint32 m_uiRemoveStunTimer;
    uint32 m_uiDismemberTimer;
    uint32 m_uiSpeedTimer;
    uint32 m_uiCreepingPlagueTimer;

	GUIDList m_lPlayers;

    void Reset() 
    {
        m_bIsEnraged = false;
		m_bTransform = false;
		m_bStop = false;
        m_creature->RemoveAllAuras();        
        m_creature->SetSpeedRate(MOVE_RUN, 0.45f);
        m_uiDismemberTimer = 5000;
        m_uiSpeedTimer = 5000;
        m_uiCreepingPlagueTimer = 6000;
		m_lPlayers.clear();

		if (m_creature->HasAura(SPELL_BURU_TRANSFORM))
			m_creature->RemoveAurasDueToSpell(SPELL_BURU_TRANSFORM);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BURU, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BURU, IN_PROGRESS);
		
        m_creature->SetArmor(20000);
    }
	
	void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_CREEPING_PLAGUE && pTarget->GetTypeId() == TYPEID_PLAYER)
			m_lPlayers.push_back(pTarget->GetObjectGuid());
	}

    void JustDied(Unit* /*pKiller*/)							// Remove Creeping Plague from all players when boss dies
    {
        if (m_pInstance)
		{
            m_pInstance->SetData(TYPE_BURU, DONE);

			for (GUIDList::iterator itr = m_lPlayers.begin(); itr != m_lPlayers.end(); itr++)			// EDIT THIS!
			{
				if (Unit* pPlayer = m_creature->GetMap()->GetUnit(*itr))
					if (pPlayer->isAlive() && pPlayer->HasAura(SPELL_CREEPING_PLAGUE))
							pPlayer->RemoveAurasDueToSpell(SPELL_CREEPING_PLAGUE);
			}
		}								
    }

	void HatchEggs()
	{
		instance_ruins_of_ahnqiraj* pInstance = dynamic_cast<instance_ruins_of_ahnqiraj*>(m_creature->GetInstanceData());
		{
			if (pInstance)
			{
				for (ObjectGuid guid : pInstance->GetBuruEggs())
				{
					Creature* pCreature = m_creature->GetMap()->GetCreature(guid);
					if (pCreature)
					{
						pCreature->DealDamage(pCreature, pCreature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
					}
				}			
			}
		}
	}

	void ChangeTarget()
	{
		if (Unit* pTarget = GetRandomPlayerInCurrentMap(100))
		{
			DoResetThreat();
			m_creature->AddThreat(pTarget,1000000.0f);
			m_creature->SelectHostileTarget();
			
			std::stringstream ss;
			ss << "Buru the Gorger sets eyes on " << pTarget->GetName() << "!";
			m_creature->GenericTextEmote(ss.str().c_str(), NULL, false);			
		}
	}

	void StopDueToStun(uint32 Duration = 0)
	{
		m_bStun = true;
		m_creature->CastSpell(m_creature, SPELL_STUN, true);

		if (Duration == 0)
			m_uiRemoveStunTimer = 3000;
		else 
		{
			m_bTransform = true;
			m_uiRemoveStunTimer = 8000;
			m_uiTransformTimer = 1000;			
		}
	}

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_EXPLODE)
        {
            m_creature->SetSpeedRate(MOVE_RUN, 0.5f);
            m_creature->SetHealth(m_creature->GetHealth() - 45000);
			if (!HealthBelowPct(20))
				StopDueToStun(0);
        }
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32& /*uiDamage*/)			// Hatch all eggs at start of p2
    {
        if (!m_bIsEnraged && HealthBelowPct(21))
        {
			m_creature->SetSpeedRate(MOVE_RUN, 0.30f);
			m_creature->SetArmor(4915);
			StopDueToStun(1);			
			DoResetThreat();
			m_bIsEnraged = true;
			HatchEggs();
        }
    }
	
    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (m_bStun)
		{
			if (m_uiRemoveStunTimer <= uiDiff)			
			{
				if (!m_bTransform)
					ChangeTarget();
				m_creature->RemoveAurasDueToSpell(25900);				
				m_bStun = false;
				m_bTransform = false;
				m_creature->UpdateVisibilityAndView();
			}
			else
				m_uiRemoveStunTimer -= uiDiff; 
		}

		if (m_bIsEnraged)
		{
            if (m_uiCreepingPlagueTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_CREEPING_PLAGUE);
                m_uiCreepingPlagueTimer = 6000;
            }
            else
                m_uiCreepingPlagueTimer -= uiDiff;
		}
		else 
		{
			if (m_uiDismemberTimer <= uiDiff)
			{
				DoCastSpellIfCan(m_creature->getVictim(), SPELL_DISMEMBER);
				m_uiDismemberTimer = 6000;
			}
			else
				m_uiDismemberTimer -= uiDiff;       

			if (m_uiSpeedTimer <= uiDiff)
			{
				m_creature->SetSpeedRate(MOVE_RUN, m_creature->GetSpeedRate(MOVE_RUN) + 0.07f);
				m_uiSpeedTimer = 5000;
			}
			else
				m_uiSpeedTimer -= uiDiff;
		}

		if (m_bTransform)
		{

			if (m_uiTransformTimer <= uiDiff)
			{
				m_creature->CastSpell(m_creature, SPELL_BURU_TRANSFORM, false);
				m_creature->CastSpell(m_creature, SPELL_FULL_SPEED, true);
			}
			else
				m_uiTransformTimer -= uiDiff;
		}
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_buru(Creature* pCreature)
{
    return new boss_buruAI(pCreature);
}

struct MANGOS_DLL_DECL mob_buru_eggAI : public ScriptedAI
{
    mob_buru_eggAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ruins_of_ahnqiraj*)pCreature->GetInstanceData();
        Reset();
    }
    instance_ruins_of_ahnqiraj* m_pInstance;

    void Reset()
    {
        SetCombatMovement(false);
    }
	
	void MoveInLineOfSight(Unit* /*pWho*/)
    {
        // Must to be empty to ignore aggro
    }

    void DamageTaken(Unit* pDealer, uint32& uiDamage)
    {
        if (!m_pInstance)
            return;

		Creature* pBuru = m_pInstance->GetSingleCreatureFromStorage(NPC_BURU);
        if (pBuru && pBuru->isAlive() && !pBuru->isInCombat())
		{
            pBuru->AI()->AttackStart(GetRandomPlayerInCurrentMap(100));

			if (boss_buruAI* pBuruAI = dynamic_cast<boss_buruAI*>(pBuru->AI()))
				pBuruAI->ChangeTarget();
		}
    }

	void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
		m_creature->SetTargetGuid(ObjectGuid());
	}

	void JustSummoned(Creature* pSummoned)
    {
		pSummoned->SetRespawnDelay(-10);			// make sure hatchers won't randomly respawn
	}

	void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
            return;

		if (Creature* pHatchling = m_creature->SummonCreature(NPC_HIVEZARA_HATCHLING, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000))
			pHatchling->AI()->AttackStart(GetRandomPlayerInCurrentMap(80));

        Creature* pBuru = m_pInstance->GetSingleCreatureFromStorage(NPC_BURU);
        if (pBuru && pBuru->isAlive() && pBuru->IsWithinDistInMap(m_creature, 10.0f) && pBuru->GetHealthPercent() > 20)// If Buru is within 10yrds of the egg he should take damage
			pBuru->CastSpell(pBuru, SPELL_EXPLODE, true);
    }
};

CreatureAI* GetAI_mob_buru_egg(Creature* pCreature)
{
    return new mob_buru_eggAI(pCreature);
}

void AddSC_boss_buru()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_buru";
    pNewscript->GetAI = &GetAI_boss_buru;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_buru_egg";
    pNewscript->GetAI = &GetAI_mob_buru_egg;
    pNewscript->RegisterSelf();
}
