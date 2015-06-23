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
SDName: bosses_emerald_dragons
SD%Complete: 95
SDComment: some spell issues, summon player NYI
SDCategory: Emerald Dragon Bosses
EndScriptData */

/* ContentData
boss_emerald_dragon -- Superclass for the four dragons
boss_emeriss
boss_taerar
boss_shade_of_taerar
boss_ysondre
mob_dementeddruids
EndContentData */

#include "precompiled.h"
#include "Spell.h"
#include "SpellMgr.h"
#include "ObjectMgr.h"
#include "TemporarySummon.h"

/*######
## boss_emerald_dragon -- Superclass for the four dragons
######*/

enum eDragonsOfNightmare
{
    SPELL_MARK_OF_NATURE_PLAYER     = 25040,
    SPELL_MARK_OF_NATURE_AURA       = 25041,
    SPELL_AURA_OF_NATURE            = 25043,
    SPELL_SEEPING_FOG_R             = 24813,                // Summons 15224 'Dream Fog'
    SPELL_SEEPING_FOG_L             = 24814,
    SPELL_DREAM_FOG                 = 24777,                // Used by summoned Adds
    SPELL_NOXIOUS_BREATH            = 24818,
    SPELL_TAILSWEEP                 = 15847,
    SPELL_SUMMON_PLAYER             = 24776,                // wiki says used since 2.1.0 but spell exists in 1.12, NYI

    NPC_DREAM_FOG                   = 15224,
};

struct MANGOS_DLL_DECL boss_emerald_dragonAI : public ScriptedAI
{
    boss_emerald_dragonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        uint32 zoneID, areaID;
        m_creature->GetZoneAndAreaId(zoneID, areaID);
        if (zoneID == 10 && areaID == 856)
        {
            m_creature->SetDefaultMovementType(WAYPOINT_MOTION_TYPE);
            m_creature->GetMotionMaster()->MoveWaypoint();
        }
        else
        {
            m_creature->SetDefaultMovementType(IDLE_MOTION_TYPE);

            // Make sure the dragon is standing in the correct spot if it shouldn't patrol.
            const CreatureData* pData = GetCreatureData(m_creature->GetObjectGuid().GetCounter());
            m_creature->RelocateCreature(pData->posX, pData->posY, pData->posZ, pData->orientation);
        }
		
        m_creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NATURE, true);
        Reset();
        lSummonList.clear();

        m_creature->CastSpell(m_creature, SPELL_MARK_OF_NATURE_AURA, true);
    }

    uint32 m_uiEventCounter;

    uint32 m_uiSeepingFogTimer;
    uint32 m_uiNoxiousBreathTimer;
    uint32 m_uiTailsweepTimer;

    GUIDList lSummonList;

    void Reset()
    {

        m_uiEventCounter = 1;

        m_uiSeepingFogTimer = urand(15000, 20000);
        m_uiNoxiousBreathTimer = 8000;
        m_uiTailsweepTimer = 4000;

        m_creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NATURE, true);
        
        m_creature->CastSpell(m_creature, SPELL_MARK_OF_NATURE_AURA, true);
        
        if (!m_creature->isActiveObject() && m_creature->isAlive())
            m_creature->SetActiveObjectState(true);
    }

    void EnterCombat(Unit* pEnemy)
    {
        ScriptedAI::EnterCombat(pEnemy);
    }

    void JustReachedHome()
    {
        if (lSummonList.empty())
            return;

        for(GUIDList::iterator itr = lSummonList.begin(); itr != lSummonList.end(); ++itr)
        {
            if (Creature* pSummon = m_creature->GetMap()->GetAnyTypeCreature(*itr))
                pSummon->RemoveFromWorld();
        }

        lSummonList.clear();
    }

    void JustDied(Unit* /* pKiller */)
    {
        uint32 respawn_time = urand(259200, 432000); // A random respawn time between 3 days and 5 days.
        
        m_creature->SetRespawnDelay(respawn_time);
        m_creature->SetRespawnTime(respawn_time);
        m_creature->SaveRespawnTime();
    }

    void KilledUnit(Unit* pVictim)
    {
        // Mark killed players with Mark of Nature
        if (pVictim->GetTypeId() == TYPEID_PLAYER)
            pVictim->CastSpell(pVictim, SPELL_MARK_OF_NATURE_PLAYER, true, NULL, NULL, m_creature->GetObjectGuid());
    }

    void JustSummoned(Creature* pSummoned)
    {
        lSummonList.push_back(pSummoned->GetObjectGuid());

        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            pSummoned->AI()->AttackStart(pTarget);

        if (pSummoned->GetEntry() == NPC_DREAM_FOG)
            pSummoned->CastSpell(pSummoned, SPELL_DREAM_FOG, true, NULL, NULL, m_creature->GetObjectGuid());
    }

    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        lSummonList.remove(pSummoned->GetObjectGuid());
    }

    // Return true, if succeeded
    virtual bool DoSpecialDragonAbility() = 0;

    // Return true to handle shared timers and MeleeAttack
    virtual bool UpdateDragonAI(const uint32 /*uiDiff*/) { return true; }

    void UpdateAI(const uint32 uiDiff)
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Trigger special ability function at 75, 50 and 25% health
        if (m_creature->GetHealthPercent() < 100.0f - m_uiEventCounter * 25.0f && DoSpecialDragonAbility())
            ++m_uiEventCounter;

        // Call dragon specific virtual function
        if (!UpdateDragonAI(uiDiff))
            return;

        if (m_uiSeepingFogTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_SEEPING_FOG_R, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_SEEPING_FOG_L, CAST_TRIGGERED);
            m_uiSeepingFogTimer = urand(2*MINUTE*IN_MILLISECONDS, 3*MINUTE*IN_MILLISECONDS);
        }
        else
            m_uiSeepingFogTimer -= uiDiff;

        if (m_uiNoxiousBreathTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_NOXIOUS_BREATH) == CAST_OK)
                m_uiNoxiousBreathTimer = urand(14000, 20000);
        }
        else
            m_uiNoxiousBreathTimer -= uiDiff;

        if (m_uiTailsweepTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_TAILSWEEP) == CAST_OK)
                m_uiTailsweepTimer = 2000;
        }
        else
            m_uiTailsweepTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## boss_emeriss
