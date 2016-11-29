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
SDName: Boss_Gluth
SD%Complete: 90
SDComment: Should he turn towards zombie that is devoured? Should he really do emote for each zombie, could mean lots of spam
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "naxxramas.h"

enum
{
    EMOTE_ZOMBIE                    = -1533119,
    EMOTE_BOSS_GENERIC_ENRAGED      = -1000006,
//     EMOTE_DECIMATE                  = -1533152, // no emote on decimate

    SPELL_DOUBLE_ATTACK = 19818,
    SPELL_MORTALWOUND               = 25646,
    SPELL_DECIMATE                  = 28374,
    SPELL_FRENZY                    = 28371,
    SPELL_BERSERK                   = 26662,
    SPELL_TERRIFYING_ROAR           = 29685,
    // SPELL_SUMMON_ZOMBIE_CHOW      = 28216,               // removed from dbc: triggers 28217 every 6 secs
    // SPELL_CALL_ALL_ZOMBIE_CHOW    = 29681,               // removed from dbc: triggers 29682
    // SPELL_ZOMBIE_CHOW_SEARCH      = 28235,               // removed from dbc: triggers 28236 every 3 secs

    NPC_ZOMBIE_CHOW                 = 16360,                // old vanilla summoning spell 28217

    MAX_ZOMBIE_LOCATIONS            = 3,
};

static const float aZombieSummonLoc[MAX_ZOMBIE_LOCATIONS][3] =
{
    {3267.9f, -3172.1f, 297.42f},
    {3253.2f, -3132.3f, 297.42f},
    {3308.3f, -3185.8f, 297.42f},
};

