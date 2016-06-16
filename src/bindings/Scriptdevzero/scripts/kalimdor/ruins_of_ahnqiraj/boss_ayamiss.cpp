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
SDName: Boss_Ayamiss
SD%Complete: 90
SDComment:
SDCategory: Ruins of Ahn'Qiraj
EndScriptData */

#include "precompiled.h"
#include "ruins_of_ahnqiraj.h"

enum eAyamiss
{
    //EMOTE_FRENZY                = -1000002,

    // Ayamiss
    SPELL_FRENZY                = 8269,
    SPELL_LASH                  = 25852,
    SPELL_PARALYZE              = 25725,
    SPELL_POISON_STINGER        = 25748,
    SPELL_STINGER_SPRAY         = 25749,
    SPELL_TRASH                 = 3391,

    // Hive'Zara Larva
    SPELL_FEED                  = 25721
};

static Loc Larva[]=
{
    {-9695.0f, 1585.0f, 25.0f, 0, 0},
    {-9650.0f, 1536.0f, 21.44f, 0, 0}
};

static Loc Swarmers[]=
{
    {-9769.0f, 1455.0f, 60.0f, 0, 0}
};

static const uint32 aSwarmType[] = {NPC_HIVEZARA_SWARMER,NPC_HIVEZARA_SWARMER,NPC_HIVEZARA_SWARMER_1};

