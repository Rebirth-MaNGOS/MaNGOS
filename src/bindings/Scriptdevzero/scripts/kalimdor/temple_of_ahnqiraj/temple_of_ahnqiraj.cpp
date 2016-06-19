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
        {
            if(GameObject* pChunk = m_creature->SummonGameObject(LARGE_OBSIDIAN_CHUNK,3600000, x, y, z, 0,GO_STATE_ACTIVE))
            {       
                pChunk->SetGoState(GO_STATE_READY);
                pChunk->SetLootState(GO_READY); 
                pChunk->SetOwnerGuid(ObjectGuid());
            }
        }
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
                  //  m_creature->MonsterYell(std::string("I have been given spell " + TranslateSpell(SPELL_PLAGUE) + "!").c_str(), LANG_UNIVERSAL);
                    break;
                case 1:
                  //  m_creature->MonsterYell(std::string("I have been given spell " + TranslateSpell(SPELL_PERIODIC_THUNDERCLAP) + "!").c_str(), LANG_UNIVERSAL);
                    DoCast(m_creature, SPELL_PERIODIC_THUNDERCLAP, true);
                    break;
                case 2:
                    DoCast(m_creature, SPELL_FIRE_ARCANE_REFLECT, true);
                //    m_creature->MonsterYell(std::string("I have been given spell " + TranslateSpell(SPELL_FIRE_ARCANE_REFLECT) + "!").c_str(), LANG_UNIVERSAL);
                    break;
                case 3:
                    m_uiEnrageAbility = SPELL_ENRAGE;
                //    m_creature->MonsterYell(std::string("I have been given spell " + TranslateSpell(SPELL_ENRAGE) + "!").c_str(), LANG_UNIVERSAL);
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
               //     m_creature->MonsterYell(std::string("I have been given spell " + TranslateSpell(SPELL_METEOR) + "!").c_str(), LANG_UNIVERSAL);
                    break;
                case 1:
                    DoCast(m_creature, SPELL_PERIODIC_SHADOW_STORM, true);
               //     m_creature->MonsterYell(std::string("I have been given spell " + TranslateSpell(SPELL_PERIODIC_SHADOW_STORM) + "!").c_str(), LANG_UNIVERSAL);
                    break;
                case 2:
               //     m_creature->MonsterYell(std::string("I have been given spell " + TranslateSpell(SPELL_SHADOW_FROST_REFLECT) + "!").c_str(), LANG_UNIVERSAL);
                    DoCast(m_creature, SPELL_SHADOW_FROST_REFLECT, true);
                    break;
                case 3:
                    m_uiEnrageAbility = SPELL_EXPLODE;
               //     m_creature->MonsterYell(std::string("I have been given spell " + TranslateSpell(SPELL_EXPLODE) + "!").c_str(), LANG_UNIVERSAL);
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
                if (target && target->isAlive() && target->GetTypeId() == TYPEID_PLAYER)
                    pEligibleTargets.push_back(target);
            }

            if(!pEligibleTargets.empty())
            {
                std::random_shuffle(pEligibleTargets.begin(), pEligibleTargets.end());
                Unit *target = pEligibleTargets.front();
                if (target && target->isAlive() && target->GetTypeId() == TYPEID_PLAYER)
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

/*######
## mob_obsidian_nullifier
######*/

enum eObsidianNullifier
{
    SPELL_NULLIFY       = 26552,
    SPELL_DRAIN_MANA  = 25755,
    SPELL_DRAIN_MANA_VISUAL  = 26639,
    SPELL_CLEAVE = 20691
};

struct MANGOS_DLL_DECL mob_obsidian_nullifierAI : public ScriptedAI
{
    mob_obsidian_nullifierAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiManaDrainTimer;
    uint32 m_uiCleave_Timer;
    short m_uiDrain_count;
    
    void Reset() 
    {
        m_uiDrain_count = 0;
        m_uiManaDrainTimer = 10000;
        m_uiCleave_Timer = urand(5000, 10000);
        m_creature->SetPower(POWER_MANA, 0);
        m_creature->SetMaxPower(POWER_MANA, 0);
    }

    void Aggro(Unit* /*pWho*/)
    {
        // Combat so we have everyone on threat list for mana drain
        m_creature->SetInCombatWithZone();
        
        m_creature->SetMaxPower(POWER_MANA, m_creature->GetCreatureInfo()->maxmana);

        const ThreatList& threatList = m_creature->getThreatManager().getThreatList();
        
        if(!threatList.empty())
        {
            for (HostileReference *currentReference : threatList)
            {
                Unit *target = currentReference->getTarget();
                if (target && target->GetTypeId() == TYPEID_PLAYER && target->getPowerType() == POWER_MANA && target->GetDistance(m_creature) < 40.0f)
                {
                    m_creature->CastSpell(target, SPELL_DRAIN_MANA, true);
                    ++m_uiDrain_count;                    
                }
                if(m_uiDrain_count >= 16)
                        break;
            }
        }        
    }
    
    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_DRAIN_MANA)
            pTarget->CastSpell(m_creature,SPELL_DRAIN_MANA_VISUAL, true);               // animation for mana drain
        // manually set hp to 1 when hit by nullify    
        if (pSpell->Id == SPELL_NULLIFY && pTarget && pTarget->isAlive())
            pTarget->SetHealth(1);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Cleave
        if (m_uiCleave_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE);
            m_uiCleave_Timer = urand(5000, 8000);
        }
        else 
            m_uiCleave_Timer -= uiDiff;
        
        if (m_uiManaDrainTimer <= uiDiff)
        {
            const ThreatList& threatList = m_creature->getThreatManager().getThreatList();

            if(!threatList.empty())
            {
                for (HostileReference *currentReference : threatList)
                {
                    Unit *target = currentReference->getTarget();
                    if (target && target->GetTypeId() == TYPEID_PLAYER && target->getPowerType() == POWER_MANA && target->GetDistance(m_creature) < 30.0f)
                        m_creature->CastSpell(target, SPELL_DRAIN_MANA, true);
                }
            }
            m_uiManaDrainTimer = urand(8000, 12000);
         } 
        else
            m_uiManaDrainTimer -= uiDiff;

        if (m_creature->GetPower(POWER_MANA) == m_creature->GetMaxPower(POWER_MANA))
            DoCastSpellIfCan(m_creature, SPELL_NULLIFY);

        DoMeleeAttackIfReady(); 
    }
};

