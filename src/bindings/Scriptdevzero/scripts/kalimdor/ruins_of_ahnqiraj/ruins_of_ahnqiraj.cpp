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
SDName: Ruins of Ahn'Qiraj
SD%Complete: 100
SDComment:
SDCategory: Ruins of Ahn'Qiraj
EndScriptData */

/* ContentData
mob_anubisath_guardian
mob_flesh_hunter
mob_obsidian_destroyer
mob_qiraji_swarmguard
EndContentData */

#include "precompiled.h"
#include "ruins_of_ahnqiraj.h"

/*######
## mob_anubisath_guardian
######*/

enum eGuardian
{
    SPELL_METEOR                 = 24340,
    SPELL_PLAGUE                 = 22997,
    SPELL_SHADOW_STORM           = 26546,
    SPELL_THUNDER_CLAP           = 26554,
    SPELL_REFLECT_ARFR           = 13022,
    SPELL_REFLECT_FSSH           = 19595,
    SPELL_ENRAGE                 = 8559,
    SPELL_EXPLODE                = 25698,

    SPELL_SUMMON_ANUB_SWARMGUARD = 17430,
    SPELL_SUMMON_ANUB_WARRIOR    = 17431,

    EMOTE_FRENZY                 = -1000002
};

struct MANGOS_DLL_DECL mob_anubisath_guardianAI : public ScriptedAI
{
    mob_anubisath_guardianAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    bool m_bIsEnraged;

    uint32 m_uiSummonCount;

    uint32 m_uiSpell1;
    uint32 m_uiSpell2;
    uint32 m_uiSpell3;
    uint32 m_uiSpell4;
    uint32 m_uiSpell5;

    uint32 m_uiSpell1Timer;
    uint32 m_uiSpell2Timer;
    uint32 m_uiSpell5Timer;

    void Reset()
    {
        m_bIsEnraged = false;

        m_uiSummonCount = 0;

        m_uiSpell1 = urand(0,1) ? SPELL_METEOR : SPELL_PLAGUE;
        m_uiSpell2 = urand(0,1) ? SPELL_SHADOW_STORM : SPELL_THUNDER_CLAP;
        m_uiSpell3 = urand(0,1) ? SPELL_REFLECT_ARFR : SPELL_REFLECT_FSSH;
        m_uiSpell4 = urand(0,1) ? SPELL_ENRAGE : SPELL_EXPLODE;
        m_uiSpell5 = urand(0,1) ? SPELL_SUMMON_ANUB_SWARMGUARD : SPELL_SUMMON_ANUB_WARRIOR;

        m_uiSpell1Timer = 10000;
        m_uiSpell2Timer = 20000;
        m_uiSpell5Timer = 10000;
    }

    void Aggro(Unit* /*pWho*/)
    {
        // spell reflection
        DoCastSpellIfCan(m_creature, m_uiSpell3);
    }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->AI()->AttackStart(m_creature->getVictim());
        ++m_uiSummonCount;
    }

    void SummonedCreatureDespawn(Creature* /*pDespawned*/)
    {
        --m_uiSummonCount;
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32& /*uiDamage*/)
    {
        // when we reach 10% of HP explode or enrage
        if (!m_bIsEnraged && HealthBelowPct(10))
        {
            if (m_uiSpell4 == SPELL_ENRAGE)
            {
                DoScriptText(EMOTE_FRENZY, m_creature);
                DoCastSpellIfCan(m_creature, m_uiSpell4);
                m_bIsEnraged = true;
            }
            else
                DoCastSpellIfCan(m_creature->getVictim(), m_uiSpell4);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Meteor or Plague
        if (m_uiSpell1Timer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), m_uiSpell1);
            m_uiSpell1Timer = 15000;
        }
        else
            m_uiSpell1Timer -= uiDiff;

        // Shadow Storm or Thunder Clap
        if (m_uiSpell2Timer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), m_uiSpell2);
            m_uiSpell2Timer = 15000;
        }
        else
            m_uiSpell2Timer -= uiDiff;

        // summon Anubisath Swarmguard or Anubisath Warrior
        if (m_uiSpell5Timer <= uiDiff)
        {
            // change for summon spell
            if (m_uiSummonCount < 4)
                DoCastSpellIfCan(m_creature->getVictim(), m_uiSpell5);

            m_uiSpell5Timer = 15000;
        }
        else
            m_uiSpell5Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_anubisath_guardian(Creature* pCreature)
{
    return new mob_anubisath_guardianAI(pCreature);
}

