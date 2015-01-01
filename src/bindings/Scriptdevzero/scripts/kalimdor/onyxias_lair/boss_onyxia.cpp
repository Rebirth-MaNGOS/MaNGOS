/* Copyright (C) 2010 Zataron
	-->	based on the script of scriptdevzero modified by Marvin Cordes 

ScriptData
SDName: Boss_Onyxia
SD%Complete: 95
SDComment: Problems: - Deep Breath emote doesnt work
                     - No flying animation while not moving in the air
					 - No visual effect for lava eruptions in phase 3
SDCategory: Onyxia's Lair
EndScriptData */

#include "precompiled.h"
#include "onyxias_lair.h"
#include "Spell.h"
#include "CreatureEventAI.h"

enum
{
    SAY_AGGRO                   = -1249000,
    SAY_KILL                    = -1249001,
    SAY_PHASE_2_TRANS           = -1249002,
    SAY_PHASE_3_TRANS           = -1249003,
    EMOTE_BREATH                = -1249004,

    SPELL_WINGBUFFET            = 18500,
    SPELL_FLAMEBREATH           = 18435,
    SPELL_CLEAVE                = 19983,
    SPELL_TAILSWEEP             = 15847,
    SPELL_KNOCK_AWAY            = 19633,

    SPELL_ENGULFINGFLAMES       = 20019,
    SPELL_DEEPBREATH            = 23461,
    SPELL_FIREBALL              = 18392,

    //Not much choise about these. We have to make own defintion on the direction/start-end point
    SPELL_BREATH_NORTH_TO_SOUTH = 17086,                  // 20x in "array"
    SPELL_BREATH_SOUTH_TO_NORTH = 18351,                  // 11x in "array"
    SPELL_BREATH_SE_TO_NW       = 18564,                    // 12x in "array"
    SPELL_BREATH_NW_TO_SE       = 18584,                    // 12x in "array"
    SPELL_BREATH_SW_TO_NE       = 18596,                    // 12x in "array"
    SPELL_BREATH_NE_TO_SW       = 18617,                    // 12x in "array"

    SPELL_BELLOWINGROAR         = 18431,
    SPELL_HEATED_GROUND         = 22191,
	GO_LAVA_FISSURE             = 178036,
	LAVA_RANGE                  = 8,

    SPELL_SUMMONWHELP           = 17646,
    //NPC_ONYXIA_WARDER           = 12129,                    // Already defined in onyxias_lair.h
    //NPC_ONYXIA_WHELP            = 11262,                    // Already defined in onyxias_lair.h
    MAX_WHELP                   = 20,
    MAX_WARDER                  = 2,

    PHASE_START                 = 1,
    PHASE_BREATH                = 2,
    PHASE_END                   = 3
};

struct sOnyxMove
{
    uint32 uiLocId;
    uint32 uiSpellId;
    float fX, fY, fZ;
};

static sOnyxMove aMoveData[8]=
{
    {0, SPELL_BREATH_SOUTH_TO_NORTH, -68.834236f, -215.036163f, -84.018875f},//south
    {1, SPELL_BREATH_SW_TO_NE,       -58.2509f, -189.020f, -85.7893f},//south-west
	{2, SPELL_BREATH_NW_TO_SE,         6.8951f, -180.246f, -85.623f},//north-west
    {3, SPELL_BREATH_NORTH_TO_SOUTH,  20.730539f, -215.237610f, -85.254387f},//north
    {4, SPELL_BREATH_NE_TO_SW,        12.120926f, -243.439407f, -85.874260f},//north-east
    {5, SPELL_BREATH_SE_TO_NW,       -56.559654f, -241.223923f, -85.423607f},//south-east
};

static float WarderSpawnLocation[2][3]=
{
    {-126.76f, -217.33f, -71.07f},
    {-125.43f, -211.67f, -71.26f},
};

static float afSpawnLocations[2][3]=
{
    {-107.582f, -222.27f, -94.757f},
    {-107.845f, -204.2f,  -94.139f},
};