######*/

enum eEmeriss
{
    SAY_EMERISS_AGGRO           = -1000401,
    SAY_CAST_CORRUPTION         = -1000402,

    SPELL_VOLATILE_INFECTION    = 24928,
    SPELL_CORRUPTION_OF_EARTH   = 24910,
    SPELL_PUTRID_MUSHROOM       = 24904,                    // Summons a mushroom on killing a player
	SPELL_SPORE_CLOUD			= 22948,					// used this for mushrooms?
	SPELL_SPORE_CLOUD1			= 24871,					// the one in DB, has mushroom as script target, this spell crashes server
    GO_PUTRID_MUSHROOM          = 180517,
};

struct MANGOS_DLL_DECL boss_emerissAI : public boss_emerald_dragonAI
{
    boss_emerissAI(Creature* pCreature) : boss_emerald_dragonAI(pCreature) { Reset(); }

    uint32 m_uiVolatileInfectionTimer;

    void Reset()
    {
        boss_emerald_dragonAI::Reset();

        m_uiVolatileInfectionTimer = 12000;
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(SAY_EMERISS_AGGRO, m_creature);
    }

    void KilledUnit(Unit* pVictim)
    {
        // summon a mushroom on the spot the player dies
        if (pVictim->GetTypeId() == TYPEID_PLAYER)
            pVictim->CastSpell(pVictim, SPELL_PUTRID_MUSHROOM, true, NULL, NULL, m_creature->GetObjectGuid());

        boss_emerald_dragonAI::KilledUnit(pVictim);
    }

    // Corruption of Earth at 75%, 50% and 25%
    bool DoSpecialDragonAbility()
    {
        m_creature->CastSpell(m_creature->getVictim(), SPELL_CORRUPTION_OF_EARTH, true);
        
        DoScriptText(SAY_CAST_CORRUPTION, m_creature);

        return true;
    }

    void JustReachedHome()
    {
        std::list<GameObject*> lMushrooms;
        GetGameObjectListWithEntryInGrid(lMushrooms, m_creature, GO_PUTRID_MUSHROOM, MAX_VISIBILITY_DISTANCE);

        if (lMushrooms.empty())
            return;

        for(std::list<GameObject*>::iterator itr = lMushrooms.begin(); itr != lMushrooms.end(); ++itr)
        {
            if (*itr)
                (*itr)->RemoveFromWorld();
        }
    }

