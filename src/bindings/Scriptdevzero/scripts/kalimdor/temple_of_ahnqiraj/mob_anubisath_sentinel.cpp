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
SDName: mob_anubisath_sentinel
SD%Complete: 75
SDComment: Abilities selection needs further improvements. Shadow storm is not properly implemented in core it should only target ppl outside of melee range.
SDCategory: Temple of Ahn'Qiraj
EndScriptData */

#include "precompiled.h"

enum
{
    EMOTE_GENERIC_FRENZY            = -1000002,

    SPELL_PERIODIC_MANA_BURN        = 812,
    SPELL_MENDING                   = 2147,
    SPELL_PERIODIC_SHADOW_STORM     = 2148,
    SPELL_PERIODIC_THUNDERCLAP      = 2834,
    SPELL_MORTAL_STRIKE             = 9347,
    SPELL_FIRE_ARCANE_REFLECT       = 13022,
    SPELL_SHADOW_FROST_REFLECT      = 19595,
    SPELL_PERIODIC_KNOCK_AWAY       = 21737,
    SPELL_THORNS                    = 25777,
    SPELL_SHARE                     = 1905,
    SPELL_HEAL_BRETHREN             = 26565,

    SPELL_ENRAGE                    = 8599,

    MAX_BUDDY                       = 4
};