struct MANGOS_DLL_DECL boss_onyxiaAI : public ScriptedAI
{
    boss_onyxiaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
		m_baseRunSpeed = m_creature->GetSpeedRate(MOVE_RUN);
        m_pInstance = (instance_onyxias_lair*)pCreature->GetInstanceData();
        Reset();
    }

    instance_onyxias_lair* m_pInstance;

    uint32 m_uiPhase;

    uint32 m_uiFlameBreathTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiTailSweepTimer;
    uint32 m_uiWingBuffetTimer;

    int32 m_uiMovePoint;
    uint32 m_uiMovementTimer;
    sOnyxMove* m_pPointData;

    uint32 m_uiSummonWhelpsTimer;
    uint32 m_uiBellowingRoarTimer;
    uint32 m_uiWhelpTimer;
	uint32 m_uiLavaTimer;
	uint32 m_deepBreathCastingTime;
	uint32 m_landingDelayTimer;
	uint32 m_nextHoverPulse;
	GUIDList m_uiWarders;

    uint32 m_uiSummonPlayerTimer;

    uint8 m_uiSummonCount;
	uint8 MaxWhelps;
	uint32 m_uiRandom;
    bool m_bIsSummoningWhelps;
    bool m_bIsSummoningWarders;
    bool m_bEntranceWardersSpawned;
	bool m_inMotion;

	float m_baseRunSpeed;

	//Unit* mTank;
    ObjectGuid m_uiTankGUID;
	bool FearMode;
	bool LavaMode;

    void Reset()
    {
        if (!IsCombatMovement())
		{
            SetCombatMovement(true);
		}

        m_uiPhase = PHASE_START;
        m_creature->SetSplineFlags(SPLINEFLAG_NONE);
		m_creature->SetHover(false);

		DespawnSummons();
        m_uiFlameBreathTimer = urand(10000, 15000);
        m_uiTailSweepTimer = urand(5000, 8000);
        m_uiCleaveTimer = urand(2000, 5000);
        m_uiWingBuffetTimer = urand(10000, 20000);

        m_uiMovePoint = urand(0, 5);
        m_uiMovementTimer = 20000;
        m_pPointData = &aMoveData[m_uiMovePoint];

        m_uiSummonWhelpsTimer = 0;
        m_uiBellowingRoarTimer = 0;
        m_uiWhelpTimer = 0;
		m_uiLavaTimer = 6000;
		m_deepBreathCastingTime=0;
		m_landingDelayTimer=0;
		m_nextHoverPulse=250;

        m_uiSummonCount = 0;
		MaxWhelps = MAX_WHELP;
        m_bIsSummoningWhelps = false;
        m_bIsSummoningWarders = true;
        m_bEntranceWardersSpawned = false;

		m_uiRandom = 0;
		//mTank = 0;
		m_uiTankGUID.Clear();
		FearMode = false;
		LavaMode = false;

        m_creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_FIRE, true);
		m_inMotion = false;
		m_creature->SetSpeedRate(MOVE_RUN,m_baseRunSpeed,true);
		m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, 0/* | UNIT_BYTE1_FLAG_UNK_2*/);

		m_creature->GetMotionMaster()->MoveTargetedHome();
	}

    void Aggro(Unit* /*pAttacker*/)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        m_creature->SetInCombatWithZone();

        if (!m_bEntranceWardersSpawned)
        {
            m_bEntranceWardersSpawned = true;
            m_creature->SummonCreature(NPC_ONYXIA_WARDER, WarderSpawnLocation[0][0], WarderSpawnLocation[0][1], WarderSpawnLocation[0][2], 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 2 * HOUR * IN_MILLISECONDS);
            m_creature->SummonCreature(NPC_ONYXIA_WARDER, WarderSpawnLocation[1][0], WarderSpawnLocation[1][1], WarderSpawnLocation[1][2], 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 2 * HOUR * IN_MILLISECONDS);
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ONYXIA, IN_PROGRESS);

		m_creature->SetStandState(UNIT_STAND_STATE_STAND);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ONYXIA, FAIL);

		m_creature->SetStandState(UNIT_STAND_STATE_SLEEP);
   }

	void DespawnSummons()
	{
		if (m_uiWarders.empty())
			return;

		for (GUIDList::iterator itr = m_uiWarders.begin(); itr != m_uiWarders.end(); itr = m_uiWarders.begin())
		{
			if (Creature* warder = m_creature->GetMap()->GetCreature(*itr))
				warder->RemoveFromWorld();
			
			m_uiWarders.erase(itr);
		}
	}

    void JustSummoned(Creature* pSummoned)
    {
		if (pSummoned->GetEntry() == NPC_ONYXIA_WARDER)
		{
			m_uiWarders.push_back(pSummoned->GetObjectGuid());
			return;
		}

        if (pSummoned->GetEntry() == NPC_ONYXIA_WHELP || pSummoned->GetEntry() == NPC_ONYXIA_WHELP_WEST)
		{
			++m_uiSummonCount;
			//Hack the whelp to trigger the summon event on itself so it can go to phase 2
			CreatureEventAI *ai = dynamic_cast<CreatureEventAI*>(pSummoned->AI());

			if (ai != NULL)
				ai->JustSummoned(pSummoned);
		}
    }

    void KilledUnit(Unit* /*pVictim*/)
    {
        DoScriptText(SAY_KILL, m_creature);
    }

    void JustDied(Unit* /*pKiller*/)
    {
		DespawnSummons();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ONYXIA, DONE);
    }

	void MovementInform(uint32 /*motionType*/, uint32 pointId)
	{
		m_creature->SetSpeedRate(MOVE_RUN,m_baseRunSpeed,true);
		if (pointId <= 8)
		{
 			m_inMotion = false;
			if(Unit* victim = m_creature->getVictim())
                m_creature->SendMeleeAttackStop(victim);

			if (m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() == CHASE_MOTION_TYPE)
			{
				m_creature->GetMotionMaster()->Clear(false);
				m_creature->GetMotionMaster()->MoveIdle();
				m_creature->StopMoving();
			}

			if (pointId == 8)
			{
				// sort of a hack, it is unclear how this really work but the values appear to be valid
				m_creature->SetHover(true);
				m_creature->SetSplineFlags(SplineFlags(SPLINEFLAG_FLYING));
				m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND/* | UNIT_BYTE1_FLAG_UNK_2)*/);

				m_creature->HandleEmote(EMOTE_ONESHOT_LIFTOFF);
			}
		} else if (pointId == 9)
		{
			m_creature->GetMotionMaster()->Clear(false);
			m_creature->GetMotionMaster()->MoveIdle();
			m_creature->HandleEmote(EMOTE_ONESHOT_LAND);
			m_creature->SetHover(false);
			m_creature->SetSplineFlags(SPLINEFLAG_NONE);
			m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, 0/* | UNIT_BYTE1_FLAG_UNK_2*/);

			m_landingDelayTimer = 3000;
		}

	}

    void OverrideAttackDistance(float &attackDistance, Unit* /*pWho*/)
    {
        attackDistance = 53;
    }

    void UpdateAI(const uint32 uiDiff)
    {
		if ( m_uiPhase != PHASE_BREATH &&  (!m_creature->SelectHostileTarget() || !m_creature->getVictim()))
		{
			return;
		} else if (m_uiPhase == PHASE_BREATH)
		{
            //Reset if there's nothing in the threat list or if the only thing in the threat list is a creature or is null
            bool doReset = m_creature->getThreatManager().isThreatListEmpty() || (m_creature->getThreatManager().getThreatList().size() == 1 && (m_creature->getThreatManager().getHostileTarget() == NULL || m_creature->getThreatManager().getHostileTarget()->GetObjectGuid().IsCreature()));
			if (!doReset)
			{
				if (!m_creature->hasUnitState(UNIT_STAT_STUNNED | UNIT_STAT_DIED))
				{
					Creature *trigger = m_pInstance->GetSingleCreatureFromStorage(NPC_ONYXIA_TRIGGER);
 					m_creature->SetInFront(trigger);
					AttackStart(trigger);
				}
			} else
			{
				// enter in evade mode in other case
				ResetToHome();
				m_pInstance->OnCreatureEvade((Creature*)this);
			}
		}

        // Summon Player (maybe crashing server)
        /*if (m_uiSummonPlayerTimer <= uiDiff)
        {
            if (m_creature->getVictim()->GetTypeId() == TYPEID_PLAYER && !m_creature->getVictim()->IsWithinDist(m_creature, SUMMON_PLAYER_TRIGGER_DISTANCE))
            {
                float fX, fY, fZ, fO;
                m_creature->GetPosition(fX, fY, fZ);
                fO = m_creature->GetOrientation();
                ((Player*)m_creature->getVictim())->TeleportTo(m_creature->GetMapId(), fX, fY, fZ, -fO);
            }
            m_uiSummonPlayerTimer = 2000;
        }
        else
            m_uiSummonPlayerTimer -= uiDiff;*/

        if (m_uiPhase == PHASE_START || m_uiPhase == PHASE_END)    //Phase 1 or 3
        {
			// Summon Warders
            if (m_bIsSummoningWarders == true)
            {
                for (uint8 itr = 0; itr < MAX_WARDER; ++itr)
                    m_creature->SummonCreature(NPC_ONYXIA_WARDER, WarderSpawnLocation[0][0], WarderSpawnLocation[0][1], WarderSpawnLocation[0][2], 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 600000);
                m_bIsSummoningWarders = false;
            }

            if (m_uiPhase == PHASE_START && m_creature->GetHealthPercent() < 65.0f)   //Switch to Phase 2
            {
 				m_creature->InterruptNonMeleeSpells(true);
                m_uiPhase = PHASE_BREATH;
                m_bIsSummoningWhelps = true;

                if(Unit* victim = m_creature->getVictim())
                    m_creature->SendMeleeAttackStop(victim);

				if (m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() == CHASE_MOTION_TYPE)
				{
					m_creature->GetMotionMaster()->Clear(false);
					m_creature->GetMotionMaster()->MoveIdle();
					m_creature->StopMoving();
				}

				this->SetCombatMovement(false);
                DoScriptText(SAY_PHASE_2_TRANS, m_creature);

                if (m_pPointData)
				{
                    m_creature->GetMotionMaster()->MovePoint(8, aMoveData->fX, aMoveData->fY, aMoveData->fZ);
					m_uiMovePoint = 0;
					m_inMotion = true;
				}

                return;
            }
			   
			if (m_inMotion)
			{
				if (m_landingDelayTimer > 0)
				{
					if (m_landingDelayTimer <= uiDiff)
					{
						m_landingDelayTimer = 0;
						m_inMotion = false;
						if(Unit* victim = m_creature->getVictim())
							m_creature->SendMeleeAttackStart(victim);

						if (m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE)
						{
							m_creature->GetMotionMaster()->Clear(false);
							m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
						}

						SetCombatMovement(true);

						//Put everything on cd so we're guaranteed to roar when we hit the ground
						m_uiFlameBreathTimer = urand(10000, 15000);
						m_uiTailSweepTimer = urand(5000, 8000);
						m_uiWingBuffetTimer = urand(15000, 20000);
						m_uiCleaveTimer = urand(2000, 5000);

					} else
						m_landingDelayTimer -= uiDiff;
				}
				return;
			}

			// Flame Breath
            if (m_uiFlameBreathTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FLAMEBREATH) == CAST_OK)
					m_uiFlameBreathTimer = urand(10000, 15000);
				else
					m_uiFlameBreathTimer = 0;
            }
            else
                m_uiFlameBreathTimer -= uiDiff;

			// Tail Sweep
            if (m_uiTailSweepTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_TAILSWEEP) == CAST_OK)
					m_uiTailSweepTimer = urand(5000, 8000);
				else
					m_uiTailSweepTimer = 0;
            }
            else
                m_uiTailSweepTimer -= uiDiff;

			// Wing Buffet
            if (m_uiWingBuffetTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_WINGBUFFET) == CAST_OK)
				{
					m_creature->getThreatManager().modifyThreatPercent(m_creature->getVictim(), -25);
					m_uiWingBuffetTimer = urand(15000, 20000);
				} else
					m_uiWingBuffetTimer = 0;
            }
            else
                m_uiWingBuffetTimer -= uiDiff;

			// Cleave
            if (m_uiCleaveTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE) == CAST_OK)
					m_uiCleaveTimer = urand(2000, 5000);
				else
					m_uiCleaveTimer = 0;
            }
            else
                m_uiCleaveTimer -= uiDiff;

			if (m_uiPhase == PHASE_END)   //Phase 3
            {
				// Bellowing Roar 18431
                if (m_uiBellowingRoarTimer <= uiDiff)
                {
					if (!m_creature->IsNonMeleeSpellCasted(true))
					{
						m_creature->CastSpell(NULL,SPELL_BELLOWINGROAR,false);

						if (Unit* mTank = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
							m_uiTankGUID = mTank->GetObjectGuid();

						m_pInstance->ShuffleFissures();

						LavaMode = true;
						m_uiLavaTimer = 2500;
						m_uiBellowingRoarTimer = urand(15000,25000);
					} else
						m_uiBellowingRoarTimer = 0;
                }
                else
                    m_uiBellowingRoarTimer -= uiDiff;

				// Make tank lose aggro when feared
                if (!FearMode && m_uiTankGUID)
                {
                    Unit* mTank = m_creature->GetMap()->GetUnit(m_uiTankGUID);
                    if (mTank && mTank->HasAura(SPELL_BELLOWINGROAR))
				    {
					    FearMode = true;
					    m_creature->getThreatManager().modifyThreatPercent(mTank, -99);
				    }
                }

                // Make tank regain aggro after fear
                if (FearMode && m_uiTankGUID)
                {
                    Unit* mTank = m_creature->GetMap()->GetUnit(m_uiTankGUID);
                    if (mTank && !mTank->HasAura(SPELL_BELLOWINGROAR))
				    {
					    m_creature->getThreatManager().modifyThreatPercent(mTank, 11000);
					    FearMode = false;					
				    }
                }

                // Lava Eruptions after fear ran out
                if (LavaMode && m_uiLavaTimer <= uiDiff)
                {
					LavaMode = m_pInstance->FissureSurge(m_creature);
                    m_uiLavaTimer = 500;
                }
                else
                    m_uiLavaTimer -= uiDiff;
            }

            DoMeleeAttackIfReady();
        }
        else  //Phase 2
        {
            if (m_creature->GetHealthPercent() < 40.0f)  //Switch to Phase 3 
            {
				m_creature->InterruptNonMeleeSpells(true);
                m_uiPhase = PHASE_END;
                DoScriptText(SAY_PHASE_3_TRANS, m_creature);

				m_creature->GetMotionMaster()->MovePoint(9, aMoveData->fX, aMoveData->fY, aMoveData->fZ);
				m_uiMovePoint = 0;
				m_inMotion = true;

                return;
            }

			// Summon Whelps: First Wave spawns 10 whelps from each side, every other wave spawns 5 from each side.
            if (m_bIsSummoningWhelps)
            {
                if (m_uiSummonCount < MaxWhelps)
                {
                    if (m_uiWhelpTimer < uiDiff)
                    {
                        Creature *eastWhelp = m_creature->SummonCreature(NPC_ONYXIA_WHELP, afSpawnLocations[0][0], afSpawnLocations[0][1], afSpawnLocations[0][2], 0.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000);
						eastWhelp->SetInCombatWithZone();					

                        Creature *westWhelp = m_creature->SummonCreature(NPC_ONYXIA_WHELP, afSpawnLocations[1][0], afSpawnLocations[1][1], afSpawnLocations[1][2], 0.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000);
						westWhelp->SetInCombatWithZone();					
                        m_uiWhelpTimer = 2000;
                    }
                    else
                        m_uiWhelpTimer -= uiDiff;
                }
                else
                {
                    MaxWhelps = 10;
                    m_bIsSummoningWhelps = false;
                    m_uiSummonCount = 0;
                    m_uiSummonWhelpsTimer = urand(85000,95000);
                }
            }
            else
            {
                if (m_uiSummonWhelpsTimer < uiDiff)
                    m_bIsSummoningWhelps = true;
                else
                    m_uiSummonWhelpsTimer -= uiDiff;
            }

			if (m_inMotion)
			{
				return;
			}

			if (m_deepBreathCastingTime > 0)
			{
				if (m_deepBreathCastingTime <= uiDiff)
				{
					m_creature->SetSpeedRate(MOVE_RUN,3,true);
					m_pPointData = &aMoveData[m_uiMovePoint];
                    m_creature->GetMotionMaster()->MovePoint(m_pPointData->uiLocId, m_pPointData->fX, m_pPointData->fY, m_pPointData->fZ);
					m_inMotion = true;
					m_deepBreathCastingTime = 0;
				} else
					m_deepBreathCastingTime -= uiDiff;

				return;
			}

            // Move to next Position
            if (m_uiMovementTimer < uiDiff)
            {
                m_uiRandom = urand(-1,1);

                m_uiMovementTimer = 25000;
				uint32 newMovePoint = m_uiMovePoint;

                // Deep Breath before Onyxia moves across the room
                if (m_uiRandom == 0)
                {
                    //Deep Breath Emote doesnt work!
					m_creature->GenericTextEmote("Onyxia takes a deep breath...", NULL, true);

                    DoCastSpellIfCan(m_creature, m_pPointData->uiSpellId, CAST_INTERRUPT_PREVIOUS/* | CAST_FORCE_CAST | CAST_TRIGGERED*/);
                    m_deepBreathCastingTime = 4500;
                    newMovePoint += 3;
                }
                else 
                    newMovePoint = m_uiMovePoint + m_uiRandom;

                if (newMovePoint > 5)
                    newMovePoint -= 6;

                if (newMovePoint > 5)
                {
                    error_log("SD0: Onyxia: m_uiMovePoint invalid, possible crash! Reseted to 0");
                    newMovePoint = 0;
                }

                m_pPointData = &aMoveData[newMovePoint];

				if (!m_pPointData)
					return;

                if ((int) newMovePoint != m_uiMovePoint && m_pPointData)
				{
					m_uiMovePoint = newMovePoint;

					//Only move now if we're not deep breathing- we'll move after the wind up if we are
					if (m_deepBreathCastingTime == 0)
					{
						m_creature->InterruptNonMeleeSpells(true);
						m_creature->GetMotionMaster()->MovePoint(m_pPointData->uiLocId, m_pPointData->fX, m_pPointData->fY, m_pPointData->fZ);
						m_inMotion = true;
					}
				}
				return;
            }
            else
                m_uiMovementTimer -= uiDiff;

            if (m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != POINT_MOTION_TYPE)
            {
                Unit* pTarget = m_creature->getThreatManager().getHostileTarget();
                if (pTarget != NULL && DoCastSpellIfCan(pTarget, SPELL_FIREBALL) == CAST_OK)
				{
                    m_creature->getThreatManager().modifyThreatPercent(pTarget,-100);
                }
            }
        }
    }
};

