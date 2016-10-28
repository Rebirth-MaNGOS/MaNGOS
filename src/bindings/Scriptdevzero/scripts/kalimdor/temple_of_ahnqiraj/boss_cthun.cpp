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
SDName: Boss_Cthun
SD%Complete: 95
SDComment: Darkglare tracking issue
SDCategory: Temple of Ahn'Qiraj
EndScriptData */

#include "precompiled.h"
#include "temple_of_ahnqiraj.h"
#include "Creature.h"

enum
{
    EMOTE_WEAKENED                  = -1531011,

    // ****** Out of Combat ******
    // Random Wispers - No text only sound, maybe handle them in instance script!
    RANDOM_SOUND_WHISPER            = 8663,

    // ***** Phase 1 ********    
    MOB_CLAW_TENTACLE               = 15725,
    MOB_EYE_TENTACLE                = 15726,
    MOB_SMALL_PORTAL                = 15904,
    MOB_CTHUN_PORTAL                = 15896,

    SPELL_GREEN_BEAM                = 26134,
    //SPELL_DARK_GLARE                = 26029,

    SPELL_ROTATE_TRIGGER            = 26137,                // phase switch spell - triggers 26009 or 26136. These trigger the Dark Glare spell - 26029
    SPELL_ROTATE_360_LEFT           = 26009,
    SPELL_ROTATE_360_RIGHT          = 26136,

    // Eye Tentacles Spells
    SPELL_MIND_FLAY                 = 26143,

    // Claw Tentacles Spells
    SPELL_GROUND_RUPTURE            = 26139,
    SPELL_HAMSTRING                 = 26141,

    // *****Phase 2******
    // Body spells
    // SPELL_CARAPACE_CTHUN         = 26156,                // Was removed from client dbcs
    SPELL_TRANSFORM                 = 26232,

    // Giant Claw Tentacles
    SPELL_MASSIVE_GROUND_RUPTURE    = 26100,

    // Also casts Hamstring
    SPELL_THRASH                    = 3391,

    // Giant Eye Tentacles
    // CHAIN CASTS "SPELL_GREEN_BEAM"

    // Stomach Spells
    SPELL_MOUTH_TENTACLE            = 26332,
    SPELL_EXIT_STOMACH_KNOCKBACK    = 25383,
    SPELL_DIGESTIVE_ACID            = 26476,

    // Mobs
    MOB_BODY_OF_CTHUN               = 15809,
    MOB_GIANT_CLAW_TENTACLE         = 15728,
    MOB_GIANT_EYE_TENTACLE          = 15334,
    MOB_FLESH_TENTACLE              = 15802,
    MOB_GIANT_PORTAL                = 15910,

    MODEL_BASE_NORMAL               = 15787,
    MODEL_BASE_TRANSFORMED          = 15786
};

//Stomach Teleport positions
#define STOMACH_X                           -8562.0f
#define STOMACH_Y                           2037.0f
#define STOMACH_Z                           -70.0f
#define STOMACH_O                           5.05f

//Flesh tentacle positions
#define TENTACLE_POS1_X                     -8571.0f
#define TENTACLE_POS1_Y                     1990.0f
#define TENTACLE_POS1_Z                     -98.0f
#define TENTACLE_POS1_O                     1.22f

#define TENTACLE_POS2_X                     -8525.0f
#define TENTACLE_POS2_Y                     1994.0f
#define TENTACLE_POS2_Z                     -98.0f
#define TENTACLE_POS2_O                     2.12f

//Kick out position -- unneeded - replace this with areatrigger handling of trigger [Trigger 4033]!
#define KICK_X                              -8545.0f
#define KICK_Y                              1984.0f
#define KICK_Z                              -96.0f

struct Loc
{
    float m_fX, m_fY, m_fZ, m_fO;
};

static const Loc aEyeLoc[8] = 
{
    {-8602.41f, 1967.61f, 100.998f, 0.f},
    {-8582.13f, 1959.f, 100.72f, 0.f},
    {-8560.77f, 1966.81f, 100.72f, 0.f},
    {-8548.13f, 1987.38f, 100.72f, 0.f},
    {-8558.32f, 2010.84f, 100.998f, 0.f},
    {-8577.8f, 2021.11f, 100.72f, 0.f},
    {-8601.7f, 2011.6f, 100.999f, 0.f},
    {-8609.79f, 1990.62f, 100.999f, 0.f},
};

ObjectGuid SummonSmallPortal(Creature* creature)
{
    float x, y, z;
    creature->GetPosition(x, y, z);
    if (Unit* pPortal = creature->SummonCreature(MOB_SMALL_PORTAL, x, y, z, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 0))
    {
        pPortal->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
        pPortal->SetOwnerGuid(creature->GetObjectGuid());
        return pPortal->GetObjectGuid();
    }

    return ObjectGuid();
}

ObjectGuid SummonGiantPortal(Creature* creature)
{
    float x, y, z;
    creature->GetPosition(x, y, z);
    if (Unit* pPortal = creature->SummonCreature(MOB_GIANT_PORTAL, x, y, z, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 0))
    {
        pPortal->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
        pPortal->SetOwnerGuid(creature->GetObjectGuid());
        return pPortal->GetObjectGuid();
    }

    return ObjectGuid();
}

struct MANGOS_DLL_DECL flesh_tentacleAI : public ScriptedAI
{
    flesh_tentacleAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);
        Reset();
    }

    ObjectGuid m_cThunGuid;
    uint32 m_uiCheckTimer;

    void SpawnedByCthun(Creature* pCThun)
    {
        m_cThunGuid = pCThun->GetObjectGuid();
    }

    void Reset()
    {
        m_uiCheckTimer = 1000;
    }

    void UpdateAI(const uint32 diff);

    void JustDied(Unit* killer);
};

struct MANGOS_DLL_DECL portalAI : public ScriptedAI
{
    portalAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
    }

    void Reset()
    {
    }

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->GetCharmerOrOwner() || m_creature->GetCharmerOrOwner()->isDead())
            m_creature->ForcedDespawn();
    }
};

