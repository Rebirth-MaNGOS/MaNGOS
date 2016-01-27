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
SDName: ZulGurub
SD%Complete: 100
SDComment:
SDCategory: ZulGurub
EndScriptData */

/* ContentData
at_altar_of_the_blood_god
mob_razzashi_venombrood
mob_gurubashi_axe_thrower
mob_gurubashi_blood_drinker
EndContentData */

#include "precompiled.h"
#include "zulgurub.h"

/*######
## at_zulgurub
######*/

bool AreaTrigger_at_zulgurub(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (pPlayer->isGameMaster() || pPlayer->isDead())
        return false;

    instance_zulgurub* m_pInstance = (instance_zulgurub*)pPlayer->GetInstanceData();

    if (!m_pInstance || m_pInstance->GetData(TYPE_HAKKAR) == DONE)
        return false;

    m_pInstance->HakkarYell(pAt->id);
    return true;
}

/*######
##  mob_razzashi_venombrood
######*/

enum
{
	SPELL_INTOXICATING_VENOM	= 24596,
	SPELL_SLOWING_POISON		= 7992,
};

struct MANGOS_DLL_DECL mob_razzashi_venombroodAI : public ScriptedAI
{
    mob_razzashi_venombroodAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_zulgurub*)pCreature->GetInstanceData();
		Reset();
    }

    instance_zulgurub* m_pInstance;

    uint32 m_uiVenomTimer;
	uint32 m_uiPoisonTimer;

    void Reset()
    {
        m_uiVenomTimer = urand(5000,15000);
		m_uiPoisonTimer = urand(1000,3000);
    }


    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		// Intoxicating venom timer
        if (m_uiVenomTimer <= uiDiff)
        {
			DoCastSpellIfCan(m_creature->getVictim(), SPELL_INTOXICATING_VENOM, CAST_AURA_NOT_PRESENT);
			m_uiVenomTimer = 60000;
		}
		else 
			m_uiVenomTimer -= uiDiff;

        // Slowing poison timer
        if (m_uiPoisonTimer <= uiDiff)
        {
			DoCastSpellIfCan(m_creature->getVictim(), SPELL_SLOWING_POISON);
            m_uiPoisonTimer = urand(20000, 25000);
        }
        else
            m_uiPoisonTimer -= uiDiff;

		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_razzashi_venombrood(Creature* pCreature)
{
    return new mob_razzashi_venombroodAI(pCreature);
}

/*######
##  mob_razzashi_broodwidow
######*/

enum
{
	SPELL_WEB_SPIN				= 24600,
	NPC_RAZZASHI_SKITTERER		= 14880
};

struct MANGOS_DLL_DECL mob_razzashi_broodwidowAI : public ScriptedAI
{
    mob_razzashi_broodwidowAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_zulgurub*)pCreature->GetInstanceData();
		Reset();
    }

    instance_zulgurub* m_pInstance;

	uint32 m_uiWebSpinTimer;
	uint8 m_uiSummonCount;

    void Reset()
    {
		m_uiWebSpinTimer = urand(10000,20000);
		m_uiSummonCount = 0;
    }

	void JustSummoned(Creature* pSummoned)	
    {
        ++m_uiSummonCount;
	}

	void JustDied(Unit* /*pKiller*/)			// Spawn adds on death
    {
		if (m_uiSummonCount < 5)	// max 5 Spiders
        {
			float fX, fY, fZ;
            m_creature->GetPosition(fX, fY, fZ);
			for(uint8 i = 0; i < 5; ++i)
                    if (Creature* pSkitterer = m_creature->SummonCreature(NPC_RAZZASHI_SKITTERER, fX+irand(-3,3), fY+irand(-3,3), fZ, 0, TEMPSUMMON_DEAD_DESPAWN, 0))
					{
						pSkitterer->AI()->AttackStart(m_creature->getVictim());
						pSkitterer->SetRespawnDelay(-10);				// to stop them from randomly respawning
					}
		}
	}

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		// Web Spin
        if (m_uiWebSpinTimer <= uiDiff)
        {
			DoCastSpellIfCan(m_creature->getVictim(), SPELL_WEB_SPIN, CAST_AURA_NOT_PRESENT);
            m_uiWebSpinTimer = urand(20000,30000);
        }
        else
            m_uiWebSpinTimer -= uiDiff;

		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_razzashi_broodwidow(Creature* pCreature)
{
    return new mob_razzashi_broodwidowAI(pCreature);
}

