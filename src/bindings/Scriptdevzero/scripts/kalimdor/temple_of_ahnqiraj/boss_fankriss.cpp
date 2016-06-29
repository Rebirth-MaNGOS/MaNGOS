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
SDName: Boss_Fankriss
SD%Complete: 100
SDComment: sound not implemented
SDCategory: Temple of Ahn'Qiraj
EndScriptData */

#include "precompiled.h"
#include "temple_of_ahnqiraj.h"

#define SOUND_SENTENCE_YOU 8588
#define SOUND_SERVE_TO     8589
#define SOUND_LAWS         8590
#define SOUND_TRESPASS     8591
#define SOUND_WILL_BE      8592

#define SPELL_MORTAL_WOUND 28467
#define SPELL_ROOT         28858

// Enrage for his spawns
#define SPELL_ENRAGE       28798

struct BugTunnels
{
    float m_fX, m_fY, m_fZ, m_fO;
};

static const BugTunnels aFankrissBugTunnels[7] =
{
    {-8000.50f, 1222.50f, -81.232f, 3.22f},  //Bug Tunnel 1
    {-8043.267f, 1256.22f, -83.59f, 4.64f},  //Bug Tunnel 2
    {-8022.94f, 1150.68f, -89.59f, 2.19f},   //Bug Tunnel 3              
};