struct MANGOS_DLL_DECL cthunAI : public ScriptedAI
{
    cthunAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);

        m_pInstance = dynamic_cast<instance_temple_of_ahnqiraj*>(pCreature->GetInstanceData());
        if (!m_pInstance)
            error_log("SD2: No Instance eye_of_cthunAI");

        float x, y, z;
        m_creature->GetPosition(x, y, z);
        Unit* pPortal = m_creature->SummonCreature(MOB_CTHUN_PORTAL, x, y, z, 0, TEMPSUMMON_MANUAL_DESPAWN, 0);
        if (pPortal)
            pPortal->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);


        Reset();
    }

    instance_temple_of_ahnqiraj* m_pInstance;

    // Out of combat whisper timer
    uint32 m_uiWisperTimer;

    // Global variables
    uint32 m_uiPhaseTimer;

    //-------------------

    // Body Phase
    uint32 m_uiEyeTentacleTimer;
    uint8 m_uiFleshTentaclesKilled;
    uint32 m_uiGiantClawTentacleTimer;
    uint32 m_uiGiantEyeTentacleTimer;
    uint32 m_uiStomachEnterTimer;
    uint32 m_uiStomachEnterVisTimer;

    uint32 m_uiSetCombatTimer;
    
    ObjectGuid m_stomachEnterTargetGuid;
    ObjectGuid m_eyeGuid;

    void Reset()
    {
        // One random wisper every 90 - 300 seconds
        m_uiWisperTimer = 90000;

        // Phase information
        m_uiPhaseTimer = 5000;                             // Emerge in 10 seconds

        // Body Phase
        m_uiEyeTentacleTimer = 30000;
        m_uiFleshTentaclesKilled = 0;
        m_uiGiantClawTentacleTimer = 15000;                 // 15 seconds into body phase (1 min repeat)
        m_uiGiantEyeTentacleTimer = 45000;                  // 15 seconds into body phase (1 min repeat)
        m_uiStomachEnterTimer = 10000;                      // Every 10 seconds
        m_uiStomachEnterVisTimer = 0;                       // Always 3.5 seconds after Stomach Enter Timer
        m_stomachEnterTargetGuid.Clear();                   // Target to be teleported to stomach

        if (Creature* pEye = m_creature->GetMap()->GetCreature(m_eyeGuid))
            pEye->Respawn();
        m_eyeGuid.Clear();

        // Clear players in stomach and outside
        if (m_pInstance)
            m_pInstance->GetStomachMap().clear();

        // Reset flags
        m_creature->RemoveAurasDueToSpell(SPELL_TRANSFORM);
        m_creature->SetDisplayId(MODEL_BASE_NORMAL);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
        m_uiSetCombatTimer = 20000;
    }

    void StartSecondPhase(ObjectGuid eyeGuid, Unit* target)
    {
        AttackStart(target);

        m_eyeGuid = eyeGuid;
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == MOB_EYE_TENTACLE)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pSummoned->AI()->AttackStart(pTarget);
        }
    }

    void SpawnEyeTentacle()
    {
        for(int i = 0; i < 8; ++i)
            m_creature->SummonCreature(MOB_EYE_TENTACLE, aEyeLoc[i].m_fX ,aEyeLoc[i].m_fY, aEyeLoc[i].m_fZ, aEyeLoc[i].m_fO, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 500);        
    }

    Player* SelectRandomNotStomach()
    {
        if (!m_pInstance || m_pInstance->GetStomachMap().empty())
            return NULL;

        std::vector<Player*> vTempTargets;
        vTempTargets.reserve(m_pInstance->GetStomachMap().size());

        // Get all players in map
        for (StomachMap::const_iterator itr = m_pInstance->GetStomachMap().begin(); itr != m_pInstance->GetStomachMap().end(); ++itr)
        {
            // Check for valid player
            Player* pPlayer = m_creature->GetMap()->GetPlayer(itr->first);

            // Only units out of stomach and that are alive.
            if (pPlayer && itr->second == false && pPlayer->isAlive())
                vTempTargets.push_back(pPlayer);
        }

        if (vTempTargets.empty())
            return NULL;

        // Get random but only if we have more than one unit on threat list
        return vTempTargets[urand(0, vTempTargets.size()- 1)];
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Check if we have a target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            // No target so we'll use this section to do our random wispers instance wide
            // WisperTimer
            if (m_uiWisperTimer < uiDiff)
            {
                if (!m_creature->GetMap()->IsDungeon())
                    return;

                // Play random sound to the map
                m_creature->GetMap()->PlayDirectSoundToMap(RANDOM_SOUND_WHISPER);

                // One random wisper every 90 - 300 seconds
                m_uiWisperTimer = urand(90000, 300000);
            }
            else
                m_uiWisperTimer -= uiDiff;

            return;
        }

        m_creature->SetTargetGuid(ObjectGuid());

        // No instance
        if (!m_pInstance)
            return;
       
        if(m_creature->isInCombat() && (m_creature->SelectHostileTarget() || !m_creature->getVictim()))
        {
            // Pulse to set everyone in combat to prevent alt f4 or ressing and not getting in combat
            if (m_uiSetCombatTimer <= uiDiff)
            {
                if(m_creature->isInCombat())
                    m_creature->SetInCombatWithZone();

                m_uiSetCombatTimer = 5000;
            }
            else
                m_uiSetCombatTimer -= uiDiff;
        }
        
        switch (m_pInstance->GetData(TYPE_CTHUN_PHASE))
        {
            // Transition phase
            case PHASE_TRANSITION:
            {
                // PhaseTimer
                if (m_uiPhaseTimer < uiDiff)
                {
                    // Switch
                    m_pInstance->SetData(TYPE_CTHUN_PHASE, PHASE_CTHUN);

                    // Switch to C'thun model
                    m_creature->InterruptNonMeleeSpells(false);
                    m_creature->SetDisplayId(MODEL_BASE_TRANSFORMED);
                    DoCastSpellIfCan(m_creature, SPELL_TRANSFORM);
                    m_creature->SetHealth(m_creature->GetMaxHealth());

                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);

                    // Emerging phase
                    m_creature->SetInCombatWithZone();

                    // Place all units in threat list on outside of stomach
                    m_pInstance->GetStomachMap().clear();

                    ThreatList const& tList = m_creature->getThreatManager().getThreatList();
                    for (ThreatList::const_iterator i = tList.begin();i != tList.end(); ++i)
                    {
                        // Outside stomach, only players
                        if ((*i)->getUnitGuid().IsPlayer())
                            m_pInstance->GetStomachMap()[(*i)->getUnitGuid()] = false;
                    }

                    // Spawn 2 flesh tentacles
                    m_uiFleshTentaclesKilled = 0;

                    // Spawn flesh tentacle
                    Creature* pSpawned = m_creature->SummonCreature(MOB_FLESH_TENTACLE, TENTACLE_POS1_X, TENTACLE_POS1_Y, TENTACLE_POS1_Z, TENTACLE_POS1_O, TEMPSUMMON_CORPSE_DESPAWN, 0);

                    if (!pSpawned)
                        ++m_uiFleshTentaclesKilled;
                    else
                    {
                        if (flesh_tentacleAI* pTentacleAI = dynamic_cast<flesh_tentacleAI*>(pSpawned->AI()))
                            pTentacleAI->SpawnedByCthun(m_creature);
                    }

                    // Spawn flesh tentacle
                    pSpawned = m_creature->SummonCreature(MOB_FLESH_TENTACLE, TENTACLE_POS2_X, TENTACLE_POS2_Y, TENTACLE_POS2_Z, TENTACLE_POS2_O, TEMPSUMMON_CORPSE_DESPAWN, 0);

                    if (!pSpawned)
                        ++m_uiFleshTentaclesKilled;
                    else
                    {
                        if (flesh_tentacleAI* pTentacleAI = dynamic_cast<flesh_tentacleAI*>(pSpawned->AI()))
                            pTentacleAI->SpawnedByCthun(m_creature);
                    }

                    m_uiPhaseTimer = 0;
                }
                else
                    m_uiPhaseTimer -= uiDiff;

                break;

            }
            // Body Phase
            case PHASE_CTHUN:
            {
                // Weaken
                if (m_uiFleshTentaclesKilled > 1)
                {
                    m_pInstance->SetData(TYPE_CTHUN_PHASE, PHASE_CTHUN_WEAKENED);

                    DoScriptText(EMOTE_WEAKENED, m_creature);
                    m_uiPhaseTimer = 45000;

                    // Kick all players out of stomach
                    for (StomachMap::iterator itr = m_pInstance->GetStomachMap().begin(); itr != m_pInstance->GetStomachMap().end(); ++itr)
                    {
                        // Check for valid player
                        Player* pPlayer = m_creature->GetMap()->GetPlayer(itr->first);

                        if (pPlayer && itr->second == true)
                        {
                            // Teleport each player out
                            DoTeleportPlayer(pPlayer, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()+10, rand()%6);

                            // Cast knockback on them
                            DoCastSpellIfCan(pPlayer, SPELL_EXIT_STOMACH_KNOCKBACK, CAST_TRIGGERED);

                            // Remove the acid debuff
                            pPlayer->RemoveAurasDueToSpell(SPELL_DIGESTIVE_ACID);

                            itr->second = false;
                        }
                    }

                    return;
                }

                // Stomach Enter Timer
                if (m_uiStomachEnterTimer < uiDiff)
                {
                    if (Player* pTarget = SelectRandomNotStomach())
                    {
                        // Set target in stomach
                        m_pInstance->GetStomachMap()[pTarget->GetObjectGuid()] = true;
                        pTarget->InterruptNonMeleeSpells(false);
                        pTarget->CastSpell(pTarget, SPELL_MOUTH_TENTACLE, true, NULL, NULL, m_creature->GetObjectGuid());
                        m_stomachEnterTargetGuid = pTarget->GetObjectGuid();
                        m_uiStomachEnterVisTimer = 3800;
                    }

                    m_uiStomachEnterTimer = 13800;
                }
                else
                    m_uiStomachEnterTimer -= uiDiff;

                if (m_uiStomachEnterVisTimer && m_stomachEnterTargetGuid)
                {
                    if (m_uiStomachEnterVisTimer <= uiDiff)
                    {
                        // Check for valid player
                        if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_stomachEnterTargetGuid))
                        {
                            DoTeleportPlayer(pPlayer, STOMACH_X, STOMACH_Y, STOMACH_Z, STOMACH_O);

                            // Cast Digestive Acid on the player.
                            m_creature->CastSpell(pPlayer, SPELL_DIGESTIVE_ACID, true);
                        }

                        m_stomachEnterTargetGuid.Clear();
                        m_uiStomachEnterVisTimer = 0;

                        // Note that actually C'Thun cannot be soloed, so kill all players, if no player left outside of stomach
                        bool bKillAllPlayer = true;
                        for (StomachMap::iterator itr = m_pInstance->GetStomachMap().begin(); itr != m_pInstance->GetStomachMap().end(); ++itr)
                        {
                            Player* pPlayer = m_creature->GetMap()->GetPlayer(itr->first);
                            if (itr->second == false && pPlayer)
                            {
                                bKillAllPlayer = false;
                                break;
                            }
                        }
                        if (bKillAllPlayer)
                        {
                            for (StomachMap::iterator itr = m_pInstance->GetStomachMap().begin(); itr != m_pInstance->GetStomachMap().end(); ++itr)
                            {
                                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(itr->first))
                                {
                                    pPlayer->RemoveAurasDueToSpell(SPELL_DIGESTIVE_ACID, nullptr, AURA_REMOVE_BY_DEFAULT);
                                    m_creature->DealDamage(pPlayer, pPlayer->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
                                }
                            }
                            ResetToHome();
                            return;
                        }
                    }
                    else
                        m_uiStomachEnterVisTimer -= uiDiff;
                }

                // GientClawTentacleTimer
                if (m_uiGiantClawTentacleTimer < uiDiff)
                {
                    if (Player* pTarget = SelectRandomNotStomach())
                    {

                        // Spawn claw tentacle on the random target
                        if (Creature* pSummoned = m_creature->SummonCreature(MOB_GIANT_CLAW_TENTACLE, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 500))
                            pSummoned->AI()->AttackStart(pTarget);
                    }

                    // One giant claw tentacle every minute
                    m_uiGiantClawTentacleTimer = 60000;
                }
                else
                    m_uiGiantClawTentacleTimer -= uiDiff;

                // GiantEyeTentacleTimer
                if (m_uiGiantEyeTentacleTimer < uiDiff)
                {
                    if (Player* pTarget = SelectRandomNotStomach())
                    {
                        // Spawn claw tentacle on the random target
                        if (Creature* pSummoned = m_creature->SummonCreature(MOB_GIANT_EYE_TENTACLE, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 500))
                            pSummoned->AI()->AttackStart(pTarget);
                    }

                    // One giant eye tentacle every minute
                    m_uiGiantEyeTentacleTimer = 60000;
                }
                else
                    m_uiGiantEyeTentacleTimer -= uiDiff;

                // EyeTentacleTimer
                if (m_uiEyeTentacleTimer < uiDiff)
                {
                    // Spawn the 8 Eye Tentacles in the corret spots
                    SpawnEyeTentacle();
                    
                    //These spawn at every 30 seconds
                    m_uiEyeTentacleTimer = 30000;
                }
                else
                    m_uiEyeTentacleTimer -= uiDiff;

                break;

            }
            // Weakened state
            case PHASE_CTHUN_WEAKENED:
            {
                // PhaseTimer
                if (m_uiPhaseTimer < uiDiff)
                {
                    // Switch
                    m_pInstance->SetData(TYPE_CTHUN_PHASE, PHASE_CTHUN);

                    // Spawn 2 flesh tentacles
                    m_uiFleshTentaclesKilled = 0;

                    // Spawn flesh tentacle
                    Creature* pSpawned = m_creature->SummonCreature(MOB_FLESH_TENTACLE, TENTACLE_POS1_X, TENTACLE_POS1_Y, TENTACLE_POS1_Z, TENTACLE_POS1_O, TEMPSUMMON_CORPSE_DESPAWN, 0);

                    if (!pSpawned)
                        ++m_uiFleshTentaclesKilled;
                    else
                    {
                        if (flesh_tentacleAI* pTentacleAI = dynamic_cast<flesh_tentacleAI*>(pSpawned->AI()))
                            pTentacleAI->SpawnedByCthun(m_creature);
                    }

                    pSpawned = NULL;
                    // Spawn flesh tentacle
                    pSpawned = m_creature->SummonCreature(MOB_FLESH_TENTACLE, TENTACLE_POS2_X, TENTACLE_POS2_Y, TENTACLE_POS2_Z, TENTACLE_POS2_O, TEMPSUMMON_CORPSE_DESPAWN, 0);

                    if (!pSpawned)
                        ++m_uiFleshTentaclesKilled;
                    else
                    {
                        if (flesh_tentacleAI* pTentacleAI = dynamic_cast<flesh_tentacleAI*>(pSpawned->AI()))
                            pTentacleAI->SpawnedByCthun(m_creature);
                    }

                    m_uiPhaseTimer = 0;
                }
                else
                    m_uiPhaseTimer -= uiDiff;
            }
        }
    }

    void JustDied(Unit* /*pKiller*/)
    {
        // Switch
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CTHUN_PHASE, PHASE_FINISH);
    }

    void ResetToHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CTHUN_PHASE, PHASE_NOT_STARTED);

        ScriptedAI::ResetToHome();
    }

    void DamageTaken(Unit* /*pDealer*/, uint32& uiDamage)
    {
        // No instance
        if (!m_pInstance)
            return;

        switch (m_pInstance->GetData(TYPE_CTHUN_PHASE))
        {
            case PHASE_CTHUN:
            {
                // Not weakened so reduce damage by 99%
                if (uiDamage / 99 > 0) uiDamage/= 99;
                else
                    uiDamage = 1;

                // Prevent death in non-weakened state
                if (uiDamage >= m_creature->GetHealth())
                    uiDamage = 0;

                return;
            }
            break;

            case PHASE_CTHUN_WEAKENED:
            {
                //Weakened - takes normal damage
                return;
            }

            default:
                uiDamage = 0;
                break;
        }
    }

    void FleshTentcleKilled()
    {
        ++m_uiFleshTentaclesKilled;
    }
};

