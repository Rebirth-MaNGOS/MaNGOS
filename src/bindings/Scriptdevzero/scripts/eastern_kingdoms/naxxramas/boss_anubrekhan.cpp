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
SDName: Boss_Anubrekhan
SD%Complete: 90
SDComment: Spells for adds, link with a dummy, check if emotes should be used, 
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "naxxramas.h"

enum
{
    SAY_GREET                   = -1533000,
    SAY_AGGRO1                  = -1533001,
    SAY_AGGRO2                  = -1533002,
    SAY_AGGRO3                  = -1533003,
    SAY_TAUNT1                  = -1533004,
    SAY_TAUNT2                  = -1533005,
    SAY_TAUNT3                  = -1533006,
    SAY_TAUNT4                  = -1533007,
    SAY_SLAY                    = -1533008,

    /*EMOTE_CRYPT_GUARD           = -1533153,     // really, have emotes?
    EMOTE_INSECT_SWARM          = -1533154,
    EMOTE_CORPSE_SCARABS        = -1533155,*/
    
    SPELL_IMPALE                = 28783,                    //May be wrong spell id. Causes more dmg than I expect
    SPELL_LOCUSTSWARM           = 28785,                    //This is a self buff that triggers the dmg debuff

    SPELL_SELF_SPAWN_5          = 29105,                    //This spawns 5 corpse scarabs ontop of us (most likely the pPlayer casts this on death)
    SPELL_SELF_SPAWN_10         = 28864,                    //This is used by the crypt guards when they die

    MOB_CRYPT_GUARD             = 16573,
    MOB_CORPSE_SCARAB         = 16698
};

static const DialogueEntry aIntroDialogue[] =
{
    {SAY_GREET,   NPC_ANUB_REKHAN,  8000},
    {SAY_TAUNT1,  NPC_ANUB_REKHAN,  13000},
    {SAY_TAUNT2,  NPC_ANUB_REKHAN,  11000},
    {SAY_TAUNT3,  NPC_ANUB_REKHAN,  10000},
    {SAY_TAUNT4,  NPC_ANUB_REKHAN,  0},
    {0, 0, 0}
};

static const float aCryptGuardLoc[4] = {3333.5f, -3475.9f, 287.1f, 3.17f};

struct MANGOS_DLL_DECL boss_anubrekhanAI : public ScriptedAI
{
    boss_anubrekhanAI(Creature* pCreature) : ScriptedAI(pCreature),
        m_introDialogue(aIntroDialogue)
    {
        m_pInstance = (instance_naxxramas*)pCreature->GetInstanceData();
        m_introDialogue.InitializeDialogueHelper(m_pInstance);
        m_bHasTaunted = false;
        Reset();
    }

    instance_naxxramas* m_pInstance;
    DialogueHelper m_introDialogue;
    
    float fX, fY, fZ;
    float FX, FY, FZ;
    
    uint32 m_uiImpaleTimer;
    uint32 m_uiLocustSwarmTimer;
    uint32 m_uiSummonTimer;
    uint32 m_uiGuardExplode;
    uint8 m_uiExplodedGuards;
    
    bool m_bHasTaunted;
    std::list<Creature*> m_lCryptGuards;
    GUIDList m_uiGuardGUID;
   
    void Reset()
    {
        m_uiExplodedGuards = 0;
        m_uiImpaleTimer = 15000;                            // 15 seconds
        m_uiLocustSwarmTimer = urand(80000, 120000);        // Random time between 80 seconds and 2 minutes for initial cast
        m_uiSummonTimer = m_uiLocustSwarmTimer + 45000;
        m_uiGuardExplode = 10000;
        GetCreatureListWithEntryInGrid(m_lCryptGuards, m_creature, MOB_CRYPT_GUARD, DEFAULT_VISIBILITY_INSTANCE);
        m_uiGuardGUID.clear();
    }

