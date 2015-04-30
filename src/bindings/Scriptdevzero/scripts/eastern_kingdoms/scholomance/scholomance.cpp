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
## mob_spectral_tutor
######*/

enum eSpectralTutor
{
    SPELL_IMAGE_PROJECTION = 17651,
    SPELL_MANA_BURN        = 17630,
    SPELL_SILENCE          = 12528
};

struct MANGOS_DLL_DECL mob_spectral_tutorAI : public ScriptedAI
{
    mob_spectral_tutorAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiImageProjection_Timer;
    uint32 m_uiManaBurn_Timer;
    uint32 m_uiSilence_Timer;
    bool IsProjected;

    void Reset()
    {
        m_uiImageProjection_Timer = 0;
        m_uiManaBurn_Timer = urand(4200, 19100);
        m_uiSilence_Timer = urand(0, 3300);

        //IsProjected = false;
        SetTutorProjection(false);
    }

    void SetTutorProjection(bool activate)
    {
        if (activate)
        {
            // become invisible
            m_creature->AttackStop();
            m_creature->StopMoving();
            m_creature->setFaction(35);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 + UNIT_FLAG_NOT_SELECTABLE);
            m_creature->addUnitState(UNIT_STAT_CAN_NOT_MOVE);
            m_creature->SetVisibility(VISIBILITY_OFF);
            
            IsProjected = true;
        }
        else
        {
            // become visible
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 + UNIT_FLAG_NOT_SELECTABLE);
            m_creature->clearUnitState(UNIT_STAT_CAN_NOT_MOVE);
            m_creature->setFaction(m_creature->GetCreatureInfo()->faction_A);
            m_creature->SetVisibility(VISIBILITY_ON);
            
            IsProjected = false;
        }
    }
    
    void Aggro(Unit* /*pWho*/)
    {
        m_uiImageProjection_Timer = urand(11800, 12700);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (IsProjected)
            SetTutorProjection(false);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Image Projection
        if (m_uiImageProjection_Timer)
        {
            if (m_uiImageProjection_Timer <= uiDiff)
            {
                //DoCastSpellIfCan(m_creature, SPELL_IMAGE_PROJECTION);     // leaving combat core issue
                if (IsProjected)
                {
                    SetTutorProjection(false);
                    m_uiImageProjection_Timer = urand(17700, 25300);
                }
                else
                {
                    SetTutorProjection(true);
                    m_uiImageProjection_Timer = 5000;
                }
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
    pNewscript->Name = "mob_spectral_tutor";
    pNewscript->GetAI = &GetAI_mob_spectral_tutor;
    pNewscript->RegisterSelf();
}