struct MANGOS_DLL_DECL eye_of_cthunAI : public ScriptedAI
{
    eye_of_cthunAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);

        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        if (!m_pInstance)
            error_log("SD2: No Instance eye_of_cthunAI");

        Reset();
    }

    ScriptedInstance* m_pInstance;

    // Global variables
    uint32 m_uiPhaseTimer;

    // Eye beam phase
    uint32 m_uiBeamTimer;
    uint32 m_uiEyeTentacleTimer;
    uint32 m_uiClawTentacleTimer;

    // Dark Glare phase
    uint32 m_uiDarkGlareTick;
    uint32 m_uiDarkGlareTickTimer;
    float m_fDarkGlareAngle;
    bool m_bClockWise;

    uint32 m_uiSetCombatTimer;
    
    void Reset()
    {
        // Phase information
        m_uiPhaseTimer = 50000;                             // First dark glare in 50 seconds

        // Eye beam phase 50 seconds
        m_uiBeamTimer = 3000;
        m_uiEyeTentacleTimer = 45000;                       // Always spawns 5 seconds before Dark Beam
        m_uiClawTentacleTimer = 12500;                      // 4 per Eye beam phase (unsure if they spawn durring Dark beam)

        // Dark Beam phase 35 seconds (each tick = 1 second, 35 ticks)
        m_uiDarkGlareTick = 0;
        m_uiDarkGlareTickTimer = 1000;
        m_fDarkGlareAngle = 0;
        m_bClockWise = false;

        // Reset flags
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
        
        m_uiSetCombatTimer = 5000;
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == MOB_EYE_TENTACLE)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pSummoned->AI()->AttackStart(pTarget);
        }
    }

    void SpawnEyeTentacle()
    {
        for(int i = 0; i < 8; ++i)
            m_creature->SummonCreature(MOB_EYE_TENTACLE, aEyeLoc[i].m_fX ,aEyeLoc[i].m_fY, aEyeLoc[i].m_fZ, aEyeLoc[i].m_fO, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 500);        
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CTHUN_PHASE, PHASE_EYE_NORMAL);

        // Nuke the aggroing player with a beam.
        DoCastSpellIfCan(pWho, SPELL_GREEN_BEAM);
    }

    void ResetToHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CTHUN_PHASE, PHASE_NOT_STARTED);

        ScriptedAI::ResetToHome();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // No instance
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_CTHUN_PHASE) == PHASE_EYE_NORMAL)
        {
            // Check if we have a target
            if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                return;
        
            if(m_creature->isInCombat() && (m_creature->SelectHostileTarget() || !m_creature->getVictim()))
            {
                // Pulse to set everyone in combat to prevent alt f4 or ressing and not getting in combat
                if (m_uiSetCombatTimer <= uiDiff)
                {
                    if(m_creature->isInCombat())
                        m_creature->SetInCombatWithZone();

                    m_uiSetCombatTimer = 5000;
                }
                else
                    m_uiSetCombatTimer -= uiDiff;
            }
        }
        
        switch (m_pInstance->GetData(TYPE_CTHUN_PHASE))
        {
            case PHASE_EYE_NORMAL:
            {
                // m_uiBeamTimer
                if (m_uiBeamTimer < uiDiff)
                {
                    std::vector<Unit*> targetList;
                    const ThreatList& threatList = m_creature->getThreatManager().getThreatList();

                    // Make sure NOT to cast the spell while iterating the threat list.
                    // Casting a spell on a new target switches target and will change
                    // the threat list while it is being iterated, thus causing a crash.
                    if(!threatList.empty())
                    {
                        for (HostileReference *currentReference : threatList)
                        {
                            Unit *target = currentReference->getTarget();
                            if (target && target->GetTypeId() == TYPEID_PLAYER)
                                targetList.push_back(target);
                        }
                    }

                    // SPELL_GREEN_BEAM
                    if (!targetList.empty())
                    {
                        Unit* pTarget = targetList[urand(0, targetList.size() - 1)];
                        m_creature->InterruptNonMeleeSpells(false);
                        DoCastSpellIfCan(pTarget, SPELL_GREEN_BEAM);

                        // Correctly update our target
                        m_creature->SetTargetGuid(pTarget->GetObjectGuid());
                    }

                    //Beam every 3 seconds
                    m_uiBeamTimer = 3000;
                }
                else
                    m_uiBeamTimer -= uiDiff;


                // m_uiClawTentacleTimer
                if (m_uiClawTentacleTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        // Spawn claw tentacle on the random target
                        if (Creature* pSummoned = m_creature->SummonCreature(MOB_CLAW_TENTACLE, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 500))
                            pSummoned->AI()->AttackStart(pTarget);
                    }

                    // One claw tentacle every 12.5 seconds
                    m_uiClawTentacleTimer = 12500;
                }
                else
                    m_uiClawTentacleTimer -= uiDiff;

                // m_uiEyeTentacleTimer
                if (m_uiEyeTentacleTimer < uiDiff)
                {
                    //Spawn the 8 Eye Tentacles in the corret spots
                    SpawnEyeTentacle();
                    
                    // No point actually putting a timer here since
                    // These shouldn't trigger agian until after phase shifts
                    m_uiEyeTentacleTimer = 45000;
                }
                else
                    m_uiEyeTentacleTimer -= uiDiff;

                // m_uiPhaseTimer
                if (m_uiPhaseTimer < uiDiff)
                {
                    //Switch to Dark Beam
                    m_pInstance->SetData(TYPE_CTHUN_PHASE, PHASE_EYE_DARK_GLARE);
                    
                    // Remove proximity aggro on Dark Glare
                    m_creature->SetAggroRangeOverride(0.f);

                    m_creature->InterruptNonMeleeSpells(false);
					Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);

                    // Remove the target focus but allow the boss to face the current victim
                    if(pTarget)
                        m_creature->SetFacingToObject(pTarget);
                    m_creature->SetTargetGuid(ObjectGuid());

					// Cast the rotation spell
                    m_creature->CastSpell(m_creature, SPELL_ROTATE_TRIGGER, false);

                    // Make C'Thun ignore target changes.
                    m_creature->SetIgnoringTargets(true);

                    // Darkbeam for 35 seconds
                    m_uiPhaseTimer = 35000;
                }
                else
                    m_uiPhaseTimer -= uiDiff;

                break;
            }
            case PHASE_EYE_DARK_GLARE:
            {
                // m_uiPhaseTimer
                if (m_uiPhaseTimer < uiDiff)
                {
                    // Switch to Eye Beam
                    m_pInstance->SetData(TYPE_CTHUN_PHASE, PHASE_EYE_NORMAL);

                    // Restore the proximity aggro
                    m_creature->SetAggroRangeOverride(1.f);
                    
                    m_uiBeamTimer = 3000;
                    m_uiEyeTentacleTimer = 45000;               //Always spawns 5 seconds before Dark Beam
                    m_uiClawTentacleTimer = 12500;              //4 per Eye beam phase (unsure if they spawn durring Dark beam)

                    m_creature->InterruptNonMeleeSpells(false);

					// Remove rotation auras
                    m_creature->RemoveAurasDueToSpell(SPELL_ROTATE_360_LEFT);
                    m_creature->RemoveAurasDueToSpell(SPELL_ROTATE_360_RIGHT);

                    // Freeze animation
                    m_creature->SetUInt32Value(UNIT_FIELD_FLAGS, 0);
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);

                    // Make C'Thun start picking targets again.
                    m_creature->SetIgnoringTargets(false);

                    // Eye Beam for 50 seconds
                    m_uiPhaseTimer = 50000;
                }
                else
                    m_uiPhaseTimer -= uiDiff;

                break;
            }
        }
    }

    void DamageTaken(Unit* pDealer, uint32& uiDamage)
    {
        // No instance
        if (!m_pInstance)
            return;

        switch (m_pInstance->GetData(TYPE_CTHUN_PHASE))
        {
            case PHASE_EYE_NORMAL:
            case PHASE_EYE_DARK_GLARE:
            {
                // Only if it will kill
                if (uiDamage < m_creature->GetHealth())
                    return;

                // Fake death in phase 0 or 1 (green beam or dark glare phase)
                m_creature->InterruptNonMeleeSpells(false);

                // Reset to normal emote state and prevent select and attack
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);

                // Remove Target field
                m_creature->SetTargetGuid(ObjectGuid());

                // Death animation/ respawning;
                m_pInstance->SetData(TYPE_CTHUN_PHASE, PHASE_TRANSITION);
                // Ensure CThun is in combat
                if (Creature* pCThun = m_pInstance->GetSingleCreatureFromStorage(NPC_CTHUN))
                {
                    cthunAI* pAI = dynamic_cast<cthunAI*>(pCThun->AI());
                    if (pAI)
                        pAI->StartSecondPhase(m_creature->GetObjectGuid(), pDealer);
                }

                m_creature->InterruptNonMeleeSpells(true);
                m_creature->RemoveAllAuras();

                break;
            }
            case PHASE_FINISH:
            {
                // Allow death here
                return;
            }

            default:
            {
                // Prevent death in this phase
                uiDamage = 0;
                return;
            }
            break;
        }
    }
};