/*######
## mob_flesh_hunter
######*/

enum eFleshHunter
{
    SPELL_CONSUME       = 25371,
    //SPELL_CONSUME_DMG   = 25373,
    SPELL_CONSUME_HEAL  = 25378,
	SPELL_CONSUME_ROOT	= 25374,
    SPELL_POISON_BOLT   = 25424,
    SPELL_TRASH         = 3391,
	SPELL_SPIT_OUT		= 25383//24199,			// used if a player survives consume, wrong spell for now
};

struct MANGOS_DLL_DECL mob_flesh_hunterAI : public ScriptedAI
{
    mob_flesh_hunterAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    bool m_bPlayerConsumed;
	bool m_bCanTurn;

	uint32 m_uiConsumeRemoveTimer;
	uint32 m_uiTurnTimer;
    uint32 m_uiPoisonBoltTimer;
    uint32 m_uiTrashTimer;
    uint32 m_uiConsumeTimer;
    
    ObjectGuid m_uiConsumeVictim;

    void Reset() 
    {
        m_bPlayerConsumed = false;
		m_bCanTurn = true;

		m_uiConsumeRemoveTimer = 0;
		m_uiTurnTimer = 0;
        m_uiPoisonBoltTimer = 10000;
        m_uiTrashTimer = 15000;
        m_uiConsumeTimer = urand(10000,30000);
		if (m_creature->HasAura(SPELL_CONSUME_ROOT))
			m_creature->RemoveAurasDueToSpell(SPELL_CONSUME_ROOT);
		m_uiConsumeVictim.Clear();
    }

    void KilledUnit(Unit* pWho)
    {
        if (pWho->GetObjectGuid() == m_uiConsumeVictim)
            m_creature->CastSpell(m_creature, SPELL_CONSUME_HEAL, true);
    }

	void JustDied(Unit* /*pKiller*/)
    {
		if (Unit* pConsumeTarget = m_creature->GetMap()->GetUnit(m_uiConsumeVictim))
            if (pConsumeTarget->HasAura(SPELL_CONSUME))
				pConsumeTarget->RemoveAurasDueToSpell(SPELL_CONSUME);
	}

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiPoisonBoltTimer <= uiDiff)
        {
                         
			Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0); 
			DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_POISON_BOLT);
			m_uiPoisonBoltTimer = urand(5000,10000);			 
        }
        else
            m_uiPoisonBoltTimer -= uiDiff;

        if (m_uiConsumeTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->getVictim())
            {
                m_creature->CastSpell(pTarget, SPELL_CONSUME, true);
				m_creature->CastSpell(pTarget, SPELL_CONSUME_ROOT, true);
				m_uiConsumeVictim = pTarget->GetObjectGuid();
                DoTeleportPlayer(m_creature->getVictim(), m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()+.75f, 0);
				if (m_creature->getThreatManager().getThreat(pTarget))
					m_creature->getThreatManager().modifyThreatPercent(pTarget, -100);			// never attack the player that is consumed
                m_bPlayerConsumed = true;
				m_bCanTurn = true;
				m_uiTurnTimer = 50;
				m_uiConsumeRemoveTimer = 15010;			// slightly longer so we're sure the aura is removed
            }
            m_uiConsumeTimer = urand(25000,35000);
        }
        else
            m_uiConsumeTimer -= uiDiff;

        if (Unit* pConsumeTarget = m_creature->GetMap()->GetUnit(m_uiConsumeVictim))
            if (pConsumeTarget->HasAura(SPELL_CONSUME))
            {
				if (m_uiTurnTimer <= uiDiff)
				{
					if (m_bCanTurn)
					{
						uint32 Direction = urand(0,6);
						m_creature->SetFacingTo(Direction);
						m_uiTurnTimer = 30000;
						m_bCanTurn = false;
					}
				}
				else
					m_uiTurnTimer -= uiDiff;				
            }
			else
				if (m_creature->HasAura(SPELL_CONSUME_ROOT))
					m_creature->RemoveAurasDueToSpell(SPELL_CONSUME_ROOT);

		if(m_bPlayerConsumed)
		{
			if (m_uiConsumeRemoveTimer <= uiDiff)
			{
				if(Unit* pConsumeTarget = m_creature->GetMap()->GetUnit(m_uiConsumeVictim))
					if(pConsumeTarget->isAlive())
						m_creature->CastSpell(pConsumeTarget, SPELL_SPIT_OUT, true);

				m_bPlayerConsumed = false;
			}
			else
				m_uiConsumeRemoveTimer -= uiDiff;  
		}
		else
			DoMeleeAttackIfReady();			// no melee while we have a player inside

        if (m_uiTrashTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_TRASH);				// this right?
            m_uiTrashTimer = urand(5000,8000);
        }
        else
            m_uiTrashTimer -= uiDiff;	 
    }
};

