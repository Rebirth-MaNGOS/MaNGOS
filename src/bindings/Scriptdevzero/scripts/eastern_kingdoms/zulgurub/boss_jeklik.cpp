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
SDName: Boss_Jeklik
SD%Complete: 90
SDComment:
SDCategory: Zul'Gurub
EndScriptData */

#include "precompiled.h"
#include "zulgurub.h"

enum eJeklik
{
    SAY_AGGRO                 = -1309000,
    SAY_RAIN_FIRE             = -1309001,
    SAY_DEATH                 = -1309002,

    SPELL_BLOOD_LEECH         = 22644,
    SPELL_CHARGE              = 22911,
    SPELL_CURSE_OF_BLOOD      = 16098,
    SPELL_GREAT_HEAL          = 23954,
    SPELL_JEKLIK_TRANSFORM    = 23966,
    SPELL_MIND_FLAY           = 23953,
    SPELL_PIERCE_ARMOR        = 12097,
    SPELL_PSYCHIC_SCREAM      = 22884,
    SPELL_SHADOW_WORD_PAIN    = 23952,
    SPELL_SONIC_BURST         = 23918,
    SPELL_CHAIN_MIND_FLAY     = 26044,                  // Right ID unknown. So disabled
    SPELL_SWOOP               = 23919,
    SPELL_SUMMON_BATS         = 23974,                  // not used for now
};