struct MANGOS_DLL_DECL eye_tentacleAI : public ScriptedAI
{
    eye_tentacleAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);
        CastGroundRupture();
        Reset();

        m_portalGuid = SummonSmallPortal(m_creature);
    }

    uint32 m_uiMindflayTimer;
    uint32 m_uiInitialGroundRupture;
    uint32 m_uiKillSelfTimer;
    ObjectGuid m_portalGuid;

    bool m_bInitialGroundRupture;
    
    void JustDied(Unit*)
    {
        if (Creature* pCreature = m_creature->GetMap()->GetCreature(m_portalGuid))
            pCreature->DealDamage(pCreature, pCreature->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
    }

    void Reset()
    {
        m_bInitialGroundRupture = true;
        m_uiInitialGroundRupture = 100;
        
        // Mind flay half a second after we spawn
        m_uiMindflayTimer = 500;

        // This prevents eyes from overlapping
        m_uiKillSelfTimer = 35000;
    }

    void Aggro(Unit* /*pWho*/)
    {
        m_creature->SetInCombatWithZone();
    }

    void CastGroundRupture()
    {
        // Cast Ground Rupture on all players within 1 yd.
        std::list<Player*> playerList = GetPlayersAtMinimumRange(1.f);
        for (Player* pPlayer : playerList)
            m_creature->CastSpell(pPlayer, SPELL_GROUND_RUPTURE, true);
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        // Check if we have a target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // KillSelfTimer
        if (m_uiKillSelfTimer < uiDiff)
        {
            m_creature->DealDamage(m_creature, m_creature->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);

            return;
        }
        else
            m_uiKillSelfTimer -= uiDiff;
        
        if(m_bInitialGroundRupture)
        {
            // Initial Ground Rupture
            if (m_uiInitialGroundRupture < uiDiff)
            {
                CastGroundRupture();
                m_bInitialGroundRupture = false;
            }
            else
                m_uiInitialGroundRupture -= uiDiff;
        }

        // MindflayTimer
        if (m_uiMindflayTimer < uiDiff)
        {
            std::vector<Unit*> targetList;
            const ThreatList& threatList = m_creature->getThreatManager().getThreatList();

            // Make sure NOT to cast the spell while iterating the threat list.
            // Casting a spell on a new target switches target and will change
            // the threat list while it is being iterated, thus causing a crash.
            if(!threatList.empty())
            {
                for (HostileReference *currentReference : threatList)
                {
                    Unit *target = currentReference->getTarget();
                    if (target && target->GetTypeId() == TYPEID_PLAYER)
                        targetList.push_back(target);
                }
            }

            if (!targetList.empty())
            {
                Unit* pTarget = targetList[urand(0, targetList.size() - 1)];
                if (!pTarget->HasAura(SPELL_DIGESTIVE_ACID, EFFECT_INDEX_0))
                    DoCastSpellIfCan(pTarget, SPELL_MIND_FLAY);
            }

            //Mindflay every 10 seconds
            m_uiMindflayTimer = 10100;
        }
        else
            m_uiMindflayTimer -= uiDiff;
    }
};

