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
SDName: Boss_Four_Horsemen
SD%Complete: 80
SDComment: Lady Blaumeux, Thane Korthazz, Sir Zeliek, Baron Rivendare, manually spawn the spirits and fix void zone
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "naxxramas.h"

enum
{
    //all horsemen
    SPELL_SHIELDWALL        = 29061,
    SPELL_BESERK            = 26662,

    //lady blaumeux
    SAY_BLAU_AGGRO          = -1533044,
    SAY_BLAU_TAUNT1         = -1533045,
    SAY_BLAU_TAUNT2         = -1533046,
    SAY_BLAU_TAUNT3         = -1533047,
    SAY_BLAU_SPECIAL        = -1533048,
    SAY_BLAU_SLAY           = -1533049,
    SAY_BLAU_DEATH          = -1533050,

    SPELL_MARK_OF_BLAUMEUX  = 28833,
    SPELL_UNYILDING_PAIN    = 57381,
    SPELL_VOIDZONE          = 28863,
    SPELL_SHADOW_BOLT       = 57374,

    //highlord mograine
    SAY_MOG_AGGRO1         = -1533065,
    SAY_MOG_AGGRO2         = -1533066,
    SAY_MOG_AGGRO3         = -1533067,
    SAY_MOG_SLAY1          = -1533068,
    SAY_MOG_SLAY2          = -1533069,
    SAY_MOG_SPECIAL        = -1533070,
    SAY_MOG_TAUNT1         = -1533071,
    SAY_MOG_TAUNT2         = -1533072,
    SAY_MOG_TAUNT3         = -1533073,
    SAY_MOG_DEATH          = -1533074,

    SPELL_MARK_OF_MOGRAINE = 28834,
    //SPELL_RIGHTEOUS_FIRE_PROC    = 28882,
    SPELL_RIGHTEOUS_FIRE = 28881,

    //thane korthazz
    SAY_KORT_AGGRO          = -1533051,
    SAY_KORT_TAUNT1         = -1533052,
    SAY_KORT_TAUNT2         = -1533053,
    SAY_KORT_TAUNT3         = -1533054,
    SAY_KORT_SPECIAL        = -1533055,
    SAY_KORT_SLAY           = -1533056,
    SAY_KORT_DEATH          = -1533057,

    SPELL_MARK_OF_KORTHAZZ  = 28832,
    SPELL_METEOR            = 26558,                        // m_creature->getVictim() auto-area spell but with a core problem

    //sir zeliek
    SAY_ZELI_AGGRO          = -1533058,
    SAY_ZELI_TAUNT1         = -1533059,
    SAY_ZELI_TAUNT2         = -1533060,
    SAY_ZELI_TAUNT3         = -1533061,
    SAY_ZELI_SPECIAL        = -1533062,
    SAY_ZELI_SLAY           = -1533063,
    SAY_ZELI_DEATH          = -1533064,

    SPELL_MARK_OF_ZELIEK    = 28835,
    SPELL_HOLY_WRATH        = 28883,

    // horseman spirits
    NPC_SPIRIT_OF_BLAUMEUX    = 16776,
    NPC_SPIRIT_MOGRAINE   = 16775,
    NPC_SPIRIT_OF_KORTHAZZ    = 16778,
    NPC_SPIRIT_OF_ZELIREK     = 16777,
    
    // spells, don't use
    SPELL_SPIRIT_BLAUMEUX = 28931,
    SPELL_SPIRIT_MOGRAINE = 28928,
    SPELL_SPIRIT_KORTHAZZ = 28932,
    SPELL_SPIRIT_ZELIEK = 28934,
};

struct MANGOS_DLL_DECL boss_lady_blaumeuxAI : public ScriptedAI
{
    boss_lady_blaumeuxAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 Mark_Timer;
    uint32 VoidZone_Timer;
    bool ShieldWall1;
    bool ShieldWall2;
    
    uint8 m_uiMarkCount;
    bool m_bEnrage;

    void Reset()
    {
        m_bEnrage = false;
        m_uiMarkCount = 0;
        Mark_Timer = 20000;                                 // First Horsemen Mark is applied at 20 sec.
        VoidZone_Timer = 12000;                             // right
        ShieldWall1 = true;
        ShieldWall2 = true;
    }

    void Aggro(Unit* /*who*/)
    {
        DoScriptText(SAY_BLAU_AGGRO, m_creature);
    }

