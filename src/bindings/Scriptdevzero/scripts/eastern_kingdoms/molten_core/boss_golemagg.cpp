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
SDName: Boss_Golemagg
SD%Complete: 85
SDComment: Timers need to be confirmed
SDCategory: Molten Core
EndScriptData */

#include "precompiled.h"
#include "molten_core.h"

enum eGolemagg
{
    // Golemagg the Incinerator
    SPELL_GOLEMAGG_TRUST    = 20553,
    SPELL_MAGMA_SPLASH      = 13879,            //This does not work correctly, need to make him use spell 13880 manually
	SPELL_MAGMA_SPLASH_ACTIVE = 13880,
    SPELL_PYROBLAST         = 20228,
    SPELL_EARTHQUAKE        = 19798,
    SPELL_ENRAGE            = 19953,            // really uses this spell? (jursky) - no, his adds get the buff of this spell when
	                                            // Golemagg uses Golemagg's Trust! (zataron)

    // Core Rager
    EMOTE_LOWHP             = -1409002,
    SPELL_MANGLE            = 19820,

    MAGMA_SPLASH_CD         = 4000,
	MAX_TARGETS             = 45
};

struct MANGOS_DLL_DECL boss_golemaggAI : public ScriptedAI
{
    boss_golemaggAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_molten_core*)pCreature->GetInstanceData();
        Reset();
    }

    instance_molten_core* m_pInstance;

    bool m_bEnraged;
    uint32 m_uiEarthquakeTimer;
    uint32 m_uiGolemaggTrustTimer;
    uint32 m_uiPyroblastTimer;

    void Reset()
    {
        m_creature->SetBoundingValue(0, 7);
        m_creature->SetBoundingValue(1, 4);
        m_bEnraged = false;
        m_uiEarthquakeTimer = 3*IN_MILLISECONDS;
        m_uiGolemaggTrustTimer = IN_MILLISECONDS;
        m_uiPyroblastTimer = 7*IN_MILLISECONDS;              // These timers are probably wrong

        m_creature->RemoveAurasDueToSpell(SPELL_MAGMA_SPLASH);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GOLEMAGG, FAIL);
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoCastSpellIfCan(m_creature, SPELL_MAGMA_SPLASH, CAST_TRIGGERED);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GOLEMAGG, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GOLEMAGG, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Enrage
        //if (!m_bEnraged && HealthBelowPct(10))
		if (!m_bEnraged && m_creature->GetHealthPercent() < 10.0f)
        {
            //DoCastSpellIfCan(m_creature, SPELL_ENRAGE);
            m_bEnraged = true;
        }

        // Earthquake
        if (m_bEnraged)
        {
            if (m_uiEarthquakeTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_EARTHQUAKE);
                m_uiEarthquakeTimer = 3*IN_MILLISECONDS;
            }
            else
                m_uiEarthquakeTimer -= uiDiff;
        }

        // Golemagg Trust
        if (m_uiGolemaggTrustTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_GOLEMAGG_TRUST, CAST_TRIGGERED);
            m_uiGolemaggTrustTimer = 1500;
        }
        else
            m_uiGolemaggTrustTimer -= uiDiff;

        // Pyroblast
        if (m_uiPyroblastTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, SPELL_PYROBLAST);

            m_uiPyroblastTimer = 7*IN_MILLISECONDS;
        }
        else
            m_uiPyroblastTimer -= uiDiff;


        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_golemagg(Creature* pCreature)
{
    return new boss_golemaggAI(pCreature);
}

struct MANGOS_DLL_DECL mob_core_ragerAI : public ScriptedAI
{
    mob_core_ragerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_molten_core*)pCreature->GetInstanceData();
        Reset();
    }

    instance_molten_core* m_pInstance;

    uint32 m_uiMangleTimer;

    void Reset()
    {
        m_uiMangleTimer = 7*IN_MILLISECONDS;                 // These times are probably wrong
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32& uiDamage)
    {
        //if (HealthBelowPct(50))
		if (m_creature->GetHealthPercent() < 50.0f)
        {
            if (m_pInstance)
            {
				if (Creature* pGolemagg = m_pInstance->GetSingleCreatureFromStorage(NPC_GOLEMAGG))
                {
                    if (pGolemagg->isAlive())
                    {
                        DoScriptText(EMOTE_LOWHP, m_creature);
                        m_creature->SetHealth(m_creature->GetMaxHealth());
                    }
                    else
                        uiDamage = m_creature->GetHealth();
                }
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Mangle
        if (m_uiMangleTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MANGLE);
            m_uiMangleTimer = 10*IN_MILLISECONDS;
        }
        else
            m_uiMangleTimer -= uiDiff;
  
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_core_rager(Creature* pCreature)
{
    return new mob_core_ragerAI(pCreature);
}

void AddSC_boss_golemagg()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_golemagg";
    pNewscript->GetAI = &GetAI_boss_golemagg;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_core_rager";
    pNewscript->GetAI = &GetAI_mob_core_rager;
    pNewscript->RegisterSelf();
}