struct MANGOS_DLL_DECL boss_jeklikAI : public ScriptedAI
{
    boss_jeklikAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_zulgurub*)pCreature->GetInstanceData();
        Reset();
    }

    instance_zulgurub* m_pInstance;

    bool m_bPhaseTwo;

    uint32 m_uiBloodLeechTimer;
    uint32 m_uiChainMindFlayTimer;
    uint32 m_uiChargeTimer;
    uint32 m_uiCurseOfBloodTimer;
    uint32 m_uiGreatHealTimer;
    uint32 m_uiMindFlayTimer;
    uint32 m_uiPierceArmorTimer;
    uint32 m_uiShadowWordPainTimer;
    uint32 m_uiSonicBurstTimer;
    uint32 m_uiSpawnBatsTimer;
    uint32 m_uiSpawnFlyingBatsTimer;
    uint32 m_uiSwoopTimer;
	uint32 m_uiSuspendTimer;
	uint8 m_uiBatRiders;
	uint32 m_uiSplineTimer;

    GUIDList m_uiSummonList;

    void Reset()
    {
        m_bPhaseTwo = false;

        m_uiBloodLeechTimer = 17000;        // wrong time
        m_uiChainMindFlayTimer = 26000;
        m_uiChargeTimer = 20000;
        m_uiCurseOfBloodTimer = 12000;      // wrong time
        m_uiGreatHealTimer = 50000;
        m_uiMindFlayTimer = 11000;
        m_uiPierceArmorTimer = 15000;       // wrong time
        m_uiShadowWordPainTimer = 6000;
        m_uiSonicBurstTimer = 6000;
        m_uiSpawnBatsTimer = urand(15000,60000);
        m_uiSpawnFlyingBatsTimer = 1000;
        m_uiSwoopTimer = 2000;
		m_uiSuspendTimer = 0;
		m_uiBatRiders = 0;
		m_uiSplineTimer = 1000;
		m_creature->AddSplineFlag(SPLINEFLAG_FLYING);

        m_uiSummonList.clear();
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(SAY_AGGRO, m_creature);

        // Transform Jeklik into a bat on Aggro
        DoCastSpellIfCan(m_creature, SPELL_JEKLIK_TRANSFORM, CAST_FORCE_TARGET_SELF);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_JEKLIK, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_JEKLIK, DONE);

        // Despawn bats
        DespawnSummonedBats();
    }

    void JustReachedHome()
    {
        // Despawn bats
        DespawnSummonedBats();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_JEKLIK, NOT_STARTED);
    }

    void DespawnSummonedBats()
    {
        if (m_pInstance)
        {
            for(GUIDList::iterator itr = m_uiSummonList.begin(); itr != m_uiSummonList.end(); ++itr)
                if (Creature* pSummon = m_pInstance->instance->GetCreature(*itr))
                {
                    pSummon->SetHealth(0);
                    pSummon->SetDeathState(JUST_DIED);
                    pSummon->RemoveCorpse();
                    //pSummon->ForcedDespawn();
                }

            std::list<Creature*> m_lBatRiders; 
			GetCreatureListWithEntryInGrid(m_lBatRiders, m_creature, NPC_SUMMONED_GURUBASHI_BAT_RIDER, 200.f);

			if (!m_lBatRiders.empty())
			{
				for (std::list<Creature*>::iterator itr = m_lBatRiders.begin(); itr != m_lBatRiders.end(); ++itr)
				{
					Creature* pBat = *itr;
					if (pBat && pBat->isAlive())
						pBat->ForcedDespawn();
				}
			}
        }
        else
            debug_log("SD0: Zul'Gurub - Jeklik: m_pInstance NULL !");
    }

    void JustSummoned(Creature* pSummoned)
    {
		pSummoned->SetRespawnEnabled(false);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Start Phase Two
        if (!m_bPhaseTwo && HealthBelowPct(50))
        {
            DoScriptText(SAY_RAIN_FIRE, m_creature);
            m_creature->RemoveAurasDueToSpell(SPELL_JEKLIK_TRANSFORM);
            DoResetThreat();
            m_bPhaseTwo = true;
        }
		
		// for charge animation
		if (m_uiSuspendTimer > uiDiff)
        {
            m_uiSuspendTimer -= uiDiff;
            return;
        }

		// Reset spline flag if its not walkmode
        if (m_uiSplineTimer <= uiDiff)
        {
			if(m_creature->GetSplineFlags() == SPLINEFLAG_FLYING)
				m_creature->RemoveSplineFlag(SPLINEFLAG_FLYING);
            m_uiSplineTimer = 1000;
        }
        else
            m_uiSplineTimer -= uiDiff;

        if (!m_bPhaseTwo)
        {
            // Charge
            if (m_uiChargeTimer <= uiDiff)		// should charge closest within 8-40 yrd?
            {
                Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_CHARGE);
				m_uiSuspendTimer = 2000;
                m_uiSonicBurstTimer = 1000;
                m_uiChargeTimer = urand(5000, 10000);      // 10000, 20000
            }
            else
                m_uiChargeTimer -= uiDiff;

            // Pierce Armor
            if (m_uiPierceArmorTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_PIERCE_ARMOR);
                m_uiPierceArmorTimer = urand(20000, 30000);
            }
            else
                m_uiPierceArmorTimer -= uiDiff;

            // Sonic Burst
            if (m_uiSonicBurstTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_SONIC_BURST);
                m_uiSonicBurstTimer = urand(7000, 12000);
            }
            else
                m_uiSonicBurstTimer -= uiDiff;

            // Spawn pack of bats
            if (m_uiSpawnBatsTimer <= uiDiff)
            {
                int rnd_bat_number = urand(6, 8);
                for(uint8 i = 0; i < rnd_bat_number; ++i)
                {
                    if (Creature* pBat = m_creature->SummonCreature(NPC_FRENZIED_BLOODSEEKER_BAT, -12276.08f, -1399.54f, 144.83f, 5.65f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000))
                        pBat->SetCharmerGuid(m_creature->GetGUID());                                       
                }

                m_uiSpawnBatsTimer = 60000;
            }
            else
                m_uiSpawnBatsTimer -= uiDiff;

            // Swoop
            if (m_uiSwoopTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_SWOOP);
                m_uiSwoopTimer = urand(8000, 10000);
            }
            else
                m_uiSwoopTimer -= uiDiff;
        }
        else
        {
            // Shadow Word: Pain
            if (m_uiShadowWordPainTimer <= uiDiff)
            {
                Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_SHADOW_WORD_PAIN);

                m_uiShadowWordPainTimer = urand(10000, 15000);
            }
            else
                m_uiShadowWordPainTimer -=uiDiff;

            // Mind Flay
            if (m_uiMindFlayTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_MIND_FLAY);
                m_uiMindFlayTimer = 16000;
            }
            else
                m_uiMindFlayTimer -=uiDiff;

            // Chain Mind Flay
            if (m_uiChainMindFlayTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_CHAIN_MIND_FLAY);
                m_uiChainMindFlayTimer = urand(25000, 45000);
            }
            else
                m_uiChainMindFlayTimer -=uiDiff;

            // Greater heal
            if (m_uiGreatHealTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_GREAT_HEAL, CAST_INTERRUPT_PREVIOUS);
                m_uiGreatHealTimer = urand(25000, 35000);
            }
            else
                m_uiGreatHealTimer -=uiDiff;

            // Blood Leech
            if (m_uiBloodLeechTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_BLOOD_LEECH);
                m_uiBloodLeechTimer = 10000;
            }
            else
                m_uiBloodLeechTimer -= uiDiff;

            // Curse of Blood
            if (m_uiCurseOfBloodTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_CURSE_OF_BLOOD);
                m_uiCurseOfBloodTimer = 10000;//5*MINUTE*IN_MILLISECONDS;
            }
            else
                m_uiCurseOfBloodTimer -= uiDiff;

            // Summon Gurubashi Bat Rider which will throws a Liquid Fire Bombs onto ground
            if (m_uiSpawnFlyingBatsTimer <= uiDiff)
            {                    
				if(Creature* pBatRider = m_creature->SummonCreature(NPC_SUMMONED_GURUBASHI_BAT_RIDER, -12291.30f, -1380.26f, 145.81f, 2.25f, TEMPSUMMON_TIMED_DESPAWN, 80000))
				{
					pBatRider->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
					++m_uiBatRiders;
				}
				if(m_uiBatRiders < 2)
					m_uiSpawnFlyingBatsTimer = 12000;
				else
					m_uiSpawnFlyingBatsTimer = 22000;
            }
            else
                m_uiSpawnFlyingBatsTimer -=uiDiff;
         
        }

		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_jeklik(Creature* pCreature)
{
    return new boss_jeklikAI(pCreature);
}