    void KilledUnit(Unit* /*Victim*/)
    {
        DoScriptText(SAY_BLAU_SLAY, m_creature);
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_BLAU_DEATH, m_creature);
        m_creature->CastSpell(m_creature, SPELL_SPIRIT_BLAUMEUX, true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Mark of Blaumeux
        if (Mark_Timer < uiDiff)
        {
            m_creature->CastSpell(m_creature->getVictim(), SPELL_MARK_OF_BLAUMEUX, true);
            ++m_uiMarkCount;
            Mark_Timer = 12000;
        }
        else 
            Mark_Timer -= uiDiff;

        if(m_uiMarkCount >= 100 && !m_bEnrage)
        {
            m_creature->CastSpell(m_creature, SPELL_BESERK, true);
            m_bEnrage = true;
        }
            
        
        // Shield Wall - All 4 horsemen will shield wall at 50% hp and 20% hp for 20 seconds
        if (ShieldWall1 && m_creature->GetHealthPercent() < 50.0f)
        {
            if (ShieldWall1)
            {
                DoCastSpellIfCan(m_creature,SPELL_SHIELDWALL);
                ShieldWall1 = false;
            }
        }
        if (ShieldWall2 && m_creature->GetHealthPercent() < 20.0f)
        {
            if (ShieldWall2)
            {
                DoCastSpellIfCan(m_creature, SPELL_SHIELDWALL);
                ShieldWall2 = false;
            }
        }

        // Void Zone
        if (VoidZone_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_VOIDZONE);
            VoidZone_Timer = m_bEnrage ? 3000 : 12000;
        }
        else 
            VoidZone_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_lady_blaumeux(Creature* pCreature)
{
    return new boss_lady_blaumeuxAI(pCreature);
}

struct MANGOS_DLL_DECL boss_highlord_mograineAI : public ScriptedAI
{
   boss_highlord_mograineAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 Mark_Timer;
    bool ShieldWall1;
    bool ShieldWall2;

    uint8 m_uiMarkCount;
    bool m_bEnrage;

    void Reset()
    {
        m_bEnrage = false;
        m_uiMarkCount = 0;
        Mark_Timer = 20000;                                 // First Horsemen Mark is applied at 20 sec.
        ShieldWall1 = true;
        ShieldWall2 = true;
        
        if(!m_creature->HasAura(SPELL_RIGHTEOUS_FIRE))
            m_creature->CastSpell(m_creature, SPELL_RIGHTEOUS_FIRE, true);    
    }

    void Aggro(Unit* /*who*/)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_MOG_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_MOG_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_MOG_AGGRO3, m_creature); break;
        }
    }

    void KilledUnit(Unit* /*Victim*/)
    {
        DoScriptText(urand(0, 1) ? SAY_MOG_SLAY1 : SAY_MOG_SLAY2, m_creature);
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_MOG_DEATH, m_creature);
        m_creature->CastSpell(m_creature, SPELL_SPIRIT_MOGRAINE, true);
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        
        // Mark of Mograine
        if(Mark_Timer < uiDiff)
        {
            m_creature->CastSpell(m_creature->getVictim(), SPELL_MARK_OF_MOGRAINE, true);
            ++m_uiMarkCount;
            Mark_Timer = 12000;
        }
        else 
            Mark_Timer -= uiDiff;

        if(m_uiMarkCount >= 100 && !m_bEnrage)
        {
            m_creature->CastSpell(m_creature, SPELL_BESERK, true);
            m_bEnrage = true;
        }
        
        // Shield Wall - All 4 horsemen will shield wall at 50% hp and 20% hp for 20 seconds
        if(ShieldWall1 && (m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 50)
        {
            if(ShieldWall1)
            {
                DoCastSpellIfCan(m_creature, SPELL_SHIELDWALL);
                ShieldWall1 = false;
            }
        }
        if(ShieldWall2 && (m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 20)
        {
            if(ShieldWall2)
            {
                DoCastSpellIfCan(m_creature, SPELL_SHIELDWALL);
                ShieldWall2 = false;
            }
        }
        
       DoMeleeAttackIfReady();       
    }
};

CreatureAI* GetAI_boss_highlord_mograine(Creature* pCreature)
{
    return new boss_highlord_mograineAI(pCreature);
}

struct MANGOS_DLL_DECL boss_thane_korthazzAI : public ScriptedAI
{
    boss_thane_korthazzAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 Mark_Timer;
    uint32 Meteor_Timer;
    bool ShieldWall1;
    bool ShieldWall2;
    
    uint8 m_uiMarkCount;    
    bool m_bEnrage;

    void Reset()
    {
        m_bEnrage = false;
        m_uiMarkCount = 0;
        Mark_Timer = 20000;                                 // First Horsemen Mark is applied at 20 sec.
        Meteor_Timer = 30000;                               // wrong
        ShieldWall1 = true;
        ShieldWall2 = true;
    }

    void Aggro(Unit* /*who*/)
    {
        DoScriptText(SAY_KORT_AGGRO, m_creature);
    }

