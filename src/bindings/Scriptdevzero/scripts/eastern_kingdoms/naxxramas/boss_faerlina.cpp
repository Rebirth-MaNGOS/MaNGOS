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
SDName: Boss_Faerlina
SD%Complete: 90
SDComment:
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "naxxramas.h"

enum
{
    SAY_GREET                 = -1533009,
    SAY_AGGRO1                = -1533010,
    SAY_AGGRO2                = -1533011,
    SAY_AGGRO3                = -1533012,
    SAY_AGGRO4                = -1533013,
    SAY_SLAY1                 = -1533014,
    SAY_SLAY2                   = -1533015,
    SAY_DEATH                   = -1533016,

    //SOUND_RANDOM_AGGRO        = 8955,                              //soundId containing the 4 aggro sounds, we not using this

    SPELL_POSIONBOLT_VOLLEY   = 28796,
    SPELL_ENRAGE              = 28798,
    SPELL_RAINOFFIRE          = 28794,
    SPELL_WIDOWS_EMBRACE        = 28732,
    
    NPC_NAXXRAMAS_FOLLOWER = 16505,
    NPC_NAXXRAMAS_WORSHIPPER = 16506
};

static Loc WorshiperSpawn[]=
{
    {3350.61f, -3619.74f, 261.18f, 4.65f},
    {3341.36f, -3619.35f, 261.18f, 4.68f},
    {3356.69f, -3621.17f, 261.18f, 4.38f},
    {3364.08f, -3622.85f, 261.18f, 4.35f}
};

static Loc FollowerSpawn[]=
{
    {3359.75f, -3621.77f, 261.18f, 4.54f},
    {3346.29f, -3619.32f, 261.18f, 4.61f}
};

struct MANGOS_DLL_DECL boss_faerlinaAI : public ScriptedAI
{
    boss_faerlinaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_naxxramas*)pCreature->GetInstanceData();
        m_bHasTaunted = false;
        SpawnAdds();
        m_bRespawnAdds = false;
        Reset();
    }

    instance_naxxramas* m_pInstance;

    uint32 m_uiPoisonBoltVolleyTimer;
    uint32 m_uiRainOfFireTimer;
    uint32 m_uiEnrageTimer;
    bool   m_bHasTaunted;
    
   bool m_bRespawnAdds; 
    
    GUIDList m_uiFollowerGUID;
    GUIDList m_uiWorshipperGUID;
    
    void Reset()
    {
        m_uiPoisonBoltVolleyTimer = 8000;
        m_uiRainOfFireTimer = 16000;
        m_uiEnrageTimer = 60000;
        
        // respawn adds 
        if(m_bRespawnAdds)
            RespawnAdds();
    }

    void Aggro(Unit* /*pWho*/)
    {
        switch(urand(0, 3))
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
            case 3: DoScriptText(SAY_AGGRO4, m_creature); break;
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FAERLINA, IN_PROGRESS);
    }

    void JustSummoned(Creature* pSummoned)
    {
            if(pSummoned->GetEntry() == NPC_NAXXRAMAS_FOLLOWER)
                m_uiFollowerGUID.push_back(pSummoned->GetObjectGuid());
            if(pSummoned->GetEntry() == NPC_NAXXRAMAS_WORSHIPPER)
                m_uiWorshipperGUID.push_back(pSummoned->GetObjectGuid());     
    }
    
    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        if(pSummoned->GetEntry() == NPC_NAXXRAMAS_WORSHIPPER)
            m_creature->CastSpell(m_creature, SPELL_WIDOWS_EMBRACE, true);
        m_bRespawnAdds = true;
    }
    
    void SpawnAdds()
    {
        for(uint8 i = 0; i < 4; ++i)
            m_creature->SummonCreature(NPC_NAXXRAMAS_WORSHIPPER, WorshiperSpawn[i].x, WorshiperSpawn[i].y, WorshiperSpawn[i].z, WorshiperSpawn[i].o, TEMPSUMMON_DEAD_DESPAWN, 0);
        for(uint8 i = 0; i < 2; ++i)
            m_creature->SummonCreature(NPC_NAXXRAMAS_FOLLOWER, FollowerSpawn[i].x, FollowerSpawn[i].y, FollowerSpawn[i].z, FollowerSpawn[i].o, TEMPSUMMON_DEAD_DESPAWN, 0);
    }
    
    void RespawnAdds()
    {       
        for (GUIDList::iterator itr = m_uiFollowerGUID.begin(); itr != m_uiFollowerGUID.end(); itr++)
            if (Creature* pFollower = m_creature->GetMap()->GetCreature(*itr))
                if (!pFollower->isAlive())
                    pFollower->Respawn();
         for (GUIDList::iterator itr = m_uiWorshipperGUID.begin(); itr != m_uiWorshipperGUID.end(); itr++)
            if (Creature* pWorshipper = m_creature->GetMap()->GetCreature(*itr))
                if (!pWorshipper->isAlive())
                    pWorshipper->Respawn();     
                
        m_bRespawnAdds = false;
    }
    
    void MoveInLineOfSight(Unit* pWho)
    {
        if (!m_bHasTaunted && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 80.0f) &&  m_creature->IsWithinLOSInMap(pWho))
        {
            DoScriptText(SAY_GREET, m_creature);
            m_bHasTaunted = true;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void KilledUnit(Unit* /*pVictim*/)
    {
        DoScriptText(urand(0, 1)?SAY_SLAY1:SAY_SLAY2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FAERLINA, DONE);
    }
    
     void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_FAERLINA, FAIL);
    }
    
    // Widow's Embrace prevents frenzy and poison bolt, if it removes frenzy, next frenzy is sceduled in 60s
    // It is likely that this _should_ be handled with some dummy aura(s) - but couldn't find any
    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpellEntry) override
    {
        // Check if we hit with Widow's Embrave
        if (pSpellEntry->Id == SPELL_WIDOWS_EMBRACE)
        {
            // If we remove the Frenzy, the Enrage Timer is reseted to 60s
            if (m_creature->HasAura(SPELL_ENRAGE))
            {
                m_uiEnrageTimer = 60000;
                m_creature->RemoveAurasDueToSpell(SPELL_ENRAGE);
            }

            // In any case we prevent Frenzy and Poison Bolt Volley for Widow's Embrace Duration (30s)
            // We do this be setting the timers to at least bigger than 30s
            m_uiEnrageTimer = std::max(m_uiEnrageTimer, (uint32)30000);
            m_uiPoisonBoltVolleyTimer = std::max(m_uiPoisonBoltVolleyTimer, urand(33000, 38000));
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Poison Bolt Volley
        if (m_uiPoisonBoltVolleyTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_POSIONBOLT_VOLLEY);
            m_uiPoisonBoltVolleyTimer = urand(10000,12000);
        }
        else
            m_uiPoisonBoltVolleyTimer -= uiDiff;

        // Rain Of Fire
        if (m_uiRainOfFireTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, SPELL_RAINOFFIRE);

            m_uiRainOfFireTimer = urand(10000, 16000);
        }
        else
            m_uiRainOfFireTimer -= uiDiff;

        //Enrage_Timer
        if (m_uiEnrageTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_ENRAGE);
            m_uiEnrageTimer = 60000;
        }
        else 
            m_uiEnrageTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_faerlina(Creature* pCreature)
{
    return new boss_faerlinaAI(pCreature);
}

void AddSC_boss_faerlina()
{
    Script* pNewscript;
    pNewscript = new Script;
    pNewscript->Name = "boss_faerlina";
    pNewscript->GetAI = &GetAI_boss_faerlina;
    pNewscript->RegisterSelf();
}