/*######
## mob_gurubashi_axe_thrower
######*/

enum eAxeThrower
{
    SPELL_THROW               = 22887,
    SPELL_AXE_FLURRY          = 24018,
    SPELL_ENRAGE              = 8269,

    SAY_ENRAGE                = -1000003
};

bool IsPolymorphed(Unit* unit)
{
    Unit::AuraList const& auras = unit->GetAurasByType(SPELL_AURA_MOD_CONFUSE);

    for (auto itr = auras.begin(); itr != auras.end(); ++itr)
    {
        const SpellEntry *spellproto = (*itr)->GetSpellProto();

        if (spellproto)
        {
            switch(spellproto->Id)
            {
            case 118:
            case 12824:
            case 12825:
            case 12826:
            case 13323:
            case 15534:
            case 27760:
            case 28271:
            case 28272:
                return true;
            default:
                return false;
            }
        }
    }

    return false;

}

struct MANGOS_DLL_DECL mob_gurubashi_axe_throwerAI : public ScriptedAI
{
    mob_gurubashi_axe_throwerAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiThrowTimer;
    uint32 m_uiAxeFlurryTimer;
    uint32 m_uiAxeFlurryThrowTimer, m_uiAxeFlurryThrowCount;
    bool bEnraged, bAxeFlurryThrow;
    
    void Reset()
    {
        m_uiThrowTimer = 0;
        m_uiAxeFlurryTimer = 5000;
        m_uiAxeFlurryThrowTimer = 0;
        m_uiAxeFlurryThrowCount = 0;
        bEnraged = false;
        bAxeFlurryThrow = false;
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Enrage
        if (!bEnraged && HealthBelowPct(30))
        {
            bEnraged = true;
            DoCastSpellIfCan(m_creature, SPELL_ENRAGE);
            DoScriptText(SAY_ENRAGE, m_creature);
        }

        // Axe Flurry
        if (m_uiAxeFlurryTimer <= uiDiff)
        {
            bool cast_ok = false;

            if (!IsPolymorphed(m_creature) && !m_creature->isConfused() && !m_creature->isFeared() && !m_creature->isStunned())
            {
                m_creature->CastSpell(m_creature->getVictim(), SPELL_AXE_FLURRY, true);
                m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);  
                cast_ok = true;
            }


            if (cast_ok)
            {
                bAxeFlurryThrow = true;
                m_uiAxeFlurryThrowCount = 6;
                m_uiAxeFlurryTimer = urand(25000, 28000);
            }
        }
        else
            m_uiAxeFlurryTimer -= uiDiff;

        if (m_uiAxeFlurryThrowTimer > 2000)
            m_uiAxeFlurryThrowTimer = 0;