    void KilledUnit(Unit* /*Victim*/)
    {
        DoScriptText(SAY_KORT_SLAY, m_creature);
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_KORT_DEATH, m_creature);
        m_creature->CastSpell(m_creature, SPELL_SPIRIT_KORTHAZZ, true);
    }
    
    void CastMeteorOnRandomTarget()
    {
        const ThreatList& threatList = m_creature->getThreatManager().getThreatList();
        std::vector<Unit*> pEligibleTargets;

        pEligibleTargets.clear();

        if(!threatList.empty())
        {
            for (HostileReference *currentReference : threatList)
            {
                Unit *target = currentReference->getTarget();
                if (target && target->isAlive() && target->GetTypeId() == TYPEID_PLAYER && target->IsWithinDist(m_creature, 20.f)) // within 20 yrds, change if we find sources for range
                    pEligibleTargets.push_back(target);
            }

            if(!pEligibleTargets.empty())
            {
                std::random_shuffle(pEligibleTargets.begin(), pEligibleTargets.end());
                Unit *target = pEligibleTargets.front();
                if (target && target->isAlive() && target->GetTypeId() == TYPEID_PLAYER)
                {
                    m_creature->CastSpell(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), SPELL_METEOR, true);
                }
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Mark of Korthazz
        if (Mark_Timer < uiDiff)
        {
            m_creature->CastSpell(m_creature->getVictim(), SPELL_MARK_OF_KORTHAZZ, true);
            ++m_uiMarkCount;
            Mark_Timer = 12000;
        }
        else 
            Mark_Timer -= uiDiff;
        
        if(m_uiMarkCount >= 100 && !m_bEnrage)
        {
            m_creature->CastSpell(m_creature, SPELL_BESERK, true);
            m_bEnrage = true;
        }

        // Shield Wall - All 4 horsemen will shield wall at 50% hp and 20% hp for 20 seconds
        if (ShieldWall1 && m_creature->GetHealthPercent() < 50.0f)
        {
            if (ShieldWall1)
            {
                DoCastSpellIfCan(m_creature, SPELL_SHIELDWALL);
                ShieldWall1 = false;
            }
        }
        if (ShieldWall2 && m_creature->GetHealthPercent() < 20.0f)
        {
            if (ShieldWall2)
            {
                DoCastSpellIfCan(m_creature, SPELL_SHIELDWALL);
                ShieldWall2 = false;
            }
        }

        // Meteor
        if (Meteor_Timer < uiDiff)
        {
            CastMeteorOnRandomTarget();
            Meteor_Timer = m_bEnrage ? 5000 : 20000;                           // wrong
        }
        else 
            Meteor_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_thane_korthazz(Creature* pCreature)
{
    return new boss_thane_korthazzAI(pCreature);
}

struct MANGOS_DLL_DECL boss_sir_zeliekAI : public ScriptedAI
{
    boss_sir_zeliekAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 Mark_Timer;
    uint32 HolyWrath_Timer;
    bool ShieldWall1;
    bool ShieldWall2;

    uint8 m_uiMarkCount;    
    bool m_bEnrage;

    void Reset()
    {
        m_bEnrage = false;
        m_uiMarkCount = 0;
        Mark_Timer = 20000;                                 // First Horsemen Mark is applied at 20 sec.
        HolyWrath_Timer = 12000;                            // right
        ShieldWall1 = true;
        ShieldWall2 = true;
    }

    void Aggro(Unit* /*who*/)
    {
        DoScriptText(SAY_ZELI_AGGRO, m_creature);
    }

    void KilledUnit(Unit* /*Victim*/)
    {
        DoScriptText(SAY_ZELI_SLAY, m_creature);
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_ZELI_DEATH, m_creature);
        m_creature->CastSpell(m_creature, SPELL_SPIRIT_ZELIEK, true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Mark of Zeliek
        if (Mark_Timer < uiDiff)
        {
            m_creature->CastSpell(m_creature->getVictim(), SPELL_MARK_OF_ZELIEK, true);
            ++m_uiMarkCount;
            Mark_Timer = 12000;
        }
        else 
            Mark_Timer -= uiDiff;

        if(m_uiMarkCount >= 100 && !m_bEnrage)
        {
            m_creature->CastSpell(m_creature, SPELL_BESERK, true);
            m_bEnrage = true;
        }
        
        // Shield Wall - All 4 horsemen will shield wall at 50% hp and 20% hp for 20 seconds
        if (ShieldWall1 && m_creature->GetHealthPercent() < 50.0f)
        {
            if (ShieldWall1)
            {
                DoCastSpellIfCan(m_creature, SPELL_SHIELDWALL);
                ShieldWall1 = false;
            }
        }
        if (ShieldWall2 && m_creature->GetHealthPercent() < 20.0f)
        {
            if (ShieldWall2)
            {
                DoCastSpellIfCan(m_creature, SPELL_SHIELDWALL);
                ShieldWall2 = false;
            }
        }

        // Holy Wrath
        if (HolyWrath_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_HOLY_WRATH);
            HolyWrath_Timer = m_bEnrage ? 3000 : 12000;
        }
        else 
            HolyWrath_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_sir_zeliek(Creature* pCreature)
{
    return new boss_sir_zeliekAI(pCreature);
}

void AddSC_boss_four_horsemen()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_lady_blaumeux";
    pNewscript->GetAI = &GetAI_boss_lady_blaumeux;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "boss_highlord_mograine";
    pNewscript->GetAI = &GetAI_boss_highlord_mograine;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "boss_thane_korthazz";
    pNewscript->GetAI = &GetAI_boss_thane_korthazz;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "boss_sir_zeliek";
    pNewscript->GetAI = &GetAI_boss_sir_zeliek;
    pNewscript->RegisterSelf();
}