    bool UpdateDragonAI(const uint32 uiDiff)
    {
        // Volatile Infection Timer
        if (m_uiVolatileInfectionTimer < uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            if (pTarget && DoCastSpellIfCan(pTarget, SPELL_VOLATILE_INFECTION) == CAST_OK)
                m_uiVolatileInfectionTimer = urand(7000, 12000);
        }
        else
            m_uiVolatileInfectionTimer -= uiDiff;

        return true;
    }
};

CreatureAI* GetAI_boss_emeriss(Creature* pCreature)
{
    return new boss_emerissAI(pCreature);
}

/*######
## boss_lethon
######*/

enum eLethon
{
    SAY_LETHON_AGGRO            = -1000285,
    SAY_DRAW_SPIRIT             = -1000286,

    SPELL_SHADOW_BOLT_WHIRL     = 24834,                    // Periodic aura
    SPELL_DRAW_SPIRIT           = 24811,
    SPELL_SUMMON_SPIRIT_SHADE   = 24810,                    // Summon spell was removed, was SPELL_EFFECT_SUMMON_DEMON

    NPC_SPIRIT_SHADE            = 15261,                    // Add summoned by Lethon
    SPELL_DARK_OFFERING         = 24804,
    SPELL_SPIRIT_SHADE_VISUAL   = 24809,
};

struct MANGOS_DLL_DECL boss_lethonAI : public boss_emerald_dragonAI
{
    boss_lethonAI(Creature* pCreature) : boss_emerald_dragonAI(pCreature) { Reset(); }

    void Reset()
    {
        boss_emerald_dragonAI::Reset();
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(SAY_LETHON_AGGRO, m_creature);
        // Shadow bolt wirl is a periodic aura which triggers a set of shadowbolts every 2 secs; may need some core tunning
        DoCastSpellIfCan(m_creature, SPELL_SHADOW_BOLT_WHIRL, CAST_TRIGGERED);
    }

    void ReleaseSpirit(Player* pPlayer)
    {
        float fX, fY, fZ, fO;
        pPlayer->GetPosition(fX, fY, fZ);
        fO = pPlayer->GetOrientation();
        if (Creature* pShade = m_creature->SummonCreature(NPC_SPIRIT_SHADE, fX, fY, fZ, fO, TEMPSUMMON_DEAD_DESPAWN, 0))
        {
            pShade->SetVisibility(VISIBILITY_OFF);
            pShade->CastSpell(pShade, SPELL_SPIRIT_SHADE_VISUAL, true);					// looks weird in combat log
            pShade->SetVisibility(VISIBILITY_ON);
            pShade->SetDisplayId(pPlayer->GetDisplayId());
            pShade->GetMotionMaster()->MoveFollow(m_creature, 1.0f, float(M_PI/2));
        }
    }

    // Summon a spirit which moves toward the boss and heals him for each player hit by the spell; used at 75%, 50% and 25%
    bool DoSpecialDragonAbility()
    {
        if (DoCastSpellIfCan(m_creature, SPELL_DRAW_SPIRIT) == CAST_OK)
        {
            DoScriptText(SAY_DRAW_SPIRIT, m_creature);

            GUIDVector vGuids;
            m_creature->FillGuidsListFromThreatList(vGuids);
            Unit* pTarget = m_creature->getVictim();
            Group* pGroup = (pTarget && pTarget->GetTypeId() == TYPEID_PLAYER) ? ((Player*)pTarget)->GetGroup() : NULL;

            if (pGroup && pGroup->isRaidGroup())
            {
                for(GroupReference* pRef = pGroup->GetFirstMember(); pRef != NULL; pRef = pRef->next())
                {
                    if (Player* pMember = pRef->getSource())
                        ReleaseSpirit(pMember);
                }
            }
            else if (!vGuids.empty())
            {
                for (GUIDVector::const_iterator itr = vGuids.begin(); itr != vGuids.end(); ++itr)
                {
                    Unit* pUnit = m_creature->GetMap()->GetUnit(*itr);

                    if (pUnit && pUnit->GetTypeId() == TYPEID_PLAYER)
                        ReleaseSpirit((Player*)pUnit);
                }
            }
            return true;
        }

        return false;
    }
	
