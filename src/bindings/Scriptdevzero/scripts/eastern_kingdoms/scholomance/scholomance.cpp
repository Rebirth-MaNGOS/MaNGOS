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
SDName: Scholomance
SD%Complete: 100
SDComment:
SDCategory: Scholomance
EndScriptData */

/* ContentData
go_iron_gate
go_viewing_room_door
item_dawns_gambit
mob_reanimated_corpse
mob_scholomance_dark_summoner
mob_unstable_corpse
mob_dark_shade
mob_spectral_tutor
EndContentData */

#include "precompiled.h"
#include "scholomance.h"
#include "TemporarySummon.h"

/*######
## go_iron_gate
######*/

bool GOUse_go_iron_gate(Player* pPlayer, GameObject* /*pGo*/)
{
    if (pPlayer->HasStealthAura())
        pPlayer->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);

    if (pPlayer->HasInvisibilityAura())
        pPlayer->RemoveSpellsCausingAura(SPELL_AURA_MOD_INVISIBILITY);

    return false;
}

/*######
## go_viewing_room_door
######*/

bool GOUse_go_viewing_room_door(Player* /*pPlayer*/, GameObject* pGo)
{
    if (pGo->GetInstanceData())
        pGo->GetInstanceData()->SetData(TYPE_VIEWING_ROOM_DOOR, DONE);
    return false;
}

/*######
## item_dawns_gambit
######*/

bool ItemUse_item_dawns_gambit(Player* pPlayer, Item* /*pItem*/, SpellCastTargets const& /*targets*/)
{
    std::list<Creature*> m_lStudents;
    GetCreatureListWithEntryInGrid(m_lStudents, pPlayer, NPC_SCHOLOMANCE_STUDENT, DEFAULT_VISIBILITY_INSTANCE);
    if (!m_lStudents.empty())
    {
        for(std::list<Creature*>::iterator itr = m_lStudents.begin(); itr != m_lStudents.end(); ++itr)
        {
            if ((*itr) && (*itr)->isAlive())
            {
                (*itr)->SetDisplayId(9786);
                (*itr)->SetHealth(6000);
                (*itr)->SetMaxHealth(6000);
                (*itr)->setFaction(FACTION_HOSTILE);
            }
        }
    }
    return false;
}

/*######
## mob_scholomance_student
######*/

enum eScholomanceStudent
{
    //SPELL_ALEXIS_GAMBIT                         = 17045,
    //SPELL_TRANSFORM_VIEWING_ROOM_STUDENTS       = 18110,
    SPELL_VIEWING_ROOM_STUDENT_TRANSFORM_EFFECT = 18115,
    MODELID_SKELETAL_SCHOLOMANCE_STUDENT        = 9785,
};

struct MANGOS_DLL_DECL mob_scholomance_studentAI : public ScriptedAI
{
    mob_scholomance_studentAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		m_bSkeleton = false;

		Reset();
	}

	bool m_bSkeleton;

    void Reset()
    {
		if(m_bSkeleton)
		{
			DoCastSpellIfCan(m_creature, SPELL_VIEWING_ROOM_STUDENT_TRANSFORM_EFFECT, CastFlags::CAST_INTERRUPT_PREVIOUS);
			m_creature->GetMotionMaster()->MoveRandom();
			m_creature->SetStandState(UNIT_STAND_STATE_STAND);
		}
		else
		{
			if(m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != MovementGeneratorType::WAYPOINT_MOTION_TYPE)
				m_creature->GetMotionMaster()->MoveIdle();
		}

    }

	void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            // transformation
            if (m_creature->GetDisplayId() == MODELID_SKELETAL_SCHOLOMANCE_STUDENT)
                m_bSkeleton = true;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_scholomance_student(Creature* pCreature)
{
    return new mob_scholomance_studentAI(pCreature);
}

/*######
## mob_reanimated_corpse
######*/

enum eReanimatedCorpse
{
    SPELL_DARK_PLAGUE = 18270,
    REVIVE_TIMER      = 30000
};

struct MANGOS_DLL_DECL mob_reanimated_corpseAI : public ScriptedAI
{
    mob_reanimated_corpseAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiDarkPlague_Timer;
    uint32 m_uiRisen_Timer;
    bool CanReviveStart;
    bool WasRevived;

