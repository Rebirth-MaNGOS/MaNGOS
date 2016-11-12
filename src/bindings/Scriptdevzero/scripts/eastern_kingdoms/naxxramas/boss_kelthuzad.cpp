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
SDName: Boss_KelThuzud
SD%Complete: 75
SDComment: Timers will need adjustments, along with tweaking positions and amounts
SDCategory: Naxxramas
EndScriptData */

// some not answered questions:
// - will intro mobs, not sent to center, despawn when phase 2 start?
// - what happens if raid fail, can they start the event as soon after as they want?

#include "precompiled.h"
#include "naxxramas.h"

enum
{
    //when shappiron dies. dialog between kel and lich king (in this order)
    SAY_SAPP_DIALOG1                    = -1533084,
    SAY_SAPP_DIALOG2_LICH               = -1533085,
    SAY_SAPP_DIALOG3                    = -1533086,
    SAY_SAPP_DIALOG4_LICH               = -1533087,
    SAY_SAPP_DIALOG5                    = -1533088,

    //when cat dies
    SAY_CAT_DIED                        = -1533089,

    //when each of the 4 wing bosses dies
    SAY_TAUNT1                          = -1533090,
    SAY_TAUNT2                          = -1533091,
    SAY_TAUNT3                          = -1533092,
    SAY_TAUNT4                          = -1533093,

    SAY_SUMMON_MINIONS                  = -1533105,         //start of phase 1

    EMOTE_PHASE2                        = -1533135,         //start of phase 2
    SAY_AGGRO1                          = -1533094,
    SAY_AGGRO2                          = -1533095,
    SAY_AGGRO3                          = -1533096,

    SAY_SLAY1                           = -1533097,
    SAY_SLAY2                           = -1533098,

    SAY_DEATH                           = -1533099,

    SAY_CHAIN1                          = -1533100,
    SAY_CHAIN2                          = -1533101,
    SAY_FROST_BLAST                     = -1533102,

    SAY_REQUEST_AID                     = -1533103,         //start of phase 3
    SAY_ANSWER_REQUEST                  = -1533104,         //lich king answer

    SAY_SPECIAL1_MANA_DET               = -1533106,
    SAY_SPECIAL3_MANA_DET               = -1533107,
    SAY_SPECIAL2_DISPELL                = -1533108,

    EMOTE_GUARDIAN                      = -1533134,         // at each guardian summon

    //spells to be casted
    SPELL_FROST_BOLT          = 28478,
    SPELL_FROST_BOLT_NOVA     = 28479,

    SPELL_CHAINS_OF_KELTHUZAD           = 28408,            // 3.x, heroic only
    SPELL_CHAINS_OF_KELTHUZAD_TARGET    = 28410,

    SPELL_MANA_DETONATION               = 27819,
    SPELL_SHADOW_FISSURE                = 27810,
    SPELL_VOID_BLAST                        = 27812,
    SPELL_FROST_BLAST                   = 27808,
    
    SPELL_CHANNEL_VISUAL                = 29423,
    NPC_SHADOW_FISSURE                  = 16129,
    MAX_SHACKLE                                 = 3
};

static float M_F_ANGLE = 0.2f;                              // to adjust for map rotation
static float M_F_HEIGHT = 3.0f;                             // adjust for height difference
static float M_F_RANGE = 60.0f;                             // ~ range from center of chamber to center of alcove