struct MANGOS_DLL_DECL boss_ayamissAI : public ScriptedAI
{
    boss_ayamissAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((instance_ruins_of_ahnqiraj*)pCreature->GetInstanceData());
        Reset();
    }

    instance_ruins_of_ahnqiraj* m_pInstance;

    bool m_bPhaseTwo;
	bool m_bEnrage;

    uint32 m_uiPoisonStingerTimer;
    uint32 m_uiStingerSprayTimer;
    uint32 m_uiParalyzeTimer;
    uint32 m_uiAfterTPParaTimer;
    uint32 m_uiLashTimer;
    uint32 m_uiTrashTimer;
    uint32 m_uiFrenzyTimer;
    uint32 m_uiSwarmerTimer;
	uint32 m_uiCallSwarmersTimer;

    ObjectGuid m_uiLarvaTargetGUID;
	GUIDList m_uiSwarmerGUID;

    ObjectGuid m_TPTarget;

    void Reset()
    {
        m_bPhaseTwo = false;
		m_bEnrage = false;

        m_uiPoisonStingerTimer = 5000;
        m_uiStingerSprayTimer = urand(10000,20000);
        m_uiParalyzeTimer = urand(35000,45000);
        m_uiAfterTPParaTimer = 200;
        m_TPTarget = ObjectGuid();
        m_uiLashTimer = urand(7000,9000);
        m_uiTrashTimer = urand(6000,8000);
        m_uiFrenzyTimer = 0;
        m_uiSwarmerTimer = urand(2000,5000);
		m_uiCallSwarmersTimer = urand(70000,76000);
		DespawnSwarmers();			// despawn before we clear the guid
		m_uiLarvaTargetGUID.Clear();
		m_uiSwarmerGUID.clear();

        SetCombatMovement(false);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AYAMISS, NOT_STARTED);		
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AYAMISS, IN_PROGRESS);

        m_creature->SendMonsterMove(-9698.37f,1535.55f,33.44f, SPLINETYPE_NORMAL, SPLINEFLAG_FLYING, 3000);
    }

	void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)		// only summon larva if a player got hit by paralyze
    {
        if (pSpell->Id == SPELL_PARALYZE && pTarget->GetTypeId() == TYPEID_PLAYER)
		{
			m_uiLarvaTargetGUID = pTarget->GetObjectGuid();	
			uint32 i = urand(0,1);
            m_creature->SummonCreature(NPC_HIVEZARA_LARVA, Larva[i].x, Larva[i].y, Larva[i].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 20000);			
		}
	}

	void SpellHit(Unit* pCaster, SpellEntry const* pSpell)							// Only do emotes when the spell actually hits in case something goes wrong
    {
        if (pSpell->Id == SPELL_FRENZY)
			m_creature->GenericTextEmote("Ayamiss the Hunter becomes enraged!", NULL, false);
	}

	void DespawnSwarmers()
	{
		for (GUIDList::iterator itr = m_uiSwarmerGUID.begin(); itr != m_uiSwarmerGUID.end(); itr++)
		{
			if (Creature* pSwarmer = m_creature->GetMap()->GetCreature(*itr))
				if (pSwarmer->isAlive())
					pSwarmer->ForcedDespawn(2000);	
		}
	}

	void CallSwarmers()
	{
		Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
		for (GUIDList::iterator itr = m_uiSwarmerGUID.begin(); itr != m_uiSwarmerGUID.end(); itr++)
			if (Creature* pSwarmer = m_creature->GetMap()->GetCreature(*itr))
				if (pSwarmer->isAlive() && pTarget)
					pSwarmer->AI()->AttackStart(pTarget);
				
	}

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AYAMISS, DONE);
		DespawnSwarmers();
    }

    void JustSummoned(Creature* pSummoned)
    {
        switch(pSummoned->GetEntry())
        {
            case NPC_HIVEZARA_LARVA:
                /*if (Unit* pLarvaTarget = m_creature->GetMap()->GetUnit(m_uiLarvaTargetGUID))
                    pSummoned->AI()->AttackStart(pLarvaTarget);*/
				pSummoned->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
				pSummoned->GetMotionMaster()->MovePoint(1,-9701.27f, 1532.64f, 21.44f,true);
                break;
            case NPC_HIVEZARA_SWARMER:
			case NPC_HIVEZARA_SWARMER_1:
				m_uiSwarmerGUID.push_back(pSummoned->GetObjectGuid());
                break;
        }
		pSummoned->SetRespawnDelay(-10);			// make sure they won't respawn
    }

	void SummonedMovementInform(Creature* pSummoned, uint32 /*uiMotionType*/, uint32 uiPointId)
    {
		if (uiPointId == 1 && pSummoned->GetEntry() == NPC_HIVEZARA_LARVA)
			pSummoned->GetMotionMaster()->MovePoint(2,-9716.18f, 1517.72f, 27.4677f,true);

		if(uiPointId == 2 && pSummoned->GetEntry() == NPC_HIVEZARA_LARVA)
		{
			// Cast feed on target
			if (Unit* pLarvaTarget = m_creature->GetMap()->GetUnit(m_uiLarvaTargetGUID))
				if (pLarvaTarget && pSummoned->IsWithinDistInMap(pLarvaTarget, ATTACK_DISTANCE) && pLarvaTarget->HasAura(SPELL_PARALYZE))
					pSummoned->CastSpell(pLarvaTarget, SPELL_FEED, true); 
		}
    }


    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		// If Ayamiss is 70% let's take him down :-) Phase 2 on the ground
        if (!m_bPhaseTwo && HealthBelowPct(70))
        {
            SetCombatMovement(true);
            m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
            m_creature->AttackerStateUpdate(m_creature->getVictim(), BASE_ATTACK, true);
			DoResetThreat();
            m_bPhaseTwo = true;
        }

		// Summon Swarmers
        if (m_uiSwarmerTimer <= uiDiff)		// every 3.5 sec spawn a swarmer
        {
			uint32 i = urand(0,2);
            m_creature->SummonCreature(aSwarmType[i], Swarmers[0].x+irand(-10,10), Swarmers[0].y+irand(-10,10), Swarmers[0].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 600000);//let them be up for 10min or when boss dies/evade
            m_uiSwarmerTimer = 3500;
        }
        else
            m_uiSwarmerTimer -= uiDiff;

		// Make the swarmers attack
        if (m_uiCallSwarmersTimer <= uiDiff)
        {
			CallSwarmers();
            m_uiCallSwarmersTimer = urand(70000,76000);
        }
        else
            m_uiCallSwarmersTimer -= uiDiff;

		// Stinger Spray
        if (m_uiStingerSprayTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_STINGER_SPRAY);
            m_uiStingerSprayTimer = urand(25000,30000);
        }
        else
            m_uiStingerSprayTimer -= uiDiff;

        // Paralyze and Summon Larva
        if (m_uiParalyzeTimer <= uiDiff)
		{
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                pTarget->NearTeleportTo(-9716.87f, 1517.79f, 27.467f, 2.73f);

                m_TPTarget = pTarget->GetObjectGuid();
            }

            m_uiParalyzeTimer = urand(15000,20000);
		}
        else
            m_uiParalyzeTimer -= uiDiff;

        if (m_TPTarget != ObjectGuid())
        {
            if (m_uiAfterTPParaTimer <= uiDiff)
            {
                if (Player* pTPTarget = m_creature->GetMap()->GetPlayer(m_TPTarget))
                    m_creature->CastSpell(pTPTarget, SPELL_PARALYZE, true);

                m_TPTarget = ObjectGuid();
                m_uiAfterTPParaTimer = 200;
            }
            else
                m_uiAfterTPParaTimer -= uiDiff;
        }

        if (!m_bPhaseTwo)
        {
            // Poison Stinger
            if (m_uiPoisonStingerTimer <= uiDiff)
            {
                m_creature->CastSpell(m_creature->getVictim(), SPELL_POISON_STINGER, false);
                m_uiPoisonStingerTimer = 4000;
            }
            else
                m_uiPoisonStingerTimer -= uiDiff;
        }
        else
        {
            // Frenzy spell
            if (HealthBelowPct(20) && !m_bEnrage)
			{
                DoCastSpellIfCan(m_creature, SPELL_FRENZY, CAST_AURA_NOT_PRESENT);                
				m_bEnrage = true;
			}
            // Lash
            if (m_uiLashTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_LASH);
                m_uiLashTimer = urand(7000,9000);
            }
            else
                m_uiLashTimer -= uiDiff;
			          
            // Trash
            if (m_uiTrashTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_TRASH);
                m_uiTrashTimer = urand(6000,8000);
            }
            else
                m_uiTrashTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_ayamiss(Creature* pCreature)
{
    return new boss_ayamissAI(pCreature);
}