        if (bAxeFlurryThrow)
        {
			// If the Axe Thrower is CC'ed we interrupt the throwing.
			if (IsPolymorphed(m_creature) || m_creature->isConfused() || m_creature->isFeared() || m_creature->isStunned())
				m_uiAxeFlurryThrowCount = 0;

            if (m_uiAxeFlurryThrowTimer <= 0 && m_uiAxeFlurryThrowCount > 0)
            {
                // Make the Axe Throwers throw seven axes at a time at random players in the raid.

				std::vector<Player*> chosenPlayers;

				for (int i = 0; i < 7; i++)
				{
					Player *rndPlayer = GetRandomPlayerInCurrentMap(28);

					if (!rndPlayer || rndPlayer->isDead() || std::find(chosenPlayers.begin(), chosenPlayers.end(), rndPlayer) != chosenPlayers.end())
                            continue;
                            
                    if (!rndPlayer->isInCombat())
                    {
                        rndPlayer->SetInCombatWith(m_creature);
                        m_creature->AddThreat(rndPlayer, 0.1f);
                    }

					if (rndPlayer && !IsPolymorphed(m_creature) && !m_creature->isConfused() && !m_creature->isFeared())
					{
						m_creature->CastSpell(rndPlayer, 24020, true);
						chosenPlayers.push_back(rndPlayer);
					}
				}

                m_uiAxeFlurryThrowCount--;
                m_uiAxeFlurryThrowTimer = 2000;
            }
            else if (m_uiAxeFlurryThrowCount <= 0)
            {
                bAxeFlurryThrow = false;
                m_uiAxeFlurryThrowTimer = 0;
                m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, false);  
            }
            else
                m_uiAxeFlurryThrowTimer -= uiDiff;
        }

        if (m_creature->getVictim()->IsWithinDist(m_creature, 10))
        {
            if (m_creature->GetSheath() != SHEATH_STATE_MELEE)
                m_creature->SetSheath(SHEATH_STATE_MELEE);

        }
        else
        {
            if (m_creature->getVictim() && m_creature->GetDistance2d(m_creature->getVictim()) > 30 && m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
            {
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim(), 30.f);
            }
            else if (m_creature->getVictim() && m_creature->GetDistance2d(m_creature->getVictim()) < 30)
                m_creature->StopMoving();

            if (m_creature->GetSheath() != SHEATH_STATE_RANGED)
                m_creature->SetSheath(SHEATH_STATE_RANGED);

            // Throw
            if (m_uiThrowTimer <= uiDiff)
            {
                Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_THROW);
                m_uiThrowTimer = urand(500, 1000);     // 1500, 2000
            }
            else
                m_uiThrowTimer -= uiDiff;
        }
            
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_gurubashi_axe_thrower(Creature* pCreature)
{
    return new mob_gurubashi_axe_throwerAI(pCreature);
}

/*######
## mob_gurubashi_blood_drinker
######*/

enum eBloodDrinker
{
    SPELL_BLOOD_LEECH = 24437,
    SPELL_DRAIN_LIFE  = 24435
};

struct MANGOS_DLL_DECL mob_gurubashi_blood_drinkerAI : public ScriptedAI
{
    mob_gurubashi_blood_drinkerAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiBloodLeechTimer;
    uint32 m_uiDrainLifeTimer;

    void Reset()
    {
        m_uiBloodLeechTimer = 0;
        m_uiDrainLifeTimer = 8000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Blood Leech
        if (m_uiBloodLeechTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_BLOOD_LEECH);
            m_uiBloodLeechTimer = 5000;
        }
        else
            m_uiBloodLeechTimer -= uiDiff;

        // Drain Life
        if (m_uiDrainLifeTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectRandomFriendlyTarget(0, 20.0f))
                DoCastSpellIfCan(pTarget, SPELL_DRAIN_LIFE);
            m_uiDrainLifeTimer = 8000;
        }
        else
            m_uiDrainLifeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_gurubashi_blood_drinker(Creature* pCreature)
{
    return new mob_gurubashi_blood_drinkerAI(pCreature);
}

/*######################
## mob_withered_mistress
######################*/

enum eWitheredMistress
{
    SPELL_DISPEL = 23859,
	SPELL_CURSE_OF_BLOOD = 24673,
	SPELL_UNHOLY_FRENZY = 24672,
	SPELL_VEIL_OF_SHADOW = 24674
};

const uint32 withered_mistress_friendlist[] = { NPC_VOODOO_SLAVE, NPC_ATALAI_MISTRESS, NPC_WITHERED_MISTRESS };

struct MANGOS_DLL_DECL mob_withered_mistressAI : public ScriptedAI
{
    mob_withered_mistressAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{
		m_pInstance = (instance_zulgurub*) pCreature->GetInstanceData();

		Reset();
	}

	uint32 m_uiGlobalCD;
	uint32 m_uiOffensiveCD;
	uint32 m_uiSelfCD;
	instance_zulgurub* m_pInstance;

