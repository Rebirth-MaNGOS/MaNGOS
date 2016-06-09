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
    SPELL_POISON_BOLT			= 21067,					// should only do this as visual, should be possible to target totems and such. REDO THIS! Should not do dmg this is just to get the cast animation + projectile

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

    NPC_GLOB_OF_VISCIDUS        = 15667,
    NPC_TOXIN_CLOUD				= 152990
};

struct MANGOS_DLL_DECL boss_viscidusAI : public ScriptedAI
{
    boss_viscidusAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NATURE, true);
        m_creature->CastSpell(m_creature, SPELL_MEMBRANE_VISCIDUS, true); // add dmg reduction
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
    uint32 m_uiPoisonBoltCastTimer;

    ObjectGuid m_PoisonTargetGuid;

    float globs;
    uint32 globCounter;
    int m_globSpawned;

    uint32 m_health;
    GUIDList m_lToxinClouds;

    void Reset()
    {
        m_globSpawned = 0;
        m_uiGlobCount = 0;
        m_uiGlobSpawnTimer = 0;
        m_uiSetInvisTimer = 4000;
        m_uiSetVisibleTimer = 0;
        m_uiPoisonVolleyTimer = 10000;									// timer confirmed
        m_uiPoisonShockTimer = 11000;
        m_uiToxicCloudTimer = urand(30000,40000);
        m_uiPoisonBoltCastTimer = 0;
        m_uiThawTimer = 20000;
        m_bSummoned = false;
        globCounter = 0;

        RemoveToxinClouds();
        RemoveAuras();
        ResetBool(0);
        ResetBool(1);
        SetVisible(1);
        m_lToxinClouds.clear();
        m_creature->SetObjectScale(1.2f);		// has to reset scale here
        m_creature->UpdateModelData();
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

    void JustDied(Unit* /*pKiller*/)							// Remove all clouds when he dies
    {
        RemoveToxinClouds();
    }

    void RemoveToxinClouds()
    {
        for (GUIDList::iterator itr = m_lToxinClouds.begin(); itr != m_lToxinClouds.end(); itr++)
        {
            if (Creature* pToxinCloud = m_creature->GetMap()->GetCreature(*itr))
                if (pToxinCloud->isAlive())
                    pToxinCloud->ForcedDespawn(urand(10000,20000));		// make them appear a bit more randomly despawning
        }
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)		// summon the poison cloud under the player but we don't want the player to take any dmg
    {
        if (pSpell->Id == SPELL_POISON_BOLT)
        {
            if(pTarget->HasAura(SPELL_POISON_BOLT))
                pTarget->RemoveAurasDueToSpell(SPELL_POISON_BOLT);
            if(Creature* pToxin = m_creature->SummonCreature(NPC_TOXIN_CLOUD,pTarget->GetPositionX(),pTarget->GetPositionY(),pTarget->GetPositionZ(),0,TEMPSUMMON_TIMED_DESPAWN,1800000,false))
                m_lToxinClouds.push_back(pToxin->GetObjectGuid());		// max 30 min alive, else removed when boss dies/reset
        }
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

    void KillViscidus()
    {
        if(Unit* pTarget = GetRandomPlayerInCurrentMap(100))
        {                   
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);  
            m_creature->SetLootRecipient(pTarget);            
        }
    }
    
    void MeleeHitCount()
    {
        if(m_uiMeleeCounter >= 1 && !m_bCracking1)		// 25, 50, 75?
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
                    KillViscidus();
                
                m_bCanDoDamage = false;
                m_bExploded = true;
                m_creature->RemoveAllAuras(AuraRemoveMode::AURA_REMOVE_BY_DEFAULT);
                m_creature->CastSpell(m_creature, SPELL_VISCIDUS_EXPLODE,true);
                m_creature->GenericTextEmote("Viscidus explodes.", NULL, false);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                // Clear the target to prevent him from rotating.
                m_creature->SetTargetGuid(ObjectGuid());

                // Set his health to zero to trigger the death animation.
                m_health = m_creature->GetHealth();
                m_creature->SetHealth(0);

                m_uiSetInvisTimer = 1700;
                m_uiGlobSpawnTimer = 4000;			// slight delay before we spawn the adds
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
            m_creature->SetUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_NONE);
        }
        else if (Visible == 1)
        {
            ResetBool(1);			// reset all counters for melee here
            ResetBool(0);			// reset all counters for spells here
            m_creature->SetVisibility(VISIBILITY_ON);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);			
            m_creature->RelocateCreature(-7991.48f, 920.19f, -52.91f, 0.f);
            
            // kill on set visible if killed all globs that wave
            if(m_globSpawned > 0 && m_globSpawned <= globCounter)
                KillViscidus();
            else  
                m_creature->SetHealth(m_health - globCounter * 0.05f * m_creature->GetMaxHealth());

            if (m_creature->getVictim())
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
        }		
    }

    void SpawnGlobs()
    {
        // reset how many spawned
        m_globSpawned = 0;
        
        //spawn all adds here
        globs = ceil(((float) m_creature->GetHealth() / (float) m_creature->GetMaxHealth()) / 0.05f);
        for (float angle = 0; angle < 2 * 3.141592654; angle += 2 * 3.141592654 / globs)
        {
            m_creature->SummonCreature(NPC_GLOB_OF_VISCIDUS, -7990.f + 50.f * cosf(angle),
                    925.f + 50.f * sinf(angle),-42.f, 0.f,
                    TEMPSUMMON_DEAD_DESPAWN, 0, true);
            ++m_globSpawned;
        }
        globCounter = 0;
    }

    void JustSummoned(Creature* pSummoned)
    {
        if(pSummoned->GetEntry() == NPC_GLOB_OF_VISCIDUS)
            pSummoned->GetMotionMaster()->MovePoint(1,-7991.48f,920.19f,-52.91f);		// a point in the middle of the room
        pSummoned->SetRespawnEnabled(false);			// make sure they won't respawn
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiMotionType, uint32 uiPointId)				// something like this?
    {
        if(uiPointId == 1)
        {
            pSummoned->CastSpell(m_creature, SPELL_REJOIN_VISCIDUS, true);
            pSummoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            pSummoned->ForcedDespawn(50);
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_GLOB_OF_VISCIDUS)
            ++globCounter;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bFrozen && !m_bExploded)
        {
            if (m_uiThawTimer <= uiDiff)			// remove all slows and reset counters
            {
                RemoveAuras();
                ResetBool(0);	
				ResetBool(1);
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
               
                SetVisible(1);
                
                // set scale 0.4f scale is the smallest             
                m_creature->SetObjectScale(m_creature->GetHealthPercent() * 0.0084 + 0.358);			// set Viscidus' size depending on the blobs that are alive 1/ too small?   
                m_creature->UpdateModelData();
                m_creature->RemoveAllAuras(AuraRemoveMode::AURA_REMOVE_BY_DEFAULT);
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
            if (m_uiPoisonBoltCastTimer)
            {
                if (m_uiPoisonBoltCastTimer > uiDiff)
                {
                    m_uiPoisonBoltCastTimer -= uiDiff;

                    Unit* pTarget = m_creature->GetMap()->GetUnit(m_PoisonTargetGuid);
                    if (m_uiPoisonBoltCastTimer < 500 && pTarget)
                    {
                        m_creature->SetTargetGuid(pTarget->GetObjectGuid());
                        m_creature->SetFacingToObject(pTarget);
                        return;
                    }
                }
                else
                {
                    if (m_creature->SelectHostileTarget() && m_creature->getVictim())
                        m_creature->SetTargetGuid(m_creature->getVictim()->GetObjectGuid());

                    m_uiPoisonBoltCastTimer = 0;
                    return;
                }
            }

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

            if (m_uiToxicCloudTimer <= uiDiff)				// redo this, should probably not cast a spell, just visually cast it. Missing that the boss should turn towards the victim during cast.
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    m_creature->CastSpell(pTarget, SPELL_POISON_BOLT, true); 
                    m_PoisonTargetGuid = pTarget->GetObjectGuid();
                    m_uiPoisonBoltCastTimer = 2800;
                }
                m_uiToxicCloudTimer = urand(30000,40000);
                return;
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
        ////Return since we have no target
        //if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        //    return;

        if (m_uiSpeedUpTimer <= uiDiff)				// increase the speed every half a sec
        {
			m_creature->UpdateSpeed(MOVE_RUN, m_creature->GetSpeedRate(MOVE_RUN)+0.15f);
			m_creature->UpdateVisibilityAndView();			// not really helping
            m_uiSpeedUpTimer = 500;
        }
        else
            m_uiSpeedUpTimer -= uiDiff;
        m_creature->SetTargetGuid(ObjectGuid());				// target self even when someone does dmg
    }
};