struct MANGOS_DLL_DECL claw_tentacleAI : public ScriptedAI
{
    claw_tentacleAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);
        Reset();

        m_portalGuid = SummonSmallPortal(m_creature);
    }

    uint32 m_uiHamstringTimer;
    uint32 m_uiEvadeTimer;
    uint32 m_uiInitialGroundRupture;
    ObjectGuid m_portalGuid;
    
    bool m_bInitialGroundRupture;
    bool m_bGroundRupture;

    void JustDied(Unit*)
    {
        if (Creature* pCreature = m_creature->GetMap()->GetCreature(m_portalGuid))
            pCreature->DealDamage(pCreature, pCreature->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
    }

    void Reset()
    {
        m_bInitialGroundRupture = true;
        m_uiInitialGroundRupture = 100;
        m_bGroundRupture = false;
        m_uiHamstringTimer = 2000;
        m_uiEvadeTimer = 5000;
    }
    
    Player* DoGetPlayerInMeleeRangeByThreat()
    {
        std::vector<Player*> tmp_list;

        if (!m_creature->CanHaveThreatList())
            return nullptr;

        GUIDVector vGuids;
        m_creature->FillGuidsListFromThreatList(vGuids);

        if (!vGuids.empty())
        {
            for (ObjectGuid current_guid : vGuids)
            {
                if (Unit* current_target = m_creature->GetMap()->GetUnit(current_guid))
                {
                    // We need only a player
                    if (current_target->GetTypeId() != TYPEID_PLAYER)
                        continue;

                    if (m_creature->CanReachWithMeleeAttack(current_target))
                        tmp_list.push_back(dynamic_cast<Player*>(current_target));
                }
            }
        }
        else
            return nullptr;

        if (tmp_list.empty())
            return nullptr;

        // If there's just one player on the threat list we return that player.
        if (tmp_list.size() == 1)
            return tmp_list.front();

        // Sort the list from highest to lowest threat.
        std::sort(tmp_list.begin(), tmp_list.end(), 
                [&]( Player* first, Player* second) -> bool 
                { 
                    return m_creature->getThreatManager().getThreat(first) > 
                    m_creature->getThreatManager().getThreat(second); 
                });

        return tmp_list.front();
    }

    void CastGroundRupture()
    {
        // Cast Ground Rupture on all players within 1 yd.
        std::list<Player*> playerList = GetPlayersAtMinimumRange(1.f);
        for (Player* pPlayer : playerList)
            m_creature->CastSpell(pPlayer, SPELL_GROUND_RUPTURE, true);
    }

    void Aggro(Unit* /*pWho*/)
    {
        m_creature->SetInCombatWithZone();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Check if we have a target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        
        if(m_bInitialGroundRupture)
        {
            // Initial Ground Rupture
            if (m_uiInitialGroundRupture < uiDiff)
            {
                CastGroundRupture();
                m_bInitialGroundRupture = false;
            }
            else
                m_uiInitialGroundRupture -= uiDiff;
        }
        
        // EvadeTimer
        if (!m_creature->CanReachWithMeleeAttack(m_creature->getVictim()) && m_creature->getVictim() && m_creature->getVictim()->isAlive())
        {            
            if (Player* melee_player = DoGetPlayerInMeleeRangeByThreat())
            {                                
                // If there are players in melee range prioritise them.
                m_creature->SetTargetGuid(melee_player->GetGUID());
                if (m_creature->getVictim())
                    m_creature->AI()->AttackStart(melee_player);
            }
            else
            {        
                if (m_uiEvadeTimer < uiDiff)
                {
                    if (Creature* pCreature = m_creature->GetMap()->GetCreature(m_portalGuid))
                        pCreature->DealDamage(pCreature, pCreature->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);

                    // Dissapear and reappear at new position
                    m_creature->SetVisibility(VISIBILITY_OFF);

                    // Reset threat
                    DoResetThreat();
                    
                    Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
                    if (!pTarget || pTarget->HasAura(SPELL_DIGESTIVE_ACID, EFFECT_INDEX_0))
                    {
                        m_creature->DealDamage(m_creature, m_creature->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
                        return;
                    }
                    else
                    {
                        m_creature->GetMap()->CreatureRelocation(m_creature, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0);

                        m_portalGuid = SummonSmallPortal(m_creature);

                        m_bGroundRupture = true;
                        m_uiHamstringTimer = 2000;
                        m_uiEvadeTimer = 5000;
                        AttackStart(pTarget);
                    }
                    m_creature->SetVisibility(VISIBILITY_ON);
                }
                else
                    m_uiEvadeTimer -= uiDiff;                           
            }           
        }             
        else
        {
            // reset the evade timer
            m_uiEvadeTimer = 5000;
            DoMeleeAttackIfReady();
        }
          
        // GroundRuptureTimer
        if(m_bGroundRupture)
        {
                CastGroundRupture();
                m_bGroundRupture = false;
        }            

        // HamstringTimer
        if (m_uiHamstringTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(),SPELL_HAMSTRING);
            m_uiHamstringTimer = 5000;
        }
        else
            m_uiHamstringTimer -= uiDiff;
    }
};