CreatureAI* GetAI_mob_flesh_hunter(Creature* pCreature)
{
    return new mob_flesh_hunterAI(pCreature);
}

/*######
## mob_obsidian_destroyer
######*/

enum eObsidianDestroyer
{
    SPELL_PURGE       = 25756,
	SPELL_DRAIN_MANA  = 25755,
    SPELL_DRAIN_MANA_VISUAL  = 26639,
    
    SPELL_SHOCK_BLAST = 26458,
    NPC_OBSIDIAN_ERADICATOR = 15262,

	SMALL_OBSIDIAN_CHUNK = 181068,
	LARGE_OBSIDIAN_CHUNK = 181069,
};

struct MANGOS_DLL_DECL mob_obsidian_destroyerAI : public ScriptedAI
{
    mob_obsidian_destroyerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        switch(m_creature->GetEntry())
        {
            case NPC_OBSIDIAN_DESTROYER:
                m_uiFullManaSpell = SPELL_PURGE;
                m_bIsEradicator = false;
                break;
            case NPC_OBSIDIAN_ERADICATOR:
                m_uiFullManaSpell = SPELL_SHOCK_BLAST;
                m_bIsEradicator = true;
                break;
        }
        Reset();
    }

	uint32 m_uiManaDrainTimer;
    uint32 m_uiFullManaSpell;;
    bool m_bIsEradicator;
    short m_uiDrain_count;
    
    void Reset() 
    {
        m_uiDrain_count = 0;
		m_uiManaDrainTimer = 10000;
		m_creature->SetPower(POWER_MANA, 0);
        m_creature->SetMaxPower(POWER_MANA, 0);
    }

	void Aggro(Unit* /*pWho*/)
    {
        // Combat so we have everyone on threat list for mana drain
        m_creature->SetInCombatWithZone();
        
        m_creature->SetMaxPower(POWER_MANA, m_creature->GetCreatureInfo()->maxmana);
        if(m_bIsEradicator)
        {
            const ThreatList& threatList = m_creature->getThreatManager().getThreatList();
            
            if(!threatList.empty())
            {
                for (HostileReference *currentReference : threatList)
                {
                    Unit *target = currentReference->getTarget();
                    if (target && target->GetTypeId() == TYPEID_PLAYER && target->getPowerType() == POWER_MANA && target->GetDistance(m_creature) < 30.0f)
                    {
                        m_creature->CastSpell(target, SPELL_DRAIN_MANA, true);
                        ++m_uiDrain_count;
                        if(m_uiDrain_count == 16)
                            break;
                    }
                }
            }
        }
    }

	void JustDied(Unit* /*pKiller*/)
    {
		float x, y, z;
        m_creature->GetClosePoint(x, y, z, 1.0f, 2.0f);
		// spawn a random obsidian
		int Size = urand(0,1);
		if(GameObject* pChunk = m_creature->SummonGameObject((Size>0?SMALL_OBSIDIAN_CHUNK:LARGE_OBSIDIAN_CHUNK),3600000, x, y, z, 0,GO_STATE_ACTIVE))
		{		
			pChunk->SetGoState(GO_STATE_READY);
			pChunk->SetLootState(GO_READY);	
            pChunk->SetOwnerGuid(ObjectGuid());
		}
    }
    
    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_DRAIN_MANA)
            pTarget->CastSpell(m_creature,SPELL_DRAIN_MANA_VISUAL, true);               // animation for mana drain
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (m_uiManaDrainTimer <= uiDiff)
        {
			const ThreatList& threatList = m_creature->getThreatManager().getThreatList();

			if(!threatList.empty())
			{
				for (HostileReference *currentReference : threatList)
				{
					Unit *target = currentReference->getTarget();
					if (target && target->GetTypeId() == TYPEID_PLAYER && target->getPowerType() == POWER_MANA && target->GetDistance(m_creature) < 30.0f)
						m_creature->CastSpell(target, SPELL_DRAIN_MANA, true);
				}
			}
			m_uiManaDrainTimer = urand(8000, 12000);
         } 
		else
			m_uiManaDrainTimer -= uiDiff;

		if (m_creature->GetPower(POWER_MANA) == m_creature->GetMaxPower(POWER_MANA))
			DoCastSpellIfCan(m_creature, m_uiFullManaSpell);

        DoMeleeAttackIfReady(); 
    }
};