CreatureAI* GetAI_mob_obsidian_nullifier(Creature* pCreature)
{
    return new mob_obsidian_nullifierAI(pCreature);
}

/*######
## mob_vekniss_stinger
######*/

enum eVeknissStinger
{
    SPELL_STINGER_CHARGE             = 26081,
    SPELL_STINGER_EMPOWERED_CHARGE   = 26082,
    SPELL_VEKNISS_CATALYST          = 26078,
};

struct MANGOS_DLL_DECL mob_vekniss_stingerAI : public ScriptedAI
{
    mob_vekniss_stingerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiChargeTimer;

    void Reset() 
    {       
        m_uiChargeTimer = 5000;
    }

    void ChargeRandomTarget()
    {
        const ThreatList& threatList = m_creature->getThreatManager().getThreatList();
        std::vector<Unit*> pEligibleTargets;

        pEligibleTargets.clear();

        if(!threatList.empty())
        {
            for (HostileReference *currentReference : threatList)
            {
                Unit *target = currentReference->getTarget();
                if (target && target->isAlive() && target->GetDistance(m_creature) <= 25.0f && target->GetDistance(m_creature) >= 5.0f)
                    pEligibleTargets.push_back(target);
            }

            if(!pEligibleTargets.empty())
            {
                std::random_shuffle(pEligibleTargets.begin(), pEligibleTargets.end());
                Unit *target = pEligibleTargets.front();
                if (target && target->isAlive())
                {
                    if(target->HasAura(SPELL_VEKNISS_CATALYST))
                        DoCast(target, SPELL_STINGER_EMPOWERED_CHARGE, true);
                    else
                        DoCast(target, SPELL_STINGER_CHARGE, true);
                }
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiChargeTimer <= uiDiff)
        {
            ChargeRandomTarget();
            m_uiChargeTimer = 5000;
        } 
        else
            m_uiChargeTimer -= uiDiff;

        DoMeleeAttackIfReady(); 
    }
};

CreatureAI* GetAI_mob_vekniss_stinger(Creature* pCreature)
{
    return new mob_vekniss_stingerAI(pCreature);
}

void AddSC_temple_of_ahnqiraj()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "mob_anubisath_defender";
    pNewscript->GetAI = &GetAI_mob_anubisath_defender;
    pNewscript->RegisterSelf();
    
    pNewscript = new Script;
    pNewscript->Name = "mob_obsidian_nullifier";
    pNewscript->GetAI = &GetAI_mob_obsidian_nullifier;
    pNewscript->RegisterSelf();
    
    pNewscript = new Script;
    pNewscript->Name = "mob_vekniss_stinger";
    pNewscript->GetAI = &GetAI_mob_vekniss_stinger;
    pNewscript->RegisterSelf();
}