CreatureAI* GetAI_boss_onyxiaAI(Creature* pCreature)
{
    return new boss_onyxiaAI(pCreature);
}

void AddSC_boss_onyxia()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_onyxia";
    pNewScript->GetAI = &GetAI_boss_onyxiaAI;
    pNewScript->RegisterSelf();
}

/*
-- SPELL_BREATH_EAST_TO_WEST
DELETE FROM spell_target_position WHERE id IN (18576, 18578, 18579, 18580, 18581, 18582, 18583);
INSERT INTO spell_target_position VALUES (18576, 249, -37.743851, -243.667923, -88.217651, 1.416);
INSERT INTO spell_target_position VALUES (18578, 249, -35.805332, -232.028900, -87.749153, 1.416);
INSERT INTO spell_target_position VALUES (18579, 249, -34.045738, -224.714661, -85.529465, 1.416);
INSERT INTO spell_target_position VALUES (18580, 249, -32.081570, -214.916962, -88.327438, 1.416);
INSERT INTO spell_target_position VALUES (18581, 249, -36.611721, -202.684677, -85.653786, 1.416);
INSERT INTO spell_target_position VALUES (18582, 249, -37.067261, -195.758652, -87.745834, 1.416);
INSERT INTO spell_target_position VALUES (18583, 249, -37.728523, -188.616806, -88.074898, 1.416);
-- SPELL_BREATH_WEST_TO_EAST
DELETE FROM spell_target_position WHERE id IN (18609, 18611, 18612, 18613, 18614, 18615, 18616);
INSERT INTO spell_target_position VALUES (18609, 249, -37.728523, -188.616806, -88.074898, 4.526);
INSERT INTO spell_target_position VALUES (18611, 249, -37.067261, -195.758652, -87.745834, 4.526);
INSERT INTO spell_target_position VALUES (18612, 249, -36.611721, -202.684677, -85.653786, 4.526);
INSERT INTO spell_target_position VALUES (18613, 249, -32.081570, -214.916962, -88.327438, 4.526);
INSERT INTO spell_target_position VALUES (18614, 249, -34.045738, -224.714661, -85.529465, 4.526);
INSERT INTO spell_target_position VALUES (18615, 249, -35.805332, -232.028900, -87.749153, 4.526);
INSERT INTO spell_target_position VALUES (18616, 249, -37.743851, -243.667923, -88.217651, 4.526);
-- SPELL_BREATH_NW_TO_SE
DELETE FROM spell_target_position WHERE id IN (18584, 18585, 18586, 18587, 18588, 18589, 18590, 18591, 18592, 18593, 18594, 18595);
INSERT INTO spell_target_position VALUES (18584, 249, 6.016711, -181.305771, -85.654648, 3.776);
INSERT INTO spell_target_position VALUES (18585, 249, 3.860220, -183.227249, -86.375381, 3.776);
INSERT INTO spell_target_position VALUES (18586, 249, -2.529650, -188.690491, -87.172859, 3.776);
INSERT INTO spell_target_position VALUES (18587, 249, -8.449303, -193.957962, -87.564957, 3.776);
INSERT INTO spell_target_position VALUES (18588, 249, -14.321238, -199.462219, -87.922478, 3.776);
INSERT INTO spell_target_position VALUES (18589, 249, -15.602085, -216.893936, -88.403183, 3.776);
INSERT INTO spell_target_position VALUES (18590, 249, -23.650263, -221.969086, -89.172699, 3.776);
INSERT INTO spell_target_position VALUES (18591, 249, -29.495876, -213.014359, -88.910423, 3.776);
INSERT INTO spell_target_position VALUES (18592, 249, -35.439922, -217.260284, -87.336311, 3.776);
INSERT INTO spell_target_position VALUES (18593, 249, -41.762104, -221.896545, -86.114113, 3.776);
INSERT INTO spell_target_position VALUES (18594, 249, -51.067528, -228.909988, -85.765556, 3.776);
INSERT INTO spell_target_position VALUES (18595, 249, -56.559654, -241.223923, -85.423607, 3.776);
-- SPELL_BREATH_SE_TO_NW
DELETE FROM spell_target_position WHERE id IN (18564, 18565, 18566, 18567, 18568, 18569, 18570, 18571, 18572, 18573, 18574, 18575);
INSERT INTO spell_target_position VALUES (18564, 249, -56.559654, -241.223923, -85.423607, 0.666);
INSERT INTO spell_target_position VALUES (18565, 249, -51.067528, -228.909988, -85.765556, 0.666);
INSERT INTO spell_target_position VALUES (18566, 249, -41.762104, -221.896545, -86.114113, 0.666);
INSERT INTO spell_target_position VALUES (18567, 249, -35.439922, -217.260284, -87.336311, 0.666);
INSERT INTO spell_target_position VALUES (18568, 249, -29.495876, -213.014359, -88.910423, 0.666);
INSERT INTO spell_target_position VALUES (18569, 249, -23.650263, -221.969086, -89.172699, 0.666);
INSERT INTO spell_target_position VALUES (18570, 249, -15.602085, -216.893936, -88.403183, 0.666);
INSERT INTO spell_target_position VALUES (18571, 249, -14.321238, -199.462219, -87.922478, 0.666);
INSERT INTO spell_target_position VALUES (18572, 249, -8.449303, -193.957962, -87.564957, 0.666);
INSERT INTO spell_target_position VALUES (18573, 249, -2.529650, -188.690491, -87.172859, 0.666);
INSERT INTO spell_target_position VALUES (18574, 249, 3.860220, -183.227249, -86.375381, 0.666);
INSERT INTO spell_target_position VALUES (18575, 249, 6.016711, -181.305771, -85.654648, 0.666);
-- SPELL_BREATH_SW_TO_NE
DELETE FROM spell_target_position WHERE id IN (18596, 18597, 18598, 18599, 18600, 18601, 18602, 18603, 18604, 18605, 18606, 18607);
INSERT INTO spell_target_position VALUES (18596, 249, -58.250900, -189.020004, -85.292267, 5.587);
INSERT INTO spell_target_position VALUES (18597, 249, -52.006271, -193.796570, -85.808769, 5.587);
INSERT INTO spell_target_position VALUES (18598, 249, -46.135464, -198.548553, -85.901764, 5.587);
INSERT INTO spell_target_position VALUES (18599, 249, -40.500187, -203.001053, -85.555107, 5.587);
INSERT INTO spell_target_position VALUES (18600, 249, -30.907579, -211.058197, -88.592125, 5.587);
INSERT INTO spell_target_position VALUES (18601, 249, -20.098139, -218.681427, -88.937088, 5.587);
INSERT INTO spell_target_position VALUES (18602, 249, -12.223192, -224.666168, -87.856300, 5.587);
INSERT INTO spell_target_position VALUES (18603, 249, -6.475297, -229.098724, -87.076401, 5.587);
INSERT INTO spell_target_position VALUES (18604, 249, -2.010256, -232.541992, -86.995140, 5.587);
INSERT INTO spell_target_position VALUES (18605, 249, 2.736300, -236.202347, -86.790367, 5.587);
INSERT INTO spell_target_position VALUES (18606, 249, 7.197779, -239.642868, -86.307297, 5.587);
INSERT INTO spell_target_position VALUES (18607, 249, 12.120926, -243.439407, -85.874260, 5.587);
-- SPELL_BREATH_NE_TO_SW
DELETE FROM spell_target_position WHERE id IN (18617, 18619, 18620, 18621, 18622, 18623, 18624, 18625, 18626, 18627, 18628, 18618);
INSERT INTO spell_target_position VALUES (18617, 249, 12.120926, -243.439407, -85.874260, 2.428);
INSERT INTO spell_target_position VALUES (18619, 249, 7.197779, -239.642868, -86.307297, 2.428);
INSERT INTO spell_target_position VALUES (18620, 249, 2.736300, -236.202347, -86.790367, 2.428);
INSERT INTO spell_target_position VALUES (18621, 249, -2.010256, -232.541992, -86.995140, 2.428);
INSERT INTO spell_target_position VALUES (18622, 249, -6.475297, -229.098724, -87.076401, 2.428);
INSERT INTO spell_target_position VALUES (18623, 249, -12.223192, -224.666168, -87.856300, 2.428);
INSERT INTO spell_target_position VALUES (18624, 249, -20.098139, -218.681427, -88.937088, 2.428);
INSERT INTO spell_target_position VALUES (18625, 249, -30.907579, -211.058197, -88.592125, 2.428);
INSERT INTO spell_target_position VALUES (18626, 249, -40.500187, -203.001053, -85.555107, 2.428);
INSERT INTO spell_target_position VALUES (18627, 249, -46.135464, -198.548553, -85.901764, 2.428);
INSERT INTO spell_target_position VALUES (18628, 249, -52.006271, -193.796570, -85.808769, 2.428);
INSERT INTO spell_target_position VALUES (18618, 249, -58.250900, -189.020004, -85.292267, 2.428);

-- Below is not needed for current script
-- SPELL_BREATH_SOUTH_TO_NORTH
DELETE FROM spell_target_position WHERE id IN (18351, 18352, 18353, 18354, 18355, 18356, 18357, 18358, 18359, 18360, 18361);
INSERT INTO spell_target_position VALUES (18351, 249, -68.834236, -215.036163, -84.018875, 6.280);
INSERT INTO spell_target_position VALUES (18352, 249, -61.834255, -215.051910, -84.673416, 6.280);
INSERT INTO spell_target_position VALUES (18353, 249, -53.343277, -215.071014, -85.597191, 6.280);
INSERT INTO spell_target_position VALUES (18354, 249, -42.619305, -215.095139, -86.663605, 6.280);
INSERT INTO spell_target_position VALUES (18355, 249, -35.899323, -215.110245, -87.196548, 6.280);
INSERT INTO spell_target_position VALUES (18356, 249, -28.248341, -215.127457, -89.191750, 6.280);
INSERT INTO spell_target_position VALUES (18357, 249, -20.324360, -215.145279, -88.963997, 6.280);
INSERT INTO spell_target_position VALUES (18358, 249, -11.189384, -215.165833, -87.817093, 6.280);
INSERT INTO spell_target_position VALUES (18359, 249, -2.047405, -215.186386, -86.279655, 6.280);
INSERT INTO spell_target_position VALUES (18360, 249, 7.479571, -215.207809, -86.075531, 6.280);
INSERT INTO spell_target_position VALUES (18361, 249, 20.730539, -215.237610, -85.254387, 6.280);
-- SPELL_BREATH_NORTH_TO_SOUTH
DELETE FROM spell_target_position WHERE id IN (17086, 17087, 17088, 17089, 17090, 17091, 17092, 17093, 17094, 17095, 17097, 22267, 22268, 21132, 21133, 21135, 21136, 21137, 21138, 21139);
INSERT INTO spell_target_position VALUES (17086, 249, 20.730539, -215.237610, -85.254387, 3.142);
INSERT INTO spell_target_position VALUES (17087, 249, 7.479571, -215.207809, -86.075531, 3.142);
INSERT INTO spell_target_position VALUES (17088, 249, -2.047405, -215.186386, -86.279655, 3.142);
INSERT INTO spell_target_position VALUES (17089, 249, -11.189384, -215.165833, -87.817093, 3.142);
INSERT INTO spell_target_position VALUES (17090, 249, -20.324360, -215.145279, -88.963997, 3.142);
INSERT INTO spell_target_position VALUES (17091, 249, -28.248341, -215.127457, -89.191750, 3.142);
INSERT INTO spell_target_position VALUES (17092, 249, -35.899323, -215.110245, -87.196548, 3.142);
INSERT INTO spell_target_position VALUES (17093, 249, -42.619305, -215.095139, -86.663605, 3.142);
INSERT INTO spell_target_position VALUES (17094, 249, -53.343277, -215.071014, -85.597191, 3.142);
INSERT INTO spell_target_position VALUES (17095, 249, -61.834255, -215.051910, -84.673416, 3.142);
INSERT INTO spell_target_position VALUES (17097, 249, -68.834236, -215.036163, -84.018875, 3.142);
INSERT INTO spell_target_position VALUES (22267, 249, -75.736046, -214.984970, -83.394188, 3.142);
INSERT INTO spell_target_position VALUES (22268, 249, -84.087578, -214.857834, -82.640053, 3.142);
INSERT INTO spell_target_position VALUES (21132, 249, -90.424416, -214.601974, -82.482697, 3.142);
INSERT INTO spell_target_position VALUES (21133, 249, -96.572411, -214.353745, -82.239967, 3.142);
INSERT INTO spell_target_position VALUES (21135, 249, -102.069931, -214.131775, -80.571190, 3.142);
INSERT INTO spell_target_position VALUES (21136, 249, -107.385597, -213.917145, -77.447037, 3.142);
INSERT INTO spell_target_position VALUES (21137, 249, -114.281258, -213.866486, -73.851128, 3.142);
INSERT INTO spell_target_position VALUES (21138, 249, -123.328560, -213.607910, -71.559921, 3.142);
INSERT INTO spell_target_position VALUES (21139, 249, -130.788300, -213.424026, -70.751007, 3.142);
*/