struct MANGOS_DLL_DECL npc_anubisath_sentinelAI : public ScriptedAI
{
    npc_anubisath_sentinelAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_lAssistList.clear();
        Reset();
    }

    uint32 m_uiMyAbility;
    bool m_bEnraged;
    bool m_hasSpell;

    std::vector<uint32> m_spellList;
    std::vector<ObjectGuid> m_lAssistList;

    void Reset()
    {
        m_uiMyAbility = 0;
        m_bEnraged = false;
        m_spellList.clear();
        m_spellList.push_back(SPELL_PERIODIC_MANA_BURN);
        m_spellList.push_back(SPELL_MENDING);
        m_spellList.push_back(SPELL_PERIODIC_SHADOW_STORM);
        m_spellList.push_back(SPELL_PERIODIC_THUNDERCLAP);
        m_spellList.push_back(SPELL_MORTAL_STRIKE);
        m_spellList.push_back(SPELL_FIRE_ARCANE_REFLECT);
        m_spellList.push_back(SPELL_SHADOW_FROST_REFLECT);
        m_spellList.push_back(SPELL_PERIODIC_KNOCK_AWAY);
        m_spellList.push_back(SPELL_THORNS);
        TauntImmune(false);
    }

    void JustReachedHome()
    {
        if(!m_lAssistList.empty())
        {
            for(ObjectGuid pAnubGuid : m_lAssistList)
            {
                if (pAnubGuid == m_creature->GetObjectGuid())
                    continue;

                if (Creature* pBuddy = m_creature->GetMap()->GetCreature(pAnubGuid))
                {
                    if (pBuddy->isDead())
                        pBuddy->Respawn();
                }
            }
        }
    }

    void Aggro(Unit* pWho)
    {
        InitSentinelsNear(pWho);
        SetAbility();   
    }

    void JustDied(Unit* /*pKiller*/)
    {
        DoCast(m_creature, SPELL_SHARE, true);
        if(!m_lAssistList.empty())
        {
            for(ObjectGuid pAnubGuid : m_lAssistList)
            {
                if(pAnubGuid == m_creature->GetObjectGuid())
                    continue;

                Creature *pAnub = m_creature->GetMap()->GetCreature(pAnubGuid);

                if(pAnub && pAnub->isAlive())
                {
                    npc_anubisath_sentinelAI *pAnubAi = dynamic_cast<npc_anubisath_sentinelAI*>(pAnub->AI());

                    if(pAnubAi)
                        pAnubAi->DoTransferAbility(m_uiMyAbility);

                    pAnub->AI()->DoCastSpellIfCan(pAnub, SPELL_HEAL_BRETHREN, CastFlags::CAST_TRIGGERED);
                }
            }
        }

        m_creature->GenericTextEmote("Anubisath Sentinel shares his powers with his brethren.", nullptr, false);
    }

    // this way will make it quite possible that sentinels get the same buff as others, need to fix that, it should be one unique each
    void SetAbility()
    {
        if(!m_lAssistList.empty())
        {
            std::sort(m_lAssistList.begin(), m_lAssistList.end());

            if(m_creature->GetObjectGuid() == m_lAssistList.front())
            {
                for(ObjectGuid pAnubGuid : m_lAssistList)
                {
                    Creature *pAnub = m_creature->GetMap()->GetCreature(pAnubGuid);

                    if(pAnub && pAnub->isAlive())
                    {
                        npc_anubisath_sentinelAI *pAnubAi = dynamic_cast<npc_anubisath_sentinelAI*>(pAnub->AI());

                        if(pAnubAi)
                        {
                            if(!m_spellList.empty())
                            {
                                std::random_shuffle(m_spellList.begin(), m_spellList.end());
                                uint32 m_spell = m_spellList.front();
                                pAnubAi->DoTransferAbility(m_spell);
                                m_spellList.erase(std::remove(m_spellList.begin(), m_spellList.end(), m_spell), m_spellList.end());
                            }
                        }
                    }
                }
            }
        }
    }

    /**** DEBUG FUNCTION ****/

    std::string TranslateSpell(uint32 spell)
    {
        switch(spell)
        {
            case SPELL_PERIODIC_MANA_BURN:
                return "SPELL_PERIODIC_MANA_BURN";
            case SPELL_MENDING: 
                return "SPELL_MENDING";
            case SPELL_PERIODIC_SHADOW_STORM: 
                return "SPELL_PERIODIC_SHADOW_STORM";
            case SPELL_PERIODIC_THUNDERCLAP: 
                return "SPELL_PERIODIC_THUNDERCLAP";
            case SPELL_MORTAL_STRIKE: 
                return "SPELL_MORTAL_STRIKE";
            case SPELL_FIRE_ARCANE_REFLECT: 
                return "SPELL_FIRE_ARCANE_REFLECT";
            case SPELL_SHADOW_FROST_REFLECT: 
                return "SPELL_SHADOW_FROST_REFLECT";
            case SPELL_PERIODIC_KNOCK_AWAY: 
                return "SPELL_PERIODIC_KNOCK_AWAY";
            case SPELL_THORNS: 
                return "SPELL_THORNS";
            default:
                return "NO SPELL WTF";
        }
    }

     /**** DEBUG FUNCTION ****/

    void DoTransferAbility(uint32 ability)
    {
        DoCast(m_creature, ability, true);

       // m_creature->MonsterYell(std::string("I have been given spell " + TranslateSpell(ability) + "!").c_str(), LANG_UNIVERSAL);

        if(m_uiMyAbility == 0)
            m_uiMyAbility = ability;
    }

    void InitSentinelsNear(Unit* pTarget)
    {
        if (!m_lAssistList.empty())
        {
            for(ObjectGuid pAnubGuid : m_lAssistList)
            {
                if (pAnubGuid == m_creature->GetObjectGuid())
                    continue;

                if (Creature* pBuddy = m_creature->GetMap()->GetCreature(pAnubGuid))
                {
                    if (pBuddy->isAlive())
                        pBuddy->AI()->AttackStart(pTarget);
                }
            }

            return;
        }

        std::list<Creature*> lAssistList;
        GetCreatureListWithEntryInGrid(lAssistList, m_creature, m_creature->GetEntry(), 80.0f);

        if (lAssistList.empty())
            return;

        for(std::list<Creature*>::iterator iter = lAssistList.begin(); iter != lAssistList.end(); ++iter)
        {
            m_lAssistList.push_back((*iter)->GetObjectGuid());

            if ((*iter)->GetObjectGuid() == m_creature->GetObjectGuid())
                continue;

            (*iter)->AI()->AttackStart(pTarget);
        }

        if (m_lAssistList.size() != MAX_BUDDY)
            error_log("SD2: npc_anubisath_sentinel found too few/too many buddies, expected %u.", MAX_BUDDY);
    }
    
    void TauntImmune(bool apply) // own function so we can remove it on reset
    {
        m_creature->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, apply);
        m_creature->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, apply);
    }
    
    void SpellHit(Unit* pCaster, SpellEntry const* pSpell)
    {                                    
        // make it immune to taunt if it is given knock away on aggro,
        if(pSpell->Id == SPELL_PERIODIC_KNOCK_AWAY)            
            TauntImmune(true);
    }
    
    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_PERIODIC_KNOCK_AWAY && pTarget->GetTypeId() == TYPEID_PLAYER)
            m_creature->getThreatManager().modifyThreatPercent(pTarget, -25);               //added threat reduction        
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->isAlive() && !m_bEnraged && m_creature->GetHealthPercent() < 30.0f)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_ENRAGE) == CAST_OK)
            {
                DoScriptText(EMOTE_GENERIC_FRENZY, m_creature);
                m_bEnraged = true;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_anubisath_sentinel(Creature* pCreature)
{
    return new npc_anubisath_sentinelAI(pCreature);
}

void AddSC_mob_anubisath_sentinel()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "mob_anubisath_sentinel";
    newscript->GetAI = &GetAI_npc_anubisath_sentinel;
    newscript->RegisterSelf();
}
