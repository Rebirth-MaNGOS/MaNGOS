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

#include "precompiled.h"
#include "temple_of_ahnqiraj.h"

enum
{
    EMOTE_GENERIC_FRENZY            = -1000002,

    SPELL_PERIODIC_SHADOW_STORM     = 2148,
    SPELL_PERIODIC_THUNDERCLAP      = 2834,
    SPELL_FIRE_ARCANE_REFLECT       = 13022,
    SPELL_SHADOW_FROST_REFLECT      = 19595,
    SPELL_METEOR                    = 24340,
    SPELL_PLAGUE                    = 26556,
    SPELL_EXPLODE                   = 28433,
    SPELL_VISUAL_EFFECT_FIERY_HANDS = 6297,
    SPELL_ROOT_SELF                 = 23973,

    SPELL_SUMMON_WARRIOR            = 17431,
    SPELL_SUMMON_SWARMER            = 17430,

    LARGE_OBSIDIAN_CHUNK            = 181069,

    SPELL_ENRAGE                    = 8599,
};

struct MANGOS_DLL_DECL mob_anubisath_defender : public ScriptedAI
{
    mob_anubisath_defender(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiCastAbility;
    uint32 m_uiEnrageAbility;
    uint32 m_uiSummonAbility;
    uint32 m_uiExplodeTimer;
    uint32 m_uiCastTimer;
    uint32 m_uiSummonTimer;
    bool m_bEnraged;
    bool m_bExploding;
    bool m_hasSpell;

    

    void Reset()
    {
        m_bEnraged = false;
        m_bExploding = false;
        m_uiExplodeTimer = 0;
        m_uiCastTimer = urand(5000, 10000);
        m_uiSummonTimer = 10000;
    }

    void JustReachedHome()
    {
    }

    void Aggro(Unit* pWho)
    {
        SetAbilites();   
    }

    void JustDied(Unit* /*pKiller*/)
    {
        float x, y, z;
        m_creature->GetClosePoint(x, y, z, 3.0f, 5.0f);
        short Drop = urand(0,1);
       // 50% drop chance
        if(Drop > 0)
            m_creature->SummonGameObject(LARGE_OBSIDIAN_CHUNK, 3600000, x, y, z, 0);
    }

    // this way will make it quite possible that sentinels get the same buff as others, need to fix that, it should be one unique each
    void SetAbilites()
    {
        for(int i = 0; i < 5; ++i)
        {
            switch(urand(0,1))
            {
            case 0:
                switch(i)
                {
                case 0:
                    m_uiCastAbility = SPELL_PLAGUE;
                    m_creature->MonsterYell(std::string("I have been given spell " + TranslateSpell(SPELL_PLAGUE) + "!").c_str(), LANG_UNIVERSAL);
                    break;
                case 1:
                    m_creature->MonsterYell(std::string("I have been given spell " + TranslateSpell(SPELL_PERIODIC_THUNDERCLAP) + "!").c_str(), LANG_UNIVERSAL);
                    DoCast(m_creature, SPELL_PERIODIC_THUNDERCLAP, true);
                    break;
                case 2:
                    DoCast(m_creature, SPELL_FIRE_ARCANE_REFLECT, true);
                    m_creature->MonsterYell(std::string("I have been given spell " + TranslateSpell(SPELL_FIRE_ARCANE_REFLECT) + "!").c_str(), LANG_UNIVERSAL);
                    break;
                case 3:
                    m_uiEnrageAbility = SPELL_ENRAGE;
                    m_creature->MonsterYell(std::string("I have been given spell " + TranslateSpell(SPELL_ENRAGE) + "!").c_str(), LANG_UNIVERSAL);
                    break;
                case 4:
                    m_uiSummonAbility = SPELL_SUMMON_SWARMER;
                    break;
                }
                
                break;
            case 1:
                switch(i)
                {
                case 0:
                    m_uiCastAbility = SPELL_METEOR;
                    m_creature->MonsterYell(std::string("I have been given spell " + TranslateSpell(SPELL_METEOR) + "!").c_str(), LANG_UNIVERSAL);
                    break;
                case 1:
                    DoCast(m_creature, SPELL_PERIODIC_SHADOW_STORM, true);
                    m_creature->MonsterYell(std::string("I have been given spell " + TranslateSpell(SPELL_PERIODIC_SHADOW_STORM) + "!").c_str(), LANG_UNIVERSAL);
                    break;
                case 2:
                    m_creature->MonsterYell(std::string("I have been given spell " + TranslateSpell(SPELL_SHADOW_FROST_REFLECT) + "!").c_str(), LANG_UNIVERSAL);
                    DoCast(m_creature, SPELL_SHADOW_FROST_REFLECT, true);
                    break;
                case 3:
                    m_uiEnrageAbility = SPELL_EXPLODE;
                    m_creature->MonsterYell(std::string("I have been given spell " + TranslateSpell(SPELL_EXPLODE) + "!").c_str(), LANG_UNIVERSAL);
                    break;
                case 4:
                    m_uiSummonAbility = SPELL_SUMMON_WARRIOR;
                    break;
                }
                
                break;
            }
        }

    }

    /**** DEBUG FUNCTION ****/

    std::string TranslateSpell(uint32 spell)
    {
        switch(spell)
        {
            case SPELL_PLAGUE:
                return "SPELL_PLAGUE";
            case SPELL_METEOR: 
                return "SPELL_METEOR";
            case SPELL_PERIODIC_SHADOW_STORM: 
                return "SPELL_PERIODIC_SHADOW_STORM";
            case SPELL_PERIODIC_THUNDERCLAP: 
                return "SPELL_PERIODIC_THUNDERCLAP";
            case SPELL_EXPLODE: 
                return "SPELL_EXPLODE";
            case SPELL_FIRE_ARCANE_REFLECT: 
                return "SPELL_FIRE_ARCANE_REFLECT";
            case SPELL_SHADOW_FROST_REFLECT: 
                return "SPELL_SHADOW_FROST_REFLECT";
            case SPELL_ENRAGE: 
                return "SPELL_ENRAGE";
        }
    }

     /**** DEBUG FUNCTION ****/

    void CastOnRandomTarget()
    {
        const ThreatList& threatList = m_creature->getThreatManager().getThreatList();
        std::vector<Unit*> pEligibleTargets;

        pEligibleTargets.clear();

        if(!threatList.empty())
        {
            for (HostileReference *currentReference : threatList)
            {
                Unit *target = currentReference->getTarget();
                if (target && target->isAlive())
                    pEligibleTargets.push_back(target);
            }

            if(!pEligibleTargets.empty())
            {
                std::random_shuffle(pEligibleTargets.begin(), pEligibleTargets.end());
                Unit *target = pEligibleTargets.front();
                if (target && target->isAlive())
                {
                    if(m_uiCastAbility == SPELL_PLAGUE)
                        DoCast(target, m_uiCastAbility);
                    else
                        m_creature->CastSpell(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), SPELL_METEOR, true);
                }
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(m_uiExplodeTimer)
        {
            if(m_uiExplodeTimer <= uiDiff)
            {
                m_uiExplodeTimer = 0;
                DoCast(m_creature, SPELL_EXPLODE);
            }
            else
                m_uiExplodeTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || m_bExploding)
            return;

        if(m_uiSummonTimer)
        {
            if(m_uiSummonTimer <= uiDiff)
            {
                m_uiSummonTimer = 10000;
                float x, y, z;
                m_creature->GetClosePoint(x, y, z, 3.0f, 10.0f);
                m_creature->CastSpell(x, y, z, m_uiSummonAbility, true);
            }
            else
                m_uiSummonTimer -= uiDiff;
        }

        if(m_uiCastTimer)
        {
            if(m_uiCastTimer <= uiDiff)
            {
                m_uiCastTimer = urand(5000, 10000);
                CastOnRandomTarget();
            }
            else
                m_uiCastTimer -= uiDiff;
        }

        if (m_creature->isAlive() && !m_bEnraged && !m_uiExplodeTimer && m_creature->GetHealthPercent() < 10.0f)
        {
            if(m_uiEnrageAbility == SPELL_ENRAGE)
            {
                DoCast(m_creature, SPELL_ENRAGE);
                DoScriptText(EMOTE_GENERIC_FRENZY, m_creature);
                m_bEnraged = true;
            }
            else
            {
                DoCast(m_creature, SPELL_ROOT_SELF, true);
                DoCast(m_creature, SPELL_VISUAL_EFFECT_FIERY_HANDS); 
                m_uiExplodeTimer = 4000;
                m_bExploding = true;
            }
        }

        if(!m_bExploding)
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_anubisath_defender(Creature* pCreature)
{
    return new mob_anubisath_defender(pCreature);
}

void AddSC_temple_of_ahnqiraj()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "mob_anubisath_defender";
    pNewscript->GetAI = &GetAI_mob_anubisath_defender;
    pNewscript->RegisterSelf();
}
