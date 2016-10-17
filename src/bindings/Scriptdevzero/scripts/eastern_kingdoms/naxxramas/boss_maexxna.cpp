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
SDName: Boss_Maexxna
SD%Complete: 60
SDComment: this needs review, and rewrite of the webwrap ability
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "naxxramas.h"

enum
{
    SPELL_WEBWRAP           = 28622,                        //Spell is normally used by the webtrap on the wall NOT by Maexxna

    SPELL_WEBSPRAY          = 29484,
    SPELL_POISONSHOCK       = 28741,
    SPELL_NECROTICPOISON    = 28776,
    SPELL_ENRAGE            = 28747,
    // For web wrap so it can "fly"
    SPELL_ROOT_SELF         = 23973,

    //spellId invalid
    //SPELL_SUMMON_SPIDERLING = 29434,
    NPC_WEB_WRAP = 16486,
    NPC_WEB_WRAP_DUMMY = 800113,
    NPC_MAEXXNA_SPIDERLING = 17055
};

#define LOC_X1    3546.796f
#define LOC_Y1    -3869.082f
#define LOC_Z1    296.450f

#define LOC_X2    3531.271f
#define LOC_Y2    -3847.424f
#define LOC_Z2    299.450f

#define LOC_X3    3497.067f
#define LOC_Y3    -3843.384f
#define LOC_Z3    302.384f

struct MANGOS_DLL_DECL npc_web_wrapAI : public ScriptedAI
{
    npc_web_wrapAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        Reset();
    }
   
    void Reset()
    {
		//m_uiVictimGUID.Clear(); // no need to clear since web wrap is gonna die
    }
  
    // Don't attack anything
    void Aggro(Unit* /*pVictim*/) { return; }
    void AttackStart(Unit* /*pVictim*/) { return; }
    void MoveInLineOfSight(Unit* /*pWho*/) { }
    void UpdateAI(const uint32 uiDiff) 
    {
        return;
    }
};

CreatureAI* GetAI_npc_web_wrap(Creature* pCreature)
{
    return new npc_web_wrapAI(pCreature);
}

struct MANGOS_DLL_DECL npc_web_wrap_dummyAI : public ScriptedAI
{
    npc_web_wrap_dummyAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        Reset();
        m_bWrap = false;
    }

    ObjectGuid m_uiVictimGUID;
    uint32 m_uiWebWrapTimer;
    uint32 m_uiSpawnWebWrapTimer;
    bool m_bWrap;    
    
    void Reset()
    {
        //m_uiVictimGUID.Clear(); // no need to clear since web wrap is gonna die
    }

       void SetVictim(Unit* pVictim)
    {
        if (pVictim && pVictim->GetTypeId() == TYPEID_PLAYER)
        {
            // Vanilla spell 28618, 28619, 28620, 28621 had effect SPELL_EFFECT_PLAYER_PULL with EffectMiscValue = 200, 300, 400 and 500
            // All these spells trigger 28622 after 1 or 2 seconds
            // the EffectMiscValue may have been based on the distance between the victim and the target

            // NOTE: This implementation may not be 100% correct, but it gets very close to the expected result

            float fDist = m_creature->GetDistance2d(pVictim);
            // Switch the speed multiplier based on the distance from the web wrap
            uint32 uiEffectMiscValue = 400; // old: 200, 300, 400, 500 
            
            if (fDist < 25.0f)
                uiEffectMiscValue = 200;
            else if (fDist < 50.0f)
                uiEffectMiscValue = 300;         
            else if (fDist < 75.0f)        
                uiEffectMiscValue = 350;
            
            // This doesn't give the expected result in all cases
            // 0.04f was kinda ok height, needs more testing at further distances
            ((Player*)pVictim)->KnockBackFrom(m_creature, -fDist, uiEffectMiscValue * 0.04f/*0.033f*/);

            m_uiVictimGUID = pVictim->GetObjectGuid();
            m_uiWebWrapTimer = uiEffectMiscValue == 200 ? 1000 : 2000;
            m_uiSpawnWebWrapTimer = m_uiWebWrapTimer + 1000;
        }
    }
    
    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        if(pSummoned->GetEntry() == NPC_WEB_WRAP)
        {
            if (Player* pTarget = m_creature->GetMap()->GetPlayer(m_uiVictimGUID))
            {
                if (pTarget->isAlive())
                    pTarget->RemoveAurasDueToSpell(SPELL_WEBWRAP);
            }
            // No need for dummy if wrap died
            m_creature->ForcedDespawn();
        }
    }

    void UpdateAI(const uint32 uiDiff) 
    {
        if(!m_bWrap)
        {
            if (m_uiWebWrapTimer < uiDiff)               
            {                
                    if (Player* pTarget = m_creature->GetMap()->GetPlayer(m_uiVictimGUID))
                        pTarget->CastSpell(pTarget, SPELL_WEBWRAP, true, nullptr, nullptr, m_creature->GetObjectGuid());                
                    m_uiWebWrapTimer = 10000; // random number to not cast again until its true below
            }
             else
                m_uiWebWrapTimer -= uiDiff;          
             
            if (m_uiSpawnWebWrapTimer < uiDiff)               
            {                
                    if (Player* pTarget = m_creature->GetMap()->GetPlayer(m_uiVictimGUID))
                    {
                        if(Creature* pWrap = m_creature->SummonCreature(NPC_WEB_WRAP, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000))
                            pWrap->CastSpell(pWrap, SPELL_ROOT_SELF, true);
                    }
                    m_bWrap = true;
            }
             else
                m_uiSpawnWebWrapTimer -= uiDiff;                  
        }       
    }          
};