    void Reset()
    {
		m_uiGlobalCD = 0;
		m_uiOffensiveCD = 0;
		m_uiSelfCD = 0;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		m_uiSelfCD -= m_uiSelfCD > 0 ? uiDiff : 0;
		m_uiOffensiveCD -= m_uiOffensiveCD > 0 ? uiDiff : 0;

		if (m_uiGlobalCD <= uiDiff)
		{
			for (SpellAuraHolder* current_aura : m_creature->GetNegativeAuras())
				if (current_aura->GetSpellProto()->Dispel == DISPEL_MAGIC)
				{
					DoCastSpellIfCan(m_creature, SPELL_DISPEL);
					m_uiGlobalCD = 1500;
				}

			if (m_uiGlobalCD <= uiDiff)
				for (int i = 0; i < 3; i++)
				{
					std::vector<Creature*> friendlist = m_pInstance->GetCreaturesFromMultiStorage(withered_mistress_friendlist[i]);

					for (Creature* current_creature : friendlist)
					{
						for (SpellAuraHolder* current_aura : current_creature->GetNegativeAuras())
							if (current_aura->GetSpellProto()->Dispel == DISPEL_MAGIC && m_uiGlobalCD <= uiDiff)
							{
								DoCastSpellIfCan(current_creature, SPELL_DISPEL);
								m_uiGlobalCD = 1500;
								break;
							}

						if (m_uiGlobalCD > uiDiff)
							break;
					}

					if (m_uiGlobalCD > uiDiff)
						break;
				}

				if ( m_uiOffensiveCD <= uiDiff)
				{
					uint32 spellID;
					switch(urand(0, 1))
					{
						case 0:
							spellID = SPELL_CURSE_OF_BLOOD;
							break;
						case 1:
							spellID = SPELL_VEIL_OF_SHADOW;
							break;
					}
					DoCastSpellIfCan(m_creature->getVictim(), spellID);
					m_uiGlobalCD = 1500;
					m_uiOffensiveCD = 5000;
				}

				if ( m_uiSelfCD <= uiDiff)
				{
					DoCastSpellIfCan(m_creature, SPELL_UNHOLY_FRENZY);
					m_uiSelfCD = 20000;
					m_uiGlobalCD = 1500;
				}

		}
		else
			m_uiGlobalCD -= uiDiff;


        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_withered_mistress(Creature* pCreature)
{
    return new mob_withered_mistressAI(pCreature);
}

/*######
## npc_Gurubashi
######*/

struct MANGOS_DLL_DECL npc_GurubashiAI : public ScriptedAI
{
    npc_GurubashiAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bMC = true;
        Reset();
    }

	bool m_bMC;
	ObjectGuid m_uiPlayerGUID;
	uint32 m_uiMCTimer;

    void Reset()
    {
    }
	void DoMC(ObjectGuid pPlayerGUID)
	{
		if (!pPlayerGUID)
            return;

        m_uiPlayerGUID = pPlayerGUID;
		
		m_uiMCTimer = 3000;
	}

	void handleMC()
	{
		Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID);
		if(pPlayer && m_creature && pPlayer->IsWithinDistInMap(m_creature,50.f,true))
			m_creature->CastSpell(pPlayer,24178,true);
	}
	void UpdateAI(const uint32 uiDiff)
    {
		if (m_uiMCTimer <= uiDiff)
		{
			if(m_bMC)
			{
				handleMC();
				m_bMC = false;
				return;			
			}
		}
		else
			m_uiMCTimer -= uiDiff;
	}
};

CreatureAI* GetAI_npc_Gurubashi(Creature* pCreature)
{
    return new npc_GurubashiAI(pCreature);
}

/*######
## go_jinxed_hoodoo_pile
######*/

bool GOUse_go_jinxed_hoodoo_pile(Player* pPlayer, GameObject* pGo)
{
	if(Creature* pGurubashi = pPlayer->SummonCreature(15047, pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ(), 0.f, TEMPSUMMON_TIMED_DESPAWN,30000,false))
	{
		if (npc_GurubashiAI* pAI = dynamic_cast<npc_GurubashiAI*>(pGurubashi->AI()))
		{
			int r = urand(1,4);		// 75% chance to MC the player
				if(r != 4)
					pAI->DoMC(pPlayer->GetObjectGuid());
			return true;
		}
	}

    return false;
}

