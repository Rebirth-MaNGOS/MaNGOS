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
    int Rand;
    int RandX;
    int RandY;

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
        const ThreatList& threatList = m_creature->getThreatManager().getThreatList();
        
        if(!threatList.empty())
        {
            for (HostileReference *currentReference : threatList)
            {
                Unit *target = currentReference->getTarget();
                if (target && target->GetTypeId() == TYPEID_PLAYER && m_creature->isInFrontInMap(target, 8.f, (PI / 180) * 90) && m_creature->CanReachWithMeleeAttack(target))
                    m_creature->CastSpell(target, SPELL_MORTAL_WOUND, true);                        
            }
        }
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
            MortalWound_Timer = urand(10000, 20000);
        }else MortalWound_Timer -= diff;

        //Summon 1-3 Spawns of Fankriss at random time.
        if (SpawnSpawns_Timer < diff)
        {
            switch(urand(0, 2))
            {
                case 0:
                    SummonSpawn(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0));
                    break;
                case 1:
                    SummonSpawn(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0));
                    SummonSpawn(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0));
                    break;
                case 2:
                    SummonSpawn(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0));
                    SummonSpawn(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0));
                    SummonSpawn(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0));
                    break;
            }
            SpawnSpawns_Timer = urand(30000, 60000);
        }else SpawnSpawns_Timer -= diff;

        // Teleporting Random Target to one of the three tunnels and spawn 4 hatchlings near the gamer.
        //We will only telport if fankriss has more than 3% of hp so teleported gamers can always loot.
        if (m_creature->GetHealthPercent() > 3.0f)
        {
            if (SpawnHatchlings_Timer< diff)
            {
                Player* target = NULL;
                target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,1);
                if (target && target->GetTypeId() == TYPEID_PLAYER)
                {
                    
                    if (m_creature->getThreatManager().getThreat(target))
                        m_creature->getThreatManager().modifyThreatPercent(target, -100);

                    uint8 bugTunnel = urand(0,2);

                    target->NearTeleportTo(aFankrissBugTunnels[bugTunnel].m_fX, 
                            aFankrissBugTunnels[bugTunnel].m_fY, aFankrissBugTunnels[bugTunnel].m_fZ, 
                            aFankrissBugTunnels[bugTunnel].m_fO);

                    m_TPTarget = target->GetObjectGuid();

                    for(int i = 0; i < 4; ++i)
                    {
                        Creature* Hatchling = m_creature->SummonCreature(15962, aFankrissBugTunnels[bugTunnel].m_fX, aFankrissBugTunnels[bugTunnel].m_fY, aFankrissBugTunnels[bugTunnel].m_fZ, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                        if (Hatchling)
                            Hatchling->AI()->AttackStart(target);
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