/*################################
## mob_frenzied_bloodseeker_bat ##
################################*/

struct BatPosition
{
    float x;
    float y;
    float z;

    BatPosition(float x_in, float y_in, float z_in) : x(x_in), y(y_in), z(z_in) {};
};

struct MANGOS_DLL_DECL mob_frenzied_bloodseeker_batAI : public ScriptedAI
{
    
    mob_frenzied_bloodseeker_batAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        BatPosition initial_positions[6] = { BatPosition(-12282.95f, -1395.81f, 131.28f), 
                                             BatPosition(-12286.60f, -1401.40f, 130.60f), 
                                             BatPosition(-12294.36f, -1404.59f, 130.60f), 
                                             BatPosition(-12298.78f, -1408.47f, 130.60f), 
                                             BatPosition(-12292.59f, -1408.25f, 130.60f), 
                                             BatPosition(-12285.87f, -1407.64f, 130.60f) };

        int rand_pos = urand(0, 5);

        m_creature->GetMotionMaster()->MovePoint(0, initial_positions[rand_pos].x, initial_positions[rand_pos].y, initial_positions[rand_pos].z, false);

        m_creature->SetSpeedRate(MOVE_WALK, 1.8f, true); 
    
    
    }

    void MovementInform(uint32, uint32)
    {
        Unit* masters_victim = m_creature->GetCharmerOrOwner()->getVictim();

        if (masters_victim)
        {
            m_creature->GetMotionMaster()->MoveChase(masters_victim);
            m_creature->SetInCombatWith(masters_victim);
        }


        m_creature->SetSpeedRate(MOVE_WALK, 1.5f);
    }

    void Reset()
    {
    }
};

CreatureAI* GetAI_mob_frenzied_bloodseeker_bat(Creature* pCreature)
{
    return new mob_frenzied_bloodseeker_batAI(pCreature);
}

/*######
## mob_summoned_gurubashi_bat_rider
######*/

enum eSumBatRider
{   
    // Flying mode (Jeklik's encounter)
    SPELL_THROW_LIQUID_FIRE__ = 23970,
    SPELL_SUMMON_LIQUID_FIRE  = 23971
};

struct MANGOS_DLL_DECL mob_summoned_gurubashi_bat_riderAI : public ScriptedAI
{
    mob_summoned_gurubashi_bat_riderAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{
		m_pInstance = (instance_zulgurub*)pCreature->GetInstanceData();
		m_uiThrowLiquidFireTimer = urand(6000,18000);
		Reset();
	}

	instance_zulgurub* m_pInstance;
    uint32 m_uiThrowLiquidFireTimer;

