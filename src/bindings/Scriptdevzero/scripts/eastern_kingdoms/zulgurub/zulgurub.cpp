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
	NPC_RAZZASHI_SKITTERER		= 14880,
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
        m_uiVenomTimer = 5000;
		m_uiPoisonTimer = 0;
    }

	void JustDied(Unit* /*pKiller*/)			// Spawn adds on death
    {
		float fX, fY, fZ;
            m_creature->GetPosition(fX, fY, fZ);
			for(uint8 i = 0; i < 5; ++i)
                    if (Creature* pSkitterer = m_creature->SummonCreature(NPC_RAZZASHI_SKITTERER, fX+irand(-3,3), fY+irand(-3,3), fZ, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000))
                        pSkitterer->AI()->AttackStart(m_creature->getVictim());
	}

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		// Intoxicating venom timer
        if (m_uiVenomTimer <= uiDiff)
        {
			DoCastSpellIfCan(m_creature->getVictim(), SPELL_INTOXICATING_VENOM);
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
}