CreatureAI* GetAI_mob_obsidian_destroyer(Creature* pCreature)
{
    return new mob_obsidian_destroyerAI(pCreature);
}

/*######
## mob_qiraji_swarmguard
######*/

enum eQirajiSwarmguard
{
    SPELL_SUNDERING_CLEAVE       = 25174,
};

struct MANGOS_DLL_DECL mob_qiraji_swarmguardAI : public ScriptedAI
{
    mob_qiraji_swarmguardAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

	uint32 m_uiCleaveTimer;

    void Reset() 
    {		
		m_uiCleaveTimer = urand(8000,12000);
    }
	void MovementInform(uint32 /*uiMotiontype*/, uint32 uiPointId)
	{
		if(uiPointId)
			m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
	}

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (m_uiCleaveTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SUNDERING_CLEAVE);
			m_uiCleaveTimer = urand(8000, 12000);
         } 
		else
			m_uiCleaveTimer -= uiDiff;

        DoMeleeAttackIfReady(); 
    }
};

CreatureAI* GetAI_mob_qiraji_swarmguard(Creature* pCreature)
{
    return new mob_qiraji_swarmguardAI(pCreature);
}

/*######
## mob_qiraji_gladiator
######*/

enum eQirajiGladiator
{
    SPELL_VENGEANCE         = 25164,
    SPELL_GLADIATOR_TRASH   = 5568,
    SPELL_UPPERCUT          = 10966,

    MOB_QIRAJI_GLADIATOR    = 15324,
};

struct MANGOS_DLL_DECL mob_qiraji_gladiator : public ScriptedAI
{
    mob_qiraji_gladiator(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

	uint32 m_uiTrashTimer;
    uint32 m_uiUpperCutTimer;
    uint32 m_uiVengeanceTimer;
    ObjectGuid m_GladiatorFriend;

    void Reset() 
    {		
        m_uiVengeanceTimer = urand(30000, 40000);
		m_uiTrashTimer = urand(8000,12000);
        m_uiUpperCutTimer = urand(7000, 11000);
    }

    void Aggro(Unit* pAggro)
    {
        std::list<Creature*> lAssistList;
        GetCreatureListWithEntryInGrid(lAssistList, m_creature, m_creature->GetEntry(), 30.0f);

        if (lAssistList.empty())
            return;

        for(std::list<Creature*>::iterator iter = lAssistList.begin(); iter != lAssistList.end(); ++iter)
        {

            if ((*iter)->GetObjectGuid() == m_creature->GetObjectGuid())
                continue;

            m_GladiatorFriend = (*iter)->GetObjectGuid();

            (*iter)->AI()->AttackStart(pAggro);
        }
    }

    void JustDied(Unit* /*pKiller*/)
    {
        Creature *pBuddy = m_creature->GetMap()->GetCreature(m_GladiatorFriend);

        if(pBuddy && pBuddy->isAlive())
        {
            pBuddy->AI()->DoCastSpellIfCan(pBuddy, SPELL_VENGEANCE, CastFlags::CAST_TRIGGERED);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (m_uiTrashTimer <= uiDiff)
        {
			if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_TRASH, true);
			m_uiTrashTimer = urand(8000, 12000);
        } 
		else
			m_uiTrashTimer -= uiDiff;

		if (m_uiUpperCutTimer <= uiDiff)
        {
			if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_UPPERCUT, true);
			m_uiUpperCutTimer = urand(7000, 11000);
        } 
		else
			m_uiUpperCutTimer -= uiDiff;

		if (m_uiVengeanceTimer <= uiDiff)
        {
            DoCast(m_creature, SPELL_VENGEANCE, true);
			m_uiVengeanceTimer = urand(30000, 40000);
        } 
		else
			m_uiVengeanceTimer -= uiDiff;



        DoMeleeAttackIfReady(); 
    }
};

