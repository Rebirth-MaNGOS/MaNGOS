/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: Boss_Viscidus
SD%Complete: 50
SDComment: place holder
SDCategory: Temple of Ahn'Qiraj
EndScriptData */

#include "precompiled.h"

enum
{
    // Timer spells
    SPELL_POISON_SHOCK          = 25993,
    SPELL_POISONBOLT_VOLLEY     = 25991,					// doesn't ignore los like it should?
    SPELL_TOXIN                 = 26575,                    // Triggers toxin cloud every 10~~ sec
    SPELL_TOXIN_CLOUD           = 25989,					

	// Root
	SPELL_ROOT					= 20548,

    // Debuffs gained by the boss on frost damage
    SPELL_VISCIDUS_SLOWED       = 26034,
    SPELL_VISCIDUS_SLOWED_MORE  = 26036,
    SPELL_VISCIDUS_FREEZE       = 25937,

    // When frost damage exceeds a certain limit, then boss explodes
    SPELL_REJOIN_VISCIDUS       = 25896,
    SPELL_VISCIDUS_EXPLODE      = 25938,                    // Casts a lot of spells in the same time: 25865 to 25884; All spells have target coords
    SPELL_VISCIDUS_SUICIDE      = 26003,					// same as above?

	SPELL_MEMBRANE_VISCIDUS     = 25994,                    // damage reduction spell

    NPC_GLOB_OF_VISCIDUS        = 15667
};