/*######
## npc_hakkari_witch_doctor
######*/

enum eWitchDoctor
{
	SPELL_SHRINK = 24054,
	SPELL_RELEASE_TOADS = 24058,
	SPELL_HEX = 24053,
	SPELL_SHADOW_SHOCK = 17289,
	SPELL_TOAD_EXPLODE = 24063,
	// used to kill toads
	SPELL_DISEASE_CLOUD = 24063,
	NPC_JUNGLE_TOAD = 15010,

	NPC_VOODOO_SPIRIT = 15009
};

struct MANGOS_DLL_DECL npc_hakkari_witch_doctorAI : public ScriptedAI
{
    npc_hakkari_witch_doctorAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

	uint32 m_uiExplodeToadTimer;
	uint32 m_uiHexTimer;
	uint32 m_uiReleaseToadsTimer;
	uint32 m_uiShadowShockTimer;
	uint32 m_uiShrinkTimer;

	bool m_bDidExplode;

    void Reset()
    {
		m_uiExplodeToadTimer = 5000;
		m_uiHexTimer = urand(3000,5000);
		m_uiReleaseToadsTimer = urand(6000,10000);
		m_uiShadowShockTimer = urand(1000,2000);
		m_uiShrinkTimer = urand(10000,15000);
		m_bDidExplode = false;
    }
	
	void JustDied(Unit* pKiller)
    {
        float fX, fY, fZ;
        m_creature->GetPosition(fX,fY,fZ);
        
        // spawn a spirit on death and cast spirit burst
        if(Creature* pSpirit = m_creature->SummonCreature(NPC_VOODOO_SPIRIT, fX, fY,fZ, 0, TEMPSUMMON_TIMED_DESPAWN, 6000, false))
        {
            pSpirit->CastSpell(pSpirit, 24051, true);
            pSpirit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            pSpirit->AI()->AttackStart(GetRandomPlayerInCurrentMap(15.f));
        }
    }

	void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if (pSpell->Id == 24062 && pTarget->GetEntry() == NPC_JUNGLE_TOAD)
			if (!m_bDidExplode)
			{
				pTarget->CastSpell(pTarget, SPELL_DISEASE_CLOUD, true);
				m_bDidExplode = true;
			}
    }

	void JustSummoned(Creature* pSummoned)
    {
		float x, y, z;
        pSummoned->GetClosePoint(x, y, z, 1.0f, 2.0f);
        pSummoned->GetMotionMaster()->MovePoint(1,x+urand(-20,20),y+urand(-20,20),z);		// move a bit further than the usual movement
    }

	void SummonedMovementInform(Creature* pSummoned, uint32 uiMotionType, uint32 uiPointId)
    {
        if(uiPointId == 1)		// stay where they moved
        {
            float x, y, z;
			pSummoned->GetClosePoint(x, y, z, 1.0f, 2.0f);
			CreatureCreatePos pos(pSummoned->GetMap(), x, y, z, 0.f);
			pSummoned->SetSummonPoint(pos);
        }
    }

	void UpdateAI(const uint32 uiDiff)
    {
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		// Release Toads
        if (m_uiReleaseToadsTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_RELEASE_TOADS);
			m_bDidExplode = false;
            m_uiReleaseToadsTimer = urand(15000,20000);
        }
        else
            m_uiReleaseToadsTimer -= uiDiff;

        // Hex
        if (m_uiHexTimer <= uiDiff)
        {
			Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_HEX);
            m_uiHexTimer = urand(3000, 7000);
        }
        else
            m_uiHexTimer -= uiDiff;

		// Shadow Shock
        if (m_uiShadowShockTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_SHOCK);
            m_uiShadowShockTimer = urand(5000,7000);
        }
        else
            m_uiShadowShockTimer -= uiDiff;

		// Shrink
        if (m_uiShrinkTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHRINK);
            m_uiShrinkTimer = 120000;
        }
        else
            m_uiShrinkTimer -= uiDiff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_hakkari_witch_doctor(Creature* pCreature)
{
    return new npc_hakkari_witch_doctorAI(pCreature);
}