struct MANGOS_DLL_DECL mob_hivezara_larvaAI : public ScriptedAI
{
    mob_hivezara_larvaAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
		m_pInstance = ((instance_ruins_of_ahnqiraj*)pCreature->GetInstanceData());
        Reset();
    }
	instance_ruins_of_ahnqiraj* m_pInstance;

    void Reset() 
    {
    }

    void Aggro(Unit* pWho)
    {
		// don't attack anything during the Ayamiss encounter
        if (m_pInstance)
        {
            if (m_pInstance->GetData(TYPE_AYAMISS) == IN_PROGRESS)
            {
                return;
            }
        }

        ScriptedAI::AttackStart(pWho);
    }

	void MoveInLineOfSight(Unit* pWho)
    {
		// don't attack anything during the Ayamiss encounter
		if (m_pInstance)
		{
			if (m_pInstance->GetData(TYPE_AYAMISS) == IN_PROGRESS)
			{
				return;
			}
		}
		ScriptedAI::MoveInLineOfSight(pWho);
    }

	void AttackStart(Unit* pVictim)
    {
        // don't attack anything during the Ayamiss encounter
		if (m_pInstance)
		{
			if (m_pInstance->GetData(TYPE_AYAMISS) == IN_PROGRESS)
			{
				return;
			}
		}
		ScriptedAI::AttackStart(pVictim);
    }

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_hivezara_larva(Creature* pCreature)
{
    return new mob_hivezara_larvaAI(pCreature);
}

struct MANGOS_DLL_DECL mob_hivezara_swarmerAI : public ScriptedAI
{
    mob_hivezara_swarmerAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{
		Reset();
	}

	void Reset()
	{
	}

	void MoveInLineOfSight(Unit* /*pWho*/)
    {
        // Must to be empty to ignore aggro
    }

	void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_mob_hivezara_swarmer(Creature* pCreature)
{
    return new mob_hivezara_swarmerAI(pCreature);
}

void AddSC_boss_ayamiss()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_ayamiss";
    pNewscript->GetAI = &GetAI_boss_ayamiss;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_hivezara_larva";
    pNewscript->GetAI = &GetAI_mob_hivezara_larva;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "mob_hivezara_swarmer";
    pNewscript->GetAI = &GetAI_mob_hivezara_swarmer;
    pNewscript->RegisterSelf();
}