struct MANGOS_DLL_DECL boss_kelthuzadAI : public ScriptedAI
{
    boss_kelthuzadAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_naxxramas*)pCreature->GetInstanceData();
        m_uiGuardiansCount = 0;
        m_uiGuardiansCountMax = 5;
        Reset();
    }

    instance_naxxramas* m_pInstance;

    ObjectGuid m_auiGuardiansGUID[5];
    uint32 m_uiGuardiansCount;
    uint32 m_uiGuardiansCountMax;
    uint32 m_uiGuardiansTimer;
    uint32 m_uiCheckShackleTimer;
    uint32 m_uiFrostBoltTimer;
    uint32 m_uiFrostBoltNovaTimer;
    uint32 m_uiChainsTimer;
    uint32 m_uiManaDetonationTimer;
    uint32 m_uiShadowFissureTimer;
    uint32 m_uiFrostBlastTimer;

    uint32 m_uiPhase1Timer;
    uint32 m_uiSoldierTimer;
    uint32 m_uiUndeadTimer;
    bool m_bSummonedIntro;
    bool m_bIsPhase3;

    GUIDSet m_lSoldierSet;                         // keeps explicit guids of intro soldiers
    GUIDSet m_lUndeadSet;                          // the rest of the intro mobs

    void Reset()
    {
        m_uiFrostBoltTimer = urand(1000, 60000);            //It won't be more than a minute without cast it
        m_uiFrostBoltNovaTimer = 15000;                     //Cast every 15 seconds
        m_uiChainsTimer = urand(30000, 60000);              //Cast no sooner than once every 30 seconds
        m_uiManaDetonationTimer = 20000;                    //Seems to cast about every 20 seconds
        m_uiShadowFissureTimer = 25000;                     //25 seconds
        m_uiFrostBlastTimer = urand(30000, 60000);          //Random time between 30-60 seconds
        m_uiGuardiansTimer = 5000;                          //5 seconds for summoning each Guardian of Icecrown in phase 3
        m_uiCheckShackleTimer = 10000;
        
        for(int i=0; i<5; ++i)
        {
            if (m_auiGuardiansGUID[i])
            {
                //delete creature
                if (Creature* pGuardian = m_creature->GetMap()->GetCreature(m_auiGuardiansGUID[i]))
                {
                    if (pGuardian->isAlive())
                        pGuardian->ForcedDespawn();
                }

				m_auiGuardiansGUID[i].Clear();
            }
        }

        m_uiPhase1Timer = 10000;//228000;                           //Phase 1 lasts "3 minutes and 48 seconds"
        m_uiSoldierTimer = 5000;
        m_uiUndeadTimer = 5000;
        m_bSummonedIntro = false;
        m_bIsPhase3 = false;

        // it may be some spell should be used instead, to control the intro phase
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void KilledUnit(Unit* /*pVictim*/)
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY1 : SAY_SLAY2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        DoScriptText(SAY_DEATH, m_creature);

        for(int i=0; i<5; ++i)
        {
            if (m_auiGuardiansGUID[i])
            {
                Creature* pGuardian = m_creature->GetMap()->GetCreature(m_auiGuardiansGUID[i]);

                if (!pGuardian || !pGuardian->isAlive())
                    continue;

                pGuardian->AI()->ResetToHome();
            }
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_KELTHUZAD, DONE);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit* /*pWho*/)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
        }
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KELTHUZAD, FAIL);

        DespawnAllIntroCreatures();
    }

    void DespawnAllIntroCreatures()
    {
        if (!m_lSoldierSet.empty())
        {
            for(GUIDSet::iterator itr = m_lSoldierSet.begin(); itr != m_lSoldierSet.end(); ++itr)
            {
                if (Creature* pSoldier = m_pInstance->instance->GetCreature(*itr))
                    pSoldier->ForcedDespawn();
            }
        }

        if (!m_lUndeadSet.empty())
        {
            for(GUIDSet::iterator itr = m_lUndeadSet.begin(); itr != m_lUndeadSet.end(); ++itr)
            {
                if (Creature* pSoldier = m_pInstance->instance->GetCreature(*itr))
                    pSoldier->ForcedDespawn();
            }
        }

        m_lSoldierSet.clear();
        m_lUndeadSet.clear();
    }

    void DespawnIntroCreaturesNonCombat()
    {
        // Only despawn the ones not sent and not already in combat
        if (!m_lSoldierSet.empty())
        {
            for(GUIDSet::iterator itr = m_lSoldierSet.begin(); itr != m_lSoldierSet.end(); ++itr)
            {
                if (Creature* pSoldier = m_pInstance->instance->GetCreature(*itr))
                {
                    // won't despawn all the adds
                    if(!pSoldier->isInCombat() && pSoldier->GetMotionMaster()->GetCurrentMovementGeneratorType() != POINT_MOTION_TYPE)
                    {
                        pSoldier->ForcedDespawn();
                        m_lSoldierSet.erase(pSoldier->GetObjectGuid());
                    }
                }
            }
        }

        if (!m_lUndeadSet.empty())
        {
            for(GUIDSet::iterator itr = m_lUndeadSet.begin(); itr != m_lUndeadSet.end(); ++itr)
            {
                if (Creature* pSoldier = m_pInstance->instance->GetCreature(*itr))
                {
                    // won't despawn all the adds
                    if(!pSoldier->isInCombat() && pSoldier->GetMotionMaster()->GetCurrentMovementGeneratorType() != POINT_MOTION_TYPE)
                    {
                        pSoldier->ForcedDespawn();
                        m_lUndeadSet.erase(pSoldier->GetObjectGuid());
                    }
                }
            }
        }
    }
    
    float GetLocationAngle(uint32 uiId)
    {
        switch(uiId)
        {
            case 1: return M_PI_F - M_F_ANGLE;              // south
            case 2: return (M_PI_F / 2) * 3 - M_F_ANGLE;    // east
            case 3: return M_PI_F / 2 - M_F_ANGLE;          // west
            case 4: return M_PI_F / 4 - M_F_ANGLE;          // north-west
            case 5: return (M_PI_F / 4) * 7 - M_F_ANGLE;    // north-east
            case 6: return (M_PI_F / 4) * 5 - M_F_ANGLE;    // south-east
            case 7: return 3*M_PI_F / 4 - M_F_ANGLE;        // south-west
        }

        return M_F_ANGLE;
    }

    void SummonIntroStart()
    {
        if (!m_pInstance)
            return;

        for(int i = 0; i < 7; ++i)
        {
            float fAngle = GetLocationAngle(i+1);

            float fx, fy, fz;
            m_pInstance->GetChamberCenterCoords(fx, fy, fz);

            fx += M_F_RANGE * cos(fAngle);
            fy += M_F_RANGE * sin(fAngle);
            fz += M_F_HEIGHT;

            MaNGOS::NormalizeMapCoord(fx);
            MaNGOS::NormalizeMapCoord(fy);

            for(int i = 0; i < 14; ++i)
            {
                uint32 uiNpcEntry = NPC_SOUL_WEAVER;

                if (i > 0)
                {
                    if (i < 4)
                        uiNpcEntry = NPC_UNSTOPPABLE_ABOM;
                    else
                        uiNpcEntry = NPC_SOLDIER_FROZEN;
                }

                float ffx, ffy, ffz;
                m_creature->GetRandomPoint(fx, fy, fz, 12.0f, ffx, ffy, ffz);

                m_creature->SummonCreature(uiNpcEntry, ffx, ffy, ffz, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 5000);
            }
        }
    }

    void SummonGuardian()
    {
        if (!m_pInstance)
            return;

        float fAngle = GetLocationAngle(urand(1,7));

        float fx, fy, fz;
        m_pInstance->GetChamberCenterCoords(fx, fy, fz);

        fx += M_F_RANGE * cos(fAngle);
        fy += M_F_RANGE * sin(fAngle);
        fz += M_F_HEIGHT;

        MaNGOS::NormalizeMapCoord(fx);
        MaNGOS::NormalizeMapCoord(fy);

        m_creature->SummonCreature(NPC_GUARDIAN, fx, fy, fz, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
    }

    void JustSummoned(Creature* pSummoned)
    {
        switch(pSummoned->GetEntry())
        {
            case NPC_GUARDIAN:
            {
                DoScriptText(EMOTE_GUARDIAN, m_creature);

                if (m_pInstance)
                {
                    float fx, fy, fz;
                    m_pInstance->GetChamberCenterCoords(fx, fy, fz);
                    pSummoned->GetMotionMaster()->MovePoint(0, fx, fy, fz);
                }

                //Safe storing of creatures
				m_auiGuardiansGUID[m_uiGuardiansCount] = pSummoned->GetObjectGuid();

                //Update guardian count
                ++m_uiGuardiansCount;
                break;
            }
            case NPC_SOLDIER_FROZEN:
                m_lSoldierSet.insert(pSummoned->GetObjectGuid());
                break;
            case NPC_UNSTOPPABLE_ABOM:
            case NPC_SOUL_WEAVER:
                m_lUndeadSet.insert(pSummoned->GetObjectGuid());
                break;
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        switch(pSummoned->GetEntry())
        {
            case NPC_SOLDIER_FROZEN:
                m_lSoldierSet.erase(pSummoned->GetObjectGuid());
                break;
            case NPC_UNSTOPPABLE_ABOM:
            case NPC_SOUL_WEAVER:
                m_lUndeadSet.erase(pSummoned->GetObjectGuid());
                break;
        }
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiMotionType, uint32 uiPointId)
    {
        if (uiMotionType == POINT_MOTION_TYPE && uiPointId == 0)
            pSummoned->SetInCombatWithZone();
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId)
    {
        if (uiMotionType == POINT_MOTION_TYPE && uiPointId == 0)
            m_creature->SetInCombatWithZone();
    }

    bool SendRandomSoldierToCenter()
    {
        GUIDSet::iterator itr = m_lSoldierSet.begin();

        uint32 uiPosition = urand(0, m_lSoldierSet.size()-1);
        advance(itr, uiPosition);

        if (*itr)
        {
            if (Creature* pSoldier = m_pInstance->instance->GetCreature(*itr))
            {
                if (pSoldier->getVictim())
                    return false;

                float fx, fy, fz;
                m_pInstance->GetChamberCenterCoords(fx, fy, fz);
                pSoldier->GetMotionMaster()->MovePoint(0, fx, fy, fz);
                return true;
            }
        }

        return false;
    }

    bool SendRandomUndeadToCenter()
    {
        GUIDSet::iterator itr = m_lUndeadSet.begin();

        uint32 uiPosition = urand(0, m_lUndeadSet.size()-1);

        advance(itr, uiPosition);

        if (*itr)
        {
            if (Creature* pUndead = m_pInstance->instance->GetCreature(*itr))
            {
                if (pUndead->getVictim())
                    return false;

                float fx, fy, fz;
                m_pInstance->GetChamberCenterCoords(fx, fy, fz);
                pUndead->GetMotionMaster()->MovePoint(0, fx, fy, fz);
                return true;
            }
        }

        return false;
    }
    
    void CastShadowFissure()
    {
        const ThreatList& threatList = m_creature->getThreatManager().getThreatList();
        std::vector<Unit*> pEligibleTargets;

        pEligibleTargets.clear();

        if(!threatList.empty())
        {
            for (HostileReference *currentReference : threatList)
            {
                Unit *target = currentReference->getTarget();
                if (target && target->isAlive() && target->GetTypeId() == TYPEID_PLAYER)
                    pEligibleTargets.push_back(target);
            }

            if(!pEligibleTargets.empty())
            {
                std::random_shuffle(pEligibleTargets.begin(), pEligibleTargets.end());
                Unit *target = pEligibleTargets.front();
                if (target && target->isAlive() && target->GetTypeId() == TYPEID_PLAYER)
                {
                    // When summoned it does damage but when spawned with spell it's visible
                    m_creature->SummonCreature(NPC_SHADOW_FISSURE, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 3200);
                    m_creature->CastSpell(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), SPELL_SHADOW_FISSURE, true); // Visually needed, above is invisible
                } 
            }
        }
    }
    
    void CheckShackle()
    {
        int ShackleCount = 0;
        // Loop through and check if more than 3 has Shackles aura
        for(int i=0; i<5; ++i)
        {
            if (m_auiGuardiansGUID[i])
            {
                if (Creature* pGuardian = m_creature->GetMap()->GetCreature(m_auiGuardiansGUID[i]))
                {
                    if (pGuardian->isAlive() && (pGuardian->HasAura(9484) || pGuardian->HasAura(9485) || pGuardian->HasAura(10955)))
                        ++ShackleCount;
                }                
            }
        }
        
        // If they do, then remove it from all
        if(ShackleCount > MAX_SHACKLE)
        {
            for(int i=0; i<5; ++i)
            {
                if (m_auiGuardiansGUID[i])
                {
                    if (Creature* pGuardian = m_creature->GetMap()->GetCreature(m_auiGuardiansGUID[i]))
                    {
                        if (pGuardian->isAlive() && (pGuardian->HasAura(9484) || pGuardian->HasAura(9485) || pGuardian->HasAura(10955)))
                        {
                            pGuardian->RemoveAurasDueToSpell(9484);
                            pGuardian->RemoveAurasDueToSpell(9485);
                            pGuardian->RemoveAurasDueToSpell(10955);
                        }
                    }                
                }
            }        
        }        
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
        {
            if (!m_pInstance)
                return;

            if (m_pInstance->GetData(TYPE_KELTHUZAD) != IN_PROGRESS)
                return;

            if (!m_bSummonedIntro)
            {
                m_bSummonedIntro = true;
                SummonIntroStart();
                
                // Cast channel visual, doesn't channel find out why
                m_creature->CastSpell(m_creature, SPELL_CHANNEL_VISUAL, true);
            }
            
            if (m_uiPhase1Timer < uiDiff)
            {
                if(m_creature->HasAura(SPELL_CHANNEL_VISUAL))
                    m_creature->RemoveAurasDueToSpell(SPELL_CHANNEL_VISUAL);
                
                // Remove all non-combat or already moving intro adds
                DespawnIntroCreaturesNonCombat();
                
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                SetCombatMovement(true);
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());

                DoScriptText(EMOTE_PHASE2, m_creature);
                
                switch (urand(0, 2))
                {
                    case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
                    case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
                    case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
                }
            }
            else
            {
                if (m_uiSoldierTimer < uiDiff)
                {
                    if (SendRandomSoldierToCenter())
                        m_uiSoldierTimer = 2000;
                }
                else
                    m_uiSoldierTimer -= uiDiff;

                if (m_uiUndeadTimer < uiDiff)
                {
                    if (SendRandomUndeadToCenter())
                        m_uiUndeadTimer = 10000;
                }
                else
                    m_uiUndeadTimer -= uiDiff;

                m_uiPhase1Timer -= uiDiff;
            }

            return;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiFrostBoltTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FROST_BOLT);
            m_uiFrostBoltTimer = urand(1000, 60000);
        }
        else
            m_uiFrostBoltTimer -= uiDiff;

        if (m_uiFrostBoltNovaTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FROST_BOLT_NOVA);
            m_uiFrostBoltNovaTimer = 15000;
        }
        else
            m_uiFrostBoltNovaTimer -= uiDiff;

        if (m_uiChainsTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CHAINS_OF_KELTHUZAD);

            DoScriptText(urand(0, 1) ? SAY_CHAIN1 : SAY_CHAIN2, m_creature);

            m_uiChainsTimer = urand(30000, 60000);
        }
        else
            m_uiChainsTimer -= uiDiff;

        if (m_uiManaDetonationTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MANA_DETONATION);

            if (urand(0, 1))
                DoScriptText(SAY_SPECIAL1_MANA_DET, m_creature);

            m_uiManaDetonationTimer = 20000;
        }
        else
            m_uiManaDetonationTimer -= uiDiff;

        if (m_uiShadowFissureTimer < uiDiff)
        {
            CastShadowFissure();
            
            if (urand(0, 1))
                DoScriptText(SAY_SPECIAL3_MANA_DET, m_creature);

            m_uiShadowFissureTimer = 10000;
        }
        else
            m_uiShadowFissureTimer -= uiDiff;

        if (m_uiFrostBlastTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FROST_BLAST);

            if (urand(0, 1))
                DoScriptText(SAY_FROST_BLAST, m_creature);

            m_uiFrostBlastTimer = urand(30000, 60000);
        }
        else
            m_uiFrostBlastTimer -= uiDiff;

        //start phase 3 when we are 40% health
        if (!m_bIsPhase3 && m_creature->GetHealthPercent() < 40.0f)
        {
            m_bIsPhase3 = true;
            DoScriptText(SAY_REQUEST_AID, m_creature);

            //here Lich King should respond to KelThuzad but I don't know which creature to make talk
            //so for now just make Kelthuzad says it.
            DoScriptText(SAY_ANSWER_REQUEST, m_creature);
        }

        if (m_bIsPhase3 && m_uiGuardiansCount < m_uiGuardiansCountMax)
        {
            if (m_uiGuardiansTimer < uiDiff)
            {
                //Summon a Guardian of Icecrown in a random alcove
                SummonGuardian();

                //5 seconds until summoning next guardian
                m_uiGuardiansTimer = 5000;
            }
            else
                m_uiGuardiansTimer -= uiDiff;
        }
        
        if (m_bIsPhase3 && m_uiGuardiansCount > 0)
        {
            if (m_uiCheckShackleTimer < uiDiff)
            {
                CheckShackle();
                
                m_uiCheckShackleTimer = 2000;
            }
            else
                m_uiCheckShackleTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_kelthuzad(Creature* pCreature)
{
    return new boss_kelthuzadAI(pCreature);
}

/*######
## npc_shadow_fissure
######*/

struct MANGOS_DLL_DECL npc_shadow_fissureAI : public ScriptedAI
{
    npc_shadow_fissureAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiVoidBlastTimer = 3000;
        m_bDoVoidBlast = true;
        Reset();
    }

    uint32 m_uiVoidBlastTimer;
    bool m_bDoVoidBlast;

    void Reset() 
    {       
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(m_bDoVoidBlast)
        {            
            if (m_uiVoidBlastTimer < uiDiff)
            {
                m_creature->CastSpell(m_creature, SPELL_VOID_BLAST, true);
                m_bDoVoidBlast = false;
            }
            else
                m_uiVoidBlastTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_shadow_fissure(Creature* pCreature)
{
    return new npc_shadow_fissureAI(pCreature);
}

void AddSC_boss_kelthuzad()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_kelthuzad";
    pNewscript->GetAI = &GetAI_boss_kelthuzad;
    pNewscript->RegisterSelf();
    
    pNewscript = new Script;
    pNewscript->Name = "npc_shadow_fissure";
    pNewscript->GetAI = &GetAI_npc_shadow_fissure;
    pNewscript->RegisterSelf();
}