/*######
## npc_edge_of_madness_lightning
######*/

struct MANGOS_DLL_DECL npc_edge_of_madness_lightningAI : public ScriptedAI
{
    npc_edge_of_madness_lightningAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
		m_uiEventTimer = 1000;
		m_uiPhase = 0;
		
        Reset();
    }

	uint8 m_uiPhase;
	uint32 m_uiEventTimer;
	std::list<GameObject*> m_lLightningsGroup1;
	std::list<GameObject*> m_lLightningsGroup2;
	std::list<GameObject*> m_lLightningsGroup3;
	std::list<GameObject*> m_lLightningsGroup4;
	std::list<GameObject*> m_lLightningsGroup5;
	std::list<GameObject*> m_lLightningsGroup6;

    void Reset()
    {
    }

	void HandleLightningGroups(int Group = 0)
	{
		if (Group == 1)
		{
			GetGameObjectListWithEntryInGrid(m_lLightningsGroup1, m_creature, 180252, 50.f);
			if (!m_lLightningsGroup1.empty())
			{
				for (std::list<GameObject*>::iterator itr = m_lLightningsGroup1.begin(); itr != m_lLightningsGroup1.end(); ++itr)
				{
					GameObject* pLightningsGroup1 = *itr;
					if (pLightningsGroup1)
						pLightningsGroup1->SendGameObjectCustomAnim(pLightningsGroup1->GetObjectGuid());
				}
			}
		}
		if (Group == 2)
		{
			GetGameObjectListWithEntryInGrid(m_lLightningsGroup2, m_creature, 183356, 50.f);
			if (!m_lLightningsGroup2.empty())
			{
				for (std::list<GameObject*>::iterator itr = m_lLightningsGroup2.begin(); itr != m_lLightningsGroup2.end(); ++itr)
				{
					GameObject* pLightningsGroup2 = *itr;
					if (pLightningsGroup2)
						pLightningsGroup2->SendGameObjectCustomAnim(pLightningsGroup2->GetObjectGuid());
				}
			}
		}
		if (Group == 3)
		{
			GetGameObjectListWithEntryInGrid(m_lLightningsGroup3, m_creature, 1802520, 50.f);
			if (!m_lLightningsGroup3.empty())
			{
				for (std::list<GameObject*>::iterator itr = m_lLightningsGroup3.begin(); itr != m_lLightningsGroup3.end(); ++itr)
				{
					GameObject* pLightningsGroup3 = *itr;
					if (pLightningsGroup3)
						pLightningsGroup3->SendGameObjectCustomAnim(pLightningsGroup3->GetObjectGuid());
				}
			}
		}
		if (Group == 4)
		{
			GetGameObjectListWithEntryInGrid(m_lLightningsGroup4, m_creature, 1802521, 50.f);
			if (!m_lLightningsGroup4.empty())
			{
				for (std::list<GameObject*>::iterator itr = m_lLightningsGroup4.begin(); itr != m_lLightningsGroup4.end(); ++itr)
				{
					GameObject* pLightningsGroup4 = *itr;
					if (pLightningsGroup4)
						pLightningsGroup4->SendGameObjectCustomAnim(pLightningsGroup4->GetObjectGuid());
				}
			}
		}
		if (Group == 5)
		{
			GetGameObjectListWithEntryInGrid(m_lLightningsGroup5, m_creature, 1802522, 50.f);
			if (!m_lLightningsGroup5.empty())
			{
				for (std::list<GameObject*>::iterator itr = m_lLightningsGroup5.begin(); itr != m_lLightningsGroup5.end(); ++itr)
				{
					GameObject* pLightningsGroup5 = *itr;
					if (pLightningsGroup5)
						pLightningsGroup5->SendGameObjectCustomAnim(pLightningsGroup5->GetObjectGuid());
				}
			}
		}
		if (Group == 6)
		{
			GetGameObjectListWithEntryInGrid(m_lLightningsGroup6, m_creature, 1833560, 50.f);
			if (!m_lLightningsGroup6.empty())
			{
				for (std::list<GameObject*>::iterator itr = m_lLightningsGroup6.begin(); itr != m_lLightningsGroup6.end(); ++itr)
				{
					GameObject* pLightningsGroup6 = *itr;
					if (pLightningsGroup6)
						pLightningsGroup6->SendGameObjectCustomAnim(pLightningsGroup6->GetObjectGuid());
				}
			}
		}
	}

	void UpdateAI(const uint32 uiDiff)
    {
		if (m_uiEventTimer)
		{
			if (m_uiEventTimer <= uiDiff)
			{
				switch (m_uiPhase)
				{
					case 0:						
						m_uiEventTimer = 1500;
						break;
					case 1:
						HandleLightningGroups(1);
						m_uiEventTimer = 4000;
						break;
					case 2:
						HandleLightningGroups(2);
						m_uiEventTimer = 3000;
						break;
					case 3:
						HandleLightningGroups(3);
						m_uiEventTimer = 4000;
						break;
					case 4:
						HandleLightningGroups(1);
						m_uiEventTimer = 2000;
						break;
					case 5:
						HandleLightningGroups(5);
						m_uiEventTimer = 500;
						break;
					case 6:
						HandleLightningGroups(6);
						m_uiEventTimer = 500;
						break;
					case 7:
						HandleLightningGroups(1);
						m_uiEventTimer = 2000;
						break;
					case 8:
						HandleLightningGroups(2);
						m_uiEventTimer = 1000;
						break;
					case 9:
						HandleLightningGroups(1);
						m_uiEventTimer = 1000;
						break;
					case 10:
						HandleLightningGroups(4);
						m_uiEventTimer = 1000;
						break;			
					case 11:
						HandleLightningGroups(4);
						m_uiEventTimer = 3000;
						break;	
					case 12:
						HandleLightningGroups(2);
						m_uiEventTimer = 4000;
						break;
					case 13:
						HandleLightningGroups(1);
						m_uiEventTimer = 5000;
						break;
					case 14:
						m_creature->ForcedDespawn();
						m_uiEventTimer = 0;
						break;
				}
			++m_uiPhase;
			}
			else
				m_uiEventTimer -= uiDiff;
		}			
		else
			return;
	}
};

