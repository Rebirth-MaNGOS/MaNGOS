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

/*
 * ScriptData
 * SDName:      Boss_Grobbulus
 * SD%Complete: 80
 * SDComment:   Timer need more care; Spells of Adds (Posion Cloud) need Mangos Fixes, and further handling. Visuals of previously mentioned spells need to be implemented
 * SDCategory:  Naxxramas
 * EndScriptData
 */

/*
 * Spell 26590: Poison Cloud
 * Spell 28157: Slime Spray
 * Spell 28218: Fallout slime
 * Spell 28169: Mutating Injection
 * Spell 26527: Enrages
 */

#include "precompiled.h"
#include "naxxramas.h"

enum
{
    EMOTE_SPRAY_SLIME               = -1533021,
    //EMOTE_INJECTION                 = -1533158,           // no whisper on video, so skipping
    EMOTE_GENERIC_BERSERK   = -1533021, // should have emote? added anyway

    SPELL_SLIME_STREAM              = 28137,
    SPELL_MUTATING_INJECTION        = 28169,
    SPELL_POISON_CLOUD              = 28240, // summons the cloud
    SPELL_POISON_CLOUD_TR = 28158,      // this triggers dmg below
    SPELL_POISON_CLOUD_DMG = 28241,
    SPELL_SLIME_SPRAY               = 28157,
    SPELL_BERSERK                   = 26662,

    NPC_FALLOUT_SLIME               = 16290
};

struct MANGOS_DLL_DECL boss_grobbulusAI : public ScriptedAI
{
    boss_grobbulusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_naxxramas*)pCreature->GetInstanceData();
        Reset();
    }

    instance_naxxramas* m_pInstance;

    uint32 m_uiInjectionTimer;
    uint32 m_uiPoisonCloudTimer;
    uint32 m_uiSlimeSprayTimer;
    uint32 m_uiBerserkTimer;
    uint32 m_uiSlimeStreamTimer;

    void Reset()
    {
        m_uiInjectionTimer = 12 * IN_MILLISECONDS;
        m_uiPoisonCloudTimer = urand(20 * IN_MILLISECONDS, 25 * IN_MILLISECONDS);
        m_uiSlimeSprayTimer = urand(20 * IN_MILLISECONDS, 30 * IN_MILLISECONDS);
        m_uiBerserkTimer = 12 * MINUTE * IN_MILLISECONDS;
        m_uiSlimeStreamTimer = 5 * IN_MILLISECONDS;         // The first few secs it is ok to be out of range
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GROBBULUS, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GROBBULUS, DONE);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GROBBULUS, FAIL);
    }
    
    // This custom selecting function, because we only want to select players without mutagen aura
    bool DoCastMutagenInjection()
    {
        if (m_creature->IsNonMeleeSpellCasted(true))
            return false;

        std::vector<Unit*> suitableTargets;
        ThreatList const& threatList = m_creature->getThreatManager().getThreatList();

        for (ThreatList::const_iterator itr = threatList.begin(); itr != threatList.end(); ++itr)
        {
            if (Unit* pTarget = m_creature->GetMap()->GetUnit((*itr)->getUnitGuid()))
            {
                if (pTarget->GetTypeId() == TYPEID_PLAYER && !pTarget->HasAura(SPELL_MUTATING_INJECTION))
                    suitableTargets.push_back(pTarget);
            }
        }

        if (suitableTargets.empty())
            return false;

        Unit* pTarget = suitableTargets[urand(0, suitableTargets.size() - 1)];
        if (DoCastSpellIfCan(pTarget, SPELL_MUTATING_INJECTION) == CAST_OK)
            return true;
        else
            return false;
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if ((pSpell->Id == SPELL_SLIME_SPRAY) && pTarget->GetTypeId() == TYPEID_PLAYER)
            m_creature->SummonCreature(NPC_FALLOUT_SLIME, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10 * IN_MILLISECONDS);
    }

    void SpellHit(Unit* pCaster, SpellEntry const* pSpell)
    {
        if (pSpell->Id == SPELL_BERSERK)
        {
            DoScriptText(EMOTE_GENERIC_BERSERK, m_creature);
            m_uiBerserkTimer = 600000;
        }
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Slime Stream
        if (!m_uiSlimeStreamTimer)
        {
            if (!m_creature->CanReachWithMeleeAttack(m_creature->getVictim()))
            {
               m_creature->CastSpell(m_creature, SPELL_SLIME_STREAM, false);
               m_uiSlimeStreamTimer = 3 * IN_MILLISECONDS; // Give some time, to re-reach grobbulus
            }
        }
        else
        {
            if (m_uiSlimeStreamTimer < uiDiff)
                m_uiSlimeStreamTimer = 0;
            else
                m_uiSlimeStreamTimer -= uiDiff;
        }

        // Berserk
        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
                m_creature->CastSpell(m_creature, SPELL_BERSERK, true);
            else
                m_uiBerserkTimer -= uiDiff;
        }

        // SlimeSpray
        if (m_uiSlimeSprayTimer < uiDiff)
        {
            m_creature->CastSpell(m_creature->getVictim(), SPELL_SLIME_SPRAY, false);
            m_uiSlimeSprayTimer = urand(30 * IN_MILLISECONDS, 60 * IN_MILLISECONDS);
            m_creature->GenericTextEmote("Grobbulus sprays slime across the room!", NULL, false);
        }
        else
            m_uiSlimeSprayTimer -= uiDiff;

        // Mutagen Injection
        if (m_uiInjectionTimer < uiDiff)
        {
            if (DoCastMutagenInjection())
            {
                // Mutagen Injection should be used more often when below 30%
                if (m_creature->GetHealthPercent() > 30.0f)
                    m_uiInjectionTimer = urand(7000, 13000);
                else
                    m_uiInjectionTimer = urand(3000, 7000);
            }
        }
        else
            m_uiInjectionTimer -= uiDiff;

        // Poison Cloud
        if (m_uiPoisonCloudTimer < uiDiff)
        {
            m_creature->CastSpell(m_creature, SPELL_POISON_CLOUD, false);
            m_uiPoisonCloudTimer = 15 * IN_MILLISECONDS;
        }
        else
            m_uiPoisonCloudTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_grobbulus(Creature* pCreature)
{
    return new boss_grobbulusAI(pCreature);
}

void AddSC_boss_grobbulus()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_grobbulus";
    pNewScript->GetAI = &GetAI_boss_grobbulus;
    pNewScript->RegisterSelf();
}