struct MANGOS_DLL_DECL giant_claw_tentacleAI : public ScriptedAI
{
    giant_claw_tentacleAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);
        Reset();

        m_portalGuid = SummonGiantPortal(m_creature);
    }

    uint32 m_uiThrashTimer;
    uint32 m_uiHamstringTimer;
    uint32 m_uiEvadeTimer;
    uint32 m_uiInitialGroundRupture;
    ObjectGuid m_portalGuid;
    
    bool m_bInitialGroundRupture;
    bool m_bGroundRupture;

    void JustDied(Unit*)
    {
        if (Creature* pCreature = m_creature->GetMap()->GetCreature(m_portalGuid))
            pCreature->DealDamage(pCreature, pCreature->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
    }

    void Reset()
    {
        m_bInitialGroundRupture = true;
        m_uiInitialGroundRupture = 100;
        m_bGroundRupture = false;
        m_uiHamstringTimer = 2000;
        m_uiThrashTimer = 5000;
        m_uiEvadeTimer = 5000;
    }

    void Aggro(Unit* /*pWho*/)
    {
        m_creature->SetInCombatWithZone();
    }
    
    void CastGroundRupture()
    {
        // Cast Ground Rupture on all players within 1 yd.
        std::list<Player*> playerList = GetPlayersAtMinimumRange(3.f);
        for (Player* pPlayer : playerList)
            m_creature->CastSpell(pPlayer, SPELL_MASSIVE_GROUND_RUPTURE, true);
    }
    
    Player* DoGetPlayerInMeleeRangeByThreat()
    {
        std::vector<Player*> tmp_list;

        if (!m_creature->CanHaveThreatList())
            return nullptr;

        GUIDVector vGuids;
        m_creature->FillGuidsListFromThreatList(vGuids);

        if (!vGuids.empty())
        {
            for (ObjectGuid current_guid : vGuids)
            {
                if (Unit* current_target = m_creature->GetMap()->GetUnit(current_guid))
                {
                    // We need only a player
                    if (current_target->GetTypeId() != TYPEID_PLAYER)
                        continue;

                    if (m_creature->CanReachWithMeleeAttack(current_target))
                        tmp_list.push_back(dynamic_cast<Player*>(current_target));
                }
            }
        }
        else
            return nullptr;

        if (tmp_list.empty())
            return nullptr;

        // If there's just one player on the threat list we return that player.
        if (tmp_list.size() == 1)
            return tmp_list.front();

        // Sort the list from highest to lowest threat.
        std::sort(tmp_list.begin(), tmp_list.end(), 
                [&]( Player* first, Player* second) -> bool 
                { 
                    return m_creature->getThreatManager().getThreat(first) > 
                    m_creature->getThreatManager().getThreat(second); 
                });

        return tmp_list.front();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Check if we have a target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_bInitialGroundRupture)
        {
            // Initial Ground Rupture
            if (m_uiInitialGroundRupture < uiDiff)
            {
                CastGroundRupture();
                m_bInitialGroundRupture = false;
            }
            else
                m_uiInitialGroundRupture -= uiDiff;
        }
        
         // EvadeTimer
        if (!m_creature->CanReachWithMeleeAttack(m_creature->getVictim()) && m_creature->getVictim() && m_creature->getVictim()->isAlive())
        {            
            if (Player* melee_player = DoGetPlayerInMeleeRangeByThreat())
            {                                
                // If there are players in melee range prioritise them.
                m_creature->SetTargetGuid(melee_player->GetGUID());
                if (m_creature->getVictim())
                    m_creature->AI()->AttackStart(melee_player);
            }
            else
            {        
                if (m_uiEvadeTimer < uiDiff)
                {
                    if (Creature* pCreature = m_creature->GetMap()->GetCreature(m_portalGuid))
                        pCreature->DealDamage(pCreature, pCreature->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);

                    // Dissapear and reappear at new position
                    m_creature->SetVisibility(VISIBILITY_OFF);
                    
                    // Reset threat
                    DoResetThreat();

                    Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);

                    if (!target || target->HasAura(SPELL_DIGESTIVE_ACID, EFFECT_INDEX_0))
                    {
                        m_creature->DealDamage(m_creature, m_creature->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
                        return;
                    }
                    else
                    {
                        m_creature->GetMap()->CreatureRelocation(m_creature, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0);

                        m_portalGuid = SummonGiantPortal(m_creature);

                        // full hp after move
                        m_creature->SetHealthPercent(100);
                        m_bGroundRupture = true;
                        m_uiHamstringTimer = 2000;
                        m_uiThrashTimer = 5000;
                        m_uiEvadeTimer = 5000;
                        AttackStart(target);
                    }
                    m_creature->SetVisibility(VISIBILITY_ON);
                }
                else
                    m_uiEvadeTimer -= uiDiff;
            }
        }
        else 
        {
            // reset the evade timer
            m_uiEvadeTimer = 5000;
            DoMeleeAttackIfReady();
        }
        
        // GroundRuptureTimer
        if(m_bGroundRupture)
        {
            CastGroundRupture();
            m_bGroundRupture = false;
        }     

        // ThrashTimer
        if (m_uiThrashTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(),SPELL_THRASH);
            m_uiThrashTimer = 10000;
        }
        else
            m_uiThrashTimer -= uiDiff;

        // HamstringTimer
        if (m_uiHamstringTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(),SPELL_HAMSTRING);
            m_uiHamstringTimer = 10000;
        }
        else
            m_uiHamstringTimer -= uiDiff;
    }
};