CreatureAI* GetAI_npc_edge_of_madness_lightning(Creature* pCreature)
{
    return new npc_edge_of_madness_lightningAI(pCreature);
}

/*######
## npc_voodoo_slave
######*/

enum eVoodooSlave
{
	SPELL_RAIN_OF_FIRE = 24669,
	SPELL_INFERNO = 24670,
	SPELL_SHADOW_BOLT = 12739,
	SPELL_CURSE_OF_SHADOW = 17937,

	NPC_INFERNAL = 89
};

struct MANGOS_DLL_DECL npc_voodoo_slaveAI : public ScriptedAI
{
    npc_voodoo_slaveAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

	uint32 m_uiInfernoTimer;
	uint32 m_uiRainOfFireTimer;
	uint32 m_uiShadowBoltTimer;
	uint32 m_uiCurseOfShadowTimer;
    uint8 m_uiInfernalCount;
	bool m_bSecondInfernal;

    void Reset()
    {
		m_uiShadowBoltTimer = urand(1000, 3000);
		m_uiCurseOfShadowTimer = 800;
		m_uiInfernoTimer = urand(4000, 15000);
		m_uiRainOfFireTimer = urand(10000, 22000);
		m_bSecondInfernal = false;
        m_uiInfernalCount = 0;
    }
	
	void JustSummoned(Creature* pSummoned)
    {        
        m_uiInfernalCount++;
		pSummoned->SetRespawnEnabled(false);
		// the first one that is summoned is by the spell and it bugs out, so despawn and make a new one
		if(pSummoned->GetEntry() == NPC_INFERNAL && m_uiInfernalCount !=  2)
		{
			pSummoned->ForcedDespawn();
			m_bSecondInfernal = true;	
		}
    }