struct MANGOS_DLL_DECL boss_viscidusAI : public ScriptedAI
{
    boss_viscidusAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{
		m_creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NATURE, true);
		Reset();
	}

	bool m_bSlowed1;
	bool m_bSlowed2;
	bool m_bFrozen;

	bool m_bCracking1;
	bool m_bCracking2;
	bool m_bExploded;
    bool m_bSummoned;

	bool m_bCanDoDamage;

	uint32 m_uiThawTimer;			// remove shatter

	uint8 m_uiFrostSpellCounter;
	uint8 m_uiMeleeCounter;
	uint8 m_uiGlobCount;

	uint32 m_uiSetInvisTimer;
	uint32 m_uiSetVisibleTimer;
	uint32 m_uiGlobSpawnTimer;
	uint32 m_uiPoisonShockTimer;
	uint32 m_uiPoisonVolleyTimer;
	uint32 m_uiToxicCloudTimer;

    float globs;
    uint32 globCounter;

    uint32 m_health;

    void Reset()
    {
		m_uiGlobCount = 0;
		m_uiGlobSpawnTimer = 0;
		m_uiSetInvisTimer = 4000;
		m_uiSetVisibleTimer = 0;
		m_uiPoisonVolleyTimer = 10000;									// timer confirmed
		m_uiPoisonShockTimer = 11000;
		m_uiToxicCloudTimer = urand(30000,40000);
		m_uiThawTimer = 20000;
        m_bSummoned = false;
        globCounter = 0;

		RemoveAuras();
		ResetBool(0);
		ResetBool(1);
		SetVisible(1);
    }

	void RemoveAuras()
	{
		if (m_creature->HasAura(SPELL_VISCIDUS_SLOWED))
			m_creature->RemoveAurasDueToSpell(SPELL_VISCIDUS_SLOWED);
		else if (m_creature->HasAura(SPELL_VISCIDUS_SLOWED_MORE))
			m_creature->RemoveAurasDueToSpell(SPELL_VISCIDUS_SLOWED_MORE);
		else if (m_creature->HasAura(SPELL_VISCIDUS_FREEZE))
			m_creature->RemoveAurasDueToSpell(SPELL_VISCIDUS_FREEZE);
	}

	void ResetBool(int Action = 0)
	{
		if(Action == 0)		// reset frost phase
		{
			m_bCanDoDamage = true;
			m_bSlowed1 = false;
			m_bSlowed2 = false;
			m_bFrozen = false;
			m_uiFrostSpellCounter = 0;
		}
		else if(Action == 1)	// reset melee phase
		{
			m_bCanDoDamage = true;
			m_bCracking1 = false;
			m_bCracking2 = false;
			m_bExploded = false;
			m_uiMeleeCounter = 0;
		}
	}
	
	void Aggro(Unit* /*pWho*/)
    {
    }

	void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell)			// Count every frost spell
    {
        if (pSpell->School == SPELL_SCHOOL_FROST && !m_bFrozen && !m_bExploded)
		{
			++m_uiFrostSpellCounter;
			SpellCount();			// count incoming spells
		}
        else if(pSpell->School == SPELL_SCHOOL_NORMAL && m_bFrozen)
		{
			++m_uiMeleeCounter;
			MeleeHitCount();		// count incoming melee
		}
	}

	void SpellCount()
	{		
		if(m_uiFrostSpellCounter >= 1 && !m_bSlowed1)		// 100, 150, 200
		{
			m_creature->CastSpell(m_creature, SPELL_VISCIDUS_SLOWED_MORE, true);
			m_creature->GenericTextEmote("Viscidus begins to slow.", NULL, false);
			m_creature->SetAttackTime(BASE_ATTACK, 2012);			// fix the attack speed, if attack speed is changed these need to be changed aswell
			m_bSlowed1 = true;
		}

		else if(m_uiFrostSpellCounter >= 2 && !m_bSlowed2)
		{
			m_creature->CastSpell(m_creature, SPELL_VISCIDUS_SLOWED_MORE, true);
			m_creature->GenericTextEmote("Viscidus is freezing up.", NULL, false);
			m_creature->SetAttackTime(BASE_ATTACK, 2275);			// fix the attack speed
			m_bSlowed2 = true;
		}

		else if(m_uiFrostSpellCounter >= 3 && !m_bFrozen)					// does this emote multiple times?
		{
			m_bCanDoDamage = false;
			m_bFrozen = true;
			m_creature->CastSpell(m_creature, SPELL_VISCIDUS_FREEZE, true);
			m_creature->GenericTextEmote("Viscidus is frozen solid.", NULL, false);					
			m_creature->SetAttackTime(BASE_ATTACK, 1750);			// set the attack speed back to normal
			m_uiThawTimer = 15000;			
		}
	}

	void MeleeHitCount()
	{
		if(m_uiMeleeCounter >= 1 && !m_bCracking1)		// 100, 150, 200
		{
			m_creature->GenericTextEmote("Viscidus begins to crack.", NULL, false);
			m_bCracking1 = true;
		}

		else if(m_uiMeleeCounter >= 2 && !m_bCracking2)
		{
			m_creature->GenericTextEmote("Viscidus looks ready to shatter.", NULL, false);
			m_bCracking2 = true;
			m_bSummoned = false;
		}

		else if(m_uiMeleeCounter >= 3 && !m_bExploded)
		{
			if (!m_bExploded)
			{
				if (HealthBelowPct(5))			// if Viscidus has less than 5% hp he should die since every glob is 5% hp
					m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);

				m_bCanDoDamage = false;
				m_bExploded = true;
				//RemoveAuras();					// remove auras if we're gonna explode, not needed?
				m_creature->RemoveAllAuras(AuraRemoveMode::AURA_REMOVE_BY_DEFAULT);
				m_creature->CastSpell(m_creature, SPELL_VISCIDUS_EXPLODE,true);
				m_creature->GenericTextEmote("Viscidus explodes.", NULL, false);			// missing death animation
				m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);		// not really working	

                // Clear the target to prevent him from rotating.
                m_creature->SetTargetGuid(ObjectGuid());

                // Set his health to zero to trigger the death animation.
                m_health = m_creature->GetHealth();
                m_creature->SetHealth(0);

				m_uiSetInvisTimer = 1700;
				m_uiGlobSpawnTimer = 3000;			// slight delay before we spawn the adds
				m_uiSetVisibleTimer = 18000;			// adjust this when adds are spawning/moving correctly
			}
		}
	}

	void SetVisible(int Visible = 0)
	{
		if (Visible == 0)
		{
			m_creature->SetVisibility(VISIBILITY_OFF);	
            m_creature->SetHealth(m_health);
		}
		else if (Visible == 1)
		{
			ResetBool(1);			// reset all counters for melee here
			ResetBool(0);			// reset all counters for spells here
            m_creature->SetVisibility(VISIBILITY_ON);
			m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);			
            m_creature->RelocateCreature(-7991.48f, 920.19f, -52.91f, 0.f);

            if (m_creature->getVictim())
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
		}		
	}

	void SpawnGlobs()
	{
		//spawn all adds here
        globs = floor(((float) m_creature->GetHealth() / (float) m_creature->GetMaxHealth()) / 0.05f);
        for (float angle = 0; angle < 2 * 3.141592654; angle += 2 * 3.141592654 / globs)
        {
            Unit* pSummon = m_creature->SummonCreature(NPC_GLOB_OF_VISCIDUS, -7990.f + 50.f * cosf(angle),
                                       925.f + 50.f * sinf(angle),-42.f, 0.f,
                                       TEMPSUMMON_DEAD_DESPAWN, 0, true);
        }
	}

	void JustSummoned(Creature* pSummoned)
    {
		pSummoned->GetMotionMaster()->MovePoint(1,-7991.48f,920.19f,-52.91f);		// a point in the middle of the room
		pSummoned->SetRespawnDelay(-10);			// make sure they won't respawn
    }

	void SummonedMovementInform(Creature* pSummoned, uint32 uiMotionType, uint32 uiPointId)				// something like this?
    {
		if(uiPointId == 1)
        {
            pSummoned->CastSpell(m_creature, SPELL_REJOIN_VISCIDUS, true);
            pSummoned->ForcedDespawn(globCounter >= globs ? 50 : 2000);
            ++globCounter;

    //        if (globCounter >= globs)
    //        {
				//m_creature->SetObjectScale(Scale-(3.f/globCounter));
    //            SetVisible(1);
    //            m_creature->RemoveAllAuras(AuraRemoveMode::AURA_REMOVE_BY_DEFAULT);
    //        }
        }
	}
	
    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_GLOB_OF_VISCIDUS)
            m_creature->DealDamage(m_creature, m_creature->GetMaxHealth() * 0.05f, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
    }

    void UpdateAI(const uint32 uiDiff)
    {
		
		if (m_bFrozen && !m_bExploded)
		{
			if (m_uiThawTimer <= uiDiff)			// remove all slows and reset counters
			{
				RemoveAuras();
				ResetBool(0);	
			}
			else
				m_uiThawTimer -= uiDiff;
		}

		if(m_bExploded)
		{
			if (m_uiSetInvisTimer <= uiDiff)
            {
				SetVisible(0);
                m_creature->SetObjectScale(0.1f);
                m_creature->UpdateModelData();
            }
			else
				m_uiSetInvisTimer -= uiDiff;

			if (m_uiSetVisibleTimer <= uiDiff)
			{
                m_creature->SetObjectScale(2*(m_creature->GetHealthPercent()/100));			// set Viscidus' size depending on the blobs that are alive 1/ too small?   
                m_creature->UpdateModelData();
                m_creature->RemoveAllAuras(AuraRemoveMode::AURA_REMOVE_BY_DEFAULT);
				SetVisible(1);
			}
			else
				m_uiSetVisibleTimer -= uiDiff;

            if (!m_bSummoned)
            {
                if (m_uiGlobSpawnTimer <= uiDiff)
                {
                    SpawnGlobs();
                    m_bSummoned = true;
                }
                else
                    m_uiGlobSpawnTimer -= uiDiff;
            }
		}

		if (m_bCanDoDamage)
		{
            //Return since we have no target
            if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                return;

			if (m_uiPoisonShockTimer <= uiDiff)
			{
				m_creature->CastSpell(m_creature, SPELL_POISON_SHOCK, false);
				m_uiPoisonShockTimer = 10000;
			}
			else
				m_uiPoisonShockTimer -= uiDiff;

			if (m_uiPoisonVolleyTimer <= uiDiff)
			{
				m_creature->CastSpell(m_creature, SPELL_POISONBOLT_VOLLEY, false);
				m_uiPoisonVolleyTimer = 10000;
			}
			else
				m_uiPoisonVolleyTimer -= uiDiff;

			if (m_uiToxicCloudTimer <= uiDiff)				// redo this, spawn the npcs instead, or don't summon the cloud under the boss
			{
				m_creature->CastSpell(m_creature, SPELL_TOXIN_CLOUD, false);   
				m_uiToxicCloudTimer = urand(10000,15000);
			}
			else
				m_uiToxicCloudTimer -= uiDiff;

			DoMeleeAttackIfReady();
		}
    }
};