    bool UpdateDragonAI(const uint32 /*uiDiff*/)
    {
		if (!m_creature->HasAura(SPELL_SHADOW_BOLT_WHIRL))
			DoCastSpellIfCan(m_creature, SPELL_SHADOW_BOLT_WHIRL, CAST_TRIGGERED);		// reapply aura if boss somehow doesn't have it

        return true;
    }
};

CreatureAI* GetAI_boss_lethon(Creature* pCreature)
{
    return new boss_lethonAI(pCreature);
}

struct MANGOS_DLL_DECL mob_spirit_shadeAI : public ScriptedAI
{
    mob_spirit_shadeAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);
        m_creature->SetAOEImmunity(true);
        Reset();
    }

    uint32 m_uiLethonCheck;
    ObjectGuid m_uiLethonGUID;

    void Reset()
    {
        m_uiLethonCheck = 1000;

        if (m_creature->IsTemporarySummon())
            m_uiLethonGUID = ((TemporarySummon*)m_creature)->GetSummonerGuid();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Dark Offering
        if (m_uiLethonCheck < uiDiff)
        {
            Creature* pLethon = m_creature->GetMap()->GetCreature(m_uiLethonGUID);
            if (pLethon && pLethon->IsWithinDistInMap(m_creature, 2.0f))
            {
                DoCastSpellIfCan(pLethon, SPELL_DARK_OFFERING, CAST_TRIGGERED);
                m_creature->ForcedDespawn();
            }
            else
                m_uiLethonCheck = 500;
        }
        else
            m_uiLethonCheck -= uiDiff;
    }
};

CreatureAI* GetAI_mob_spirit_shade(Creature* pCreature)
{
    return new mob_spirit_shadeAI(pCreature);
}

/*######
## boss_taerar
######*/

enum eTaerar
{
    SAY_TAERAR_AGGRO        = -1000399,
    SAY_SUMMONSHADE         = -1000400,

    SPELL_ARCANE_BLAST      = 24857,
    SPELL_BELLOWING_ROAR    = 22686,

    SPELL_SUMMON_SHADE_1    = 24841,
    SPELL_SUMMON_SHADE_2    = 24842,
    SPELL_SUMMON_SHADE_3    = 24843,
    SPELL_SELF_STUN         = 24883,                        // Stunns the main boss until the shades are dead or timer expires

    NPC_SHADE_OF_TAERAR     = 15302,
    SPELL_POSION_CLOUD      = 24840,
    SPELL_POSION_BREATH     = 20667
};

struct MANGOS_DLL_DECL boss_taerarAI : public boss_emerald_dragonAI
{
    boss_taerarAI(Creature* pCreature) : boss_emerald_dragonAI(pCreature) { Reset(); }

    uint32 m_uiArcaneBlastTimer;
    uint32 m_uiBellowingRoarTimer;
    uint32 m_uiShadesTimeoutTimer;
    uint8 m_uiShadesDead;

    void Reset()
    {
        boss_emerald_dragonAI::Reset();

        m_uiArcaneBlastTimer = 12000;
        m_uiBellowingRoarTimer = 30000;
        m_uiShadesTimeoutTimer = 0;                         // The time that Taerar is banished
        m_uiShadesDead = 0;

        // Remove Unselectable if needed
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(SAY_TAERAR_AGGRO, m_creature);
    }