struct MANGOS_DLL_DECL giant_eye_tentacleAI : public ScriptedAI
{
    giant_eye_tentacleAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);
        Reset();

        m_portalGuid = SummonGiantPortal(m_creature);
    }

    uint32 m_uiBeamTimer;
    uint32 m_uiInitialGroundRupture;
    ObjectGuid m_portalGuid;
    bool m_bInitialGroundRupture;
    
    void JustDied(Unit*)
    {
        if (Creature* pCreature = m_creature->GetMap()->GetCreature(m_portalGuid))
            pCreature->DealDamage(pCreature, pCreature->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
    }

    void Reset()
    {
        m_bInitialGroundRupture = true;
        m_uiInitialGroundRupture = 100;

        //Green Beam half a second after we spawn
        m_uiBeamTimer = 500;
    }

    void Aggro(Unit* /*pWho*/)
    {
        m_creature->SetInCombatWithZone();
    }

    void CastGroundRupture()
    {
        // Cast Ground Rupture on all players within 1 yd.
        std::list<Player*> playerList = GetPlayersAtMinimumRange(3.f);
        for (Player* pPlayer : playerList)
            m_creature->CastSpell(pPlayer, SPELL_MASSIVE_GROUND_RUPTURE, true);
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        // Check if we have a target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_bInitialGroundRupture)
        {
            // Initial Ground Rupture
            if (m_uiInitialGroundRupture < uiDiff)
            {
                CastGroundRupture();
                m_bInitialGroundRupture = false;
            }
            else
                m_uiInitialGroundRupture -= uiDiff;
        }
        
        // BeamTimer
        if (m_uiBeamTimer < uiDiff)
        {
            Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
            if (target && !target->HasAura(SPELL_DIGESTIVE_ACID, EFFECT_INDEX_0))
                DoCastSpellIfCan(target,SPELL_GREEN_BEAM);

            // Beam every 2 seconds
            m_uiBeamTimer = 2100;
        }
        else
            m_uiBeamTimer -= uiDiff;
    }
};

// Flesh tentacle functions
void flesh_tentacleAI::UpdateAI(const uint32 uiDiff)
{
    if (m_cThunGuid)
    {
        if (m_uiCheckTimer < uiDiff)
        {
            Creature* pParent = m_creature->GetMap()->GetCreature(m_cThunGuid);

            if (!pParent || !pParent->isAlive() || !pParent->isInCombat())
            {
                m_cThunGuid.Clear();
                m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
                return;
            }

            m_uiCheckTimer = 1000;
        }
        else
            m_uiCheckTimer -= uiDiff;
    }

    // Check if we have a target
    if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        return;

    DoMeleeAttackIfReady();
}

void flesh_tentacleAI::JustDied(Unit* /*killer*/)
{
    if (Creature* pCthun = m_creature->GetMap()->GetCreature(m_cThunGuid))
    {
        if (cthunAI* pCthunAI = dynamic_cast<cthunAI*>(pCthun->AI()))
            pCthunAI->FleshTentcleKilled();
    }
    else
        error_log("SD2: flesh_tentacle: No Cthun");
}


/*######
## npc_cthun_aggro_dummy
######*/

struct MANGOS_DLL_DECL npc_cthun_aggro_dummyAI : public ScriptedAI
{
    npc_cthun_aggro_dummyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }
    
    ScriptedInstance* m_pInstance;

    void Reset()
    {
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 5.0f) &&  m_creature->IsWithinLOSInMap(pWho))
        {
            if(m_pInstance)
            {
                Creature* pCthun = m_pInstance->GetSingleCreatureFromStorage(NPC_EYE_OF_CTHUN);
                if(pCthun && pCthun->isAlive() && !pCthun->isInCombat())
                    pCthun->AI()->AttackStart(pWho);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
    }
};