CreatureAI* GetAI_boss_viscidus(Creature* pCreature)
{
    return new boss_viscidusAI(pCreature);
}

struct MANGOS_DLL_DECL boss_glob_of_viscidusAI : public ScriptedAI
{
    boss_glob_of_viscidusAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{
		m_creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NATURE, true);
		Reset();
	}
	uint32 m_uiSpeedUpTimer;

	void Reset()
	{
		m_uiSpeedUpTimer = 500;
	}

	void MoveInLineOfSight(Unit* /*pWho*/)
    {
        // Must to be empty to ignore aggro
    }

    void Aggro(Unit* /*pVictim*/)
    {
        return;
    }

    void AttackStart(Unit* /*pVictim*/)
    {
        return;
    }

	void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (m_uiSpeedUpTimer <= uiDiff)				// increase the speed every half a sec
		{
			m_creature->SetSpeedRate(MOVE_RUN, m_creature->GetSpeedRate(MOVE_RUN)+0.05f);
			m_uiSpeedUpTimer = 500;
		}
		else
			m_uiSpeedUpTimer -= uiDiff;
		m_creature->SetTargetGuid(ObjectGuid());				// target self even when someone does dmg, - needs testing if this is needed
	}
};

CreatureAI* GetAI_boss_glob_of_viscidus(Creature* pCreature)
{
    return new boss_glob_of_viscidusAI(pCreature);
}

void AddSC_boss_viscidus()
{
	Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_viscidus";
    pNewscript->GetAI = &GetAI_boss_viscidus;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "boss_glob_of_viscidus";
    pNewscript->GetAI = &GetAI_boss_glob_of_viscidus;
    pNewscript->RegisterSelf();
}