    // Summon 3 Shades at 75%, 50% and 25% and Banish Self
    bool DoSpecialDragonAbility()
    {
        if (DoCastSpellIfCan(m_creature, SPELL_SELF_STUN) == CAST_OK)				// Double check so the shades doesn't randomly respawn.
        {
			float fX, fY, fZ;
				m_creature->GetPosition(fX, fY, fZ);
				for(uint8 i = 0; i < 3; ++i)
                    if (Creature* pShade = m_creature->SummonCreature(NPC_SHADE_OF_TAERAR, fX+irand(-2,2), fY+irand(-2,2), fZ, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000))
						if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
							pShade->AI()->AttackStart(pTarget);

           /* // Summon the shades at boss position
            DoCastSpellIfCan(m_creature, SPELL_SUMMON_SHADE_1, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_SUMMON_SHADE_2, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_SUMMON_SHADE_3, CAST_TRIGGERED);*/

            // Make boss not selectable when banished
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            DoScriptText(SAY_SUMMONSHADE, m_creature);
            m_uiShadesTimeoutTimer = 60000;

            return true;
        }

        return false;
    }

    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_SHADE_OF_TAERAR)
        {
            ++m_uiShadesDead;

            // If all shades are dead then unbanish the boss
            if (m_uiShadesDead == 3)
                DoUnbanishBoss();
        }
    }

    void DoUnbanishBoss()
    {
        m_creature->RemoveAurasDueToSpell(SPELL_SELF_STUN);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        m_uiShadesTimeoutTimer = 0;
        m_uiShadesDead = 0;
    }

    bool UpdateDragonAI(const uint32 uiDiff)
    {
        // Timer to unbanish the boss
        if (m_uiShadesTimeoutTimer)
        {
            if (m_uiShadesTimeoutTimer <= uiDiff)
               DoUnbanishBoss();
            else
                m_uiShadesTimeoutTimer -= uiDiff;

            // Prevent further spells or timer handling while banished
            return false;
        }

        // Arcane Blast Timer
        if (m_uiArcaneBlastTimer < uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            if (pTarget) 
            {
                SpellAuraHolder* pHolder = pTarget->GetSpellAuraHolder(24778);
                if (pHolder)
                    pTarget->RemoveSpellAuraHolder(pHolder, AURA_REMOVE_BY_CANCEL);

                if (DoCastSpellIfCan(pTarget, SPELL_ARCANE_BLAST) == CAST_OK)
                    m_uiArcaneBlastTimer = urand(7000, 12000);
            }
        }
        else
            m_uiArcaneBlastTimer -= uiDiff;

        // Bellowing Roar Timer
        if (m_uiBellowingRoarTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BELLOWING_ROAR) == CAST_OK)
                m_uiBellowingRoarTimer = urand(20000, 30000);
        }
        else
            m_uiBellowingRoarTimer -= uiDiff;

        return true;
    }
};

// Shades of Taerar Script
struct MANGOS_DLL_DECL mob_shade_of_taerarAI : public ScriptedAI
{
    mob_shade_of_taerarAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{
		m_creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NATURE, true);		// should they be immune to nature?
		Reset();
	}

    uint32 m_uiPoisonCloudTimer;
    uint32 m_uiPosionBreathTimer;

    void Reset()
    {
        m_uiPoisonCloudTimer = 8000;
        m_uiPosionBreathTimer = 12000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Poison Cloud Timer
        if (m_uiPoisonCloudTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_POSION_CLOUD);
            m_uiPoisonCloudTimer = urand(20000, 30000);
        }
        else
            m_uiPoisonCloudTimer -= uiDiff;

        // Posion Breath Timer
        if (m_uiPosionBreathTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_POSION_BREATH);
            m_uiPosionBreathTimer = 12000;
        }
        else
            m_uiPosionBreathTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_taerar(Creature* pCreature)
{
    return new boss_taerarAI(pCreature);
}

CreatureAI* GetAI_mob_shade_of_taerar(Creature* pCreature)
{
    return new mob_shade_of_taerarAI(pCreature);
}

/*######
## boss_ysondre
######*/

enum eYsondre
{
    SAY_YSONDRE_AGGRO       = -1000360,
    SAY_SUMMON_DRUIDS       = -1000361,

    SPELL_LIGHTNING_WAVE    = 24819,
    SPELL_SUMMON_DRUIDS     = 24795,

    // Druid spells
    SPELL_CURSE_OF_THORNS           = 16247,
    SPELL_MOONFIRE                  = 24957,        // 21669,
    SPELL_SILENCE                   = 6726,

    NPC_DEMENTED_DRUID_SPIRIT       = 15260
};

// Ysondre script
struct MANGOS_DLL_DECL boss_ysondreAI : public boss_emerald_dragonAI
{
    boss_ysondreAI(Creature* pCreature) : boss_emerald_dragonAI(pCreature) {Reset(); }

    uint32 m_uiLightningWaveTimer;

    void Reset()
    {
        boss_emerald_dragonAI::Reset();

        m_uiLightningWaveTimer = 12000;
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(SAY_YSONDRE_AGGRO, m_creature);
    }

    // Summon Druids - TODO FIXME (spell not understood)
    bool DoSpecialDragonAbility()
    {
        DoScriptText(SAY_SUMMON_DRUIDS, m_creature);

        for(int i = 0; i < 10; ++i)
            DoSpawnCreature(NPC_DEMENTED_DRUID_SPIRIT, irand(-5,5), irand(-5,5), 0, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 10*MINUTE*IN_MILLISECONDS);
            //DoCastSpellIfCan(m_creature, SPELL_SUMMON_DRUIDS, CAST_TRIGGERED);

        return true;
    }