CreatureAI* GetAI_boss_glob_of_viscidus(Creature* pCreature)
{
    return new boss_glob_of_viscidusAI(pCreature);
}

/*######
## mob_toxin_cloud
######*/

struct MANGOS_DLL_DECL mob_toxin_cloudAI : public ScriptedAI
{
    mob_toxin_cloudAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        m_uiFirstTick = 3000;
        Reset();
    }
    uint32 m_uiFirstTick;
    bool m_bFirstTick;
    void Reset()
    {
    }

    void MoveInLineOfSight(Unit* pWho)
    {
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
        if(!m_bFirstTick)
            if (m_uiFirstTick <= uiDiff)
            {
                m_creature->CastSpell(m_creature, SPELL_TOXIN, true);
                m_creature->CastSpell(m_creature, SPELL_TOXIN_CLOUD, true);			// do the first tick here, else we have to wait 10 sec before we notice the cloud
                m_bFirstTick = true;
            }
            else
                m_uiFirstTick -= uiDiff;

        // no meele
    }
};

CreatureAI* GetAI_mob_toxin_cloud(Creature* pCreature)
{
    return new mob_toxin_cloudAI(pCreature);
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

    pNewscript = new Script;
    pNewscript->Name = "mob_toxin_cloud";
    pNewscript->GetAI = &GetAI_mob_toxin_cloud;
    pNewscript->RegisterSelf();
}