	void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_INFERNO && pTarget && m_bSecondInfernal)
        {
            if(Creature* pInfernal = m_creature->SummonCreature(NPC_INFERNAL,pTarget->GetPositionX(),pTarget->GetPositionY(),pTarget->GetPositionZ(),0,TEMPSUMMON_TIMED_DESPAWN,300000,false))
			{
				pInfernal->setFaction(m_creature->getFaction());
				pInfernal->SetLevel(60);
				pInfernal->SetOwnerGuid(m_creature->GetGUID());
				pInfernal->AI()->AttackStart(pTarget);      
			}
        }
    }

	void UpdateAI(const uint32 uiDiff)
    {
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Inferno
        if (m_uiInfernoTimer <= uiDiff)
        {
			Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
			if(pTarget && pTarget->GetTypeId() == TYPEID_PLAYER)
				DoCastSpellIfCan(pTarget, SPELL_INFERNO, CAST_INTERRUPT_PREVIOUS);
			m_uiInfernoTimer = 320000;
        }
        else
            m_uiInfernoTimer -= uiDiff;

		// Rain of Fire
        if (m_uiRainOfFireTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
			DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_RAIN_OF_FIRE, CAST_INTERRUPT_PREVIOUS);
            m_uiRainOfFireTimer = urand(25000, 35000);
        }
        else
            m_uiRainOfFireTimer -= uiDiff;

		// Shadow bolt
        if (m_uiShadowBoltTimer <= uiDiff)
        {
			DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_BOLT);
            m_uiShadowBoltTimer = urand(2000, 3000);
        }
        else
            m_uiShadowBoltTimer -= uiDiff;

		// Curse of Shadow
        if (m_uiCurseOfShadowTimer <= uiDiff)
        {
			DoCastSpellIfCan(m_creature->getVictim(), SPELL_CURSE_OF_SHADOW, CAST_AURA_NOT_PRESENT);
            m_uiCurseOfShadowTimer = urand(20000, 30000);
        }
        else
            m_uiCurseOfShadowTimer -= uiDiff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_voodoo_slave(Creature* pCreature)
{
    return new npc_voodoo_slaveAI(pCreature);
}

/*######
## AddSC
######*/

void AddSC_zulgurub()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "at_zulgurub";
    pNewScript->pAreaTrigger = &AreaTrigger_at_zulgurub;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
    pNewScript->Name = "mob_razzashi_venombrood";
    pNewScript->GetAI = GetAI_razzashi_venombrood;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
    pNewScript->Name = "mob_razzashi_broodwidow";
    pNewScript->GetAI = GetAI_mob_razzashi_broodwidow;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_gurubashi_axe_thrower";
    pNewScript->GetAI = GetAI_mob_gurubashi_axe_thrower;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_gurubashi_blood_drinker";
    pNewScript->GetAI = GetAI_mob_gurubashi_blood_drinker;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
	pNewScript->Name = "mob_withered_mistress";
	pNewScript->GetAI = GetAI_mob_withered_mistress;
	pNewScript->RegisterSelf();

	pNewScript = new Script;
	pNewScript->Name = "npc_Gurubashi";
	pNewScript->GetAI = GetAI_npc_Gurubashi;
	pNewScript->RegisterSelf();

	pNewScript = new Script;
    pNewScript->Name = "go_jinxed_hoodoo_pile";
    pNewScript->pGOUse = &GOUse_go_jinxed_hoodoo_pile;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
	pNewScript->Name = "npc_hakkari_witch_doctor";
	pNewScript->GetAI = GetAI_npc_hakkari_witch_doctor;
	pNewScript->RegisterSelf();

	pNewScript = new Script;
	pNewScript->Name = "npc_edge_of_madness_lightning";
	pNewScript->GetAI = GetAI_npc_edge_of_madness_lightning;
	pNewScript->RegisterSelf();

	pNewScript = new Script;
	pNewScript->Name = "npc_voodoo_slave";
	pNewScript->GetAI = GetAI_npc_voodoo_slave;
	pNewScript->RegisterSelf();
}