    bool UpdateDragonAI(const uint32 uiDiff)
    {
        // Lightning Wave Timer
        if (m_uiLightningWaveTimer < uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            if (pTarget && DoCastSpellIfCan(pTarget, SPELL_LIGHTNING_WAVE) == CAST_OK)
                m_uiLightningWaveTimer = urand(7000, 12000);
        }
        else
            m_uiLightningWaveTimer -= uiDiff;

        return true;
    }
};

CreatureAI* GetAI_boss_ysondre(Creature* pCreature)
{
    return new boss_ysondreAI(pCreature);
}

// Summoned druid script
struct MANGOS_DLL_DECL mob_demented_druidAI : public ScriptedAI
{
    mob_demented_druidAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{
		m_creature->SetAOEImmunity(true);
		Reset();
	}

    uint32 m_uiCurseOfThornsTimer;
    uint32 m_uiMoonfireTimer;
    uint32 m_uiSilenceTimer;

    void Reset()
    {
        m_uiCurseOfThornsTimer = 10000;
        m_uiMoonfireTimer = 3000;
        m_uiSilenceTimer = 15000;
    }

    void JustReachedHome()
    {
        m_creature->RemoveFromWorld();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Curse of Thorns
        if (m_uiCurseOfThornsTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CURSE_OF_THORNS);
            m_uiCurseOfThornsTimer = 20000;
        }
        else
            m_uiCurseOfThornsTimer -= uiDiff;

        // Moonfire
        if (m_uiMoonfireTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MOONFIRE);
            m_uiMoonfireTimer = urand(7000,9000);
        }
        else
            m_uiMoonfireTimer -= uiDiff;

        // Silence
        if (m_uiSilenceTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SILENCE);
            m_uiSilenceTimer = urand(10000,13000);
        }
        else
            m_uiSilenceTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_demented_druid(Creature* pCreature)
{
    return new mob_demented_druidAI(pCreature);
}

struct MANGOS_DLL_DECL mob_dream_fogAI : public ScriptedAI
{
    mob_dream_fogAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiRoamTimer;
    ObjectGuid BossGuid;

    void Reset()
    {
        m_uiRoamTimer = 0;
		m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

        if (m_creature->IsTemporarySummon())
            BossGuid = ((TemporarySummon*)m_creature)->GetSummonerGuid();
    }

    void UpdateAI(uint32 const uiDiff)
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiRoamTimer < uiDiff)
        {
            // Chase target, but don't attack - otherwise just roam around
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                m_uiRoamTimer = urand(15000, 30000);
                m_creature->GetMotionMaster()->Clear(false);
                m_creature->GetMotionMaster()->MoveChase(pTarget, 0.2f);
            }
            else
            {
                m_uiRoamTimer = 2500;
                m_creature->GetMotionMaster()->Clear(false);
                m_creature->GetMotionMaster()->MoveRandom();
            }

            // Seeping fog movement is slow enough for a player to be able to walk backwards and still outpace it
			// Changed speed in DB instead************************
            //m_creature->AddSplineFlag(SPLINEFLAG_WALKMODE);
            //m_creature->SetSpeedRate(MOVE_WALK, 0.3f);			// 0.75 before, not working still fast speed.
        }
        else
            m_uiRoamTimer -= uiDiff;
    }
};

CreatureAI* GetAI_mob_dream_fog(Creature* pCreature)
{
    return new mob_dream_fogAI(pCreature);
}

void AddSC_bosses_emerald_dragons()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_emeriss";
    pNewScript->GetAI = &GetAI_boss_emeriss;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_lethon";
    pNewScript->GetAI = &GetAI_boss_lethon;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_spirit_shade";
    pNewScript->GetAI = &GetAI_mob_spirit_shade;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_taerar";
    pNewScript->GetAI = &GetAI_boss_taerar;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_shade_of_taerar";
    pNewScript->GetAI = &GetAI_mob_shade_of_taerar;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_ysondre";
    pNewScript->GetAI = &GetAI_boss_ysondre;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_demented_druid";
    pNewScript->GetAI = &GetAI_mob_demented_druid;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_dream_fog";
    pNewScript->GetAI = &GetAI_mob_dream_fog;
    pNewScript->RegisterSelf();
}
