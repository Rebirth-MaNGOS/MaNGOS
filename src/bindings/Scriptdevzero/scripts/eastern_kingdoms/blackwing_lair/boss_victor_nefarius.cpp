/*
 * Copyright (C) 2006-2011 ScriptDev2 <http://www.scriptdev2.com/>
 * Copyright (C) 2010-2011 ScriptDev0 <http://github.com/mangos-zero/scriptdev0>
 *
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
SDName: Boss_Victor_Nefarius
SD%Complete: 75
SDComment: Missing some text, Vael beginning event, and spawns Nef in wrong place
SDCategory: Blackwing Lair
EndScriptData */

#include "precompiled.h"
#include "blackwing_lair.h"
#include "TemporarySummon.h"

enum
{
    SAY_GAMESBEGIN_1                = -1469004,
    SAY_GAMESBEGIN_2                = -1469005,

    GOSSIP_ITEM_NEFARIUS_1          = -3469005,
    GOSSIP_ITEM_NEFARIUS_2          = -3469006,
    GOSSIP_ITEM_NEFARIUS_3          = -3469007,
    GOSSIP_TEXT_NEFARIUS_1          = 7134,
    GOSSIP_TEXT_NEFARIUS_2          = 7198,
    GOSSIP_TEXT_NEFARIUS_3          = 7199,

    MAX_DRAKES                      = 5,
    MAX_DRAKE_SUMMONS               = 42,
    NPC_BRONZE_DRAKANOID            = 14263,
    NPC_BLUE_DRAKANOID              = 14261,
    NPC_RED_DRAKANOID               = 14264,
    NPC_GREEN_DRAKANOID             = 14262,
    NPC_BLACK_DRAKANOID             = 14265,
    NPC_CHROMATIC_DRAKANOID         = 14302,

    SPELL_NEFARIUS_BARRIER          = 22663,                // immunity in phase 1
    SPELL_SHADOWBOLT                = 22677,
    SPELL_FEAR                      = 22678,
    SPELL_SHADOW_COMMAND	    = 22667,
    SPELL_SHADOWBLINK0		    = 22681,
    SPELL_SHADOWBLINK1		    = 22664,
    // shadowbolt vollye = 22665
    // silence = 22666 -> silence a player
    // shadow command = 22667 -> charm a player
    // shadowblink = 22664, 22681 -> teleport around the room, possibly random

    FACTION_BLACK_DRAGON            = 103,
    FACTION_FRIENDLY                = 35
};

struct SpawnLocation
{
    float m_fX, m_fY, m_fZ;
};

static const SpawnLocation aNefarianLocs[5] =
{
    {-7599.32f, -1191.72f, 475.545f},                       // opening where red/blue/black darknid spawner appear (ori 3.05433)
    {-7526.27f, -1135.04f, 473.445f},                       // same as above, closest to door (ori 5.75959)
    {-7498.177f, -1273.277f, 481.649f},                     // nefarian spawn location (ori 1.798)
    {-7592.0f, -1264.0f, 481.0f},                           // hide pos (useless; remove this)
    {-7502.002f, -1256.503f, 476.758f},                     // nefarian fly to this position
};

static const uint32 aPossibleDrake[MAX_DRAKES] = {NPC_BRONZE_DRAKANOID, NPC_BLUE_DRAKANOID, NPC_RED_DRAKANOID, NPC_GREEN_DRAKANOID, NPC_BLACK_DRAKANOID};

//This script is complicated
//Instead of morphing Victor Nefarius we will have him control phase 1
//And then have him spawn "Nefarian" for phase 2
//When phase 2 starts Victor Nefarius will go invisible and stop attacking
//If Nefarian reched home because nef killed the players then nef will trigger this guy to EnterEvadeMode
//and allow players to start the event over
//If nefarian dies then he will kill himself then he will be despawned in Nefarian script
//To prevent players from doing the event twice

// Dev note: Lord Victor Nefarius should despawn completely, then ~5 seconds later Nefarian should appear.

struct MANGOS_DLL_DECL boss_victor_nefariusAI : public ScriptedAI
{
    boss_victor_nefariusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();

        // Select the 2 different drakes that we are going to use until despawned
        // 5 possiblities for the first drake, 4 for the second, 20 total possiblites