CreatureAI* GetAI_npc_cthun_aggro_dummy(Creature* pCreature)
{
    return new npc_cthun_aggro_dummyAI(pCreature);
}

// GetAIs
CreatureAI* GetAI_eye_of_cthun(Creature* pCreature)
{
    return new eye_of_cthunAI(pCreature);
}

CreatureAI* GetAI_cthun(Creature* pCreature)
{
    return new cthunAI(pCreature);
}

CreatureAI* GetAI_eye_tentacle(Creature* pCreature)
{
    return new eye_tentacleAI(pCreature);
}

CreatureAI* GetAI_claw_tentacle(Creature* pCreature)
{
    return new claw_tentacleAI(pCreature);
}

CreatureAI* GetAI_giant_claw_tentacle(Creature* pCreature)
{
    return new giant_claw_tentacleAI(pCreature);
}

CreatureAI* GetAI_giant_eye_tentacle(Creature* pCreature)
{
    return new giant_eye_tentacleAI(pCreature);
}

CreatureAI* GetAI_flesh_tentacle(Creature* pCreature)
{
    return new flesh_tentacleAI(pCreature);
}

CreatureAI* GetAI_portal(Creature* pCreature)
{
    return new portalAI(pCreature);
}

struct npc_cthun_stomach_exit_triggerAI : public ScriptedAI
{
    npc_cthun_stomach_exit_triggerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);

        Reset();
    }

    uint32 m_uiKnockbackTimer;
    uint32 m_uiSecondKnockbackTimer;
    ObjectGuid m_targetPlayerGuid;

    void Reset()
    {
        m_uiKnockbackTimer = 0;
        m_uiSecondKnockbackTimer = 0;
        m_targetPlayerGuid = ObjectGuid();
    }

    void StartFirstKnockback(ObjectGuid targetGuid)
    {
        m_targetPlayerGuid = targetGuid;

        m_uiKnockbackTimer = 3000;
        m_creature->CastSpell(m_creature, 26092, true);
    }

    void StartSecondKnockback(ObjectGuid targetGuid)
    {
        m_targetPlayerGuid = targetGuid;

        m_uiSecondKnockbackTimer = 100;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiKnockbackTimer)
        {
            if (m_uiKnockbackTimer <= uiDiff)
            {
                Player* pPlayer = m_creature->GetMap()->GetPlayer(m_targetPlayerGuid);
                if (pPlayer && m_creature->GetDistance2d(pPlayer) <= 5.f)
                    pPlayer->KnockBackFrom(pPlayer, 0.f, 56.f);

                m_uiKnockbackTimer = 0;
            }
            else
                m_uiKnockbackTimer -= uiDiff;
        }

        if (m_uiSecondKnockbackTimer)
        {
            if (m_uiSecondKnockbackTimer <= uiDiff)
            {
                Player* pPlayer = m_creature->GetMap()->GetPlayer(m_targetPlayerGuid);
                if (pPlayer)
                    pPlayer->KnockBackFrom(pPlayer, 24.f, 24.f);

                m_uiSecondKnockbackTimer = 0;
            }
            else
                m_uiSecondKnockbackTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_cthun_stomach_exit_triggerAI(Creature* pCreature)
{
    return new npc_cthun_stomach_exit_triggerAI(pCreature);
}

// 26224 - knockback
// 26221 - summon spell, not used
// Trigger ID: 4033
bool AreaTrigger_stomach_knockback(Player* pPlayer, const AreaTriggerEntry* pTrigger)
{
    if (Creature* pTriggerCreature = pPlayer->SummonCreature(15800, pTrigger->x, pTrigger->y,
                                                             pTrigger->z, 0,
                                                             TEMPSUMMON_TIMED_DESPAWN, 10000))
    {
       npc_cthun_stomach_exit_triggerAI* pAI = dynamic_cast<npc_cthun_stomach_exit_triggerAI*>(pTriggerCreature->AI());
       if (pAI)
          pAI->StartFirstKnockback(pPlayer->GetObjectGuid()); 
    }

    return true;
}

// Trigger ID: 4034
bool AreaTrigger_stomach_teleport(Player* pPlayer, const AreaTriggerEntry* pTrigger)
{
    // Teleport each player out
    pPlayer->NearTeleportTo(-8570.f, 1991.f, 100.4, rand()%6);

    // Remove the acid debuff
    pPlayer->RemoveAurasDueToSpell(SPELL_DIGESTIVE_ACID);

    // Remove the player from the map of players in C'Thun's stomach.
    instance_temple_of_ahnqiraj* pInstance = dynamic_cast<instance_temple_of_ahnqiraj*>(pPlayer->GetInstanceData());
    if (pInstance)
        pInstance->GetStomachMap()[pPlayer->GetObjectGuid()] = false;

    if (Creature* pTriggerCreature = pPlayer->SummonCreature(15800, pTrigger->x, pTrigger->y,
                                                             pTrigger->z, 0,
                                                             TEMPSUMMON_TIMED_DESPAWN, 10000))
    {
       npc_cthun_stomach_exit_triggerAI* pAI = dynamic_cast<npc_cthun_stomach_exit_triggerAI*>(pTriggerCreature->AI());
       if (pAI)
          pAI->StartSecondKnockback(pPlayer->GetObjectGuid()); 
    }


    return true;
}

void AddSC_boss_cthun()
{
    Script* pNewScript;

    //Eye
    pNewScript = new Script;
    pNewScript->Name = "boss_eye_of_cthun";
    pNewScript->GetAI = &GetAI_eye_of_cthun;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_cthun";
    pNewScript->GetAI = &GetAI_cthun;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_eye_tentacle";
    pNewScript->GetAI = &GetAI_eye_tentacle;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_claw_tentacle";
    pNewScript->GetAI = &GetAI_claw_tentacle;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_giant_claw_tentacle";
    pNewScript->GetAI = &GetAI_giant_claw_tentacle;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_giant_eye_tentacle";
    pNewScript->GetAI = &GetAI_giant_eye_tentacle;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_giant_flesh_tentacle";
    pNewScript->GetAI = &GetAI_flesh_tentacle;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_tentacle_portal";
    pNewScript->GetAI = &GetAI_portal;
    pNewScript->RegisterSelf();
    
    pNewScript = new Script;
    pNewScript->Name = "npc_cthun_aggro_dummy";
    pNewScript->GetAI = &GetAI_npc_cthun_aggro_dummy;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_cthun_stomach_exit_trigger";
    pNewScript->GetAI = &GetAI_npc_cthun_stomach_exit_triggerAI;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "area_trigger_cthun_stomach_knockback";
    pNewScript->pAreaTrigger = &AreaTrigger_stomach_knockback;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "area_trigger_cthun_stomach_teleport";
    pNewScript->pAreaTrigger = &AreaTrigger_stomach_teleport;
    pNewScript->RegisterSelf();
}