CreatureAI* GetAI_mob_qiraji_gladiator(Creature* pCreature)
{
    return new mob_qiraji_gladiator(pCreature);
}

/*######
## mob_hivezara_stinger
######*/

enum eHiveZaraStinger
{
    SPELL_STINGER_CHARGE             = 25190,
    SPELL_STINGER_EMPOWERED_CHARGE   = 25191,
    SPELL_HIVEZARA_CATALYST          = 25187,
};

struct MANGOS_DLL_DECL mob_hivezara_stinger : public ScriptedAI
{
    mob_hivezara_stinger(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

	uint32 m_uiChargeTimer;

    void Reset() 
    {		
		m_uiChargeTimer = 5000;
    }

    void ChargeRandomTarget()
    {
        const ThreatList& threatList = m_creature->getThreatManager().getThreatList();
        std::vector<Unit*> pEligibleTargets;

        pEligibleTargets.clear();

        if(!threatList.empty())
        {
            for (HostileReference *currentReference : threatList)
            {
                Unit *target = currentReference->getTarget();
                if (target && target->isAlive() && target->GetDistance(m_creature) <= 25.0f && target->GetDistance(m_creature) >= 5.0f)
                    pEligibleTargets.push_back(target);
            }

            if(!pEligibleTargets.empty())
            {
                std::random_shuffle(pEligibleTargets.begin(), pEligibleTargets.end());
                Unit *target = pEligibleTargets.front();
                if (target && target->isAlive())
                {
                    if(target->HasAura(SPELL_HIVEZARA_CATALYST))
                        DoCast(target, SPELL_STINGER_EMPOWERED_CHARGE, true);
                    else
                        DoCast(target, SPELL_STINGER_CHARGE, true);
                }
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (m_uiChargeTimer <= uiDiff)
        {
			ChargeRandomTarget();
			m_uiChargeTimer = 5000;
        } 
		else
			m_uiChargeTimer -= uiDiff;

        DoMeleeAttackIfReady(); 
    }
};

CreatureAI* GetAI_mob_hivezara_stinger(Creature* pCreature)
{
    return new mob_hivezara_stinger(pCreature);
}


void AddSC_ruins_of_ahnqiraj()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "mob_anubisath_guardian";
    pNewscript->GetAI = &GetAI_mob_anubisath_guardian;
    pNewscript->RegisterSelf();
    
    pNewscript = new Script;
    pNewscript->Name = "mob_flesh_hunter";
    pNewscript->GetAI = &GetAI_mob_flesh_hunter;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "mob_obsidian_destroyer";
    pNewscript->GetAI = &GetAI_mob_obsidian_destroyer;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "mob_qiraji_swarmguard";
    pNewscript->GetAI = &GetAI_mob_qiraji_swarmguard;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "mob_qiraji_gladiator";
    pNewscript->GetAI = &GetAI_mob_qiraji_gladiator;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "mob_hivezara_stinger";
    pNewscript->GetAI = &GetAI_mob_hivezara_stinger;
    pNewscript->RegisterSelf();
}