        if (m_pInstance && m_pInstance->GetData(DRAKONID_1) && m_pInstance->GetData(DRAKONID_2))
        {
            m_uiDrakeTypeOne = m_pInstance->GetData(DRAKONID_1);
            m_uiDrakeTypeTwo = m_pInstance->GetData(DRAKONID_2);
        }
        else
        {
            // select two different numbers between 0..MAX_DRAKES-1
            uint8 uiPos1 = urand(0, MAX_DRAKES - 1);
            uint8 uiPos2 = (uiPos1 + urand(1, MAX_DRAKES - 1)) % MAX_DRAKES;

            m_uiDrakeTypeOne = aPossibleDrake[uiPos1];
            m_uiDrakeTypeTwo = aPossibleDrake[uiPos2];

            if (m_pInstance)
            {
                m_pInstance->SetData(DRAKONID_1, m_uiDrakeTypeOne);
                m_pInstance->SetData(DRAKONID_2, m_uiDrakeTypeTwo);
            }
        }

        // Make him friendly from the start so you can talk to him.
        m_creature->setFaction(FACTION_FRIENDLY);

        // Make visible if needed
        if (m_creature->GetVisibility() != VISIBILITY_ON)
            m_creature->SetVisibility(VISIBILITY_ON);

        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiAddSpawnTimer;
    uint32 m_uiSpawnWaveCounter;
    uint32 m_uiSpawnChromaticTimer;
    uint32 m_uiShadowBoltTimer;
    uint32 m_uiFearTimer;
    uint32 m_uiMindControlTimer;
    uint32 m_uiResetTimer;
    uint32 m_uiDrakeTypeOne;
    uint32 m_uiDrakeTypeTwo;
    uint32 m_uiShadowBlinkTimer0;
    uint32 m_uiShadowBlinkTimer1;
    uint32 m_uiKilledAdds;
    bool m_bIsNefarianSpawned;