struct MANGOS_DLL_DECL boss_gluthAI : public ScriptedAI
{
    boss_gluthAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_naxxramas*)pCreature->GetInstanceData();
        Reset();       
    }

    instance_naxxramas* m_pInstance;

    uint32 m_uiMortalWoundTimer;
    uint32 m_uiDecimateTimer;
    uint32 m_uiFrenzyTimer;
    uint32 m_uiRoarTimer;
    uint32 m_uiSummonTimer;
    uint32 m_uiZombieSearchTimer;

    uint8 m_uiDecimateCount;
    uint32 m_uiBerserkTimer;

    GUIDList m_lZombieChowGuidList;

    void Reset()
    {
        m_uiMortalWoundTimer  = 10000;
        m_uiDecimateTimer     = 110000; // approx 105 sec wowwiki, or is that after first one only?
        m_uiFrenzyTimer       = 10000;
        m_uiSummonTimer       = 6000;
        m_uiRoarTimer         = urand(15000, 20000); // seems legit, every one after is 20 sec. In video its after 17 sec
        m_uiZombieSearchTimer = 3000;

        m_uiDecimateCount = 0;
        m_uiBerserkTimer      = m_uiDecimateTimer * 5; // ~15 seconds after the third Decimate, random high amount since it'll be changed after 3rd decimate
        // kill all zombies that are alive
        DoCallAllZombieChow(1);
        
         // Cmangos, should this be here?
        DoCastSpellIfCan(m_creature, SPELL_DOUBLE_ATTACK, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void JustDied(Unit* /*pKiller*/) 
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GLUTH, DONE);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GLUTH, IN_PROGRESS);
    }

    void KilledUnit(Unit* pVictim)
    {
        // Restore 5% hp when killing a zombie
        if (pVictim->GetEntry() == NPC_ZOMBIE_CHOW)
        {
            DoScriptText(EMOTE_ZOMBIE, m_creature);
            m_creature->SetHealth(m_creature->GetHealth() + m_creature->GetMaxHealth() * 0.05f);
        }
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GLUTH, FAIL);
    }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->GetMotionMaster()->MoveFollow(m_creature, ATTACK_DISTANCE, 0);
        m_lZombieChowGuidList.push_back(pSummoned->GetObjectGuid());
    }

    void SummonedCreatureDespawn(Creature* pSummoned)
    {
        m_lZombieChowGuidList.remove(pSummoned->GetObjectGuid());
    }

    // Replaces missing spell 29682
    void DoCallAllZombieChow(int action = 0)
    {
        for (GUIDList::const_iterator itr = m_lZombieChowGuidList.begin(); itr != m_lZombieChowGuidList.end(); ++itr)
        {
            if (Creature* pZombie = m_creature->GetMap()->GetCreature(*itr))
            {
                if(action == 0)
                {
                    pZombie->GetMotionMaster()->MoveFollow(m_creature, ATTACK_DISTANCE, 0);
                    pZombie->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                }
                else    // kill zombies on reset
                    m_creature->DealDamage(pZombie, pZombie->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
        }
    }

    // Replaces missing spell 28236
    void DoSearchZombieChow()
    {
        for (GUIDList::const_iterator itr = m_lZombieChowGuidList.begin(); itr != m_lZombieChowGuidList.end(); ++itr)
        {
            if (Creature* pZombie = m_creature->GetMap()->GetCreature(*itr))
            {
                if (!pZombie->isAlive())
                    continue;

                // Devour a Zombie
                if (pZombie->IsWithinDistInMap(m_creature, 10.0f))  // or 15yrds?
                    m_creature->DealDamage(pZombie, pZombie->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
        }
    }
    
    // Need emote for berzerk
    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpellEntry)
    {
        // Only do emote if hit by the spells
        if (pSpellEntry->Id == SPELL_FRENZY)
            m_creature->GenericTextEmote("Gluth goes into a frenzy!", NULL, false);            
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiZombieSearchTimer < uiDiff)
        {
            DoSearchZombieChow();
            m_uiZombieSearchTimer = 3000;
        }
        else
            m_uiZombieSearchTimer -= uiDiff;

        // Mortal Wound
        if (m_uiMortalWoundTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_MORTALWOUND) == CAST_OK)
                m_uiMortalWoundTimer = 10000;
        }
        else
            m_uiMortalWoundTimer -= uiDiff;

        // Decimate
        if (m_uiDecimateTimer < uiDiff)
        {
            m_creature->CastSpell(m_creature, SPELL_DECIMATE, true);
            // This is called after like 2 sec of spell being cast should be instantly
            DoCallAllZombieChow(0);
            m_uiDecimateTimer = urand(100000, 110000); // approx 105 sec wowwiki, 108 in a video
            ++m_uiDecimateCount;
            
            // Always approx 15 sec after 3rd decimate
            if(m_uiDecimateCount >= 3)
                m_uiBerserkTimer = urand(10000, 15000);
        }
        else
            m_uiDecimateTimer -= uiDiff;

        // Enrage
        if (m_uiFrenzyTimer< uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FRENZY) == CAST_OK)
                m_uiFrenzyTimer = urand(10000, 12000); // approx 10 sec wowwiki
        }
        else
            m_uiFrenzyTimer -= uiDiff;

        // Terrifying Roar
        if (m_uiRoarTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_TERRIFYING_ROAR) == CAST_OK)
                m_uiRoarTimer = 20000;
        }
        else
            m_uiRoarTimer -= uiDiff;

        // Summon
        if (m_uiSummonTimer < uiDiff)
        {
            uint8 uiPos1 = urand(0, MAX_ZOMBIE_LOCATIONS - 1);
            m_creature->SummonCreature(NPC_ZOMBIE_CHOW, aZombieSummonLoc[uiPos1][0], aZombieSummonLoc[uiPos1][1], aZombieSummonLoc[uiPos1][2], 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0);

            uint8 uiPos2 = (uiPos1 + urand(1, MAX_ZOMBIE_LOCATIONS - 1)) % MAX_ZOMBIE_LOCATIONS;
            m_creature->SummonCreature(NPC_ZOMBIE_CHOW, aZombieSummonLoc[uiPos2][0], aZombieSummonLoc[uiPos2][1], aZombieSummonLoc[uiPos2][2], 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0);

            m_uiSummonTimer = 6000;
        }
        else
            m_uiSummonTimer -= uiDiff;

        // Berserk
        if (m_uiBerserkTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                m_uiBerserkTimer = MINUTE * 5 * IN_MILLISECONDS;
        }
        else
            m_uiBerserkTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_gluth(Creature* pCreature)
{
    return new boss_gluthAI(pCreature);
}

void AddSC_boss_gluth()
{
    Script* pNewscript;
    pNewscript = new Script;
    pNewscript->Name = "boss_gluth";
    pNewscript->GetAI = &GetAI_boss_gluth;
    pNewscript->RegisterSelf();
}