    void Reset()
    {
        m_uiDarkPlague_Timer = urand(3600, 17300);
        m_uiRisen_Timer = REVIVE_TIMER;
        CanReviveStart = false;
        WasRevived = false;
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32 &uiDamage)
    {
        if (!WasRevived && uiDamage > m_creature->GetHealth())
        {
            m_creature->AttackStop();
            m_creature->StopMoving();
            m_creature->addUnitState(UNIT_STAT_CAN_NOT_MOVE);

            if (m_creature->getStandState() != UNIT_STAND_STATE_DEAD)
                m_creature->SetStandState(UNIT_STAND_STATE_DEAD);

            CanReviveStart = true;
            m_creature->SetHealth(1);
            uiDamage = 0;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (CanReviveStart && !WasRevived)
        {
            if (m_uiRisen_Timer <= uiDiff)
            {
                CanReviveStart = false;
                WasRevived = true;
                m_creature->clearUnitState(UNIT_STAT_CAN_NOT_MOVE);
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                m_creature->SetHealth(m_creature->GetMaxHealth());
            }
            else
                m_uiRisen_Timer -= uiDiff;
        }

        //return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Dark Plague
        if (!CanReviveStart && m_uiDarkPlague_Timer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_DARK_PLAGUE);
            m_uiDarkPlague_Timer = urand(10300, 18200);
        }
        else
            m_uiDarkPlague_Timer -= uiDiff;

        if (!CanReviveStart)
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_reanimated_corpse(Creature* pCreature)
{
    return new mob_reanimated_corpseAI(pCreature);
}

/*######
## mob_scholomance_dark_summoner
######*/

enum eScholomanceDarkSummoner
{
    SPELL_CURSE_OF_BLOOD = 12279,
    SPELL_DRAIN_LIFE = 17620,
    SPELL_SUMMON_RISEN_LACKEY = 17618,
    
    SAY1   = -1289002,
    SAY2   = -1289003,
    SAY3   = -1289004
};

struct MANGOS_DLL_DECL mob_scholomance_dark_summonerAI : public ScriptedAI
{
    mob_scholomance_dark_summonerAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiCurseOfBlood_Timer;
    uint32 m_uiDrainLife_Timer;
    uint32 m_uiSummonRisenLackey_Timer;

    void Reset()
    {
        m_uiCurseOfBlood_Timer = urand(0, 500);
        m_uiDrainLife_Timer = urand(12600, 25200);
        m_uiSummonRisenLackey_Timer = urand(500, 2800);
    }

    void Aggro(Unit* /*pWho*/)
    {
        switch(urand(1,3))
        {
            case 1: DoScriptText(SAY1, m_creature); break;
            case 2: DoScriptText(SAY2, m_creature); break;
            case 3: DoScriptText(SAY3, m_creature); break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Curse Of Blood
        if (m_uiCurseOfBlood_Timer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CURSE_OF_BLOOD);
            m_uiCurseOfBlood_Timer = urand(1900, 3700);
        }
        else
            m_uiCurseOfBlood_Timer -= uiDiff;

        // Drain Life
        if (m_uiDrainLife_Timer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_DRAIN_LIFE);
            m_uiDrainLife_Timer = urand(16700, 32300);
        }
        else
            m_uiDrainLife_Timer -= uiDiff;

        // Summon Risen Lackey
        if (m_uiSummonRisenLackey_Timer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_SUMMON_RISEN_LACKEY);
            m_uiSummonRisenLackey_Timer = 1000;
        }
        else
            m_uiSummonRisenLackey_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_mob_scholomance_dark_summoner(Creature* pCreature)
{
    return new mob_scholomance_dark_summonerAI(pCreature);
}

/*######
## mob_unstable_corpse
######*/

#define SPELL_DISEASE_BURST 17689

struct MANGOS_DLL_DECL mob_unstable_corpseAI : public ScriptedAI
{
    mob_unstable_corpseAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiKill_Timer;

    void Reset()
    {
        m_uiKill_Timer = 0;
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32 &uiDamage)
    {
        if (!m_uiKill_Timer && uiDamage >= m_creature->GetHealth())
        {
            DoCastSpellIfCan(m_creature, SPELL_DISEASE_BURST);
            m_uiKill_Timer = 150;
            uiDamage = 0;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // KaaBoom! :D
        if (m_uiKill_Timer)
        {
            if (m_uiKill_Timer <= uiDiff)
            {
                m_creature->SetHealth(0);
                m_creature->SetDeathState(JUST_DIED);
            }
            else
                m_uiKill_Timer -= uiDiff;
        }

        // return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_uiKill_Timer)
            DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_mob_unstable_corpse(Creature* pCreature)
{
    return new mob_unstable_corpseAI(pCreature);
}

/*######
## mob_dark_shade
######*/

#define SPELL_PHYSICAL_IMMUNITY 17674

struct MANGOS_DLL_DECL mob_dark_shadeAI : public ScriptedAI
{
    mob_dark_shadeAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiCheckImmunity_Timer;

    void Reset()
    {
        SetPhysicalImmunity();
        m_uiCheckImmunity_Timer = 2000;
    }

    void SetPhysicalImmunity()
    {
        if (!m_creature->HasAura(SPELL_PHYSICAL_IMMUNITY))
            DoCastSpellIfCan(m_creature, SPELL_PHYSICAL_IMMUNITY, CAST_FORCE_TARGET_SELF);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiCheckImmunity_Timer <= uiDiff)
        {
            SetPhysicalImmunity();
            m_uiCheckImmunity_Timer = 2000;
        }
        else
            m_uiCheckImmunity_Timer -= uiDiff;

        // return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_mob_dark_shade(Creature* pCreature)
{
    return new mob_dark_shadeAI(pCreature);
}

/*######
## mob_spectral_projection
######*/

enum eSpectralProjection
{
	//SPELL_IMAGE_PROJECTION_HEAL = 17652,				// the heal? not working
	SPELL_DARK_MENDING			= 16588,				// a heal, heals 2k each
	NPC_SPECTRAL_TUTOR			= 10498,
};

struct MANGOS_DLL_DECL mob_spectral_projectionAI : public ScriptedAI
{
    mob_spectral_projectionAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiTutorCheck;
	ObjectGuid m_uiTutorGUID;

    void Reset()
    {
        m_uiTutorCheck = 7000;
		if (m_creature->IsTemporarySummon())
            m_uiTutorGUID = ((TemporarySummon*)m_creature)->GetSummonerGuid();
    }

    void UpdateAI(const uint32 uiDiff)
    {
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		// Heal tutor/ despawn
        if (m_uiTutorCheck <= uiDiff)
        {
			Creature* pTutor = m_creature->GetMap()->GetCreature(m_uiTutorGUID);
			if (pTutor && pTutor->IsWithinDistInMap(m_creature, 100.0f))
			{
				m_creature->CastSpell(pTutor, SPELL_DARK_MENDING, true);
				m_creature->ForcedDespawn();
			}
			else 
				m_creature->ForcedDespawn();
        }
        else
            m_uiTutorCheck -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_spectral_projection(Creature* pCreature)
{
    return new mob_spectral_projectionAI(pCreature);
}

/*######
## mob_spectral_tutor
######*/

enum eSpectralTutor
{
    SPELL_IMAGE_PROJECTION = 17651,				// turn invisible + invulnerable	
    SPELL_MANA_BURN        = 17630,
    SPELL_SILENCE          = 12528,

	NPC_SPECTRAL_PROJECTION	= 11263,
};

struct MANGOS_DLL_DECL mob_spectral_tutorAI : public ScriptedAI
{
    mob_spectral_tutorAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}			// commented out the invisible hack since the spell works

    uint32 m_uiImageProjection_Timer;
    uint32 m_uiManaBurn_Timer;
    uint32 m_uiSilence_Timer;
    //bool IsProjected;

    void Reset()
    {
        m_uiImageProjection_Timer = 0;
        m_uiManaBurn_Timer = urand(4200, 19100);
        m_uiSilence_Timer = urand(0, 3300);
        //IsProjected = false;
        //SetTutorProjection(false);
    }

    //void SetTutorProjection(bool activate)
    //{
    //    if (activate)
    //    {
    //        // become invisible
    //        m_creature->AttackStop();
    //        m_creature->StopMoving();
    //        m_creature->setFaction(35);
    //        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 + UNIT_FLAG_NOT_SELECTABLE);
    //        m_creature->addUnitState(UNIT_STAT_CAN_NOT_MOVE);
    //        m_creature->SetVisibility(VISIBILITY_OFF);            

    //        IsProjected = true;
    //    }
    //    else
    //    {
    //        // become visible
    //        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 + UNIT_FLAG_NOT_SELECTABLE);
    //        m_creature->clearUnitState(UNIT_STAT_CAN_NOT_MOVE);
    //        m_creature->setFaction(m_creature->GetCreatureInfo()->faction_A);
    //        m_creature->SetVisibility(VISIBILITY_ON);
		
    //        IsProjected = false;
    //    }
    //}
	
    void Aggro(Unit* /*pWho*/)
    {
		m_uiImageProjection_Timer = urand(40000,60000);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        /*if (IsProjected)
            SetTutorProjection(false);*/
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Image Projection
        if (m_uiImageProjection_Timer)
        {
            if (m_uiImageProjection_Timer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_IMAGE_PROJECTION);     // leaving combat core issue, seems to work fine.
                /*if (IsProjected)
                {
                    SetTutorProjection(false);
                    m_uiImageProjection_Timer = urand(17700, 25300);
                }
                else
                {
                    SetTutorProjection(true);
                    m_uiImageProjection_Timer = 5000;
                }*/
				float fX, fY, fZ;
				m_creature->GetPosition(fX, fY, fZ);
				for(uint8 i = 0; i < 5; ++i)
					if (Creature* pProjection = m_creature->SummonCreature(NPC_SPECTRAL_PROJECTION, fX+irand(-3,3), fY+irand(-3,3), fZ, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
						pProjection->SetRespawnDelay(-10);				// to stop them from randomly respawning
				m_uiImageProjection_Timer = urand(45000,60000);
            }
            else
                m_uiImageProjection_Timer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Mana Burn
        if (m_uiManaBurn_Timer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MANA_BURN);
            m_uiManaBurn_Timer = urand(8600, 26400);
        }
        else
            m_uiManaBurn_Timer -= uiDiff;

        // Silence
        if (m_uiSilence_Timer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_SILENCE);
            m_uiSilence_Timer = urand(12000, 26300);
        }
        else
            m_uiSilence_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_spectral_tutor(Creature* pCreature)
{
    return new mob_spectral_tutorAI(pCreature);
}

/*######
## npc_paladin_event
######*/

struct Locd
{
    float x, y, z;
};

static Locd aAspectSpawn[]=
{
    {141.07f,173.05f,95.70f},				// middle, for every aspect and death knight
    {133.29f,172.48f,95.38f},				// add, infront
	{142.70f,169.97f,95.27f},				// add, left 
	{142.68f,181.81f,94.72f},				// add, right
};

static Locd aWaveSpawn[]=					// 12 different spawns
{
    {108.79f,185.70f,94.71f},				// corner at door
    {105.19f,177.30f,93.03f},
	{115.91f,184.17f,92.75f},
	{151.15f,184.94f,93.08f},				// north west
	{143.74f,188.39f,93.97f},
    {155.45f,176.52f,93.08f},
	{150.68f,155.39f,93.08f},				// north east
	{154.74f,161.46f,93.08f},
	{142.40f,151.92f,92.79f},
	{112.32f,148.92f,92.70f},				// south east
	{121.15f,147.81f,92.71f},
	{106.44f,157.38f,92.77f},
};

struct MANGOS_DLL_DECL npc_paladin_eventAI : public ScriptedAI
{
    npc_paladin_eventAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{	
		m_uiEventPhase		= 0;
		m_uiKilledCreatures = 0;
		m_uiCurrentWave     = 0;
		m_uiSummonCount		= 0;
		m_uiEventTimer		= 5000;
		m_bEvent			= true;			// only start event once
		m_bDarkreaver		= true;
		Reset();
	}

	uint8 m_uiCurrentWave;
	uint8 m_uiKilledCreatures;

	uint32 m_uiSummonCount;
	uint32 m_uiEventTimer;
	uint32 m_uiEventPhase;

	GUIDList m_uiSummonList;

	bool m_bEvent;
	bool m_bDarkreaver;

    void Reset()
    {
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32 &uiDamage)
    {
    }

	void JustSummoned(Creature* pSummoned)
    {
        ++m_uiSummonCount;
		pSummoned->SetRespawnDelay(-10);			// make sure they won't respawn

		m_uiSummonList.push_front(pSummoned->GetObjectGuid());
    }

	void SummonedCreatureJustDied(Creature* pSummoned)
    {
        --m_uiSummonCount;
		m_uiSummonList.remove(pSummoned->GetObjectGuid());

		++m_uiKilledCreatures;

        if (m_uiSummonCount == 0 && m_uiEventTimer > 1000 && m_uiEventPhase < 11)
            m_uiEventTimer = 1000;

		else if (m_uiSummonCount == 0 && m_uiEventTimer > 1000 && m_uiEventPhase > 17)
            m_uiEventTimer = 1000;

		else if (m_uiSummonCount == 0 && m_uiEventPhase >= 19 && m_bDarkreaver)
            DoSummonWave(12);
    }
	
	void DoSummonWave(uint32 uiSummonId = 0)
    {
        switch (uiSummonId)
        {
		case 1:
            for (uint8 i = 0; i < 9; ++i)
            {
                m_creature->SummonCreature(NPC_BANAL_SPIRIT, aWaveSpawn[i].x, aWaveSpawn[i].y, aWaveSpawn[i].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000, true);
            }
            ++m_uiCurrentWave;
			break;
        case 2:
			for (uint8 i = 0; i < 1; ++i)
			{
				m_creature->SummonCreature(NPC_ASPECT_OF_BANALITY, aAspectSpawn[0].x, aAspectSpawn[i].y, aAspectSpawn[0].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000, true);
				m_creature->SummonCreature(NPC_BANAL_SPIRIT, aAspectSpawn[1].x, aAspectSpawn[1].y, aAspectSpawn[1].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000, true);
				m_creature->SummonCreature(NPC_BANAL_SPIRIT, aAspectSpawn[2].x, aAspectSpawn[2].y, aAspectSpawn[2].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000, true);
				m_creature->SummonCreature(NPC_BANAL_SPIRIT, aAspectSpawn[3].x, aAspectSpawn[3].y, aAspectSpawn[3].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000, true);
			}
			++m_uiCurrentWave;
			break;
		case 3:
            for (uint8 i = 0; i < 3; ++i)
            {
                m_creature->SummonCreature(NPC_MALICIOUS_SPIRIT, aWaveSpawn[i].x, aWaveSpawn[i].y, aWaveSpawn[i].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000, true);
            }
			++m_uiCurrentWave;
			break;
		case 4:			// do this 3 times
            for (uint8 i = 0; i < 2; ++i)
            {
				int Rnd = irand(0, 11);
                m_creature->SummonCreature(NPC_MALICIOUS_SPIRIT, aWaveSpawn[Rnd].x, aWaveSpawn[Rnd].y, aWaveSpawn[Rnd].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000, true);
            }
			++m_uiCurrentWave;
			break;
		case 5:
            for (uint8 i = 0; i < 1; ++i)
            {
                m_creature->SummonCreature(NPC_ASPECT_OF_MALICE, aAspectSpawn[i].x, aAspectSpawn[i].y, aAspectSpawn[i].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000, true);
            }
			++m_uiCurrentWave;
			break;
		case 6:
            for (uint8 i = 0; i < 9; ++i)
            {
                m_creature->SummonCreature(NPC_CORRUPTED_SPIRIT, aWaveSpawn[i].x, aWaveSpawn[i].y, aWaveSpawn[i].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000, true);
            }
            ++m_uiCurrentWave;
			break;
		case 7:
            for (uint8 i = 0; i < 2; ++i)
            {
				int Rnd = irand(0, 11);
                m_creature->SummonCreature(NPC_CORRUPTED_SPIRIT, aWaveSpawn[Rnd].x, aWaveSpawn[Rnd].y, aWaveSpawn[Rnd].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000, true);
            }
            ++m_uiCurrentWave;
			break;
		case 8:
            for (uint8 i = 0; i < 1; ++i)
            {
                m_creature->SummonCreature(NPC_ASPECT_OF_CORRUPTION, aAspectSpawn[i].x, aAspectSpawn[i].y, aAspectSpawn[i].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000, true);
            }
			++m_uiCurrentWave;
			break;
		case 9:
            for (uint8 i = 0; i < 1; ++i)
            {
                m_creature->SummonCreature(NPC_SHADOWED_SPIRIT, aWaveSpawn[1].x, aWaveSpawn[1].y, aWaveSpawn[1].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000, true);
				m_creature->SummonCreature(NPC_SHADOWED_SPIRIT, aWaveSpawn[4].x, aWaveSpawn[4].y, aWaveSpawn[4].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000, true);
				m_creature->SummonCreature(NPC_SHADOWED_SPIRIT, aWaveSpawn[9].x, aWaveSpawn[9].y, aWaveSpawn[9].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000, true);
            }
			++m_uiCurrentWave;
			break;
		case 10:
            for (uint8 i = 0; i < 1; ++i)
            {
				int Rnd = irand(0, 11);
                m_creature->SummonCreature(NPC_SHADOWED_SPIRIT, aWaveSpawn[Rnd].x, aWaveSpawn[Rnd].y, aWaveSpawn[Rnd].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000, true);
            }
			++m_uiCurrentWave;
			break;
		case 11:
            for (uint8 i = 0; i < 1; ++i)
            {
                m_creature->SummonCreature(NPC_ASPECT_OF_SHADOW, aAspectSpawn[i].x, aAspectSpawn[i].y, aAspectSpawn[i].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000, true);
            }
			++m_uiCurrentWave;
			break;
		case 12:
            for (uint8 i = 0; i < 1; ++i)
            {
                m_creature->SummonCreature(NPC_DEATH_KNIGHT_DARKREAVER, aAspectSpawn[i].x, aAspectSpawn[i].y, aAspectSpawn[i].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000, true);
				m_bDarkreaver = false;
            }
			break;
		}
	}

    void UpdateAI(const uint32 uiDiff)
    {
         if (m_bEvent)
        {
            if (m_uiEventTimer <= uiDiff)
            {
                switch(++m_uiEventPhase)
                {
                    case 1:
						DoSummonWave(1);
						m_creature->GenericTextEmote("Banal Spirit emerges from the darkness, drawn out by the divination scryer!", NULL, false);
                        break;
                    case 2:
						DoSummonWave(2);
						m_creature->GenericTextEmote("Aspect of Banality emerges from the darkness, drawn out by the divination scryer!", NULL, false);
                        break;
                    case 3:
						DoSummonWave(3);
						m_creature->GenericTextEmote("Malicious Spirit emerges from the darkness, drawn out by the divination scryer!", NULL, false);
                        break;
                    case 4:
						DoSummonWave(4);
                        break;
                    case 5:
						DoSummonWave(4);
                        break;
                    case 6:
						DoSummonWave(4);
                        break;
                    case 7:
						DoSummonWave(5);
						m_creature->GenericTextEmote("Aspect of Malice emerges from the darkness, drawn out by the divination scryer!", NULL, false);
                        break;
                    case 8:
						DoSummonWave(6);
						m_creature->GenericTextEmote("Corrupted Spirit emerges from the darkness, drawn out by the divination scryer!", NULL, false);
                        break;
					case 9:
						DoSummonWave(7);
						break;
                    case 10:
						DoSummonWave(8);
						m_creature->GenericTextEmote("Aspect of Corruption emerges from the darkness, drawn out by the divination scryer!", NULL, false);
                        break;
                    case 11:
						DoSummonWave(9);
						m_creature->GenericTextEmote("Shadowed Spirit emerges from the darkness, drawn out by the divination scryer!", NULL, false);
						m_uiEventTimer = 30000;
                        break;
                    case 12:
						DoSummonWave(10);
						m_uiEventTimer = 30000;
                        break;
					case 13:
						DoSummonWave(10);
						m_uiEventTimer = 30000;
                        break;
					case 14:
						DoSummonWave(10);
						m_uiEventTimer = 30000;
                        break;
					case 15:
						DoSummonWave(10);
						m_uiEventTimer = 30000;
                        break;
					case 16:
						DoSummonWave(10);
						m_uiEventTimer = 30000;
                        break;
					case 17:
						DoSummonWave(10);
						m_uiEventTimer = 30000;
                        break;
					case 18:
						DoSummonWave(11);
						m_creature->GenericTextEmote("Aspect of Shadow emerges from the darkness, drawn out by the divination scryer!", NULL, false);
                        break;
					case 19:
						m_uiEventTimer = 0;
						m_bEvent = false;
                        break;
                    /*default:
						m_uiEventTimer = 1000;
                        break;*/
                }
				if (m_uiEventPhase < 11)
					m_uiEventTimer = 300000;
            }
            else
                m_uiEventTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_paladin_event(Creature* pCreature)
{
    return new npc_paladin_eventAI(pCreature);
}

/*######
## npc_jeevee_dummy
######*/
enum 
{
	SAY_1					= -1720043,
	SAY_2					= -1720044,
	SAY_3					= -1720045,
	SAY_4					= -1720046,

	SPELL_TELEPORT			= 21649,

	QUEST_ID_IMP_DELIVERY	= 7629,
};

struct MANGOS_DLL_DECL npc_jeevee_dummyAI : public ScriptedAI
{
    npc_jeevee_dummyAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{	
		m_uiEventPhase		= 0;
		m_uiEventTimer		= 1000;
		m_bEvent			= true;
		Reset();
	}

	uint32 m_uiEventTimer;
	uint32 m_uiEventPhase;

	bool m_bEvent;

    void Reset()
    {
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32 &uiDamage)
    {
    }

	/*void HandleQuestCredit()				// How the fuck do you give quest credit? Not working so hack in DB instead, but in future pls fix this
	{
		if (Player* pPlayer = GetPlayerAtMinimumRange(20))
			if (pPlayer->HasQuest(QUEST_ID_IMP_DELIVERY) && QuestStatus(IN_PROGRESS))
				pPlayer->GroupEventHappens(QUEST_ID_IMP_DELIVERY, m_creature);
	}*/

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bEvent)
        {
            if (m_uiEventTimer <= uiDiff)
            {
                switch(++m_uiEventPhase)
                {
					case 1:
						DoScriptText(SAY_1, m_creature);								// say on spawn
						m_uiEventTimer = 4000;
						break;
					case 2:
						m_creature->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);			// first spot
						m_uiEventTimer = 1000;
						break;
					case 3:
						DoScriptText(SAY_2, m_creature);
						m_uiEventTimer = 8000;
						break;
					case 4:
						m_creature->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);			// second spot
						m_uiEventTimer = 1500;
						break;
					case 5:
						m_creature->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);
						m_uiEventTimer = 2000;
						break;
					case 6:
						DoScriptText(SAY_3, m_creature);
						m_uiEventTimer = 10000;
						break;
					case 7:
						m_creature->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);			// last spot
						m_uiEventTimer = 1500;
						break;
					case 8:
						m_creature->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);
						m_uiEventTimer = 1500;
						break;
					case 9:
						m_creature->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);
						m_uiEventTimer = 1500;
						break;
					case 10:
						DoScriptText(SAY_4, m_creature);								// give quest credit
						//HandleQuestCredit();
						m_creature->SetFacingTo(4.71f);
						m_uiEventTimer = 1000;
						break;
					case 11:
						m_uiEventTimer = 3000;											// placeholder
						break;
					case 12:
						DoCast(m_creature, SPELL_TELEPORT, true);
						m_uiEventTimer = 1500;
						break;
					case 13:
						m_creature->SetVisibility(VISIBILITY_OFF);
						m_creature->ForcedDespawn();
						break;
                    /*default:
						m_uiEventTimer = 1000;
                        break;*/
                }
            }
            else
                m_uiEventTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_jeevee_dummy(Creature* pCreature)
{
    return new npc_jeevee_dummyAI(pCreature);
}

void AddSC_scholomance()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "go_iron_gate";
    pNewscript->pGOUse = &GOUse_go_iron_gate;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_viewing_room_door";
    pNewscript->pGOUse = &GOUse_go_viewing_room_door;
    pNewscript->RegisterSelf();

    /*pNewscript = new Script;
    pNewscript->Name = "item_dawns_gambit";
    pNewscript->pItemUse = &ItemUse_item_dawns_gambit;
    pNewscript->RegisterSelf();*/

    pNewscript = new Script;
    pNewscript->Name = "mob_scholomance_student";
    pNewscript->GetAI = &GetAI_mob_scholomance_student;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_reanimated_corpse";
    pNewscript->GetAI = &GetAI_mob_reanimated_corpse;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_scholomance_dark_summoner";
    pNewscript->GetAI = &GetAI_mob_scholomance_dark_summoner;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_unstable_corpse";
    pNewscript->GetAI = &GetAI_mob_unstable_corpse;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_dark_shade";
    pNewscript->GetAI = &GetAI_mob_dark_shade;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "mob_spectral_projection";
    pNewscript->GetAI = &GetAI_mob_spectral_projection;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_spectral_tutor";
    pNewscript->GetAI = &GetAI_mob_spectral_tutor;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_paladin_event";
    pNewscript->GetAI = &GetAI_npc_paladin_event;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_jeevee_dummy";
    pNewscript->GetAI = &GetAI_npc_jeevee_dummy;
    pNewscript->RegisterSelf();
}