    void Reset()
    {
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_THROW_LIQUID_FIRE__)
            pTarget->CastSpell(pTarget, SPELL_SUMMON_LIQUID_FIRE, true);
    }

    void UpdateAI (const uint32 uiDiff)
    {
        // Throw Liquid Fire
        if (m_uiThrowLiquidFireTimer <= uiDiff)
        {
			Unit* pTarget = m_creature->SelectRandomUnfriendlyTarget(0,50.f);
			if(pTarget)
				DoCastSpellIfCan(pTarget,SPELL_THROW_LIQUID_FIRE__);
			m_uiThrowLiquidFireTimer = urand(8000,18000);
		}
		else
			m_uiThrowLiquidFireTimer -= uiDiff;


		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        return;
    }
};

CreatureAI* GetAI_mob_summoned_gurubashi_bat_rider(Creature* pCreature)
{
    return new mob_summoned_gurubashi_bat_riderAI(pCreature);
}

/*######
## mob_gurubashi_bat_rider
######*/

enum eBatRider
{
    SPELL_DEMORALIZING_SHOUT        = 23511,
    SPELL_BATTLE_COMMAND            = 5115,
    SPELL_INFECTED_BITE             = 16128,
    SPELL_THRASH                    = 3391,
    SPELL_UNSTABLE_CONCOTION        = 24024,
};

struct MANGOS_DLL_DECL mob_gurubashi_bat_riderAI : public ScriptedAI
{
    mob_gurubashi_bat_riderAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    bool bInfectedBiteCasted;
    bool bExploded;

    uint32 m_uiBattleCommandTimer;
    uint32 m_uiInfectedBiteTimer;
    uint32 m_uiThrashTimer;

    void Reset()
    {
        bInfectedBiteCasted = false;
        bExploded = false;
		SetCombatMovement(true);
        m_uiBattleCommandTimer = 8000;
        m_uiInfectedBiteTimer = 6500;
        m_uiThrashTimer = 6000;
    }

    void Aggro(Unit* pVictim)
    {
		DoCastSpellIfCan(pVictim, SPELL_DEMORALIZING_SHOUT);
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32 &uiDamage)
    {
        // If new health is 40 percent or lesser, cast explode
        if (!bExploded && ( (uiDamage >= m_creature->GetHealth() ) || (HealthBelowPct(40)) ))
        {
            bExploded = true;
            uiDamage = 0;
            //DoCastSpellIfCan(m_creature, SPELL_THROW_LIQUID_FIRE__);
            DoCastSpellIfCan(m_creature, SPELL_UNSTABLE_CONCOTION, CAST_FORCE_CAST);
			m_creature->GenericTextEmote("Gurubashi Bat Rider fully engulfs in flame and a maddened look appears in his eyes!", NULL,false);
			SetCombatMovement(false);
        }
    }

    void UpdateAI (const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Battle Command
        if (m_uiBattleCommandTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BATTLE_COMMAND);
            m_uiBattleCommandTimer = 25000;
        }
        else
            m_uiBattleCommandTimer -= uiDiff;

        // Infected Bite
        if (!bInfectedBiteCasted && m_uiInfectedBiteTimer <= uiDiff)
        {
            bInfectedBiteCasted = true;
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_INFECTED_BITE);
        }
        else
            m_uiInfectedBiteTimer -= uiDiff;

        // Thrash
        if (m_uiThrashTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_THRASH);
            m_uiThrashTimer = 6000;
        }
        else
            m_uiThrashTimer -= uiDiff;

        DoMeleeAttackIfReady();     
    }
};

CreatureAI* GetAI_mob_gurubashi_bat_rider(Creature* pCreature)
{
    return new mob_gurubashi_bat_riderAI(pCreature);
}

/*######
## AddSC
######*/

void AddSC_boss_jeklik()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_jeklik";
    pNewScript->GetAI = &GetAI_boss_jeklik;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
    pNewScript->Name = "mob_summoned_gurubashi_bat_rider";
    pNewScript->GetAI = &GetAI_mob_summoned_gurubashi_bat_rider;
    pNewScript->RegisterSelf();


    pNewScript = new Script;
    pNewScript->Name = "mob_gurubashi_bat_rider";
    pNewScript->GetAI = &GetAI_mob_gurubashi_bat_rider;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_frenzied_bloodseeker_bat";
    pNewScript->GetAI = &GetAI_mob_frenzied_bloodseeker_bat;
    pNewScript->RegisterSelf();
}