    void KilledUnit(Unit* pVictim)
    {
        //Force the player to spawn corpse scarabs via spell
        if (pVictim->GetTypeId() == TYPEID_PLAYER)
        {
            pVictim->CastSpell(pVictim, SPELL_SELF_SPAWN_5, true);
            pVictim->GetPosition(FX, FY, FZ);
            for(uint8 i = 0; i < 5; ++i)
                m_creature->SummonCreature(MOB_CORPSE_SCARAB, FX+irand(-3,3), FY+irand(-3,3), FZ, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
        }
        if (urand(0, 4))
            return;

        DoScriptText(SAY_SLAY, m_creature);
    }

    void Aggro(Unit* pWho)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ANUB_REKHAN, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ANUB_REKHAN, DONE);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!m_bHasTaunted && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 110.0f) && m_creature->IsWithinLOSInMap(pWho))
        {
            m_introDialogue.StartNextDialogueText(SAY_GREET);
            m_bHasTaunted = true;
        }
        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == MOB_CRYPT_GUARD)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pSummoned->AI()->AttackStart(pTarget);
        }
        if (pSummoned->GetEntry() == MOB_CORPSE_SCARAB)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pSummoned->AddThreat(pTarget,10000.0f);
        }
        pSummoned->SetRespawnEnabled(false);        // no respawns
    }
    
    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == MOB_CRYPT_GUARD)
        {
            pSummoned->CastSpell(pSummoned, SPELL_SELF_SPAWN_10, true);
            pSummoned->GetPosition(FX, FY, FZ);
            for(uint8 i = 0; i < 10; ++i)
                m_creature->SummonCreature(MOB_CORPSE_SCARAB, FX+irand(-3,3), FY+irand(-3,3), FZ, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
        }            
    }
        
    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ANUB_REKHAN, FAIL);
        
        // Respawn Guards       
        if (!m_lCryptGuards.empty())
        {
            for(std::list<Creature*>::iterator itr = m_lCryptGuards.begin(); itr != m_lCryptGuards.end(); ++itr)
            {
                if ((*itr) && !(*itr)->isAlive())
                    (*itr)->Respawn();
            }
        }
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        m_introDialogue.DialogueUpdate(uiDiff);
        
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        
       if(m_uiExplodedGuards <= 2)
       {
            if (m_uiGuardExplode < uiDiff)
            {            
                 if (!m_lCryptGuards.empty())
                {
                    for(std::list<Creature*>::iterator itr = m_lCryptGuards.begin(); itr != m_lCryptGuards.end(); ++itr)
                    {
                        if ((*itr) && !(*itr)->isAlive())
                        {                            
                            // Check if we already exploded corpse    
                            if(std::find(m_uiGuardGUID.begin(), m_uiGuardGUID.end(), (*itr)->GetObjectGuid()) != m_uiGuardGUID.end())
                                continue;
                            
                            // Spawn scarabs         
                            (*itr)->CastSpell((*itr), SPELL_SELF_SPAWN_10, true);    
                            (*itr)->GetPosition(fX, fY, fZ);
                            for(uint8 i = 0; i < 10; ++i)
                                m_creature->SummonCreature(MOB_CORPSE_SCARAB, fX+irand(-3,3), fY+irand(-3,3), fZ, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                           
                            ++m_uiExplodedGuards;
                            m_uiGuardGUID.push_back((*itr)->GetObjectGuid());
                            break;
                        }
                    }
                }
                m_uiGuardExplode = 10000;
            }            
            else
                m_uiGuardExplode -= uiDiff;            
       }  
       
        // Impale
        if (m_uiImpaleTimer < uiDiff)
        {
            //Cast Impale on a random target
            //Do NOT cast it when we are afflicted by locust swarm
            if (!m_creature->HasAura(SPELL_LOCUSTSWARM))
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                    DoCastSpellIfCan(target, SPELL_IMPALE);
            }

            m_uiImpaleTimer = 15000;
        }
        else
            m_uiImpaleTimer -= uiDiff;

        // Locust Swarm
        if (m_uiLocustSwarmTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_LOCUSTSWARM);
            m_uiLocustSwarmTimer = urand(70000, 120000);
            m_uiSummonTimer = 3000;
        }
        else
            m_uiLocustSwarmTimer -= uiDiff;

        // Summon
        if (m_uiSummonTimer < uiDiff)
        {
            m_creature->SummonCreature(MOB_CRYPT_GUARD, aCryptGuardLoc[0], aCryptGuardLoc[1], aCryptGuardLoc[2], aCryptGuardLoc[3], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
            m_uiSummonTimer = m_uiLocustSwarmTimer + 45000;
        }
        else
            m_uiSummonTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_anubrekhan(Creature* pCreature)
{
    return new boss_anubrekhanAI(pCreature);
}

struct MANGOS_DLL_DECL mob_cryptguardsAI : public ScriptedAI
{
    mob_cryptguardsAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
        m_pInstance = (instance_naxxramas*)pCreature->GetInstanceData();
        Reset();
	}
	
	instance_naxxramas* m_pInstance;
    float FX, FY, FZ;
        
	void Reset()
    {
    } 
		
	void KilledUnit(Unit* pVictim)
    {
        //Force the player to spawn corpse scarabs via spell
        if (pVictim->GetTypeId() == TYPEID_PLAYER)
        {
            pVictim->CastSpell(pVictim, SPELL_SELF_SPAWN_5, true);
            pVictim->GetPosition(FX, FY, FZ);
            for(uint8 i = 0; i < 5; ++i)
                m_creature->SummonCreature(MOB_CORPSE_SCARAB, FX+irand(-3,3), FY+irand(-3,3), FZ, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
        }
    }
    	
	 void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_cryptguards(Creature* pCreature)
{
    return new mob_cryptguardsAI(pCreature);
}

void AddSC_boss_anubrekhan()
{
    Script* pNewscript;
    
	pNewscript = new Script;
    pNewscript->Name = "boss_anubrekhan";
    pNewscript->GetAI = &GetAI_boss_anubrekhan;
    pNewscript->RegisterSelf();
	
	pNewscript = new Script;
    pNewscript->Name = "mob_cryptguards";
    pNewscript->GetAI = &GetAI_mob_cryptguards;
    pNewscript->RegisterSelf();
} 