    void Reset()
    {
        m_uiAddSpawnTimer   = 10000;
        m_uiSpawnWaveCounter = 0;
        m_uiSpawnChromaticTimer = urand(5000, 20000);
        m_uiShadowBoltTimer = 5000;
        m_uiFearTimer       = 8000;
        m_uiMindControlTimer = 15000;
        m_uiShadowBlinkTimer0 = 20000;
        m_uiShadowBlinkTimer1 = 0;
        m_uiResetTimer = 0;
        m_uiKilledAdds = 0;
        m_bIsNefarianSpawned = false;

        // set gossip flag to begin the event
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NEFARIAN, IN_PROGRESS);
    }

    void JustReachedHome()
    {
        m_creature->setFaction(FACTION_FRIENDLY);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_NEFARIAN, FAIL);

        // Remove all Drakanoids.
        instance_blackwing_lair* blackwing_lair = dynamic_cast<instance_blackwing_lair*>(m_pInstance);
        if (blackwing_lair)
        {
            for ( ObjectGuid current_drakanoid_guid : blackwing_lair->GetDrakonoidsAndBoneConstructs())
            {
                Creature* current_drakanoid = m_creature->GetMap()->GetCreature(current_drakanoid_guid);
                if (current_drakanoid)
                    ((TemporarySummon*) current_drakanoid)->UnSummon();
            }

            blackwing_lair->GetDrakonoidsAndBoneConstructs().clear();
        }

        // Set a 15 minute reset timer.
        m_uiResetTimer = time(nullptr) + 15 * MINUTE;
        m_creature->SetVisibility(VISIBILITY_OFF);

    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_NEFARIAN)
        {
            pSummoned->RemoveSplineFlag(SPLINEFLAG_WALKMODE);

            // see boss_onyxia (also note the removal of this in boss_nefarian)
            pSummoned->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND/* | UNIT_BYTE1_FLAG_UNK_2*/);
            pSummoned->AddSplineFlag(SPLINEFLAG_FLYING);

            // Let Nefarian fly towards combat area
            pSummoned->GetMotionMaster()->MovePoint(1, aNefarianLocs[4].m_fX, aNefarianLocs[4].m_fY, aNefarianLocs[4].m_fZ);
        }
        else
        {

            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pSummoned->AI()->AttackStart(pTarget);
        }

        pSummoned->SetRespawnDelay(7*DAY);
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiMotionType, uint32 uiPointId)
    {
        // If Nefarian has reached combat area, let him attack
        if (pSummoned->GetEntry() == NPC_NEFARIAN && uiMotionType == POINT_MOTION_TYPE && uiPointId == 1)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pSummoned->AI()->AttackStart(pTarget);
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        // Despawn self when Nefarian is killed
        if (pSummoned->GetEntry() == NPC_NEFARIAN)
            m_creature->ForcedDespawn();

        ++m_uiKilledAdds;

        // When a dragon is killed it should turn in to a skeleton.
        if (pSummoned->GetEntry() != NPC_NEFARIAN && pSummoned->GetDisplayId() != 12073)
            pSummoned->SetDisplayId(12073);

    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiResetTimer)
        {
            if (time(nullptr) >= m_uiResetTimer)
            {
                m_creature->SetVisibility(VISIBILITY_ON);
                m_uiResetTimer = 0;
            }
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        instance_blackwing_lair* blackwing_lair = dynamic_cast<instance_blackwing_lair*>(m_creature->GetInstanceData());


        if (!m_bIsNefarianSpawned)
        {
            //Begin phase 2 by spawning Nefarian
            if ( m_uiKilledAdds >= MAX_DRAKE_SUMMONS)
            {
                //Teleport Victor Nefarius way out of the map
                //MapManager::Instance().GetMap(m_creature->GetMapId(), m_creature)->CreatureRelocation(m_creature,0,0,-5000,0);

                //Inturrupt any spell casting
                m_creature->InterruptNonMeleeSpells(false);

                //Root self
                DoCastSpellIfCan(m_creature, 33356, CAST_TRIGGERED);

                //Make super invis
                if (m_creature->GetVisibility() != VISIBILITY_OFF)
                    m_creature->SetVisibility(VISIBILITY_OFF);

                // Do not teleport him away, this is not needed (invisible and rooted)
                //Teleport self to a hiding spot
                //m_creature->NearTeleportTo(aNefarianLocs[3].m_fX, aNefarianLocs[3].m_fY, aNefarianLocs[3].m_fZ, 0.0f);

                // Spawn Nefarian
                // Summon as active, to be able to work proper!
                m_creature->SummonCreature(NPC_NEFARIAN, aNefarianLocs[2].m_fX, aNefarianLocs[2].m_fY, aNefarianLocs[2].m_fZ, 0, TEMPSUMMON_DEAD_DESPAWN, 0, true);

                m_bIsNefarianSpawned = true;
            }


            // Only do this if we haven't spawned nef yet
            if ( m_uiKilledAdds <= MAX_DRAKE_SUMMONS)
            {
                // Shadowbolt Timer
                if (m_uiShadowBoltTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_SHADOWBOLT) == CAST_OK)
                            m_uiShadowBoltTimer = urand(3000, 10000);
                    }
                }
                else
                    m_uiShadowBoltTimer -= uiDiff;

                // Fear Timer
                if (m_uiFearTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_FEAR) == CAST_OK)
                            m_uiFearTimer = urand(10000, 20000);
                    }
                }
                else
                    m_uiFearTimer -= uiDiff;

                if (m_uiMindControlTimer <= uiDiff)
                {
                    Unit* random_target = GetRandomTargetFromThreatList();
                    if (random_target)
                    {
                        DoCastSpellIfCan(random_target, SPELL_SHADOW_COMMAND);

                        m_uiMindControlTimer = urand(20000, 45000);
                    }
                }
                else
                    m_uiMindControlTimer -= uiDiff;


                // Switch between the two Shadowblink spells to guarantee he doesn't end up in the same place twice.
                if (m_uiShadowBlinkTimer0)
                {
                    if (m_uiShadowBlinkTimer0 <= uiDiff)
                    {
                        DoCast(m_creature, SPELL_SHADOWBLINK0);

                        m_uiShadowBlinkTimer0 = 0;
                        m_uiShadowBlinkTimer1 = urand(10000, 20000);
                    }
                    else
                        m_uiShadowBlinkTimer0 -= uiDiff;
                }

                if (m_uiShadowBlinkTimer1)
                {
                    if (m_uiShadowBlinkTimer1 <= uiDiff)
                    {
                        DoCast(m_creature, SPELL_SHADOWBLINK0);

                        m_uiShadowBlinkTimer1 = 0;
                        m_uiShadowBlinkTimer0 = urand(10000, 20000);
                    }
                    else
                        m_uiShadowBlinkTimer1 -= uiDiff;
                }

                // Add spawning mechanism
                if (m_uiAddSpawnTimer < uiDiff)
                {
                    //Spawn 2 random types of creatures at the 2 locations
                    Creature* summoned_drakonoid;

                    summoned_drakonoid = m_creature->SummonCreature(m_uiDrakeTypeOne, aNefarianLocs[0].m_fX, aNefarianLocs[0].m_fY, aNefarianLocs[0].m_fZ,
                                         5.000f, TEMPSUMMON_MANUAL_DESPAWN, 30*MINUTE*IN_MILLISECONDS);

                    if (blackwing_lair && summoned_drakonoid)
                    {
                        summoned_drakonoid->setFaction(FACTION_BLACK_DRAGON);
                        blackwing_lair->GetDrakonoidsAndBoneConstructs().push_back(summoned_drakonoid->GetObjectGuid());
                    }

                    summoned_drakonoid = m_creature->SummonCreature(m_uiDrakeTypeTwo, aNefarianLocs[1].m_fX, aNefarianLocs[1].m_fY, aNefarianLocs[1].m_fZ,
                                         5.000f, TEMPSUMMON_MANUAL_DESPAWN, 0);

                    if (blackwing_lair && summoned_drakonoid)
                    {
                        summoned_drakonoid->setFaction(FACTION_BLACK_DRAGON);
                        blackwing_lair->GetDrakonoidsAndBoneConstructs().push_back(summoned_drakonoid->GetObjectGuid());
                    }

                    if (m_uiSpawnWaveCounter <= 20)
                    {
                        m_uiAddSpawnTimer = 8000 - 100.f * m_uiSpawnWaveCounter;
                    }
                    else
                        m_uiAddSpawnTimer = 6000;

                    ++m_uiSpawnWaveCounter;
                }
                else
                    m_uiAddSpawnTimer -= uiDiff;

                if (m_uiSpawnChromaticTimer <= uiDiff)
                {
                    for (short i = 0; i < 2; ++i)
                    {
                        Creature* summoned_drakonid;
                        summoned_drakonid = m_creature->SummonCreature(NPC_CHROMATIC_DRAKANOID, aNefarianLocs[i].m_fX, aNefarianLocs[i].m_fY, aNefarianLocs[i].m_fZ,
                                            5.000f, TEMPSUMMON_MANUAL_DESPAWN, 30*MINUTE*IN_MILLISECONDS);
                        
                        if (blackwing_lair && summoned_drakonid)
                        {
                            summoned_drakonid->setFaction(FACTION_BLACK_DRAGON);
                            blackwing_lair->GetDrakonoidsAndBoneConstructs().push_back(summoned_drakonid->GetObjectGuid());
                        }

                        m_uiSpawnChromaticTimer = 60000; // TODO: This time needs to be verified.
                    }
                }
                else
                    m_uiSpawnChromaticTimer -= uiDiff;
            }
        }
    }
};

