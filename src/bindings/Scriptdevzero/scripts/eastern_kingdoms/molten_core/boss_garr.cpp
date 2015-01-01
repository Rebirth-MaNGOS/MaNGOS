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
SDName: Boss_Garr
SD%Complete: 90
SDComment:
SDCategory: Molten Core
EndScriptData */

#include "precompiled.h"
#include "molten_core.h"

enum eGarr
{
    // Garr spells
    SPELL_ANTIMAGIC_PULSE       = 19492,
    SPELL_MAGMA_SHACKLES        = 19496,
    SPELL_FRENZY                = 19516,                  // Stacking frenzy (stacks to 10 times)
    SPELL_SUMMON_PLAYER         = 20477,

    // Add spells
    SPELL_IMMOLATE              = 20294,
    SPELL_ERUPTION              = 19497,
    //SPELL_MASSIVE_ERUPTION      = 20483,
    SPELL_SEPARATION_ANXIETY    = 23492
};

struct MANGOS_DLL_DECL boss_garrAI : public ScriptedAI
{
    boss_garrAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_molten_core*)pCreature->GetInstanceData();
        Reset();
    }

    instance_molten_core* m_pInstance;

    uint32 m_uiAntiMagicPulseTimer;
    uint32 m_uiMagmaShacklesTimer;
	uint32 m_uiAddExplodeTimer;
	GUIDList GarrAdds;
	Creature* NextExplode;

    void Reset()
    {
        m_uiAntiMagicPulseTimer = 25000;                       // These times are probably wrong
        m_uiMagmaShacklesTimer = 15000;
		m_uiAddExplodeTimer = urand(5000, 10000);
		NextExplode = NULL;
    }

    void Aggro(Unit* /*pAttacker*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GARR, IN_PROGRESS);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GARR, FAIL);

        m_creature->RemoveAurasDueToSpell(SPELL_FRENZY);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GARR, DONE);
    }

	void AddFiresworn(ObjectGuid AddGUID)
	{
		GarrAdds.push_back(AddGUID);
	}

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Antimagic Pulse
        if (m_uiAntiMagicPulseTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_ANTIMAGIC_PULSE);
            m_uiAntiMagicPulseTimer = urand(10000, 15000);
        }
        else
            m_uiAntiMagicPulseTimer -= uiDiff;

		//Randomly explode adds
		if (m_creature->GetHealthPercent() < 20.0f)
		{
			if (NextExplode && NextExplode->IsInWorld() && NextExplode->isAlive())
			{
				m_creature->DealDamage(NextExplode,NextExplode->GetHealth()+1000,NULL,DIRECT_DAMAGE,SPELL_SCHOOL_MASK_NATURE,NULL,false);
				NextExplode = 0;
			}
			
			if (m_uiAddExplodeTimer <= uiDiff)
			{
				m_uiAddExplodeTimer = urand(18000,25000);
				for (GUIDList::const_iterator itr = GarrAdds.begin(); itr != GarrAdds.end(); ++itr)
				{
					Creature* U = m_creature->GetMap()->GetCreature(*itr);
					if (U && U->IsInWorld() && U->isAlive())
					{
						NextExplode = U;
						U->CastSpell(U, SPELL_ERUPTION, false);
						break;
					}
				}
			}
			else
				m_uiAddExplodeTimer -= uiDiff;
		}

        // Magma Shackles
        if (m_uiMagmaShacklesTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_MAGMA_SHACKLES);
            m_uiMagmaShacklesTimer = urand(8000, 12000);
        }
        else
            m_uiMagmaShacklesTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_garr(Creature* pCreature)
{
    return new boss_garrAI(pCreature);
}

struct MANGOS_DLL_DECL mob_fireswornAI : public ScriptedAI
{
    mob_fireswornAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_molten_core*)pCreature->GetInstanceData();
        Reset();
    }

    instance_molten_core* m_pInstance;

    bool m_bExplosion;

    uint32 m_uiEruptionTimer;
    uint32 m_uiImmolateTimer;
    uint32 m_uiSeparationAnxietyTimer;

    void Reset()
    {
        m_bExplosion = false;

        m_uiEruptionTimer = 30000;
        m_uiImmolateTimer = 5000;                              // These times are probably wrong
        m_uiSeparationAnxietyTimer = 2000;

		FindGarr();
    }

	void Aggro(Unit* /*pAttacker*/)
    {
		FindGarr();
		m_creature->SetInCombatWithZone();
    }

	void FindGarr()
	{
        if (m_pInstance)
            if (Creature* pGarr = m_pInstance->GetSingleCreatureFromStorage(NPC_GARR))
                if (boss_garrAI* GarrAI = dynamic_cast<boss_garrAI*>(pGarr->AI()))
                    GarrAI->AddFiresworn(m_creature->GetObjectGuid());
	}

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
        {
            // Garr's Frenzy
            Creature* pGarr = m_pInstance->GetSingleCreatureFromStorage(NPC_GARR);
            if (pGarr && pGarr->isAlive())
            {
                pGarr->InterruptNonMeleeSpells(false);
                pGarr->CastSpell(pGarr, SPELL_FRENZY, false);
            }
        }
    }

	//Doesnt work properly for some reason, uiDamage is always 0
   // void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
   // {
   //     // Prevent more than one eplosion (maybe)
   //     if (!m_bExplosion)
   //     {
			//if (m_creature->GetHealth() <= uiDamage)
   //         {
   //             m_bExplosion = true;
   //             m_creature->RemoveAurasDueToSpell(SPELL_SEPARATION_ANXIETY);
   //             m_creature->CastSpell(m_creature, SPELL_ERUPTION, false);
   //         }
   //     }
   // }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		// Selfkill after explosion
        if (m_bExplosion)
            m_creature->DealDamage(m_creature,m_creature->GetHealth()+1000,NULL,DIRECT_DAMAGE,SPELL_SCHOOL_MASK_NATURE,NULL,false);

		// Explode
        if (!m_bExplosion)
        {
            if (m_creature->GetHealthPercent() < 7.0f)  //&& !m_creature->hasUnitState(UNIT_STAT_ISOLATED)
            {
                m_bExplosion = true;
                m_creature->RemoveAurasDueToSpell(SPELL_SEPARATION_ANXIETY);
                m_creature->CastSpell(m_creature, SPELL_ERUPTION, false);
            }
        }

        // Eruption
        //if (m_uiEruptionTimer <= uiDiff)
        //{
        //    DoCastSpellIfCan(m_creature, SPELL_ERUPTION);
        //    m_uiEruptionTimer = urand(25000, 35000);
        //}
        //else
        //    m_uiEruptionTimer -= uiDiff;

        // Immolate
        if (m_uiImmolateTimer <= uiDiff)
        {
			Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            if (pTarget && pTarget->GetDistance(m_creature) < 35.0f)
			{
                DoCastSpellIfCan(pTarget, SPELL_IMMOLATE);
				m_uiImmolateTimer = urand(8000, 15000);
			}
        }
        else
            m_uiImmolateTimer -= uiDiff;

        // Separation Anxiety
        if (m_uiSeparationAnxietyTimer <= uiDiff)
        {
            if (m_pInstance)
            {
				Creature* pGarr = m_pInstance->GetSingleCreatureFromStorage(NPC_GARR);
                if (pGarr && pGarr->isAlive() && pGarr->GetDistance(m_creature) > 33.0f)
                    DoCastSpellIfCan(m_creature, SPELL_SEPARATION_ANXIETY);
            }

            m_uiSeparationAnxietyTimer = 5000;
        }
        else
            m_uiSeparationAnxietyTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_firesworn(Creature* pCreature)
{
    return new mob_fireswornAI(pCreature);
}

void AddSC_boss_garr()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_garr";
    pNewscript->GetAI = &GetAI_boss_garr;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_firesworn";
    pNewscript->GetAI = &GetAI_mob_firesworn;
    pNewscript->RegisterSelf();
}