CreatureAI* GetAI_npc_web_wrap_dummy(Creature* pCreature)
{
    return new npc_web_wrap_dummyAI(pCreature);
}

struct MANGOS_DLL_DECL boss_maexxnaAI : public ScriptedAI
{
    boss_maexxnaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_naxxramas*)pCreature->GetInstanceData();
        Reset();
    }

    instance_naxxramas* m_pInstance;

    uint32 m_uiWebWrapTimer;
    uint32 m_uiWebSprayTimer;
    uint32 m_uiPoisonShockTimer;
    uint32 m_uiNecroticPoisonTimer;
    uint32 m_uiSummonSpiderlingTimer;
    bool   m_bEnraged;

    void Reset()
    {
        m_uiWebWrapTimer = 5000;//20000;                           //20 sec init, 40 sec normal
        m_uiWebSprayTimer = 40000;                          //40 seconds
        m_uiPoisonShockTimer = 20000;                       //20 seconds
        m_uiNecroticPoisonTimer = 30000;                    //30 seconds
        m_uiSummonSpiderlingTimer = 30000;                  //30 sec init, 40 sec normal
        m_bEnraged = false;
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MAEXXNA, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MAEXXNA, DONE);
    }
    
    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpellEntry)
    {
        // Only do emote if hit by enrage
        if (pSpellEntry->Id == SPELL_ENRAGE)
            m_creature->GenericTextEmote("Maexxna becomes enraged!", NULL, false);            
    }
    
    void DoCastWebWrap()
    {
        ThreatList const& tList = m_creature->getThreatManager().getThreatList();
        std::vector<Unit *> targets;

        //This spell doesn't work if we only have 1 player on threat list
        if (tList.size() < 2)
            return;

        //begin + 1 , so we don't target the one with the highest threat
        ThreatList::const_iterator itr = tList.begin();
        std::advance(itr, 1);

        //store the threat list in a different container
        for (;itr != tList.end(); ++itr)
        {
            Unit* target = m_creature->GetMap()->GetUnit((*itr)->getUnitGuid());

            //only on alive players
            if (target && target->isAlive() && target->GetTypeId() == TYPEID_PLAYER)
                targets.push_back(target);
        }

        //cut down to size if we have more than 3 targets
        while(targets.size() > 3)
            targets.erase(targets.begin()+rand()%targets.size());

        int i = 0;

        for(std::vector<Unit *>::iterator iter = targets.begin(); iter!= targets.end(); ++iter, ++i)
        {
            // Teleport the 3 targets to a location on the wall and summon a Web Wrap on them
            switch(i)
            {
                case 0:
//                     (*iter)->NearTeleportTo(LOC_X1, LOC_Y1, LOC_Z1, (*iter)->GetOrientation());
                    if (Creature* pWrap = m_creature->SummonCreature(NPC_WEB_WRAP_DUMMY, LOC_X1, LOC_Y1, LOC_Z1, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000))
                        ((npc_web_wrap_dummyAI*)pWrap->AI())->SetVictim((*iter));
                    break;
                case 1:
//                     (*iter)->NearTeleportTo(LOC_X2, LOC_Y2, LOC_Z2, (*iter)->GetOrientation());
                    if (Creature* pWrap = m_creature->SummonCreature(NPC_WEB_WRAP_DUMMY, LOC_X2, LOC_Y2, LOC_Z2, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000))
                        ((npc_web_wrap_dummyAI*)pWrap->AI())->SetVictim((*iter));
                    break;
                case 2:
//                     (*iter)->NearTeleportTo(LOC_X3, LOC_Y3, LOC_Z3, (*iter)->GetOrientation());
                    if (Creature* pWrap = m_creature->SummonCreature(NPC_WEB_WRAP_DUMMY, LOC_X3, LOC_Y3, LOC_Z3, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000))
                        ((npc_web_wrap_dummyAI*)pWrap->AI())->SetVictim((*iter));
                    break;
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Web Wrap
        if (m_uiWebWrapTimer < uiDiff)
        {
            DoCastWebWrap();
            m_uiWebWrapTimer = 40000;
        }
        else
            m_uiWebWrapTimer -= uiDiff;

        // Web Spray
        if (m_uiWebSprayTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_WEBSPRAY);
            m_uiWebSprayTimer = 40000;
        }
        else
            m_uiWebSprayTimer -= uiDiff;

        // Poison Shock
        if (m_uiPoisonShockTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_POISONSHOCK);
            m_uiPoisonShockTimer = 20000;
        }
        else
            m_uiPoisonShockTimer -= uiDiff;

        // Necrotic Poison
        if (m_uiNecroticPoisonTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_NECROTICPOISON);
            m_uiNecroticPoisonTimer = 30000;
        }
        else
            m_uiNecroticPoisonTimer -= uiDiff;

        // Summon Spiderling
        if (m_uiSummonSpiderlingTimer < uiDiff)
        {
            int a = urand(8,10);
            for(int i = 0; i < a; ++i)
            {
                Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
                Creature* Summoned = m_creature->SummonCreature(NPC_MAEXXNA_SPIDERLING, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                if (Summoned && target)
                    Summoned->AI()->AttackStart(target);
            }
                m_uiSummonSpiderlingTimer = 40000;
        }
        else
            m_uiSummonSpiderlingTimer -= uiDiff;

        //Enrage if not already enraged and below 30%
        if (!m_bEnraged && m_creature->GetHealthPercent() < 30.0f)
        {
            DoCastSpellIfCan(m_creature, SPELL_ENRAGE);
            m_bEnraged = true;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_maexxna(Creature* pCreature)
{
    return new boss_maexxnaAI(pCreature);
}

void AddSC_boss_maexxna()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "npc_web_wrap";
    pNewscript->GetAI = &GetAI_npc_web_wrap;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_web_wrap_dummy";
    pNewscript->GetAI = &GetAI_npc_web_wrap_dummy;
    pNewscript->RegisterSelf();
    
    pNewscript = new Script;
    pNewscript->Name = "boss_maexxna";
    pNewscript->GetAI = &GetAI_boss_maexxna;
    pNewscript->RegisterSelf();
}