CreatureAI* GetAI_boss_victor_nefarius(Creature* pCreature)
{
    return new boss_victor_nefariusAI(pCreature);
}

bool GossipHello_boss_victor_nefarius(Player* pPlayer, Creature* pCreature)
{
    // Only allow the Nef encounter in BWL.
    Map* pMap = pPlayer->GetMap();
    if (pMap && pMap->GetId() != 469)
        return true;

    pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NEFARIUS_1 , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_NEFARIUS_1, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_boss_victor_nefarius(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
    case GOSSIP_ACTION_INFO_DEF+1:
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NEFARIUS_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_NEFARIUS_2, pCreature->GetObjectGuid());
        break;
    case GOSSIP_ACTION_INFO_DEF+2:
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NEFARIUS_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_NEFARIUS_3, pCreature->GetObjectGuid());
        DoScriptText(SAY_GAMESBEGIN_1, pCreature);
        break;
    case GOSSIP_ACTION_INFO_DEF+3:
        pPlayer->CLOSE_GOSSIP_MENU();
        DoScriptText(SAY_GAMESBEGIN_2, pCreature);
        // remove gossip, set hostile and attack
        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        pCreature->setFaction(FACTION_BLACK_DRAGON);
        pCreature->CastSpell(pCreature, SPELL_NEFARIUS_BARRIER, false);
        pCreature->AI()->AttackStart(pPlayer);
        break;
    }
    return true;
}

void AddSC_boss_victor_nefarius()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_victor_nefarius";
    pNewScript->GetAI = &GetAI_boss_victor_nefarius;
    pNewScript->pGossipHello = &GossipHello_boss_victor_nefarius;
    pNewScript->pGossipSelect = &GossipSelect_boss_victor_nefarius;
    pNewScript->RegisterSelf();
}