struct MANGOS_DLL_DECL boss_fankrissAI : public ScriptedAI
{
    boss_fankrissAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();        
    }
    ScriptedInstance* m_pInstance;
    uint32 MortalWound_Timer;
    uint32 SpawnHatchlings_Timer;
    uint32 SpawnSpawns_Timer;
    uint32 m_secondSpawn;
    int32 m_extraSpawns;
    int Rand;
    int RandX;
    int RandY;
    
    int32 m_uiHealthSpawnAdd;
    bool m_b1add;
    bool m_b2add;
    bool m_b3add;

    Creature* Spawn;

    ObjectGuid m_TPTarget;
    uint32 m_uiRootTimer;
    
    void Reset()
    {
        MortalWound_Timer = urand(10000, 15000);
        SpawnHatchlings_Timer = urand(6000, 12000);
        SpawnSpawns_Timer = urand(15000, 45000);
        m_TPTarget = ObjectGuid();
        m_uiRootTimer = 200;
        m_uiHealthSpawnAdd = urand(88,96);

        m_secondSpawn = 0;
        m_extraSpawns = 0;
        
        m_b1add = false;
        m_b2add = false;
        m_b3add = false;
    }
    
    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_FANKRISS, IN_PROGRESS);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_FANKRISS, FAIL);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_FANKRISS, DONE);
    }


    void SummonSpawn(Unit* pVictim)
    {
        Rand = 10 + (rand()%10);
        switch(urand(0, 1))
        {
            case 0: RandX = 0 - Rand; break;
            case 1: RandX = 0 + Rand; break;
        }
        Rand = 0;
        Rand =  10 + (rand()%10);
        switch(urand(0, 1))
        {
            case 0: RandY = 0 - Rand; break;
            case 1: RandY = 0 + Rand; break;
        }
        Rand = 0;
        Spawn = DoSpawnCreature(15630, RandX, RandY, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
        if (Spawn && pVictim)
            Spawn->AI()->AttackStart(pVictim);
    }
    
    void CastCleaveMortalWound()
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
                if (target && target->GetTypeId() == TYPEID_PLAYER && m_creature->isInFrontInMap(target, 8.f, (PI / 180) * 90) && m_creature->CanReachWithMeleeAttack(target))
                    targetList.push_back(target);
            }
        }

        for (Unit* target : targetList)
            m_creature->CastSpell(target, SPELL_MORTAL_WOUND, true);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //MortalWound_Timer
        if (MortalWound_Timer < diff)
        {
            CastCleaveMortalWound();
            MortalWound_Timer = urand(8000,12000);

            // Recheck the target after having cast on multiple people.
            if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                return;

        }else MortalWound_Timer -= diff;

        if(m_creature->GetHealthPercent() <= m_uiHealthSpawnAdd && m_creature->GetHealthPercent() > 6)
        {
            SummonSpawn(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0));
            m_uiHealthSpawnAdd = m_creature->GetHealthPercent() - urand(15,20);

            m_extraSpawns = urand(0,2);

            if(m_extraSpawns > 0)
                m_secondSpawn = 5000;
        }

        if(m_secondSpawn)
        {
            if(m_secondSpawn <= diff)
            {
                SummonSpawn(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0));

                --m_extraSpawns;

                if(m_extraSpawns > 0)
                    m_secondSpawn = 5000;
                else
                    m_secondSpawn = 0;
            }
            else
                m_secondSpawn -= diff;
        }
                 
        // Teleporting Random Target to one of the three tunnels and spawn 4 hatchlings near the gamer.
        //We will only telport if fankriss has more than 3% of hp so teleported gamers can always loot.
        if (m_creature->GetHealthPercent() > 3.0f)
        {
            if (SpawnHatchlings_Timer< diff)
            {
                Unit* target = NULL;
                target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,1);
                
                // 10 tries to find a player
                for(int i = 0; i < 10; ++i)
                {
                    if(target && target->GetTypeId() != TYPEID_PLAYER)
                        target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,1);
                    else
                        break;
                }
                
                if (target && target->GetTypeId() == TYPEID_PLAYER)
                {
                    
                    if (m_creature->getThreatManager().getThreat(target))
                        m_creature->getThreatManager().modifyThreatPercent(target, -100);

                    uint8 bugTunnel = urand(0,2);

                    target->NearTeleportTo(aFankrissBugTunnels[bugTunnel].m_fX, 
                            aFankrissBugTunnels[bugTunnel].m_fY, aFankrissBugTunnels[bugTunnel].m_fZ, 
                            aFankrissBugTunnels[bugTunnel].m_fO);

                    m_TPTarget = target->GetObjectGuid();

                    int a = urand(3,7);
                    for(int i = 0; i < a; ++i)
                    {
                        Creature* Hatchling = m_creature->SummonCreature(15962, aFankrissBugTunnels[0].m_fX, aFankrissBugTunnels[0].m_fY, aFankrissBugTunnels[0].m_fZ, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                         if (Hatchling)
                        {                                  
                            if(bugTunnel == 0)
                                Hatchling->AI()->AttackStart(target);
                            else
                            {
                                if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                                    Hatchling->AI()->AttackStart(pTarget);
                            }
                        }                        
                    }                    
                    a = urand(3,7);
                    for(int i = 0; i < a; ++i)
                    {
                        Creature* Hatchling = m_creature->SummonCreature(15962, aFankrissBugTunnels[1].m_fX, aFankrissBugTunnels[1].m_fY, aFankrissBugTunnels[1].m_fZ, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                        if (Hatchling)
                        {                                  
                            if(bugTunnel == 1)
                                Hatchling->AI()->AttackStart(target);
                            else
                            {
                                if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                                    Hatchling->AI()->AttackStart(pTarget);
                            }
                        }
                    }
                    a = urand(3,7);
                    for(int i = 0; i < a; ++i)
                    {
                        Creature* Hatchling = m_creature->SummonCreature(15962, aFankrissBugTunnels[2].m_fX, aFankrissBugTunnels[2].m_fY, aFankrissBugTunnels[2].m_fZ, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                        if (Hatchling)
                        {                                  
                            if(bugTunnel == 2)
                                Hatchling->AI()->AttackStart(target);
                            else
                            {
                                if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                                    Hatchling->AI()->AttackStart(pTarget);
                            }
                        }
                    }                    
                }
                SpawnHatchlings_Timer = urand(45000, 60000);
            }else SpawnHatchlings_Timer -= diff;
        }

        if (m_TPTarget != ObjectGuid())
        {
            if (m_uiRootTimer <= diff)
            {
                if (Player* pTarget = m_creature->GetMap()->GetPlayer(m_TPTarget))
                    DoCastSpellIfCan(pTarget, SPELL_ROOT, CAST_TRIGGERED);

                m_TPTarget = ObjectGuid();
                m_uiRootTimer = 200;
            }
            else
                m_uiRootTimer -= diff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_fankriss(Creature* pCreature)
{
    return new boss_fankrissAI(pCreature);
}

/*######
## npc_fankriss_aggro_dummy
######*/

struct MANGOS_DLL_DECL npc_fankriss_aggro_dummyAI : public ScriptedAI
{
    npc_fankriss_aggro_dummyAI(Creature* pCreature) : ScriptedAI(pCreature)
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
        if (pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 10.0f) &&  m_creature->IsWithinLOSInMap(pWho))
        {
            if(m_pInstance)
            {
                Creature* pFankriss = m_pInstance->GetSingleCreatureFromStorage(NPC_FANKRISS);
                if(pFankriss && pFankriss->isAlive() && !pFankriss->isInCombat())
                    pFankriss->AI()->AttackStart(pWho);
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

CreatureAI* GetAI_npc_fankriss_aggro_dummy(Creature* pCreature)
{
    return new npc_fankriss_aggro_dummyAI(pCreature);
}

void AddSC_boss_fankriss()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_fankriss";
    newscript->GetAI = &GetAI_boss_fankriss;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "npc_fankriss_aggro_dummy";
    newscript->GetAI = &GetAI_npc_fankriss_aggro_dummy;
    newscript->RegisterSelf();
}
