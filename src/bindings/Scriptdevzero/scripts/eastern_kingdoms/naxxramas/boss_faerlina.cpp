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
    SAY_AGGRO_1                = -1533010,
    SAY_ENRAGE_1                = -1533011,
    SAY_ENRAGE_2                = -1533012,
    SAY_ENRAGE_3                = -1533013,
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

struct MANGOS_DLL_DECL boss_faerlinaAI : public ScriptedAI
{
    boss_faerlinaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_naxxramas*)pCreature->GetInstanceData();
        m_bHasTaunted = false;
        Reset();
    }

    instance_naxxramas* m_pInstance;

    uint32 m_uiPoisonBoltVolleyTimer;
    uint32 m_uiRainOfFireTimer;
    uint32 m_uiEnrageTimer;
    bool   m_bHasTaunted;
    
    void Reset()
    {
        m_uiPoisonBoltVolleyTimer = 8000;
        m_uiRainOfFireTimer = 16000;
        m_uiEnrageTimer = 60000;       
    }

    void Aggro(Unit* /*pWho*/)
    {
       DoScriptText(SAY_AGGRO_1, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FAERLINA, IN_PROGRESS);
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
        
        // Respawn Worshippers 
        std::list<Creature*> m_lWorshippers;
        GetCreatureListWithEntryInGrid(m_lWorshippers, m_creature, NPC_NAXXRAMAS_WORSHIPPER, DEFAULT_VISIBILITY_INSTANCE);

        if (!m_lWorshippers.empty())
        {
            for(std::list<Creature*>::iterator itr = m_lWorshippers.begin(); itr != m_lWorshippers.end(); ++itr)
            {
                if ((*itr) && !(*itr)->isAlive())
                    (*itr)->Respawn();
            }
        }
        
        // Respawn followers
        std::list<Creature*> m_lFollowers;
        GetCreatureListWithEntryInGrid(m_lFollowers, m_creature, NPC_NAXXRAMAS_FOLLOWER, DEFAULT_VISIBILITY_INSTANCE);

        if (!m_lFollowers.empty())
        {
            for(std::list<Creature*>::iterator itr = m_lFollowers.begin(); itr != m_lFollowers.end(); ++itr)
            {
                if ((*itr) && !(*itr)->isAlive())
                    (*itr)->Respawn();
            }
        }        
    }
    
    // Widow's Embrace prevents frenzy and poison bolt, if it removes frenzy, next frenzy is sceduled in 60s
    // It is likely that this _should_ be handled with some dummy aura(s) - but couldn't find any
    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpellEntry)
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
        
        // Text on real apply of enrage
        if (pSpellEntry->Id == SPELL_ENRAGE)
        {
            switch (urand(0,2))
            {
                case 0: DoScriptText(SAY_ENRAGE_1, m_creature); break;
                case 1: DoScriptText(SAY_ENRAGE_2, m_creature); break;
                case 2: DoScriptText(SAY_ENRAGE_3, m_creature); break;
            }
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
            m_creature->CastSpell(m_creature, SPELL_ENRAGE, true);
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

/*######
## mob_naxxramas_worshipper
######*/

enum eNaxxramasWorshipper
{
    SPELL_FIREBALL = 20692,       // not sure which spell
};

struct MANGOS_DLL_DECL mob_naxxramas_worshipperAI : public ScriptedAI
{
    mob_naxxramas_worshipperAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_naxxramas*)pCreature->GetInstanceData();
        Reset();
    }

    instance_naxxramas* m_pInstance;

    bool m_bDie;
    uint32 m_bDieTimer;
    uint32 m_bFireballTimer;
    
    void Reset()
    {
        m_bDie = false;
        m_bFireballTimer = urand(1000, 10000);
    }    
    
    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (uiDamage > m_creature->GetHealth()) 
        {
            m_creature->CastSpell(m_creature, SPELL_WIDOWS_EMBRACE, true);            
            m_bDieTimer = 1000;
            m_bDie = true;
        }
        if(m_bDie)
            uiDamage = 0;
    }
        
    void UpdateAI(const uint32 uiDiff)
    {
        // Return if we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        
        // Fireball Timer
        if (m_bFireballTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FIREBALL);
            m_bFireballTimer = urand(5000, 10000);
        }
        else 
            m_bFireballTimer -= uiDiff;
        
        // Die Timer
        if (m_bDieTimer < uiDiff && m_bDie)
        {
            m_creature->DealDamage(m_creature, m_creature->GetHealth() + 1, nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, true);
        }
        else 
            m_bDieTimer -= uiDiff;
        
        if(!m_bDie)
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_naxxramas_worshipper(Creature* pCreature)
{
    return new mob_naxxramas_worshipperAI(pCreature);
}

/*######
## mob_naxxramas_follower
######*/

enum eNaxxramasFollower
{
    SPELL_CHARGE = 22911,       // not sure which spell
    SPELL_SILENCE  = 29943       // not sure which spell
};

struct MANGOS_DLL_DECL mob_naxxramas_followerAI : public ScriptedAI
{
    mob_naxxramas_followerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_naxxramas*)pCreature->GetInstanceData();
        Reset();
    }

    instance_naxxramas* m_pInstance;

    uint32 m_uiChargeTimer;
    uint32 m_uiSilenceTimer;
    uint32 m_uiSuspendTimer;

    void Reset()
    {
        m_uiChargeTimer = urand(8000,15000);
        m_uiSilenceTimer = urand(6000,20000);
        m_uiSuspendTimer = 0;
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        // Return if we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        
        // for charge animation
        if (m_uiSuspendTimer > uiDiff)
        {
            m_uiSuspendTimer -= uiDiff;
            return;
        }
        
        // Charge
        if (m_uiChargeTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                if(DoCastSpellIfCan(pTarget, SPELL_CHARGE)  == CAST_OK)
                    m_uiSuspendTimer = 2000;
            m_uiChargeTimer = urand(17000,22000);
        }
        else
            m_uiChargeTimer -= uiDiff;

        // Silence
        if (m_uiSilenceTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SILENCE);
            m_uiSilenceTimer = urand(15000,25000);
        }
        else
            m_uiSilenceTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_naxxramas_follower(Creature* pCreature)
{
    return new mob_naxxramas_followerAI(pCreature);
}

void AddSC_boss_faerlina()
{
    Script* pNewscript;
    pNewscript = new Script;
    pNewscript->Name = "boss_faerlina";
    pNewscript->GetAI = &GetAI_boss_faerlina;
    pNewscript->RegisterSelf();
    
    pNewscript = new Script;
    pNewscript->Name = "mob_naxxramas_follower";
    pNewscript->GetAI = &GetAI_mob_naxxramas_follower;
    pNewscript->RegisterSelf();
    
    pNewscript = new Script;
    pNewscript->Name = "mob_naxxramas_worshipper";
    pNewscript->GetAI = &GetAI_mob_naxxramas_worshipper;
    pNewscript->RegisterSelf();
}
